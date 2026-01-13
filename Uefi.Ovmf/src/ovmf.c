#include "ovmf.h"
#include "uefi_lib.h"
#include "../Uefi.StdLib/include/string.h"
#include "../Uefi.StdLib/include/stdlib.h"

static OvmfInfo g_ovmf_info = {0};

int ovmf_init(OvmfInfo* info) {
    if (!info) return 0;
    
    if (!ovmf_detect_environment()) {
        return 0;
    }
    
    return ovmf_get_firmware_info(info);
}

int ovmf_detect_environment(void) {
    if (!gST || !gST->FirmwareVendor) return 0;
    
    char16_t* vendor = gST->FirmwareVendor;
    char vendor_str[64];
    
    for (int i = 0; i < 63 && vendor[i]; i++) {
        vendor_str[i] = (char)vendor[i];
        vendor_str[i + 1] = '\0';
    }
    
    if (strstr(vendor_str, "EDK") || strstr(vendor_str, "OVMF") || 
        strstr(vendor_str, "Tianocore") || strstr(vendor_str, "QEMU")) {
        return 1;
    }
    
    return 0;
}

int ovmf_get_firmware_info(OvmfInfo* info) {
    if (!info || !gST) return 0;
    
    memset(info, 0, sizeof(OvmfInfo));
    
    if (gST->FirmwareVendor) {
        char16_t* vendor = gST->FirmwareVendor;
        for (int i = 0; i < 63 && vendor[i]; i++) {
            info->vendor[i] = (char)vendor[i];
            info->vendor[i + 1] = '\0';
        }
    }
    
    snprintf(info->version, sizeof(info->version), "%d.%d", 
             gST->FirmwareRevision >> 16, gST->FirmwareRevision & 0xFFFF);
    
    info->capabilities = 0;
    
    if (gST->ConOut) {
        info->capabilities |= OVMF_CAPABILITY_GRAPHICS;
    }
    
    info->capabilities |= OVMF_CAPABILITY_ACPI;
    info->capabilities |= OVMF_CAPABILITY_SMBIOS;
    
    g_ovmf_info = *info;
    return 1;
}

void* ovmf_find_table(const char* signature) {
    if (!signature || strlen(signature) != 4) return NULL;
    
    void** config_tables = gST->ConfigurationTable;
    uintn_t num_entries = gST->NumberOfTableEntries;
    
    for (uintn_t i = 0; i < num_entries; i++) {
        char* table_sig = (char*)&config_tables[i]->VendorGuid;
        if (memcmp(table_sig, signature, 4) == 0) {
            return config_tables[i]->VendorTable;
        }
    }
    
    return NULL;
}

int ovmf_is_qemu_environment(void) {
    if (!g_ovmf_info.vendor[0]) {
        ovmf_get_firmware_info(&g_ovmf_info);
    }
    
    return strstr(g_ovmf_info.vendor, "QEMU") != NULL;
}

int ovmf_is_virtualbox_environment(void) {
    if (!g_ovmf_info.vendor[0]) {
        ovmf_get_firmware_info(&g_ovmf_info);
    }
    
    return strstr(g_ovmf_info.vendor, "VirtualBox") != NULL;
}

int ovmf_is_vmware_environment(void) {
    if (!g_ovmf_info.vendor[0]) {
        ovmf_get_firmware_info(&g_ovmf_info);
    }
    
    return strstr(g_ovmf_info.vendor, "VMware") != NULL;
}

int ovmf_get_memory_map(void** memory_map, size_t* map_size) {
    if (!memory_map || !map_size || !gBS) return 0;
    
    size_t size = 0;
    EFI_STATUS status = gBS->GetMemoryMap(&size, NULL, NULL, NULL, NULL);
    
    if (status != EFI_BUFFER_TOO_SMALL) return 0;
    
    void* buffer = uefi_allocate_pool(EfiLoaderData, size);
    if (!buffer) return 0;
    
    uintn_t map_key, descriptor_size;
    uint32_t descriptor_version;
    
    status = gBS->GetMemoryMap(&size, buffer, &map_key, &descriptor_size, &descriptor_version);
    if (EFI_ERROR(status)) {
        uefi_free_pool(buffer);
        return 0;
    }
    
    *memory_map = buffer;
    *map_size = size;
    return 1;
}

int ovmf_reset_system(void) {
    if (!gRT) return 0;
    
    gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
    return 1;
}

int ovmf_shutdown_system(void) {
    if (!gRT) return 0;
    
    gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    return 1;
}
