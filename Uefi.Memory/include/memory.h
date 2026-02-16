#ifndef MEMORY_H
#define MEMORY_H

#include "uefi.h"
#include <stddef.h>
#include <stdint.h>

#define UEFI_MEMORY_ARENA_DEFAULT_ALIGNMENT 8

typedef struct {
    void* base;
    size_t size;
    size_t offset;
    size_t peak;
    EFI_MEMORY_TYPE memory_type;
} MemoryArena;

typedef struct {
    size_t pool_allocations;
    size_t pool_frees;
    size_t pool_bytes_allocated;
    size_t pool_peak_bytes;
} MemoryStats;

int memory_init(void);
void memory_reset_stats(void);
void memory_get_stats(MemoryStats* out_stats);

void* memory_allocate_pool(EFI_MEMORY_TYPE pool_type, size_t size);
void memory_free_pool(void* ptr, size_t size);

int memory_arena_create(MemoryArena* arena, size_t size, EFI_MEMORY_TYPE memory_type);
void memory_arena_destroy(MemoryArena* arena);
void memory_arena_reset(MemoryArena* arena);
void* memory_arena_allocate(MemoryArena* arena, size_t size, size_t alignment);
size_t memory_arena_remaining(const MemoryArena* arena);

void memory_set(void* dst, uint8_t value, size_t size);
void memory_copy(void* dst, const void* src, size_t size);
int memory_compare(const void* a, const void* b, size_t size);

#endif
