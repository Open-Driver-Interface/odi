#include "pci_dd.h"

#include "pci.h"

//This includes are not important!
#include "../../../core/debug.h"
#include "../../../core/driver.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

void scan_callback(struct pci_device_header* device, u32 base_address) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER SCAN: %d %d %d\n", device->class_code, device->subclass, device->prog_if);
}

void * pci_init(struct odi_driver_info * self, void* iobuff) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER INIT\n");
    return 0;
}
void * pci_exit (struct odi_driver_info * self, void* iobuff) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER EXIT\n");
    return 0;
}
void * pci_read (struct odi_driver_info * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER READ\n");
    return 0;
}
void * pci_write (struct odi_driver_info * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    odi_debug_append(ODI_DTAG_INFO, "BAPCISIC DRIVER WRITE\n");
    return 0;
}
void * pci_ioctl (struct odi_driver_info * self, void* iobuff, void* control, u64 operation) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER IOCTL\n");

    switch (operation) {
        case PCI_IOCTL_SCAN_BUS: {
            struct pci_ioctl_scan_bus_control * data = (struct pci_ioctl_scan_bus_control*)control;

            odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER IOCTL SCAN BUS\n");
            scan_pci(data->entries, data->devconf_size, scan_callback);
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

void init_pci(void) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER REGISTER STARTED\n");
    struct odi_driver_info * driver = odi_driver_register(PCI_DD_MAJOR, (void*)&pci_ops, PCI_DD_LICENSE, PCI_DD_VENDOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "PCI DRIVER REGISTER FAILED\n");
        return;
    }

    ((struct odi_driver_functions*)driver->functions)->init(driver, 0);

    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER REGISTER FINISHED\n");
}

void exit_pci(void) {
    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER UNREGISTER STARTED\n");
    struct odi_driver_info * driver = odi_driver_get(PCI_DD_MAJOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "PCI DRIVER UNREGISTER FAILED\n");
        return;
    }

    ((struct odi_driver_functions*)driver->functions)->exit(driver, 0);

    if (odi_driver_unregister(PCI_DD_MAJOR)) {
        odi_debug_append(ODI_DTAG_ERROR, "PCI DRIVER UNREGISTER FAILED\n");
        return;
    }

    odi_debug_append(ODI_DTAG_INFO, "PCI DRIVER UNREGISTER FINISHED\n");
}