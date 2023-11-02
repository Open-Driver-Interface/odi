#include "ahci.h"
#include "../../../deps.h"
#include "../../../core/debug.h"

s8 find_cmd_slot(struct ahci_port* port) {
    u32 slots = (port->hba_port->sata_active | port->hba_port->command_issue);
    int cmd_slots = (port->abar->host_capabilities & 0x0f00) >> 8;
    for (int i = 0; i < cmd_slots; i++) {
        if ((slots & 1) == 0) {
            return i;
        }
        slots >>= 1;
    }

    odi_debug_append(ODI_DTAG_ERROR, "Cannot find free command list entry\n");
    return -1;
}

u8 identify(struct ahci_port* port) {
    if ((u64)port == 0) return 0;
    port->hba_port->interrupt_status = (u32)-1;

    int spin = 0;
    int slot = (int)find_cmd_slot(port);
    if (slot == -1) {
        odi_debug_append(ODI_DTAG_ERROR, "No free command slots\n");
        return 0;
    }

    struct hba_command_header * command_header = (struct hba_command_header*)odi_dep_get_virtual_address((void*)(u64)(port->hba_port->command_list_base));
    command_header += slot;
    command_header->command_fis_length = 5;
    command_header->write = 0;
    command_header->prdt_length = 1;
    command_header->prefetchable = 1;
    command_header->clear_busy_on_ok = 1;

    void* buffer = port->buffer;

    struct hba_command_table* command_table = (struct hba_command_table*)(u64)(command_header->command_table_base_address);
    odi_dep_memset(command_table, 0, sizeof(struct hba_command_table) + (command_header->prdt_length - 1) * sizeof(struct hba_prdt_entry));
    command_table->prdt_entry[0].data_base_address_upper = (u32)((u64)buffer >> 32);
    command_table->prdt_entry[0].data_base_address = (u32)((u64)buffer);
    command_table->prdt_entry[0].byte_count = 512 - 1;
    command_table->prdt_entry[0].interrupt_on_completion = 1;

    struct hba_command_fis* command_fis = (struct hba_command_fis*)command_table->command_fis;
    odi_dep_memset(command_fis, 0, sizeof(struct hba_command_fis));
    command_fis->fis_type = FIS_TYPE_REG_H2D;
    command_fis->command_control = 1;
    command_fis->command = ATA_CMD_IDENTIFY;

    while (port->hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ) && spin < 1000000) {
        spin++;
    };
    if (spin == 1000000) {
        odi_debug_append(ODI_DTAG_ERROR, "Port is hung\n");
        return 0;
    }

    port->hba_port->command_issue = 1;

    while(1) {
        if ((port->hba_port->command_issue & (1<<slot)) == 0) break;
        if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
            return 0;
        }
    }

    if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
        return 0;
    }

    return 1;
}

u8 write_atapi_port(struct ahci_port* port, u64 sector, u32 sector_count) {
    (void)port;
    (void)sector;
    (void)sector_count;
    odi_debug_append(ODI_DTAG_ERROR, "write_atapi_port not implemented");
    return 0;
}

