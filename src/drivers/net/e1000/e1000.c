#include <stdint.h>
#include "e1000c.h"
#include "../../../io/io.h"
#include "../../../util/printf.h"
#include "../../../util/dbgprinter.h"
#include "../../../util/string.h"
#include "../../../dev/pci/pci.h"
#include "../../../memory/heap.h"
#include "../../../memory/paging.h"

//Internal functions, architecture specific

#define GET_PAGE_SIZE(x) (((x) % 4096) ? (((x) >>12) + 1) : ((x) >> 12))

static inline void mmio_write32(void* address, uint32_t value) {
    *(volatile uint32_t*) address = value;
}

static inline uint32_t mmio_read32(void* address) {
    return *(volatile uint32_t*) address;
}

void write_register(e1000_t *nic, uint16_t reg, uint32_t value) {
    if(nic->bar_type == PCI_BAR_TYPE_IO) {
        outl(nic->io_base, reg);
        outl(nic->io_base + sizeof(uint32_t), value);
    } else {
        mmio_write32((void*) ((uint64_t)nic->mem_base + reg), value);
    }
}

uint32_t read_register(e1000_t *nic, uint16_t reg) {
    if(nic->bar_type == PCI_BAR_TYPE_IO) {
        outl(nic->io_base, reg);
        return inl(nic->io_base + sizeof(uint32_t));
    } else {
        return mmio_read32((void*) ((uint64_t)e->mem_base + reg));
    }
}

//Callback functions

void e1000_link_up(e1000_t *nic)
{
	uint32_t val = ReadRegister(nic, REG_CTRL);
	WriteRegister(nic, REG_CTRL, val | ECTRL_SLU);

	if (nic->inject_status_change != 0) {
        uint8_t status = ReadRegister(nic, 0x0008);
		nic->inject_status_change(nic->mac, status);
    }
}

void e1000_interrupt(void * addr) {
	struct e1000 *e = (struct e1000*)addr;
	uint32_t status = ReadRegister(e, 0xc0);

	if(status & 0x04)
	{
		e1000_linkup(e);
	}

	if(status & 0x10)
	{
		//printf("threshold good\n");
	}

	if(status & 0x80)
	{
        uint16_t old_cur;
        while((e->rx_descs[e->rx_cur]->status & 0x1))
        {
            uint8_t *buf = (uint8_t*)e->rx_descs[e->rx_cur]->addr;
            uint16_t len = e->rx_descs[e->rx_cur]->length;

            if (e->inject_packet != 0) {
                e->inject_packet(e->mac, buf, len);
            }  

            e->rx_descs[e->rx_cur]->status = 0;
            old_cur = e->rx_cur;
            e->rx_cur = (e->rx_cur + 1) % NUM_RX_DESC;
            WriteRegister(e, REG_RXDESCTAIL, old_cur ) ;
        }
	}
}

//Nic functions

uint32_t e1000_read_eeprom(e1000_t *nic, uint8_t addr)
{
	uint32_t val = 0;
	uint32_t test;

	if(nic->eeprom_type == 0)
		test = addr << 8;
	else
		test = addr << 2;

	WriteRegister(e, REG_EEPROM, test | 0x1);

	if(nic->eeprom_type == 0)
		while(!((val = ReadRegister(e, REG_EEPROM)) & (1<<4)))
	else
		while(!((val = ReadRegister(e, REG_EEPROM)) & (1<<1)))

	val >>= 16;
	return val;
}

void e1000_read_mac(e1000_t *nic, u8 *mac)
{
	uint32_t temp;

	temp = e1000_read_eeprom(e, 0);
	mac[0] = temp &0xff;
	mac[1] = temp >> 8;

	temp = e1000_read_eeprom(e, 1);
	mac[2] = temp &0xff;
	mac[3] = temp >> 8;

	temp = e1000_read_eeprom(e, 2);
	mac[4] = temp &0xff;
	mac[5] = temp >> 8;
}

