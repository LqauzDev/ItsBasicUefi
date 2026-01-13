#ifndef ACPI_H
#define ACPI_H

#include "../Uefi.Core/include/uefi.h"

typedef struct {
    uint32_t signature;
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oem_id[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} AcpiTableHeader;

typedef struct {
    AcpiTableHeader header;
    uint8_t checksum;
    char oem_table_id[8];
    uint8_t reserved;
    uint32_t rsdt_address;
} AcpiRsdp;

typedef struct {
    AcpiTableHeader header;
    uint32_t* tables;
    uint32_t table_count;
} AcpiRsdt;

typedef struct {
    AcpiTableHeader header;
    uint64_t* tables;
    uint32_t table_count;
    uint64_t reserved;
} AcpiXsdt;

typedef struct {
    AcpiTableHeader header;
    uint32_t local_apic_address;
    uint32_t flags;
} AcpiMadt;

typedef struct {
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
} AcpiMadtProcessor;

typedef struct {
    uint8_t type;
    uint8_t length;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
} AcpiMadtEntry;

int acpi_init(void);
AcpiRsdp* acpi_find_rsdp(void);
AcpiRsdt* acpi_find_rsdt(void);
AcpiXsdt* acpi_find_xsdt(void);
void* acpi_find_table(const char* signature);
AcpiMadt* acpi_find_madt(void);
int acpi_get_processor_count(void);
int acpi_validate_table_checksum(AcpiTableHeader* table);
int acpi_get_system_info(void);

#define ACPI_SIGNATURE_RSDP 0x20445352
#define ACPI_SIGNATURE_RSDT 0x54445352
#define ACPI_SIGNATURE_XSDT 0x54445358
#define ACPI_SIGNATURE_MADT 0x43494150

#endif
