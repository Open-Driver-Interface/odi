#include "pci_dd.h"

#include "pci.h"

//This includes are not important!
#include "../../../core/debug.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

void scan_callback(struct pci_device_header* device, u32 base_address) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER SCAN: %d %d %d\n", device->class_code, device->subclass, device->prog_if);
}

void * pci_init(void * self, void* iobuff, void* control) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER INIT\n");
    return 0;
}
void * pci_exit (void * self, void* iobuff, void* control) {
    return 0;
}
void * pci_read (void * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER READ\n");
    return 0;
}
void * pci_write (void * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    odi_debug_append(ODI_DTAG_INFO, "BAPCISIC DRIVER WRITE\n");
    return 0;
}
void * pci_ioctl (void * self, void* iobuff, void* control, u64 operation) {

    switch (operation) {
        case PCI_IOCTL_SCAN_BUS: {
            struct pci_ioctl_scan_bus_control * data = (struct pci_ioctl_scan_bus_control*)control;

            void (*callback)(struct pci_device_header*, u32) = (void (*)(struct pci_device_header*, u32))iobuff;
            scan_pci(data->trail, data->devconf_size, callback);
            break;
        }
    }
    return 0;
}

struct odi_driver_functions pci_ops = {
    .init = pci_init,
    .exit = pci_exit,
    .read = pci_read,
    .write = pci_write,
    .ioctl = pci_ioctl
};

struct odi_driver_info * init_pci() {
    struct odi_driver_info * driver = odi_driver_register(PCI_DD_MAJOR, (void*)&pci_ops, PCI_DD_LICENSE, PCI_DD_VENDOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "PCI DRIVER REGISTER FAILED\n");
        return 0;
    }

    return driver;
}

void exit_pci(void) {
    struct odi_driver_info * driver = odi_driver_get(PCI_DD_MAJOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "PCI DRIVER UNREGISTER FAILED\n");
        return;
    }

    if (odi_driver_unregister(PCI_DD_MAJOR)) {
        odi_debug_append(ODI_DTAG_ERROR, "PCI DRIVER UNREGISTER FAILED\n");
        return;
    }
}