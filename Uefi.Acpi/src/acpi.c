#include "acpi.h"
#include "uefi_lib.h"
#include "../Uefi.StdLib/include/string.h"
#include "../Uefi.StdLib/include/stdlib.h"

static AcpiRsdp* g_rsdp = NULL;
static AcpiRsdt* g_rsdt = NULL;
static AcpiXsdt* g_xsdt = NULL;

int acpi_init(void) {
    g_rsdp = acpi_find_rsdp();
    if (!g_rsdp) return 0;
    
    if (g_rsdp->revision >= 2 && g_rsdp->rsdt_address > 0xFFFFFFFF) {
        g_xsdt = (AcpiXsdt*)(uint64_t)g_rsdt_address;
    } else {
        g_rsdt = (AcpiRsdt*)(uint32_t)g_rsdp->rsdt_address;
    }
    
    return 1;
}

AcpiRsdp* acpi_find_rsdp(void) {
    if (!gST) return NULL;
    
    void** config_tables = gST->ConfigurationTable;
    uintn_t num_entries = gST->NumberOfTableEntries;
    
    for (uintn_t i = 0; i < num_entries; i++) {
        EFI_GUID* guid = &config_tables[i]->VendorGuid;
        if (guid->Data1 == ACPI_SIGNATURE_RSDP) {
            return (AcpiRsdp*)config_tables[i]->VendorTable;
        }
    }
    
    uint8_t* ebda = (uint8_t*)0x00080000;
    for (size_t i = 0; i < 0x10000; i += 16) {
        if (memcmp(&ebda[i], "RSD PTR ", 8) == 0) {
            return (AcpiRsdp*)&ebda[i];
        }
    }
    
    uint8_t* bios_area = (uint8_t*)0x000F0000;
    for (size_t i = 0; i < 0x10000; i += 16) {
        if (memcmp(&bios_area[i], "RSD PTR ", 8) == 0) {
            return (AcpiRsdp*)&bios_area[i];
        }
    }
    
    return NULL;
}

AcpiRsdt* acpi_find_rsdt(void) {
    if (!g_rsdp) {
        g_rsdp = acpi_find_rsdp();
        if (!g_rsdp) return NULL;
    }
    
    if (g_rsdt) return g_rsdt;
    
    g_rsdt = (AcpiRsdt*)(uint32_t)g_rsdp->rsdt_address;
    return g_rsdt;
}

AcpiXsdt* acpi_find_xsdt(void) {
    if (!g_rsdp) {
        g_rsdp = acpi_find_rsdp();
        if (!g_rsdp) return NULL;
    }
    
    if (g_xsdt) return g_xsdt;
    
    if (g_rsdp->revision >= 2) {
        g_xsdt = (AcpiXsdt*)g_rsdp->rsdt_address;
    }
    
    return g_xsdt;
}

void* acpi_find_table(const char* signature) {
    if (!signature || strlen(signature) != 4) return NULL;
    
    AcpiXsdt* xsdt = acpi_find_xsdt();
    if (xsdt) {
        uint32_t table_count = (xsdt->header.length - sizeof(AcpiTableHeader)) / 8;
        for (uint32_t i = 0; i < table_count; i++) {
            AcpiTableHeader* table = (AcpiTableHeader*)xsdt->tables[i];
            if (memcmp(&table->signature, signature, 4) == 0) {
                return table;
            }
        }
    }
    
    AcpiRsdt* rsdt = acpi_find_rsdt();
    if (rsdt) {
        uint32_t table_count = (rsdt->header.length - sizeof(AcpiTableHeader)) / 4;
        for (uint32_t i = 0; i < table_count; i++) {
            AcpiTableHeader* table = (AcpiTableHeader*)(uint32_t)rsdt->tables[i];
            if (memcmp(&table->signature, signature, 4) == 0) {
                return table;
            }
        }
    }
    
    return NULL;
}

AcpiMadt* acpi_find_madt(void) {
    return (AcpiMadt*)acpi_find_table("APIC");
}

int acpi_get_processor_count(void) {
    AcpiMadt* madt = acpi_find_madt();
    if (!madt) return 0;
    
    uint8_t* entry = (uint8_t*)madt + sizeof(AcpiMadt);
    uint8_t* end = (uint8_t*)madt + madt->header.length;
    
    int count = 0;
    while (entry < end) {
        AcpiMadtEntry* madt_entry = (AcpiMadtEntry*)entry;
        if (madt_entry->type == 0) {
            count++;
        }
        entry += madt_entry->length;
    }
    
    return count;
}

int acpi_validate_table_checksum(AcpiTableHeader* table) {
    if (!table) return 0;
    
    uint8_t sum = 0;
    uint8_t* bytes = (uint8_t*)table;
    for (uint32_t i = 0; i < table->length; i++) {
        sum += bytes[i];
    }
    
    return sum == 0;
}

int acpi_get_system_info(void) {
    if (!acpi_init()) return 0;
    
    printf("ACPI System Information:\r\n");
    printf("RSDP Revision: %d\r\n", g_rsdp ? g_rsdp->revision : 0);
    
    AcpiMadt* madt = acpi_find_madt();
    if (madt) {
        printf("Local APIC Address: 0x%x\r\n", madt->local_apic_address);
        printf("Processor Count: %d\r\n", acpi_get_processor_count());
    }
    
    return 1;
}
