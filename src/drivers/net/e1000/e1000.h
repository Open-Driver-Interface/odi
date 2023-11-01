#ifndef ODI_E1000_H
#define ODI_E1000_H

#define E1000_DEV 0x100E

#define REG_CTRL 		0x0
#define REG_EEPROM 		0x14
#define REG_IMASK 		0xD0
#define REG_RCTRL 		0x100
#define REG_RXDESCLO  	0x2800
#define REG_RXDESCHI  	0x2804
#define REG_RXDESCLEN 	0x2808
#define REG_RXDESCHEAD 	0x2810
#define REG_RXDESCTAIL 	0x2818

#define REG_TCTRL 		0x400
#define REG_TXDESCLO  	0x3800
#define REG_TXDESCHI  	0x3804
#define REG_TXDESCLEN 	0x3808
#define REG_TXDESCHEAD 	0x3810
#define REG_TXDESCTAIL 	0x3818

#define ECTRL_FD   	0x01
#define ECTRL_ASDE 	0x20
#define ECTRL_SLU  	0x40

#define RCTRL_EN 	0x2
#define RCTRL_SBP 	0x4
#define RCTRL_UPE 	0x8
#define RCTRL_MPE 	0x10
#define RCTRL_8192 	0x30000

#define NUM_RX_DESCRIPTORS 768
#define NUM_TX_DESCRIPTORS 768

typedef struct __attribute__((packed)) e1000_tx_desc {
	uint64_t address;
	uint16_t length;
	uint8_t cso;
	uint8_t command;
	uint8_t status;
	uint8_t css;
	uint16_t special;
} e1000_tx_desc_t;

typedef struct __attribute__((packed)) e1000_rx_desc {
	uint64_t addres;
	uint16_t length;
	uint16_t checksum;
	uint8_t status;
	uint8_t errors_pressent;
	uint16_t special;
} e1000_rx_desc_t;

typedef struct e1000_device {
	uint8_t  mac[6];
    uint8_t bar_type;
    int eeprom_type;

	uint16_t io_base;
	uint8_t *mem_base;
	void * pci_config_header;
    
	uint8_t *rx_free;
	uint8_t *tx_free;
	struct e1000_rx_desc *rx_descs[NUM_RX_DESC];
	struct e1000_tx_desc *tx_descs[NUM_TX_DESC];
	uint16_t rx_index;
	uint16_t tx_index;

	void (*inject_packet)(uint8_t *, uint8_t * p_data, uint16_t p_len);
	void (*inject_status_change)(uint8_t *, uint8_t);
} e1000_t;

void handle_nic_int();
uint8_t get_status(void *e);
void inject_status_change_callback(struct e1000 *e, void (*)(uint8_t *, uint8_t));
void set_injection_callback(struct e1000 *e, void (*)(uint8_t *, uint8_t * p_data, uint16_t p_len));
struct e1000 *e1000_init(void * _pciConfigHeader, uint32_t base_address);
uint8_t * get_mac_address(void *e);                         // Returns the MAC address
int send_packet(void*e, uint8_t* p_data, uint16_t p_len);
#endif