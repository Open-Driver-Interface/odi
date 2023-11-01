#include "autoconf.h"
#include "debug.h"
#include "device.h"
#include "driver.h"

#ifdef ODI_DRIVERS_MISC_ACPI
#include "../drivers/misc/acpi_dd.h"
#endif

#ifdef ODI_AUTOCONF_DEPS_BUS_PCI
#include "../drivers/bus/pci/pci_dd.h"
#endif

void odi_autoconf_autoinit() {
    for (int i = 0; i < ODI_MAX_MAJORS; i++) {
        if (i == ODI_AUTOCONF_UNSUPPORTED_MAJOR) continue;
        struct odi_driver_info * driver = odi_driver_get(i);
        if (driver == 0) continue;

        struct odi_device_info * queue = odi_device_getall(i);
        while (queue != 0) {
            if (queue->init == 0) {
                driver->functions->init(driver, 0, queue->control);
                queue->init = 1;
            }
            queue = odi_device_next(queue);
        }
    }
}

void odi_autoconf_pci_scan_callback(struct pci_dd_device_header* device, u32 base_address) {
    for (u32 i = 0; i < ODI_DRIVER_AUTOCONF_PCI_MAJOR_ASSIGNMENTS_COUNT; i++) {
        struct odi_autoconf_pci_major_assignment * assignment = &ODI_DRIVER_AUTOCONF_PCI_MAJOR_ASSIGNMENTS[i];
        if (assignment->class == device->class_code && assignment->subclass == device->subclass && assignment->prog_if == device->prog_if) {
            odi_device_register(assignment->major, (void*)device, (void*)(u64)base_address);
            return;
        }
    }
    odi_device_register(ODI_AUTOCONF_UNSUPPORTED_MAJOR, (void*)device, (void*)(u64)base_address);
}

void odi_autoconf_scan_pci(void * rsdp) {
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

    struct acpi_dd_mcfg_header * mcfg = ((struct odi_driver_functions*)acpi->functions)->ioctl(acpi, 0x0, rsdp, ACPI_DD_IOCTL_GET_MCFG);
    if (mcfg == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "ODI_AUTOCONF_SCAN: ACPI BUS DRIVER CANT GET MCFG\n");
        return;
    }

#ifndef ODI_AUTOCONF_DEPS_BUS_PCI
    odi_debug_append(ODI_DTAG_ERROR, "ODI_AUTOCONF_SCAN: PCI BUS DRIVER NOT INCLUDED\n");
    return;
#endif
    struct odi_driver_info * pci = init_pci();
    if (pci == 0) {
        odi_debug_append(ODI_DTAG_ERROR, "ODI_AUTOCONF_SCAN: PCI BUS DRIVER CANT START\n");
        return;
    }

    struct pci_ioctl_scan_bus_control scan_control = {
        .trail = (void*)((u64)mcfg + sizeof(struct acpi_dd_mcfg_header)),
        .devconf_size = (mcfg->length - sizeof(struct acpi_dd_mcfg_header))
    };

    ((struct odi_driver_functions*)pci->functions)->ioctl(pci, (void*)odi_autoconf_pci_scan_callback, &scan_control, PCI_IOCTL_SCAN_BUS);

#endif

}

void odi_autoconf_scan_drivers() {
    for (u32 i = 0; i < ODI_DRIVER_AUTOCONF_COUNT; i++) {
        struct odi_autoconf_pack * pack = &ODI_DRIVER_AUTOCONF_PACKS[i];
        if (pack->init != 0) {
            void (*init)(void) = (void (*)(void))pack->init;
            init();
        }
    }
}

void odi_autoconf_scan(void * rsdp) {
    odi_autoconf_scan_drivers();
    odi_autoconf_scan_pci(rsdp);
    odi_autoconf_autoinit();
}

struct odi_device_info * odi_autoconf_get_device(u32 major, u32 minor) {
    return odi_device_get(major, minor);
}

struct odi_driver_info * odi_autoconf_get_driver(u32 major) {
    return odi_driver_get(major);
}