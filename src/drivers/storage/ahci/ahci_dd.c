#include "ahci_dd.h"
#include "ahci.h"

#include "../../bus/pci/pci_dd.h"

#include "../../../core/debug.h"
#include "../../../core/driver.h"
#include "../../../core/device.h"

#include "../../../deps.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

struct ahci_device {
    struct hba_memory * abar;
    struct ahci_port ahci_ports[MAX_AHCI_PORTS_PER_DEVICE];
    u8 port_count;
};

struct ahci_device ahci_devices[MAX_AHCI_DEVICES];

u8 ahci_device_count = 0;

struct ahci_device * get_device_by_abar(struct hba_memory * abar) {
    for (u8 i = 0; i < ahci_device_count; i++) {
        if (ahci_devices[i].abar == abar) {
            return &ahci_devices[i];
        }
    }

    return 0;
}

void * _ahci_dd_init(void * self, void* iobuff, void* control) {
    return 0;
}

void * _ahci_dd_exit (void * self, void* iobuff, void* control) {
    return 0;
}

u64 _ahci_dd_read (void * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    struct ahci_port * port = (struct ahci_port*)control;
    if (port == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATA READ FAILED: PORT IS NULL\n");
        return 0;
    }

    if (port->port_type != PORT_TYPE_SATA) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATA READ FAILED: PORT IS NOT SATA\n");
        return 0;
    }
    
    u64 i;
    for (i = 0; i < read_size; i++) {
        if (!read_port(port, read_offset+i, 1)) break;
        odi_dep_memcpy(iobuff + (512*i), port->vbuffer, 512);
    }
    return i;
}

u64 _ahci_dd_write (void * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    struct ahci_port * port = (struct ahci_port*)control;
    if (port == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATA WRITE FAILED: PORT IS NULL\n");
        return 0;
    }

    if (port->port_type != PORT_TYPE_SATA) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATA WRITE FAILED: PORT IS NOT SATA\n");
        return 0;
    }

    u64 i;
    for (i = 0; i < write_size; i++) {
        odi_dep_memcpy(port->vbuffer, iobuff + (512*i), 512);
        if (!write_port(port, write_offset+i, 1)) break;
    }

    return i;
}

void * _ahci_dd_ioctl (void * self, void* iobuff, void* control, u64 operation) {
    
    struct ahci_port * port = (struct ahci_port*)control;
    if (port == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER IOCTL FAILED: PORT IS NULL\n");
        return 0;
    }

    switch (operation) {
        case AHCI_IOCTL_ATAPI_IDENTIFY: {
            identify(port);
            odi_dep_memcpy(iobuff, port->vbuffer, 512);
            break;
        }
        case AHCI_IOCTL_INIT: { //TODO: Not implemented
            *(u64*)iobuff = 1;
            return (void*)1;
        }
        case AHCI_IOCTL_CTRL_SYNC:
            *(u64*)iobuff = 1;
            return (void*)1; //TODO: Modify if the buffer cache is implemented or async reads
        case AHCI_IOCTL_CTRL_TRIM:
            *(u64*)iobuff = 1;
            return (void*)1; //TODO: Modify if the buffer cache is implemented or async reads
        case AHCI_IOCTL_GET_SECTOR_SIZE: { //TODO: Not implemented
            *(u64*)iobuff = 512;
            return (void*)512;
        }
        case AHCI_IOCTL_GET_SECTOR_COUNT: {
            identify(port);
            struct ahci_sata_ident * sident = (struct ahci_sata_ident*) port->vbuffer;
            *(u64*)iobuff = sident->CurrentSectorCapacity;
            return (void*)sident->CurrentSectorCapacity;
        }
        case AHCI_IOCTL_GET_BLOCK_SIZE: //TODO: Not implemented
            *(u64*)iobuff = 512;
            return (void*)512;
    }

    return (void*)0;
}

u64 _ahci_dd_read_atapi (void * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    struct ahci_port * port = (struct ahci_port*)control;
    if (port == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATAPI READ FAILED: PORT IS NULL\n");
        return 0;
    }

    if (port->port_type != PORT_TYPE_SATAPI) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATAPI READ FAILED: PORT IS NOT SATAPI\n");
        return 0;
    }

    if (read_offset != 0)
        odi_debug_append(ODI_DTAG_WARN, "ATAPI READS ARE NOT WORKING WELL, AN OFFSET != 0 YIELDS ZEROS\n");

    if (!read_atapi_port(port, read_offset, read_size))
        return 0;

    odi_dep_memcpy(iobuff, port->vbuffer, 512*read_size);

    return read_size;
}

u64 _ahci_dd_write_atapi (void * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER ATAPI WRITE NOT IMPLEMENTED\n");
    return 0;
}

u64 _ahci_dd_rw_invalid (void * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER INVALID READ/WRITE ON REGISTRAR DEVICE\n");
    return 0;
}

void * _ahci_dd_ioctl_invalid (void * self, void* iobuff, void* control, u64 operation) {
    odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER INVALID IOCTL ON REGISTRAR DEVICE\n");
    return 0;
}

void * _ahci_dd_init_generic(void * self, void* iobuff, void* control) {
    struct pci_dd_device_header * header = (struct pci_dd_device_header*)control;
    struct hba_memory* abar = (struct hba_memory*)(u64)header->bar5;

    if (get_device_by_abar(abar) != 0) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER INIT FAILED: DEVICE ALREADY INITIALIZED\n");
        return (void*)0;
    }

    if (ahci_device_count >= MAX_AHCI_DEVICES) {
        odi_debug_append(ODI_DTAG_ERROR, "AHCI DRIVER INIT FAILED: TOO MANY DEVICES\n");
        return (void*)0;
    }

    struct ahci_device * device = &ahci_devices[ahci_device_count++];

    device->abar = abar;
    device->port_count = 0;

    init_ahci(device->abar, device->ahci_ports, &(device->port_count));

    for (u8 i = 0; i < device->port_count; i++) {
        struct ahci_port * port = &(device->ahci_ports[i]);
        switch (port->port_type) {
            case PORT_TYPE_NONE: {
                break;
            }
            case PORT_TYPE_SATA: {
                odi_device_register(AHCI_DD_MAJOR_ATA, (void*)port, (void*)0x0);
                break;
            }
            case PORT_TYPE_SEMB: {
                break;
            }
            case PORT_TYPE_PM: {
                break;
            }
            case PORT_TYPE_SATAPI: {
                odi_device_register(ACHI_DD_MAJOR_ATAPI, (void*)port, (void*)0x0);
                break;
            }
            default : {
                break;
            }
        }
    }

    return (void*)1;
}

void * _ahci_dd_exit_generic (void * self, void* iobuff, void* control) {
    odi_debug_append(ODI_DTAG_INFO, "AHCI DRIVER GENERIC EXIT\n");
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