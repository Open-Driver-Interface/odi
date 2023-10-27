#include "autoconf.h"
#include "debug.h"
#include "device.h"
#include "driver.h"

#ifdef ODI_DRIVERS_MISC_ACPI
#include "../drivers/misc/acpi_dd.h"
#endif

#ifdef ODI_DRIVERS_BUS_PCI
#include "../drivers/bus/pci/pci_dd.h"
#endif

u8 driver_list[ODI_MAX_MAJORS/8] = {0};
u8 device_list[(ODI_MAX_MAJORS*ODI_MAX_MINORS)/8] = {0};

void odi_autoconf_driver_cleanup(u32 major) {
    odi_driver_unregister(major);
    driver_list[major/8] &= ~(1 << (major % 8));
}

void odi_autoconf_device_cleanup(u32 major, u32 minor) {
    odi_device_unregister(major, minor);
    device_list[(major*ODI_MAX_MINORS + minor)/8] &= ~(1 << ((major*ODI_MAX_MINORS + minor) % 8));
}

void odi_autoconf_cleanup() {
    for (u32 i = 0; i < ODI_MAX_MAJORS; i++) {
        if (driver_list[i/8] & (1 << (i % 8))) {
            odi_autoconf_driver_cleanup(i);
        }
    }

    for (u32 i = 0; i < ODI_MAX_MAJORS*ODI_MAX_MINORS; i++) {
        if (device_list[i/8] & (1 << (i % 8))) {
            odi_autoconf_device_cleanup(i/ODI_MAX_MINORS, i % ODI_MAX_MINORS);
        }
    }
}

void odi_autoconf_scan(void * rsdp) {
#ifndef ODI_DRIVERS_MISC_ACPI
    odi_debug_append(ODI_DTAG_INFO, "ODI_AUTOCONF_SCAN: ACPI BUS DRIVER INCLUDED\n");
    return;
#else
    struct odi_driver_info * acpi = init_acpi();
    if (acpi == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "ODI_AUTOCONF_SCAN: ACPI BUS DRIVER CANT START\n");
        return;
    }

    ((struct odi_driver_functions*)acpi->functions)->ioctl(acpi, 0x0, rsdp, ACPI_DD_IOCTL_INIT);
    odi_debug_append(ODI_DTAG_INFO, "ODI_AUTOCONF_SCAN: ACPI BUS INITIALIZED\n");

    struct acpi_dd_mcfg_header * mcfg = ((struct odi_driver_functions*)acpi->functions)->ioctl(acpi, 0x0, 0, ACPI_DD_IOCTL_GET_MCFG);
    if (mcfg == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "ODI_AUTOCONF_SCAN: ACPI BUS DRIVER CANT GET MCFG\n");
        return;
    }

#ifndef ODI_DRIVERS_BUS_PCI
    odi_debug_append(ODI_DTAG_ERROR, "ODI_AUTOCONF_SCAN: PCI BUS DRIVER NOT INCLUDED\n");
    return;
#endif
    struct odi_driver_info * pci = init_pci();
    if (pci == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "ODI_AUTOCONF_SCAN: PCI BUS DRIVER CANT START\n");
        return;
    }

    ((struct odi_driver_functions*)pci->functions)->ioctl(pci, 0x0, mcfg, PCI_IOCTL_SCAN_BUS);
    odi_debug_append(ODI_DTAG_INFO, "ODI_AUTOCONF_SCAN: PCI BUS INITIALIZED\n");

#endif

}

struct odi_device_info * odi_autoconf_get_device(u32 major, u32 minor) {
    u64 index = ((u64)major << 32) | minor;
    if (device_list[index/8] & (1 << (index % 8))) {
        return odi_device_get(major, minor);
    }
    return 0;
}

struct odi_driver_info * odi_autoconf_get_driver(u32 major) {
    if (driver_list[major/8] & (1 << (major % 8))) {
        return odi_driver_get(major);
    }
    return 0;
}