void e1000_get_eeprom_type(e1000_t *nic)
{
	uint32_t val = 0;
	WriteRegister(nic, REG_EEPROM, 0x1);

	for(int i = 0; i < 1000; i++)
	{
		val = ReadRegister(nic, REG_EEPROM);
		if(val & 0x10)
			nic->eeprom_type = 0;
		else
			nic->eeprom_type = 1;
	}
}

uint8_t e1000_get_status(e1000_t *nic) {
	return ReadRegister(nic, 0x0008);
}

void e1000_set_injection_callback(e1000_t *nic, void (*inject_packet)(uint8_t*, uint8_t * p_data, uint16_t p_len)) {
	e->inject_packet = inject_packet;
}

void e1000_set_status_change_callback(e1000_t *nic, void (*data)(uint8_t*, uint8_t)) {
	nic->inject_status_change = data;
}

void e1000_enable_interrupts(e1000_t *nic)
{
	WriteRegister(nic, REG_IMASK ,0x1F6DC);
	WriteRegister(nic, REG_IMASK ,0xff & ~4);
	ReadRegister(nic, 0xc0);
}

size_t e1000_send(struct e1000 *e, uint8_t *_buf, size_t length)
{
    memcpy((void*)e->tx_descs[e->tx_cur]->addr, _buf, length);
	e->tx_descs[e->tx_cur]->length = length;
	e->tx_descs[e->tx_cur]->cmd = ((1 << 3) | 3);
	uint8_t old_cur = e->tx_cur;
	e->tx_cur = (e->tx_cur + 1) % NUM_TX_DESC;
	WriteRegister(e, REG_TXDESCTAIL, e->tx_cur);
	while(!(e->tx_descs[old_cur]->status & 0xff));
    memset((void*)e->tx_descs[old_cur]->addr, 0, 8192);
	return 0;
}

//Startup functions
void e1000_rxinit(struct e1000 *e)
{
	uint8_t* ptr;
	struct e1000_rx_desc *descs;
    ptr = (uint8_t*)request_current_pages_identity(GET_PAGE_SIZE(sizeof(struct e1000_rx_desc)*NUM_RX_DESC + 16));
    memset(ptr, 0, sizeof(struct e1000_rx_desc)*NUM_RX_DESC);
	e->rx_free = (uint8_t *)ptr;
	if((uint64_t)ptr % 16 != 0)
		ptr = (ptr + 16) - ((uint64_t)ptr % 16);
	descs = (struct e1000_rx_desc *)ptr;
	for(int i = 0; i < NUM_RX_DESC; i++)
	{
		e->rx_descs[i] = (struct e1000_rx_desc *)((uint8_t*)descs + i*16);
		//XXX: Don't use kmalloc here
		e->rx_descs[i]->addr = (uint64_t)(uint8_t*)(request_current_pages_identity(GET_PAGE_SIZE(8192 + 16)));
        memset((void*)e->rx_descs[i]->addr, 0, 8192);
		e->rx_descs[i]->status = 0;
	}

	//give the card the pointer to the descriptors
    WriteRegister(e, REG_TXDESCLO, (uint32_t)((uint64_t)ptr >> 32) );
    WriteRegister(e, REG_TXDESCHI, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));
 
    WriteRegister(e, REG_RXDESCLO, (uint64_t)ptr);
    WriteRegister(e, REG_RXDESCHI, 0);

	//now setup total length of descriptors
	WriteRegister(e, REG_RXDESCLEN, NUM_RX_DESC * 16);

	//setup numbers
	WriteRegister(e, REG_RXDESCHEAD, 0);
	WriteRegister(e, REG_RXDESCTAIL, NUM_RX_DESC);
	e->rx_cur = 0;

	//enable receiving
	//(2 << 16) | (1 << 25) RCTL_BSIZE_8192
	//(1 << 26) RCTL_SECRC
	//(1 << 15) RCTL_BAM
	//(1 << 5) RCTL_LPE
	//(0 << 8) RDMTS_HALF
	//(0 << 4) RCTL_MPE = 0
	//(0 << 3) RCTL_UPE = 0
	//(1 << 2) RCTL_SBP = 1
	uint32_t flags = (2 << 16) | (1 << 25) | (1 << 26) | (1 << 15) | (1 << 5) | (0 << 8) | (0 << 4) | (0 << 3) | ( 1 << 2);
	WriteRegister(e, REG_RCTRL, flags);
}
void e1000_txinit(struct e1000 *e)
{
	uint8_t* ptr;
	struct e1000_tx_desc *descs;
    ptr = (uint8_t *)(request_current_pages_identity(GET_PAGE_SIZE(sizeof(struct e1000_tx_desc)*NUM_TX_DESC + 16)));
    memset(ptr, 0, sizeof(struct e1000_tx_desc)*NUM_TX_DESC);
    e->tx_free = (uint8_t *)ptr;
	if((uint64_t)ptr % 16 != 0)
		ptr = (ptr + 16) - ((uint64_t)ptr % 16);
	descs = (struct e1000_tx_desc *)ptr;
	for(int i = 0; i < NUM_TX_DESC; i++)
	{
		e->tx_descs[i] = (struct e1000_tx_desc *)((uint8_t*)descs + i*16);
		e->tx_descs[i]->addr = (uint64_t)(uint8_t *)(request_current_pages_identity(GET_PAGE_SIZE(8192 + 16)));
        memset((void*)e->tx_descs[i]->addr, 0, 8192);
		e->tx_descs[i]->cmd = 0;
	}

	//give the card the pointer to the descriptors
    WriteRegister(e, REG_TXDESCHI, (uint32_t)((uint64_t)ptr >> 32) );
    WriteRegister(e, REG_TXDESCLO, (uint32_t)((uint64_t)ptr & 0xFFFFFFFF));

	//now setup total length of descriptors
	WriteRegister(e, REG_TXDESCLEN, NUM_TX_DESC * 16);

	//setup numbers
	WriteRegister(e, REG_TXDESCHEAD, 0);
	WriteRegister(e, REG_TXDESCTAIL, NUM_TX_DESC);
	e->tx_cur = 0;

	WriteRegister(e, REG_TCTRL, (1 << 1) | (1 << 3));
}
void e1000_start(struct e1000 *e)
{
	//set link up
	e1000_linkup(e);
	//have to clear out the multicast filter, otherwise shit breaks
	for(int i = 0; i < 0x80; i++)
		WriteRegister(e, 0x5200 + i*4, 0);

	e1000_interrupt_enable(e);

	e1000_rxinit(e);
	e1000_txinit(e);
}

