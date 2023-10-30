#ifndef _PCI_DD_H
#define _PCI_DD_H

#include "../../../core/types.h"
#include "../../../core/driver.h"

#define PCI_IOCTL_SCAN_BUS      0x0001

struct pci_dd_device_header {
    u16 vendor_id;
    u16 device_id;
    u16 command;
    u16 status;
    u8 revision_id;
    u8 prog_if;
    u8 subclass;
    u8 class_code;
    u8 cache_line_size;
    u8 latency_timer;
    u8 header_type;
    u8 bist;
    u32 bar0;
    u32 bar1;
    u32 bar2;
    u32 bar3;
    u32 bar4;
    u32 bar5;
    u32 cardbus_cis_pointer;
    u16 subsystem_vendor_id;
    u16 subsystem_id;
    u32 expansion_rom_base_address;
    u8 capabilities_pointer;
    u8 reserved0;
    u16 reserved1;
    u32 reserved2;
    u8 interrupt_line;
    u8 interrupt_pin;
    u8 min_grant;
    u8 max_latency;
} __attribute__ ((packed));

typedef void (*pci_dd_scan_callback_t)(struct pci_dd_device_header* device, u32 base_address);
struct pci_ioctl_scan_bus_control {
    void *trail;
    u64 devconf_size;
};

#define PCI_DD_LICENSE    ODI_LICENSE_TYPE_MIT
#define PCI_DD_VENDOR     "OPENDRIVERINTERFACE\0"
#define PCI_DD_NAME       "PCI DRIVER\0"
#define PCI_DD_MAJOR      86

struct odi_driver_info * init_pci(void);
void exit_pci(void);
#endif