#include "config.h"

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