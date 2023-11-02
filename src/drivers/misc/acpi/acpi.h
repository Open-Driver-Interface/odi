#ifndef _ACPI_H
#define _ACPI_H

#include "../../../core/types.h"

#define RSDP_SIGNATURE "RSD PTR "
#define RSDP2_SIGNATURE "RSDP PTR "

#define RSDT_SIGNATURE "RSDT"
#define XSDT_SIGNATURE "XSDT"
#define MCFG_SIGNATURE "MCFG"
#define MADT_SIGNATURE "APIC"
#define FADT_SIGNATURE "FACP"

#define ADDRESS_SPACE_SYSTEM_MEMORY                     0
#define ADDRESS_SPACE_SYSTEM_IO                         1
#define ADDRESS_SPACE_PCI_CONFIGURATION_SPACE           2
#define ADDRESS_SPACE_EMBEDDED_CONTROLLER               3
#define ADDRESS_SPACE_SMBUS                             4
#define ADDRESS_SPACE_CMOS                              5
#define ADDRESS_SPACE_PCI_BAR_TARGET                    6
#define ADDRESS_SPACE_IPMI                              7
#define ADDRESS_SPACE_GENERAL_PURPOSE_IO                8
#define ADDRESS_SPACE_GENERIC_SERIAL_BUS                9
#define ADDRESS_SPACE_PLATFORM_COMMUNICATION_CHANNEL    10

#define ACCESS_SIZE_UNDEFINED                           0
#define ACCESS_SIZE_BYTE                                1
#define ACCESS_SIZE_WORD                                2
#define ACCESS_SIZE_DWORD                               3
#define ACCESS_SIZE_QWORD                               4

#define PM_PROFILE_UNSPECIFIED                          0
#define PM_PROFILE_DESKTOP                              1
#define PM_PROFILE_MOBILE                               2
#define PM_PROFILE_WORKSTATION                          3
#define PM_PROFILE_ENTERPRISE_SERVER                    4
#define PM_PROFILE_SOHO_SERVER                          5
#define PM_PROFILE_APPLIANCE_PC                         6
#define PM_PROFILE_PERFORMANCE_SERVER                   7

struct rsdp_descriptor {
    char signature[8];
    u8 checksum;
    char oem_id[6];
    u8 revision;
    u32 rsdt_address;
} __attribute__ ((packed));

struct rsdp2_descriptor {
    struct rsdp_descriptor first_part;

    u32 length;
    u64 xsdt_address;
    u8 extended_checksum;
    u8 reserved[3];
} __attribute__ ((packed));

struct acpi_sdt_header {
    char signature[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
} __attribute__ ((packed));

struct rsdt {
    struct acpi_sdt_header header;
    u32 pointer_other_sdt[];
} __attribute__ ((packed)); 

struct xsdt {
  struct acpi_sdt_header header;
  u64 pointer_other_sdt[];
} __attribute__ ((packed));

struct mcfg_header {
    struct acpi_sdt_header header;
    u64 reserved;
} __attribute__ ((packed));

struct madt_header {
    struct acpi_sdt_header header;
    u32 local_apic_address;
    u32 flags; //1 for dual 8259 PIC, 0 for single
} __attribute__ ((packed));

struct generic_address_structure {
    u8 address_space;
    u8 bit_width;
    u8 bit_offset;
    u8 access_size;
    u64 address;
} __attribute__ ((packed));

struct fadt_header {
    struct acpi_sdt_header header;
    u32 firmware_control;
    u32 dsdt;

    // field used in ACPI 1.0; no longer in use, for compatibility only
    u8 reserved;

    u8 preferred_pm_profile;
    u16 sci_interrupt;
    u32 smi_command_port;
    u8 acpi_enable;
    u8 acpi_disable;
    u8 s4bios_request;
    u8 pstate_control;
    u32 pm1a_event_block;
    u32 pm1b_event_block;
    u32 pm1a_control_block;
    u32 pm1b_control_block;
    u32 pm2_control_block;
    u32 pm_timer_block;
    u32 gpe0_block;
    u32 gpe1_block;
    u8 pm1_event_length;
    u8 pm1_control_length;
    u8 pm2_control_length;
    u8 pm_timer_length;
    u8 gpe0_length;
    u8 gpe1_length;
    u8 gpe1_base;
    u8 cstate_control;
    u16 worst_c2_latency;
    u16 worst_c3_latency;
    u16 flush_size;
    u16 flush_stride;
    u8 duty_offset;
    u8 duty_width;
    u8 day_alarm;
    u8 month_alarm;
    u8 century;

    // reserved in ACPI 1.0; used since ACPI 2.0+
    u16 boot_architecture_flags;

    u8 reserved2;
    u32 flags;

    // 12 byte structure; see below for details
    struct generic_address_structure reset_reg;

    u8 reset_value;
    u8 reserved3[3];

    // 64bit pointers - Available on ACPI 2.0+
    u64 x_firmware_control;
    u64 x_dsdt;

    struct generic_address_structure x_pm1a_event_block;
    struct generic_address_structure x_pm1b_event_block;
    struct generic_address_structure x_pm1a_control_block;
    struct generic_address_structure x_pm1b_control_block;
    struct generic_address_structure x_pm2_control_block;
    struct generic_address_structure x_pm_timer_block;
    struct generic_address_structure x_gpe0_block;
    struct generic_address_structure x_gpe1_block;
} __attribute__ ((packed));

struct mcfg_header* get_acpi_mcfg(void * rsdp_address);
struct madt_header* get_acpi_madt(void * rsdp_address);
struct fadt_header* get_acpi_fadt(void * rsdp_address);

void startup(void * rsdp_address);
#endif