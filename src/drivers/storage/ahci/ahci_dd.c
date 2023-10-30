#include "ahci_dd.h"
#include "ahci.h"
#include "../../../core/debug.h"
#include "../../../core/driver.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

struct ahci_device {
    struct hba_memory * abar;
    struct ahci_port  ahci_ports[MAX_AHCI_PORTS_PER_DEVICE];
    u8 port_count;
};

struct ahci_device ahci_devices[MAX_AHCI_DEVICES];
u8 ahci_device_count = 0;

struct ahci_device * get_device_by_abar(void * abar) {
    for (u8 i = 0; i < ahci_device_count; i++) {
        if (ahci_devices[i].abar == abar) {
            return &ahci_devices[i];
        }
    }

    return 0;
}

void * _ahci_dd_init(void * self, void* iobuff, void* control) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER INIT\n");
    return 0;
}
void * _ahci_dd_exit (void * self, void* iobuff, void* control) {
    return 0;
}

void * _ahci_dd_read (void * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER READ\n");
    return 0;
}
void * _ahci_dd_write (void * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER WRITE\n");
    return 0;
}

void * _ahci_dd_ioctl (void * self, void* iobuff, void* control, u64 operation) {
    
    struct ahci_device * device = get_device_by_abar(control);
    if (device == 0 && operation != AHCI_IOCTL_INIT) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER IOCTL FAILED: DEVICE NOT INITIALIZED\n");
        return 0;
    }

    //struct hba_memory* abar = device->abar;
    //struct ahci_port* port = device->ahci_ports[iobuff];

    switch (operation) {
        case AHCI_IOCTL_ATAPI_IDENTIFY: {
            //if (device == 0) return 0;
            //identify(device->abar, &(device->ahci_ports[port]));
            //odi_dep_memcpy(iobuff, hw_buffer, 512);
            break;
        }
        case AHCI_IOCTL_INIT: {
            if (get_device_by_abar(control) != 0) {
                odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER INIT FAILED: DEVICE ALREADY INITIALIZED\n");
                return (void*)0;
            }

            if (ahci_device_count >= MAX_AHCI_DEVICES) {
                odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER INIT FAILED: TOO MANY DEVICES\n");
                return (void*)0;
            }

            ahci_devices[ahci_device_count].abar = control;
            ahci_devices[ahci_device_count].port_count = 0;
            ahci_device_count++;

            init_ahci(ahci_devices[ahci_device_count].abar, ahci_devices[ahci_device_count].ahci_ports, &ahci_devices[ahci_device_count].port_count);
            return (void*)1;
        }
        case AHCI_IOCTL_CTRL_SYNC:
            *(u32*)iobuff = 1;
            return (void*)1; //TODO: Modify if the buffer cache is implemented or async reads
        case AHCI_IOCTL_CTRL_TRIM:
            *(u32*)iobuff = 1;
            return (void*)1; //TODO: Modify if the buffer cache is implemented or async reads
        case AHCI_IOCTL_GET_SECTOR_SIZE: {
            *(u32*)iobuff = 512;
            return (void*)512;
        }
        case AHCI_IOCTL_GET_SECTOR_COUNT: {
            //identify((u8)control);
            //struct ahci_sata_ident * sident = (struct ahci_sata_ident*) hw_buffer;
            //*(u64*)iobuff = sident->CurrentSectorCapacity;
            //return sident->CurrentSectorCapacity;
        }
        case AHCI_IOCTL_GET_BLOCK_SIZE:
            *(u32*)iobuff = 512;
            return (void*)512;
    }
    return (void*)0;
}

void * _ahci_dd_read_atapi (void * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER ATAPI READ\n");
    return 0;
}
void * _ahci_dd_write_atapi (void * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER ATAPI WRITE\n");
    return 0;
}

void * _ahci_dd_rw_invalid (void * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER INVALID READ/WRITE ON REGISTRAR DEVICE\n");
    return 0;
}

void * _ahci_dd_ioctl_invalid (void * self, void* iobuff, void* control, u64 operation) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER INVALID IOCTL ON REGISTRAR DEVICE\n");
    return 0;
}

void * _ahci_dd_init_generic(void * self, void* iobuff, void* control) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER GENERIC INIT\n");
    return 0;
}
void * _ahci_dd_exit_generic (void * self, void* iobuff, void* control) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER GENERIC INIT\n");
    return 0;
}

struct odi_driver_functions ahci_ata_ops = {
    .init = _ahci_dd_init,
    .exit = _ahci_dd_exit,
    .read = _ahci_dd_read,
    .write = _ahci_dd_write,
    .ioctl = _ahci_dd_ioctl
};

struct odi_driver_functions ahci_atapi_ops = {
    .init = _ahci_dd_init,
    .exit = _ahci_dd_exit,
    .read = _ahci_dd_read_atapi,
    .write = _ahci_dd_write_atapi,
    .ioctl = _ahci_dd_ioctl
};

struct odi_driver_functions ahci_ops= {
    .init = _ahci_dd_init_generic,
    .exit = _ahci_dd_exit_generic,
    .read = _ahci_dd_rw_invalid,
    .write = _ahci_dd_rw_invalid,
    .ioctl = _ahci_dd_ioctl_invalid
};

void ahci_dd_init(void) {
    struct odi_driver_info * driver = odi_driver_register(AHCI_DD_MAJOR, (void*)&ahci_ops, AHCI_DD_LICENSE, AHCI_DD_VENDOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER REGISTER FAILED\n");
        return;
    }

    driver = odi_driver_register(AHCI_DD_MAJOR_ATA, (void*)&ahci_ata_ops, AHCI_DD_LICENSE, AHCI_DD_VENDOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER REGISTER FAILED\n");
        return;
    }
    
    driver = odi_driver_register(ACHI_DD_MAJOR_ATAPI, (void*)&ahci_atapi_ops, AHCI_DD_LICENSE, AHCI_DD_VENDOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATAPI REGISTER FAILED\n");
        return;
    }
}

void ahci_dd_exit(void) {

    struct odi_driver_info * driver = odi_driver_get(AHCI_DD_MAJOR_ATA);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATA UNREGISTER FAILED\n");
        return;
    } else {
        if (odi_driver_unregister(AHCI_DD_MAJOR_ATA)) {
            odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATA UNREGISTER FAILED\n");
            return;
        }
    }

    driver = odi_driver_get(ACHI_DD_MAJOR_ATAPI);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATAPI UNREGISTER FAILED\n");
        return;
    } else {
        //We dont exit the driver because it is the same as the ata driver
        if (odi_driver_unregister(ACHI_DD_MAJOR_ATAPI)) {
            odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATAPI UNREGISTER FAILED\n");
            return;
        }
    }

    driver = odi_driver_get(AHCI_DD_MAJOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER UNREGISTER FAILED\n");
        return;
    } else {
        if (odi_driver_unregister(AHCI_DD_MAJOR)) {
            odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER UNREGISTER FAILED\n");
            return;
        }
    }
}