//Move this to e1000_dd.c
struct e1000 *e1000_init(struct pci_device_header_0 * _pciConfigHeader, uint32_t base_address)
{
	struct e1000 *e = (struct e1000 *)malloc(sizeof(*e));
	e->inject_packet = 0x0;
	e->inject_status_change = 0x0;
	e->pciConfigHeader = _pciConfigHeader;
    uint32_t bar_address = (uint32_t)(uint64_t)get_bar_address(_pciConfigHeader, 0x0);
    e->bar_type = get_bar_type(bar_address);

    printf("[NIC] Bar0 Type: %x interrupt line: %x\n", e->bar_type, _pciConfigHeader->interrupt_line);

    if (e->bar_type == PCI_BAR_TYPE_IO) {
        printf("[NIC] IO Base: %x\n", bar_address);
        e->io_base = bar_address;
    } else {
        printf("[NIC] Memory Base: %x\n", bar_address);
        uint64_t bar_size = get_bar_size((void*)(uint64_t)bar_address, base_address);
        map_current_memory_size((void*)(uint64_t)bar_address, (void*)(uint64_t)bar_address, bar_size);
        e->mem_base = (uint8_t*)(uint64_t) bar_address;
    }
	
	subscribe_pci_interrupt("E1000", (struct pci_device_header*)_pciConfigHeader, handle_nic_int, (void*)e);

	e1000_eeprom_gettype(e);
	e1000_getmac(e, (char *)e->mac);
	print_mac((char *)&e->mac);

	e1000_start(e);
	uint32_t flags = ReadRegister(e, REG_RCTRL);
	WriteRegister(e, REG_RCTRL, flags | RCTRL_EN);//RCTRL_8192 | RCTRL_MPE | RCTRL_UPE |RCTRL_EN);

    
	return e;
}