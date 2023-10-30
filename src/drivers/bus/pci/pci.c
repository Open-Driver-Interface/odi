#include "pci.h"

#include "../../../deps.h"
#include "../../../core/debug.h"

#define PCI_VIRT_BASE 0xffffaaff00000000

//Ignore -waddress-of-packed-member
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

struct pci_dev_list {
    char name[32];
    void * data;
    struct pci_device_header* device;
    void (*cb)(void*);
    struct pci_dev_list* next;
};

struct pci_dev_list* pci_dev_list_head = {0};

const char* pci_device_classes[] = {    
    "Unclassified",
    "Mass Storage Controller",
    "Network Controller",
    "Display Controller",
    "Multimedia Controller",
    "Memory Controller",
    "Bridge Device",
    "Simple Communication Controller",
    "Base System Peripheral",
    "Input Device Controller",
    "Docking Station", 
    "Processor",
    "Serial Bus Controller",
    "Wireless Controller",
    "Intelligent Controller",
    "Satellite Communication Controller",
    "Encryption Controller",
    "Signal Processing Controller",
    "Processing Accelerator",
    "Non Essential Instrumentation"
};

u8 has_interrupted(struct pci_device_header_0* device) {
    u16 status = ((struct pci_device_header*)device)->status;
    u16 command = ((struct pci_device_header*)device)->command;

    //If bit 3 of the status register == 1 and the bit 10 of the command register == 0 return 1
    return ((status & 0b1000) && !(command & 0b10000000000));
}

void PCIWrite32(u32 addr, u32 data) {
    addr &= ~(0b11);
    /* Write address */
    odi_dep_outl(PCI_CONFIG_ADDR, addr);
    /* Write data */
    odi_dep_outl(PCI_CONFIG_DATA, data);
}

u32 PCIRead32(u32 addr) {
    addr &= ~(0b11);
    /* Write address */
    odi_dep_outl(PCI_CONFIG_ADDR, addr);
    /* Read data */
    return odi_dep_inl(PCI_CONFIG_DATA);
}

u16 PCIRead16(u32 addr) {
    u8 offset = addr & 0xff;
    addr &= ~(0b11);
    return (u16) ((PCIRead32(addr) >> ((offset & 0b10) * 0x10)) & 0xffff);
}

u8 PCIRead8(u32 addr) {
    u8 offset = addr & 0xff;
    addr &= ~(0b11);
    return (u16) ((PCIRead16(addr) >> ((offset & 0b1) * 0x8)) & 0xff);
}

void PCIMemcpy8(void* dst, u32 src, u64 size){
    src &= ~(0b11);
    for(u64 i = 0; i < size; i += 0x1){
        *(u8*)((u64)dst + i) = PCIRead8(src + i);
    }
}

void PCIMemcpy16(void* dst, u32 src, u64 size){
    src &= ~(0b11);
    for(u64 i = 0; i < size; i += 0x2){
        *(u16*)((u64)dst + i) = PCIRead16(src + i);
    }
}

void PCIMemcpy32(void* dst, u32 src, u64 size){
    src &= ~(0b11);
    for(u64 i = 0; i < size; i += 0x4){
        *(u32*)((u64)dst + i) = PCIRead32(src + i);
    }
}


const char* get_vendor_name(u16 vendor_id) {
    switch (vendor_id) {
        case 0x8086:
            return "Intel";
        case 0x10de:
            return "nVidia";
        case 0x1022:
            return "AMD";
        case 0x1234:
            return "Bochs";
    }
    return odi_dep_itoa(vendor_id, 0x0, 16);
}

const char* get_device_class(u8 class) {
    if (class > 0x12) return "Unknown";
    return pci_device_classes[class];
}

const char* get_device_name(u16 vendor_id, u16 device_id) {
    switch (vendor_id) {
        case 0x8086: {
            switch (device_id) {
                case 0x29c0:
                    return "Express DRAM Controller";
                case 0x2918:
                    return "LPC Interface Controller";
                case 0x2922:
                    return "SATA Controller [AHCI]";
                case 0x2930:
                    return "SMBus Controller";
                case 0x10d3:
                    return "Gigabit Network Connection";
            }
            break;
        }
        case 0x1234: {
            switch (device_id) {
                case 0x1111:
                    return "Virtual VGA Controller";
            }
            break;
        }
    }
    
    return odi_dep_itoa(device_id, 0x0, 16);
}

