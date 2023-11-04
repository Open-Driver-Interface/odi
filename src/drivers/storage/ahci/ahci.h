#ifndef _ODI_AHCI_H
#define _ODI_AHCI_H
#include "../../../core/types.h"

#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_ATA   0x00000101
#define SATA_SIG_SEMB  0xC33C0101
#define SATA_SIG_PM    0x96690101

#define HBA_PxCMD_CR   0x8000
#define HBA_PxCMD_FRE  0x0010
#define HBA_PxCMD_FR   0x4000
#define HBA_PxCMD_ST   0x0001

#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35
#define ATA_CMD_IDENTIFY 0xEC
#define ATA_CMD_PACKET 0xA0

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define HBA_PxIS_TFES (1 << 30)

#define ATAPI_READ_CMD 0xA8

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200


enum port_type {
    PORT_TYPE_NONE = 0,
    PORT_TYPE_SATA = 1,
    PORT_TYPE_SEMB = 2,
    PORT_TYPE_PM = 3,
    PORT_TYPE_SATAPI = 4
};

enum fis_type {
    FIS_TYPE_REG_H2D = 0x27,
    FIS_TYPE_REG_D2H = 0x34,
    FIS_TYPE_DMA_ACT = 0x39,
    FIS_TYPE_DMA_SETUP = 0x41,
    FIS_TYPE_DATA = 0x46,
    FIS_TYPE_BIST = 0x58,
    FIS_TYPE_PIO_SETUP = 0x5F,
    FIS_TYPE_DEV_BITS = 0xA1
};

struct hba_port {
    u32 command_list_base;
    u32 command_list_base_upper;
    u32 fis_base_address;
    u32 fis_base_address_upper;
    u32 interrupt_status;
    u32 interrupt_enable;
    u32 command_status;
    u32 reserved;
    u32 task_file_data;
    u32 signature;
    u32 sata_status;
    u32 sata_control;
    u32 sata_error;
    u32 sata_active;
    u32 command_issue;
    u32 sata_notification;
    u32 fis_based_switching_control;
    u32 device_sleep;
    u32 reserved2[10];
    u32 vendor[4];
} __attribute__ ((packed));

struct hba_memory {
    u32 host_capabilities;
    u32 global_host_control;
    u32 interrupt_status;
    u32 ports_implemented;
    u32 version;
    u32 ccc_control;
    u32 ccc_ports;
    u32 enclosure_management_location;
    u32 enclosure_management_control;
    u32 host_capabilities_extended;
    u32 bios_handoff_control_status;
    u8 reserved[116];
    u8 vendor_specific[96];
    struct hba_port ports[1];
} __attribute__ ((packed));

struct ahci_port {
    volatile struct hba_port* hba_port;
    struct hba_memory * abar;
    enum port_type port_type;
    u8 *buffer;
    u8 *vbuffer;
    u8 port_number;
};

struct hba_command_header {
    u8 command_fis_length : 5;
    u8 atapi : 1;
    u8 write : 1;
    u8 prefetchable : 1;

    u8 reset : 1;
    u8 bist : 1;
    u8 clear_busy_on_ok : 1;
    u8 reserved0 : 1;
    u8 port_multiplier : 4;

    u16 prdt_length;
    u32 prdb_count;
    u32 command_table_base_address;
    u32 command_table_base_address_upper;
    u32 reserved1[4];
} __attribute__ ((packed));

struct hba_command_fis {
    u8 fis_type;

    u8 pm_port : 4;
    u8 reserved0 : 3;
    u8 command_control : 1;

    u8 command;
    u8 feature_low;

    u8 lba0;
    u8 lba1;
    u8 lba2;
    u8 device_register;

    u8 lba3;
    u8 lba4;
    u8 lba5;
    u8 feature_high;

    u8 count_low;
    u8 count_high;
    u8 icc;
    u8 control;

    u8 reserved1[4];
} __attribute__ ((packed));

struct hba_prdt_entry {
    u32 data_base_address;
    u32 data_base_address_upper;
    u32 reserved0;

    u32 byte_count : 22;
    u32 reserved1 : 9;
    u32 interrupt_on_completion : 1;
} __attribute__ ((packed));

struct hba_command_table {
    u8 command_fis[64];
    u8 atapi_command[16];
    u8 reserved[48];
    struct hba_prdt_entry prdt_entry[32];
} __attribute__ ((packed));

void init_ahci(struct hba_memory* abar, struct ahci_port* ahci_ports, u8 * port_count);

u8 identify(struct ahci_port* port);

u8 read_port(struct ahci_port* port, u64 sector, u32 sector_count);
u8 write_port(struct ahci_port* port, u64 sector, u32 sector_count);

u8 write_atapi_port(struct ahci_port* port, u64 sector, u32 sector_count);
u8 read_atapi_port(struct ahci_port* port, u64 sector, u32 sector_count);

#endif