#ifndef _ACPI_DD_H
#define _ACPI_DD_H

#include "../../../core/types.h"
#include "../../../core/driver.h"

#define ACPI_DD_IOCTL_INIT 0
#define ACPI_DD_IOCTL_GET_MCFG 1
#define ACPI_DD_IOCTL_GET_FADT 2
#define ACPI_DD_IOCTL_GET_MADT 3

struct acpi_dd_mcfg_header {
    char signature[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
    u64 reserved;
} __attribute__((packed));

struct acpi_dd_madt_header {
    char signature[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
    u32 local_apic_address;
    u32 flags; //1 for dual 8259 PIC, 0 for single
} __attribute__((packed));

struct acpi_dd_generic_address_structure {
    u8 address_space;
    u8 bit_width;
    u8 bit_offset;
    u8 access_size;
    u64 address;
} __attribute__ ((packed));

struct acpi_dd_fadt_header {
    char signature[4];
    u32 length;
    u8 revision;
    u8 checksum;
    char oem_id[6];
    char oem_table_id[8];
    u32 oem_revision;
    u32 creator_id;
    u32 creator_revision;
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
    struct acpi_dd_generic_address_structure reset_reg;

    u8 reset_value;
    u8 reserved3[3];

    // 64bit pointers - Available on ACPI 2.0+
    u64 x_firmware_control;
    u64 x_dsdt;

    struct acpi_dd_generic_address_structure x_pm1a_event_block;
    struct acpi_dd_generic_address_structure x_pm1b_event_block;
    struct acpi_dd_generic_address_structure x_pm1a_control_block;
    struct acpi_dd_generic_address_structure x_pm1b_control_block;
    struct acpi_dd_generic_address_structure x_pm2_control_block;
    struct acpi_dd_generic_address_structure x_pm_timer_block;
    struct acpi_dd_generic_address_structure x_gpe0_block;
    struct acpi_dd_generic_address_structure x_gpe1_block;
} __attribute__((packed));

#define ACPI_DD_LICENSE    ODI_LICENSE_TYPE_MIT
#define ACPI_DD_VENDOR     "OPENDRIVERINTERFACE\0"
#define ACPI_DD_NAME       "ACPI DRIVER\0"
#define ACPI_DD_MAJOR      87

struct odi_driver_info * init_acpi(void);
void exit_acpi(void);
#endif