const char * mass_storage_controller_subclass_name(u8 subclass_code) {
    switch (subclass_code) {
        case 0x00:
            return "SCSI Bus Controller";
        case 0x01:
            return "IDE Controller";
        case 0x02:
            return "Floppy Disk Controller";
        case 0x03:
            return "IPI Bus Controller";
        case 0x04:
            return "RAID Controller";
        case 0x05:
            return "ATA Controller";
        case 0x06:
            return "Serial ATA";
        case 0x07:
            return "Serial Attached SCSI";
        case 0x08:
            return "Non-Volatile Memory Controller";
        case 0x80:
            return "Other";
    }

    return odi_dep_itoa(subclass_code, 0x0, 16);
}

const char* serial_bus_controller_subclass_name(u8 subclass_code){
    switch (subclass_code){
        case 0x00:
            return "FireWire (IEEE 1394) Controller";
        case 0x01:
            return "ACCESS Bus";
        case 0x02:
            return "SSA";
        case 0x03:
            return "USB Controller";
        case 0x04:
            return "Fibre Channel";
        case 0x05:
            return "SMBus";
        case 0x06:
            return "Infiniband";
        case 0x07:
            return "IPMI Interface";
        case 0x08:
            return "SERCOS Interface (IEC 61491)";
        case 0x09:
            return "CANbus";
        case 0x80:
            return "SerialBusController - Other";
    }
    return odi_dep_itoa(subclass_code, 0x0, 16);
}

const char* bridge_device_subclass_name(u8 subclass_code){
    switch (subclass_code){
        case 0x00:
            return "Host Bridge";
        case 0x01:
            return "ISA Bridge";
        case 0x02:
            return "EISA Bridge";
        case 0x03:
            return "MCA Bridge";
        case 0x04:
            return "PCI-to-PCI Bridge";
        case 0x05:
            return "PCMCIA Bridge";
        case 0x06:
            return "NuBus Bridge";
        case 0x07:
            return "CardBus Bridge";
        case 0x08:
            return "RACEway Bridge";
        case 0x09:
            return "PCI-to-PCI Bridge";
        case 0x0a:
            return "InfiniBand-to-PCI Host Bridge";
        case 0x80:
            return "Other";
    }

    return odi_dep_itoa(subclass_code, 0x0, 16);
}

const char * get_subclass_name(u8 class_code, u8 subclass_code) {
    switch (class_code) {
        case 0x01:
            return mass_storage_controller_subclass_name(subclass_code);
        case 0x03: {
            switch (subclass_code) {
                case 0x00:
                    return "VGA Compatible Controller";
            }
            break;
        }
        case 0x06:
            return bridge_device_subclass_name(subclass_code);
        case 0x0c:
            return serial_bus_controller_subclass_name(subclass_code);
    }
    return odi_dep_itoa(subclass_code, 0x0, 16);
}

const char* get_prog_interface(u8 class_code, u8 subclass_code, u8 prog_interface){
    switch (class_code){
        case 0x01: {
            switch (subclass_code){
                case 0x06: {
                    switch (prog_interface){
                        case 0x00:
                            return "Vendor Specific Interface";
                        case 0x01:
                            return "AHCI 1.0";
                        case 0x02:
                            return "Serial Storage Bus";
                    }
                    break;
                }
            }
            break;
        }
        case 0x03: {
            switch (subclass_code) {
                case 0x00: {
                    switch (prog_interface){
                        case 0x00:
                            return "VGA Controller";
                        case 0x01:
                            return "8514-Compatible Controller";
                    }
                    break;
                }
            }
            break;
        }
        case 0x0C: {
            switch (subclass_code){
                case 0x03: {
                    switch (prog_interface){
                        case 0x00:
                            return "UHCI Controller";
                        case 0x10:
                            return "OHCI Controller";
                        case 0x20:
                            return "EHCI (USB2) Controller";
                        case 0x30:
                            return "XHCI (USB3) Controller";
                        case 0x80:
                            return "Unspecified";
                        case 0xFE:
                            return "USB Device (Not a Host Controller)";
                    }
                    break;
                }
            }    
            break;
        }
    }
    return odi_dep_itoa(prog_interface, 0x0, 16);
}

void trigger_pci_interrupt() {
    //Iterate pci_dev_list_head and check if any of the devices have an interrupt.
    //If so call cb
    struct pci_dev_list* current = pci_dev_list_head;
    while (current != 0) {
        if (has_interrupted((struct pci_device_header_0*)current->device)) {
            //odi_debug_append(ODI_DTAG_INFO, "[PCI] Device %s has interrupted\n", current->name);
            current->cb(current->data);
        }
        current = current->next;
    }
}

