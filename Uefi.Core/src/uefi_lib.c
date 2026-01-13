#include "uefi_lib.h"

EFI_SYSTEM_TABLE* gST = NULL;
EFI_BOOT_SERVICES* gBS = NULL;
EFI_RUNTIME_SERVICES* gRT = NULL;

void uefi_init(EFI_SYSTEM_TABLE* SystemTable) {
    gST = SystemTable;
    gBS = SystemTable->BootServices;
    gRT = SystemTable->RuntimeServices;
}

void uefi_print(const char* str) {
    if (!gST || !gST->ConOut) return;
    
    char16_t* wide_str = NULL;
    size_t len = 0;
    
    while (str[len]) len++;
    
    wide_str = (char16_t*)uefi_allocate_pool(EfiLoaderData, (len + 1) * sizeof(char16_t));
    if (!wide_str) return;
    
    for (size_t i = 0; i < len; i++) {
        wide_str[i] = (char16_t)str[i];
    }
    wide_str[len] = 0;
    
    gST->ConOut->OutputString(gST->ConOut, wide_str);
    uefi_free_pool(wide_str);
}

void uefi_println(const char* str) {
    uefi_print(str);
    uefi_print("\r\n");
}

void uefi_print_hex(uint64_t value) {
    const char hex_chars[] = "0123456789ABCDEF";
    char buffer[17];
    buffer[16] = '\0';
    
    for (int i = 15; i >= 0; i--) {
        buffer[i] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    uefi_print(buffer);
}

void uefi_delay(uint32_t microseconds) {
    if (!gBS) return;
    gBS->Stall(microseconds);
}

EFI_STATUS uefi_wait_for_key(void) {
    if (!gST || !gST->ConIn) return EFI_INVALID_PARAMETER;
    
    EFI_INPUT_KEY key;
    EFI_STATUS status;
    uintn_t index;
    
    do {
        status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &index);
        if (EFI_ERROR(status)) return status;
        
        status = gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
    } while (status == EFI_NOT_READY);
    
    return status;
}

void* uefi_allocate_pool(EFI_MEMORY_TYPE PoolType, size_t Size) {
    if (!gBS) return NULL;
    
    void* memory = NULL;
    EFI_STATUS status = gBS->AllocatePool(PoolType, Size, &memory);
    
    if (EFI_ERROR(status)) {
        return NULL;
    }
    
    return memory;
}

void uefi_free_pool(void* Memory) {
    if (!gBS || !Memory) return;
    gBS->FreePool(Memory);
}