u8 read_atapi_port(struct ahci_port* port, u64 sector, u32 sector_count) {
    //odi_debug_append(ODI_DTAG_ERROR, "Atapi read issued (port %d, sector %d, sector_count %d)\n", port_no, sector, sector_count);

    void* buffer = port->buffer;

    port->hba_port->interrupt_status = (u32)-1;
    int spin = 0;
    int slot = (int)find_cmd_slot(port);
    if (slot == -1) {
        odi_debug_append(ODI_DTAG_ERROR, "No free command slots\n");
        return 0;
    }

    struct hba_command_header* command_header = (struct hba_command_header*)odi_dep_get_virtual_address((void*)(u64)(port->hba_port->command_list_base));
    command_header += slot;

    command_header->command_fis_length = sizeof(struct hba_command_fis) / sizeof(u32);
    command_header->write = 0;
    command_header->atapi = 1;
    command_header->prdt_length = 1;
    
    struct hba_command_table* command_table = (struct hba_command_table*)(u64)(command_header->command_table_base_address);
    odi_dep_memset(command_table, 0, sizeof(struct hba_command_table) + (command_header->prdt_length - 1) * sizeof(struct hba_prdt_entry));

    command_table->prdt_entry[0].data_base_address = (u32)(u64)buffer;
    command_table->prdt_entry[0].data_base_address_upper = (u32)((u64)buffer >> 32);
    command_table->prdt_entry[0].byte_count = (sector_count << 9) - 1;
    command_table->prdt_entry[0].interrupt_on_completion = 1;

    struct hba_command_fis* command_fis = (struct hba_command_fis*)command_table->command_fis;
    odi_dep_memset(command_fis, 0, sizeof(struct hba_command_fis));

    command_fis->fis_type = FIS_TYPE_REG_H2D;
    command_fis->command_control = 1;
    command_fis->feature_low = 5;
    command_fis->command = ATA_CMD_PACKET;

    command_table->atapi_command[0] = ATAPI_READ_CMD;
    command_table->atapi_command[1] = 0;
    command_table->atapi_command[2] = (u8)((sector >> 24)& 0xff);
    command_table->atapi_command[3] = (u8)((sector >> 16)& 0xff);
    command_table->atapi_command[4] = (u8)((sector >> 8)& 0xff);
    command_table->atapi_command[5] = (u8)((sector >> 0)& 0xff);
    command_table->atapi_command[6] = 0;
    command_table->atapi_command[7] = 0;
    command_table->atapi_command[8] = 0;
    command_table->atapi_command[9] = (u8)(sector_count & 0xff);
    command_table->atapi_command[10] = 0;
    command_table->atapi_command[11] = 0;
    command_table->atapi_command[12] = 0;
    command_table->atapi_command[13] = 0;
    command_table->atapi_command[14] = 0;
    command_table->atapi_command[15] = 0;

    while (port->hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ) && spin < 1000000) {
        spin++;
    };
    if (spin == 1000000) {
        odi_debug_append(ODI_DTAG_ERROR, "Port is hung\n");
        return 0;
    }

    port->hba_port->command_issue = (1 << slot);

    while(1) {
        if ((port->hba_port->command_issue & (1<<slot)) == 0) break;
        if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
            return 0;
        }
    }

    if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
        return 0;
    }

    return 1;
}

u8 read_port(struct ahci_port* port, u64 sector, u32 sector_count) {

    u32 sector_low = (u32)sector;
    u32 sector_high = (u32)(sector >> 32);

    port->hba_port->interrupt_status = (u32)-1;
    int spin = 0;
    int slot = (int)find_cmd_slot(port);
    if (slot == -1) {
        odi_debug_append(ODI_DTAG_ERROR, "No free command slots\n");
        return 0;
    }

    struct hba_command_header* command_header = (struct hba_command_header*)odi_dep_get_virtual_address((void*)(u64)(port->hba_port->command_list_base));
    command_header += slot;
    command_header->command_fis_length = sizeof(struct hba_command_fis) / sizeof(u32);
    command_header->write = 0;
    command_header->prdt_length = (u16)((sector_count - 1) >> 4) + 1;

    struct hba_command_table* command_table = (struct hba_command_table*)(u64)(command_header->command_table_base_address);
    odi_dep_memset(command_table, 0, sizeof(struct hba_command_table) + (command_header->prdt_length - 1) * sizeof(struct hba_prdt_entry));
    void* buffer = port->buffer;
    int i;
    for (i = 0; i < command_header->prdt_length - 1; i++) {
        command_table->prdt_entry[i].data_base_address = (u32)(u64)buffer;
        command_table->prdt_entry[i].data_base_address_upper = (u32)((u64)buffer >> 32);
        command_table->prdt_entry[i].byte_count = 8 * 1024 - 1;
        command_table->prdt_entry[i].interrupt_on_completion = 1;
        buffer = (void*)((u64*)buffer+0x1000);
        sector_count -= 16;
    }

    command_table->prdt_entry[i].data_base_address = (u32)(u64)buffer;
    command_table->prdt_entry[i].data_base_address_upper = (u32)((u64)buffer >> 32);
    command_table->prdt_entry[i].byte_count = (sector_count << 9) - 1;
    command_table->prdt_entry[i].interrupt_on_completion = 1;

    struct hba_command_fis* command_fis = (struct hba_command_fis*)command_table->command_fis;

    command_fis->fis_type = FIS_TYPE_REG_H2D;
    command_fis->command_control = 1;
    command_fis->command = ATA_CMD_READ_DMA_EX;

    command_fis->lba0 = (u8)sector_low;
    command_fis->lba1 = (u8)(sector_low >> 8);
    command_fis->lba2 = (u8)(sector_low >> 16);

    command_fis->device_register = 1 << 6;

    command_fis->lba3 = (u8)(sector_low >> 24);
    command_fis->lba4 = (u8)(sector_high);
    command_fis->lba5 = (u8)(sector_high >> 8);

    command_fis->count_low = sector_count & 0xFF;
    command_fis->count_high = (sector_count >> 8);

    while (port->hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ) && spin < 1000000) {
        spin++;
    };
    if (spin == 1000000) {
        odi_debug_append(ODI_DTAG_ERROR, "Port is hung\n");
        return 0;
    }

    port->hba_port->command_issue = (1 << slot);

    while(1) {
        if ((port->hba_port->command_issue & (1<<slot)) == 0) break;
        if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
            return 0;
        }
    }

    if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
        return 0;
    }

    return 1;
}

