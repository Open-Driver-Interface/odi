#include "acpi_dd.h"

#include "acpi.h"

//This includes are not important!
#include "../../../core/debug.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

void * dd_acpi_init(void * self, void* iobuff, void* control) {
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER INIT\n");
    return 0;
}

void * dd_acpi_exit (void * self, void* iobuff, void* control) {
    return 0;
}

u64 dd_acpi_read (void * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER READ\n");
    return 0;
}

u64 dd_acpi_write (void * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER WRITE\n");
    return 0;
}

void * dd_acpi_ioctl (void * self, void* iobuff, void* control, u64 operation) {

    switch (operation) {
        case ACPI_DD_IOCTL_INIT: {
            startup(control);
            break;
        }
        case ACPI_DD_IOCTL_GET_MCFG: {
            return (struct odi_acpi_dd_mcfg_header*)get_acpi_mcfg(control);
            break;
        }
        case ACPI_DD_IOCTL_GET_MADT: {
            return (struct odi_acpi_dd_madt_header*)get_acpi_madt(control);
            break;
        }
        case ACPI_DD_IOCTL_GET_FADT: {
            return (struct odi_acpi_dd_fadt_header*)get_acpi_fadt(control);
            break;
        }

    }
    return 0;
}

struct odi_driver_functions acpi_ops = {
    .init = dd_acpi_init,
    .exit = dd_acpi_exit,
    .read = dd_acpi_read,
    .write = dd_acpi_write,
    .ioctl = dd_acpi_ioctl
};

struct odi_driver_info* init_acpi(void) {
    struct odi_driver_info * driver = odi_driver_register(ACPI_DD_MAJOR, (void*)&acpi_ops, ACPI_DD_LICENSE, ACPI_DD_VENDOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "ACPI DRIVER REGISTER FAILED\n");
        return 0;
    }

    return driver;
}

void exit_acpi(void) {
    struct odi_driver_info * driver = odi_driver_get(ACPI_DD_MAJOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "ACPI DRIVER UNREGISTER FAILED\n");
        return;
    }

    if (odi_driver_unregister(ACPI_DD_MAJOR)) {
        odi_debug_append(ODI_DTAG_ERROR, "ACPI DRIVER UNREGISTER FAILED\n");
        return;
    }

}