#ifndef UEFI_LIB_H
#define UEFI_LIB_H

#include "uefi.h"

void uefi_init(EFI_SYSTEM_TABLE* SystemTable);
void uefi_print(const char* str);
void uefi_println(const char* str);
void uefi_print_hex(uint64_t value);
void uefi_delay(uint32_t microseconds);
EFI_STATUS uefi_wait_for_key(void);

void* uefi_allocate_pool(EFI_MEMORY_TYPE PoolType, size_t Size);
void uefi_free_pool(void* Memory);

typedef struct {
    uint16_t ScanCode;
    uint16_t UnicodeChar;
} EFI_INPUT_KEY;

#endif