void subscribe_pci_interrupt(const char* id, struct pci_device_header * dev, void (*cb)(void*), void * data) {
    struct pci_dev_list* new_dev_list = odi_dep_malloc(sizeof(struct pci_dev_list));
    new_dev_list->device = dev;
    new_dev_list->cb = cb;
    new_dev_list->data = data;
    new_dev_list->next = pci_dev_list_head;
    odi_dep_strncpy(new_dev_list->name, id, odi_dep_strlen(id));
    pci_dev_list_head = new_dev_list;
}

void pci_set_irq(struct pci_device_header_0* pci, u8 irq) {
    pci->interrupt_line = irq;
}

void enumerate_function(u64 device_address, u64 bus, u64 device, u64 function, void (*cb)(struct pci_device_header*, u32)) {

    u64 offset = function << 12;

    u64 function_address = device_address + offset;
    u64 function_address_virt = PCI_VIRT_BASE + function_address;

    odi_dep_map_current_memory_size((void*)function_address_virt, (void*)function_address, 0x1000);

    struct pci_device_header* pci_device_header = (struct pci_device_header*)function_address_virt;

    if (pci_device_header->device_id == 0x0) return;
    if (pci_device_header->device_id == 0xFFFF) return;

    //odi_debug_append(ODI_DTAG_INFO, "[PCI] %s %s: %s %s / %s\n",
    //    get_device_class(pci_device_header->class_code),
    //    get_subclass_name(pci_device_header->class_code, pci_device_header->subclass),
    //    get_vendor_name(pci_device_header->vendor_id),
    //    get_device_name(pci_device_header->vendor_id, pci_device_header->device_id),
    //    get_prog_interface(pci_device_header->class_code, pci_device_header->subclass, pci_device_header->prog_if)
    //);

    u32 device_base_address = ((1 << 31) | (bus << 16) | (device << 11) | (function << 8));
    //odi_debug_append(ODI_DTAG_INFO, "[PCI] Device base address: %x\n", device_base_address);
    cb(pci_device_header, device_base_address);
}

void enumerate_device(u64 bus_address, u64 device, u64 bus, void (*cb)(struct pci_device_header*, u32)) {
    u64 offset = device << 15;

    u64 device_address = bus_address + offset;
    u64 device_address_virt = PCI_VIRT_BASE + device_address;
    odi_dep_map_current_memory_size((void*)device_address_virt, (void*)device_address, 0x1000);

    struct pci_device_header* pci_device_header = (struct pci_device_header*)device_address_virt;

    if (pci_device_header->device_id == 0x0) return;
    if (pci_device_header->device_id == 0xFFFF) return;

    //odi_debug_append(ODI_DTAG_INFO, "[PCI] enumerating device %x:%x\n", bus_address, device_address);

    if (pci_device_header->header_type & 0x80) {
        for (u64 function = 0; function < 8; function++) {
            enumerate_function(device_address, bus, device, function, cb);
        }
    } else {
        enumerate_function(device_address, bus, device, 0, cb);
    }
}

void enumerate_bus(u64 base_address, u64 bus,  void (*cb)(struct pci_device_header*, u32)) {
    u64 offset = bus << 20;

    u64 bus_address = base_address + offset;
    u64 bus_address_virt = PCI_VIRT_BASE + bus_address;
    odi_dep_map_current_memory((void*)bus_address_virt, (void*)bus_address);
    struct pci_device_header* pci_device_header = (struct pci_device_header*)bus_address_virt;

    if (pci_device_header->device_id == 0x0) return;
    if (pci_device_header->device_id == 0xFFFF) return;

    //odi_debug_append(ODI_DTAG_INFO, "\n[PCI] Enumerating bus %x\n", bus);
    for (u64 device = 0; device < 32; device++) {
        enumerate_device(bus_address, device, bus, cb);
    }
}

//Shoutout to kot!
//https://github.com/kot-org/Kot/blob/f14dabe3771fa5d633ab5523c888f3bf428f89b8/Sources/Modules/Drivers/bus/pci/Src/core/main.cpp#L152

