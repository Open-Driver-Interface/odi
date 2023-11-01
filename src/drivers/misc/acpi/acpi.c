#include "acpi.h"
#include "../../deps.h"

u8 acpi_sdt_checksum(struct acpi_sdt_header* table_header)
{
    u8 sum = 0;
 
    for (u32 i = 0; i < table_header->length; i++)
    {
        sum += ((u8 *) table_header)[i];
    }
 
    return (sum == 0);
}

struct acpi_sdt_header* find_rsdt(struct rsdt* rsdt, const char* signature, u64 sig_len) {
    u32 entries = (rsdt->header.length - sizeof(struct acpi_sdt_header)) / sizeof(u32);

    for (u32 i = 0; i < entries; i++) {
        u32 pto = rsdt->pointer_other_sdt[i];
        struct acpi_sdt_header* header = (struct acpi_sdt_header*)(u64)pto; //TODO: delete this inmediately

        if (!odi_dep_memcmp(header->signature, signature, sig_len))
            return header;
    }

    return 0;
}

struct acpi_sdt_header* find_xsdt(struct xsdt* xsdt, const char* signature, u64 sig_len) {
    u32 entries = (xsdt->header.length - sizeof(struct acpi_sdt_header)) / sizeof(u64);

    for (u32 i = 0; i < entries; i++) {
        u32 pto = xsdt->pointer_other_sdt[i];
        struct acpi_sdt_header* header = (struct acpi_sdt_header*)(u64)pto; //TODO: delete this inmediately

        if (!odi_dep_memcmp(header->signature, signature, sig_len))
            return header;
    }

    return 0;
}

void* init_acpi_vt(void* rsdp_address, const char* target) {
    char signature[9];
    char oemid[7];

    //printf("RSDP revision 2\n");

    struct rsdp2_descriptor* rsdp = (struct rsdp2_descriptor*)rsdp_address;

    if (odi_dep_memcmp(rsdp->first_part.signature, "RSD PTR ", 8))
        return 0;

    odi_dep_strncpy(oemid, rsdp->first_part.oem_id, 6);
    odi_dep_strncpy(signature, rsdp->first_part.signature, 8);

    /*   
    printf("RSDPv2 at: 0x%llx Sig: %.4s Check: %d OEM: %s Rev: %d RsdtAddr: 0x%llx Len: 0x%llx XsdtAddr: 0x%llx ExtCheck: %d Res: %s\n",
        rsdp, signature, rsdp->first_part.checksum, oemid, rsdp->first_part.revision, rsdp->first_part.rsdt_address, rsdp->length,
        rsdp->xsdt_address, rsdp->extended_checksum, rsdp->reserved
    );
    */
    struct xsdt* xsdt = (struct xsdt*)(u64)(rsdp->xsdt_address);
    struct acpi_sdt_header* result = find_xsdt(xsdt, target, 4);

    if (!acpi_sdt_checksum(result))
        return 0;

    return (void*)result;
}

void* init_acpi_vz(void* rsdp_address, const char * target) {
    char signature[9];
    char oemid[7];

    //printf("RSDP revision 0\n");

    struct rsdp_descriptor* rsdp = (struct rsdp_descriptor*)rsdp_address;

    if (odi_dep_memcmp(rsdp->signature, "RSD PTR ", 8))
        return 0;

    odi_dep_strncpy(oemid, rsdp->oem_id, 6);
    odi_dep_strncpy(signature, rsdp->signature, 8);

    /*   
    printf("RSDP at: 0x%llx Sig: %.4s Check: %d OEM: %s Rev: %d RsdtAddr: 0x%llx\n",
        rsdp, signature, rsdp->checksum, oemid, rsdp->revision, rsdp->rsdt_address
    );
    */

    struct rsdt* rsdt = (struct rsdt*)(u64)(rsdp->rsdt_address);
    struct acpi_sdt_header* result = find_rsdt(rsdt, target, 4);

    if (!acpi_sdt_checksum(result))
        return 0;

    return (void*)result;
}

