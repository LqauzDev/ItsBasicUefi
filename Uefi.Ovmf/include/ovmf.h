#ifndef OVMF_H
#define OVMF_H

#include "../Uefi.Core/include/uefi.h"

typedef struct {
    uint8_t* firmware_data;
    size_t firmware_size;
    char version[64];
    char vendor[64];
    uint32_t capabilities;
} OvmfInfo;

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
} OvmfTableHeader;

int ovmf_init(OvmfInfo* info);
int ovmf_detect_environment(void);
int ovmf_get_firmware_info(OvmfInfo* info);
void* ovmf_find_table(const char* signature);
int ovmf_is_qemu_environment(void);
int ovmf_is_virtualbox_environment(void);
int ovmf_is_vmware_environment(void);
int ovmf_get_memory_map(void** memory_map, size_t* map_size);
int ovmf_reset_system(void);
int ovmf_shutdown_system(void);

#define OVMF_CAPABILITY_GRAPHICS 0x01
#define OVMF_CAPABILITY_NETWORK   0x02
#define OVMF_CAPABILITY_USB       0x04
#define OVMF_CAPABILITY_ACPI      0x08
#define OVMF_CAPABILITY_SMBIOS    0x10

#endif