void* get_bar_address(struct pci_device_header_0 * devh, u8 index) {
    if (index > 5) return 0x0;
    u32 *bar_pointer = &(devh->bar0);
    u32 bar_value = bar_pointer[index];
    u32 bar_value2 = bar_pointer[index + 1];
    switch (get_bar_type(bar_value)){
        case PCI_BAR_TYPE_IO:
            return (void*)(u64)(bar_value & 0xFFFFFFFC);
        case PCI_BAR_TYPE_32:
            return (void*)(u64)(bar_value & 0xFFFFFFF0);
        case PCI_BAR_TYPE_64: {
            if (index == 5) return 0x0;
            return (void*)(u64)((bar_value & 0xFFFFFFF0) | ((u64)(bar_value2 & 0xFFFFFFFF) << 32));
        }
        default:
            break;
    }

    return 0x0;
}

u8 get_bar_type(u32 value){
    if(value & 0b1){ /* i/o */
        return PCI_BAR_TYPE_IO;
    }else{
        if(!(value & 0b110)){ /* 32bits */
            return PCI_BAR_TYPE_32;
        }else if((value & 0b110) == 0b110){ /* 64bits */
            return PCI_BAR_TYPE_64;
        }
    }
    return PCI_BAR_TYPE_NULL;
}

void ReceiveConfigurationSpacePCI(u32 address, void * buffer){
    PCIMemcpy32(buffer, address, PCI_CONFIGURATION_SPACE_SIZE);
}

void SendConfigurationSpacePCI(u32 address, void * buffer){
    u64 ubuffer = (u64)buffer;

    ubuffer &= ~(0b11);

    for(u64 i = 0; i < PCI_CONFIGURATION_SPACE_SIZE; i += 4) {
        PCIWrite32(address, *(u32*)ubuffer);
        ubuffer += 4;
        address += 4;
    }
}

u64 get_bar_size(void* addresslow, u32 base_address){ //Address low is bar0.
    u32 BARValueLow = *(u32*)addresslow;
    u8 Type = get_bar_type(BARValueLow);

    u8 * confspace = odi_dep_malloc(PCI_CONFIGURATION_SPACE_SIZE);
    ReceiveConfigurationSpacePCI(base_address, confspace);

    if(Type != PCI_BAR_TYPE_NULL){
        /* Write into bar low */
        *(u32*)addresslow = 0xFFFFFFFF;
        SendConfigurationSpacePCI(base_address, confspace);

        /* Read bar low */
        ReceiveConfigurationSpacePCI(base_address, confspace);
        u32 SizeLow = *(u32*)addresslow;

        if(Type == PCI_BAR_TYPE_IO){
            SizeLow &= 0xFFFFFFFC;
        }else{
            SizeLow &= 0xFFFFFFF0;
        }


        u32 SizeHigh = 0xFFFFFFFF;

        if(Type == PCI_BAR_TYPE_64){
            void* addresshigh = (void*)((u64)addresslow + 0x4);

            u32 BARValueHigh = *(u32*)addresshigh;
            /* Write into bar high */
            *(u32*)addresshigh = 0xFFFFFFFF;
            SendConfigurationSpacePCI(base_address, confspace);

            /* Read bar high */
            ReceiveConfigurationSpacePCI(base_address, confspace);
            SizeHigh = *(u32*)addresshigh;

            /* Restore value */
            *(u32*)addresshigh = BARValueHigh;   
            SendConfigurationSpacePCI(base_address, confspace);
        }

        /* Restore value */
        *(u32*)addresslow = BARValueLow;
        SendConfigurationSpacePCI(base_address, confspace);

        u64 Size = ((u64)(SizeHigh & 0xFFFFFFFF) << 32) | (SizeLow & 0xFFFFFFFF);
        Size = ~Size + 1;

        odi_dep_free(confspace);
        return Size;
    }

    return 0;
}

void enable_bus_mastering(struct pci_device_header* pci_device_header) {
    u16 command = pci_device_header->command;
    command |= 0x4;
    pci_device_header->command = command;
}

void scan_pci(void *trail, u64 devconf_size,  void (*cb)(struct pci_device_header*, u32)) {
    u64 entryno = devconf_size / sizeof(struct device_config);

    for (u64 i = 0; i < entryno; i++) {
        struct device_config * new_device_config = (struct device_config*)((u64)trail + (sizeof(struct device_config) * i));	
        //odi_debug_append(ODI_DTAG_INFO, "[PCI] Base address as registering: %x\n", new_device_config->base_address);
        //odi_debug_append(ODI_DTAG_INFO, "[PCI] Enumerating buses %x:%x\n", new_device_config->start_bus, new_device_config->end_bus);
        for (u64 bus = new_device_config->start_bus; bus < new_device_config->end_bus; bus++) {
            enumerate_bus(new_device_config->base_address, bus, cb);
        }
    }
}