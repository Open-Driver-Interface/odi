#include "config.h"

#pragma GCC diagnostic ignored "-Woverride-init"

// Default included drivers configuration
// Add your drivers here!!!

#include "drivers/test/basic/basic_dd.h"
#include "drivers/storage/ahci/ahci_dd.h"

struct odi_autoconf_pack ODI_DRIVER_AUTOCONF_PACKS[] = {
    {basic_dd_init, basic_dd_exit},
    {ahci_dd_init, ahci_dd_exit}
};
u32 ODI_DRIVER_AUTOCONF_PCI_MAJOR_ASSIGNMENTS_COUNT = 2;

struct odi_autoconf_pci_major_assignment ODI_DRIVER_AUTOCONF_PCI_MAJOR_ASSIGNMENTS[] = {
    {0x00, 0x00, 0x00, BASIC_DD_MAJOR},
    {0x01, 0x06, 0x01, AHCI_DD_MAJOR}
};
u32 ODI_DRIVER_AUTOCONF_COUNT = 2;

const char* MAJOR_PREFIXES[ODI_MAX_MAJORS] = {
    [0x0 ... ODI_MAX_MAJORS -1] = "none",
    [8] = "hd",   // 8
    [9] = "cd",   // 9
    [0xa] = "semb", // a
    [0xb] = "pm",   // b
    [0xc] = "umsd", // c
    [0xd] = "usb", // d
    [0xe] = "tty", // e
    [0xf] = "kbd", // f
    [0x10] = "mouse", // 10
    [0x11] = "net", // 11
    [86] = "pci",
    [252] = "ahci",
    [255] = "basic",
    [65535] = "unknown"
};