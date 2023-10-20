#ifndef _PCI_DD_H
#define _PCI_DD_H

#include "../../../core/types.h"

#define PCI_IOCTL_SCAN_BUS      0x0001
struct pci_ioctl_scan_bus_control {
    void *entries;
    u64 devconf_size;
};

#define PCI_DD_LICENSE    ODI_LICENSE_TYPE_MIT
#define PCI_DD_VENDOR     "OPENDRIVERINTERFACE\0"
#define PCI_DD_NAME       "PCI DRIVER\0"
#define PCI_DD_MAJOR      86

void init_pci(void);
void exit_pci(void);
#endif