u8 write_port(struct ahci_port* port, u64 sector, u32 sector_count) {

    u32 sector_low = (u32)sector;
    u32 sector_high = (u32)(sector >> 32);

    port->hba_port->interrupt_status = (u32)-1;
    int spin = 0;
    int slot = (int)find_cmd_slot(port);
    if (slot == -1) {
        odi_debug_append(ODI_DTAG_ERROR, "No free command slots\n");
        return 0;
    }

    struct hba_command_header* command_header = (struct hba_command_header*)odi_dep_get_virtual_address((void*)(u64)(port->hba_port->command_list_base));
    command_header += slot;
    command_header->command_fis_length = sizeof(struct hba_command_fis) / sizeof(u32);
    command_header->write = 1;
    command_header->prdt_length = (u16)((sector_count - 1) >> 4) + 1;

    struct hba_command_table* command_table = (struct hba_command_table*)(u64)(command_header->command_table_base_address);
    odi_dep_memset(command_table, 0, sizeof(struct hba_command_table) + (command_header->prdt_length - 1) * sizeof(struct hba_prdt_entry));
    void* buffer = port->buffer;
    int i;
    for (i = 0; i < command_header->prdt_length - 1; i++) {
        command_table->prdt_entry[i].data_base_address = (u32)(u64)buffer;
        command_table->prdt_entry[i].data_base_address_upper = (u32)((u64)buffer >> 32);
        command_table->prdt_entry[i].byte_count = 8 * 1024 - 1;
        command_table->prdt_entry[i].interrupt_on_completion = 1;
        buffer = (void*)((u64*)buffer+0x1000);
        sector_count -= 16;
    }

    command_table->prdt_entry[i].data_base_address = (u32)(u64)buffer;
    command_table->prdt_entry[i].data_base_address_upper = (u32)((u64)buffer >> 32);
    command_table->prdt_entry[i].byte_count = (sector_count << 9) - 1;
    command_table->prdt_entry[i].interrupt_on_completion = 1;

    struct hba_command_fis* command_fis = (struct hba_command_fis*)command_table->command_fis;

    command_fis->fis_type = FIS_TYPE_REG_H2D;
    command_fis->command_control = 1;
    command_fis->command = ATA_CMD_WRITE_DMA_EX;

    command_fis->lba0 = (u8)sector_low;
    command_fis->lba1 = (u8)(sector_low >> 8);
    command_fis->lba2 = (u8)(sector_low >> 16);

    command_fis->device_register = 1 << 6;

    command_fis->lba3 = (u8)(sector_low >> 24);
    command_fis->lba4 = (u8)(sector_high);
    command_fis->lba5 = (u8)(sector_high >> 8);

    command_fis->count_low = sector_count & 0xFF;
    command_fis->count_high = (sector_count >> 8);

    while (port->hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ) && spin < 1000000) {
        spin++;
    };
    if (spin == 1000000) {
        odi_debug_append(ODI_DTAG_ERROR, "Port is hung\n");
        return 0;
    }

    port->hba_port->command_issue = (1 << slot);

    while(1) {
        if ((port->hba_port->command_issue & (1<<slot)) == 0) break;
        if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
            return 0;
        }
    }

    if (port->hba_port->interrupt_status & HBA_PxIS_TFES) {
        return 0;
    }

    return 1;
}

