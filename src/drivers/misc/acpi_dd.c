#include "acpi_dd.h"

#include "acpi.h"

//This includes are not important!
#include "../../core/debug.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

void * dd_acpi_init(struct odi_driver_info * self, void* iobuff) {
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER INIT\n");
    return 0;
}

void * dd_acpi_exit (struct odi_driver_info * self, void* iobuff) {
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER EXIT\n");
    return 0;
}

void * dd_acpi_read (struct odi_driver_info * self, void* iobuff, void* control, u64 read_size, u64 read_offset) {
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER READ\n");
    return 0;
}

void * dd_acpi_write (struct odi_driver_info * self, void* iobuff, void* control, u64 write_size, u64 write_offset) {
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER WRITE\n");
    return 0;
}

void * dd_acpi_ioctl (struct odi_driver_info * self, void* iobuff, void* control, u64 operation) {
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER IOCTL\n");

    switch (operation) {
        case ACPI_DD_IOCTL_INIT: {
            odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER IOCTL INIT\n");
            startup(control);
            break;
        }
        case ACPI_DD_IOCTL_GET_MCFG: {
            odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER IOCTL GET MCFG\n");
            return (struct acpi_dd_mcfg_header*)get_acpi_mcfg(control);
            break;
        }
        case ACPI_DD_IOCTL_GET_MADT: {
            odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER IOCTL GET MADT\n");
            return (struct acpi_dd_madt_header*)get_acpi_madt(control);
            break;
        }
        case ACPI_DD_IOCTL_GET_FADT: {
            odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER IOCTL GET FADT\n");
            return (struct acpi_dd_fadt_header*)get_acpi_fadt(control);
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
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER REGISTER STARTED\n");
    struct odi_driver_info * driver = odi_driver_register(ACPI_DD_MAJOR, (void*)&acpi_ops, ACPI_DD_LICENSE, ACPI_DD_VENDOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "ACPI DRIVER REGISTER FAILED\n");
        return 0;
    }

    ((struct odi_driver_functions*)driver->functions)->init(driver, 0);

    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER REGISTER FINISHED\n");

    return driver;
}

void exit_acpi(void) {
    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER UNREGISTER STARTED\n");
    struct odi_driver_info * driver = odi_driver_get(ACPI_DD_MAJOR);
    if (driver == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "ACPI DRIVER UNREGISTER FAILED\n");
        return;
    }

    ((struct odi_driver_functions*)driver->functions)->exit(driver, 0);

    if (odi_driver_unregister(ACPI_DD_MAJOR)) {
        odi_debug_append(ODI_DTAG_ERROR, "ACPI DRIVER UNREGISTER FAILED\n");
        return;
    }

    odi_debug_append(ODI_DTAG_INFO, "ACPI DRIVER UNREGISTER FINISHED\n");
}