struct mcfg_header* get_acpi_mcfg(void * rsdp_address) {
    char signature[9];
    char oemid[7];
    char oemtableid[9];

    if (rsdp_address == 0) return 0;

    struct rsdp_descriptor* prev_rsdp = (struct rsdp_descriptor*)rsdp_address;
    struct mcfg_header* mcfg_header = 0x0;

    if (prev_rsdp->revision == 0) {
        mcfg_header = (struct mcfg_header*)init_acpi_vz(rsdp_address, "MCFG");
    } else if (prev_rsdp->revision == 2) {
        mcfg_header = (struct mcfg_header*)init_acpi_vt(rsdp_address, "MCFG");
    } else {
        return 0;
    }

    if (mcfg_header == 0) return 0;

    odi_dep_strncpy(signature, mcfg_header->header.signature, 4);
    odi_dep_strncpy(oemtableid, mcfg_header->header.oem_table_id, 8);
    odi_dep_strncpy(oemid, mcfg_header->header.oem_id, 6);
    return mcfg_header;
}

struct madt_header* get_acpi_madt(void * rsdp_address) {
    char signature[9];
    char oemid[7];
    char oemtableid[9];

    if (rsdp_address == 0) return 0;

    struct rsdp_descriptor* prev_rsdp = (struct rsdp_descriptor*)rsdp_address;
    struct madt_header* madt_header = 0x0;

    if (prev_rsdp->revision == 0) {
        madt_header = (struct madt_header*)init_acpi_vz(rsdp_address, "APIC");
    } else if (prev_rsdp->revision == 2) {
        madt_header = (struct madt_header*)init_acpi_vt(rsdp_address, "APIC");
    } else {
        return 0;
    }   

    if (madt_header == 0) return 0;

    odi_dep_strncpy(signature, madt_header->header.signature, 4);
    odi_dep_strncpy(oemtableid, madt_header->header.oem_table_id, 8);
    odi_dep_strncpy(oemid, madt_header->header.oem_id, 6);

    return madt_header;
}

struct fadt_header* get_acpi_fadt(void * rsdp_address) {
    char signature[9];
    char oemid[7];
    char oemtableid[9];

    if (rsdp_address == 0) return 0;

    struct rsdp_descriptor* prev_rsdp = (struct rsdp_descriptor*)rsdp_address;
    struct fadt_header* fadt_header = 0x0;

    if (prev_rsdp->revision == 0) {
        fadt_header = (struct fadt_header*)init_acpi_vz(rsdp_address, "FACP");
    } else if (prev_rsdp->revision == 2) {
        fadt_header = (struct fadt_header*)init_acpi_vt(rsdp_address, "FACP");
    } else {
        return 0;
    }   

    if (fadt_header == 0) return 0;

    odi_dep_strncpy(signature, fadt_header->header.signature, 4);
    odi_dep_strncpy(oemtableid, fadt_header->header.oem_table_id, 8);
    odi_dep_strncpy(oemid, fadt_header->header.oem_id, 6);

    return fadt_header;
}

u8 is_enabled(struct fadt_header* fadt_header) {
    //Check if ACPI is enabled
    if (fadt_header->smi_command_port != 0) return 0;
    if (fadt_header->acpi_enable != 0 || fadt_header->acpi_disable != 0) return 0;
    if (!(fadt_header->pm1a_event_block & 1)) return 0;

    return 1;
}

void startup(void * rsdp_address) {

    if (rsdp_address == 0) return;

    struct fadt_header* fadt_header = get_acpi_fadt(rsdp_address);
    if (fadt_header == 0) return;
    if (is_enabled(fadt_header)) return;
    if (odi_dep_inw(fadt_header->pm1a_control_block) & 1) return;
    odi_dep_outb(fadt_header->smi_command_port, fadt_header->acpi_enable);
    while (((odi_dep_inw(fadt_header->pm1a_control_block) & 1) == 0));
}