void port_stop_command(struct ahci_port* port) {
    port->hba_port->command_status &= ~HBA_PxCMD_ST;
    port->hba_port->command_status &= ~HBA_PxCMD_FRE;

    while (1) {
        if (port->hba_port->command_status & HBA_PxCMD_FR) continue;
        if (port->hba_port->command_status & HBA_PxCMD_CR) continue;
        break;
    }
}

void port_start_command(struct ahci_port* port) {
    while (port->hba_port->command_status & HBA_PxCMD_CR);
    port->hba_port->command_status |= HBA_PxCMD_FRE;
    port->hba_port->command_status |= HBA_PxCMD_ST;
}

void configure_port(struct ahci_port* port) {
    port_stop_command(port);

    void * new_base = odi_dep_request_current_page();
    port->hba_port->command_list_base = (u32)(u64)new_base;
    port->hba_port->command_list_base_upper = (u32)((u64)new_base >> 32);
    odi_dep_memset(odi_dep_get_virtual_address(new_base), 0, 1024);

    void * new_fis_base = odi_dep_request_current_page();
    port->hba_port->fis_base_address = (u32)(u64)new_fis_base;
    port->hba_port->fis_base_address_upper = (u32)((u64)new_fis_base >> 32);
    odi_dep_memset(odi_dep_get_virtual_address(new_fis_base), 0, 256);

    struct hba_command_header* command_header = (struct hba_command_header*)odi_dep_get_virtual_address((void*)((u64)port->hba_port->command_list_base + ((u64) port->hba_port->command_list_base_upper << 32)));

    for (int i = 0; i < 32; i++) {
        command_header[i].prdt_length = 8;

        void * cmd_table_address = odi_dep_request_current_page();
        u64 address = (u64)cmd_table_address + (i << 8);
        command_header[i].command_table_base_address = (u32)(u64)address;
        command_header[i].command_table_base_address_upper = (u32)((u64)address >> 32);
        odi_dep_memset(odi_dep_get_virtual_address(cmd_table_address), 0, 256);
    }

    port_start_command(port);
}

enum port_type check_port_type(struct hba_port* port) {
    u32 stata_status  = port->sata_status;
    u8 interface_power_management = (stata_status >> 8) & 0x7;
    u8 device_detection = stata_status & 0x7;

    if (device_detection != 0x03) return PORT_TYPE_NONE;
    if (interface_power_management != 0x01) return PORT_TYPE_NONE;

    switch(port->signature) {
        case SATA_SIG_ATAPI:
            return PORT_TYPE_SATAPI;
        case SATA_SIG_SEMB:
            return PORT_TYPE_SEMB;
        case SATA_SIG_PM:
            return PORT_TYPE_PM;
        case SATA_SIG_ATA:
            return PORT_TYPE_SATA;
        default:
            odi_debug_append(ODI_DTAG_ERROR, "Unknown port type: 0x%x\n", port->signature);
            return PORT_TYPE_NONE;
    }
}

void probe_ports(struct hba_memory* abar, struct ahci_port* ahci_ports, u8 * port_count) {
    u32 ports_implemented = abar->ports_implemented;

    for (int i = 0; i < 32; i++) { 
        if (ports_implemented & (1 << i)) {
            enum port_type port_type = check_port_type(&abar->ports[i]);
            if (port_type == PORT_TYPE_SATA || port_type == PORT_TYPE_SATAPI) {
                ahci_ports[*port_count].port_type = port_type;
                ahci_ports[*port_count].hba_port = &abar->ports[i];
                ahci_ports[*port_count].port_number = *port_count;
                ahci_ports[*port_count].abar = abar;
                (*port_count)++;
            }
        }
    }
}

void init_ahci(struct hba_memory* abar, struct ahci_port* ahci_ports, u8 * port_count) {
    odi_dep_map_current_memory(abar, abar);
    odi_dep_mprotect_current((void*)abar, 4096, ODI_DEP_MPROTECT_PAGE_CACHE_DISABLE | ODI_DEP_MPROTECT_PAGE_WRITE_BIT | ODI_DEP_MPROTECT_PAGE_USER_BIT | ODI_DEP_MPROTECT_PAGE_NX_BIT);

    probe_ports(abar, ahci_ports, port_count);

    for (int i = 0; i < *port_count; i++) {
        struct ahci_port* port = &ahci_ports[i];
        configure_port(port);
        port->buffer = (u8*)odi_dep_request_current_page();
        odi_dep_memset(odi_dep_get_virtual_address(port->buffer), 0, 4096);
    }

}