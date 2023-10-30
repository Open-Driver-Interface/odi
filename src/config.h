#ifndef _ODI_CONFIG_H
#define _ODI_CONFIG_H
#include "core/types.h"

//ODI Configuration file

#define ODI_VERSION_MAJOR 0
#define ODI_VERSION_MINOR 1
#define ODI_VERSION_STRING "0.1"
#define ODI_DEBUG_ENABLED 1
#define ODI_MAX_MAJORS 0x10000
#define ODI_MAX_MINORS 1024
#define ODI_AUTOCONF_UNSUPPORTED_MAJOR 0xffff


//Drivers to be compiled before autoconf can be run.
#define ODI_DRIVERS_BUS_PCI
#define ODI_DRIVERS_MISC_ACPI

//Don't touch below this, see config.c
struct odi_autoconf_pack {
    void * init;
    void * exit;
};
struct odi_autoconf_pci_major_assignment {
    u8 class;
    u8 subclass;
    u8 prog_if;
    u32 major;
};

extern struct odi_autoconf_pack ODI_DRIVER_AUTOCONF_PACKS[];
extern struct odi_autoconf_pci_major_assignment ODI_DRIVER_AUTOCONF_PCI_MAJOR_ASSIGNMENTS[];
extern u32 ODI_DRIVER_AUTOCONF_COUNT;
extern u32 ODI_DRIVER_AUTOCONF_PCI_MAJOR_ASSIGNMENTS_COUNT;

//Name prefixes for devices
extern const char* MAJOR_PREFIXES[ODI_MAX_MAJORS];

#endif
