#include "memory.h"
#include "uefi_lib.h"

static MemoryStats g_memory_stats;

static size_t memory_align_forward(size_t value, size_t alignment) {
    if (alignment == 0) {
        alignment = UEFI_MEMORY_ARENA_DEFAULT_ALIGNMENT;
    }

    size_t mask = alignment - 1;
    return (value + mask) & ~mask;
}

int memory_init(void) {
    if (!gBS) {
        return 0;
    }

    memory_reset_stats();
    return 1;
}

void memory_reset_stats(void) {
    g_memory_stats.pool_allocations = 0;
    g_memory_stats.pool_frees = 0;
    g_memory_stats.pool_bytes_allocated = 0;
    g_memory_stats.pool_peak_bytes = 0;
}

void memory_get_stats(MemoryStats* out_stats) {
    if (!out_stats) {
        return;
    }

    *out_stats = g_memory_stats;
}

void* memory_allocate_pool(EFI_MEMORY_TYPE pool_type, size_t size) {
    if (!gBS || size == 0) {
        return NULL;
    }

    EFI_STATUS (*allocate_pool)(EFI_MEMORY_TYPE, size_t, void**) =
        (EFI_STATUS (*)(EFI_MEMORY_TYPE, size_t, void**))gBS->AllocatePool;

    void* ptr = NULL;
    EFI_STATUS status = allocate_pool(pool_type, size, &ptr);
    if (status != EFI_SUCCESS || !ptr) {
        return NULL;
    }

    g_memory_stats.pool_allocations++;
    g_memory_stats.pool_bytes_allocated += size;
    if (g_memory_stats.pool_bytes_allocated > g_memory_stats.pool_peak_bytes) {
        g_memory_stats.pool_peak_bytes = g_memory_stats.pool_bytes_allocated;
    }

    return ptr;
}

void memory_free_pool(void* ptr, size_t size) {
    if (!gBS || !ptr) {
        return;
    }

    EFI_STATUS (*free_pool)(void*) = (EFI_STATUS (*)(void*))gBS->FreePool;
    free_pool(ptr);

    g_memory_stats.pool_frees++;
    if (size <= g_memory_stats.pool_bytes_allocated) {
        g_memory_stats.pool_bytes_allocated -= size;
    } else {
        g_memory_stats.pool_bytes_allocated = 0;
    }
}

int memory_arena_create(MemoryArena* arena, size_t size, EFI_MEMORY_TYPE memory_type) {
    if (!arena || size == 0) {
        return 0;
    }

    void* block = memory_allocate_pool(memory_type, size);
    if (!block) {
        return 0;
    }

    arena->base = block;
    arena->size = size;
    arena->offset = 0;
    arena->peak = 0;
    arena->memory_type = memory_type;

    return 1;
}

void memory_arena_destroy(MemoryArena* arena) {
    if (!arena || !arena->base) {
        return;
    }

    memory_free_pool(arena->base, arena->size);
    arena->base = NULL;
    arena->size = 0;
    arena->offset = 0;
    arena->peak = 0;
}

void memory_arena_reset(MemoryArena* arena) {
    if (!arena) {
        return;
    }

    arena->offset = 0;
}

void* memory_arena_allocate(MemoryArena* arena, size_t size, size_t alignment) {
    if (!arena || !arena->base || size == 0) {
        return NULL;
    }

    size_t aligned_offset = memory_align_forward(arena->offset, alignment);
    if (aligned_offset + size > arena->size) {
        return NULL;
    }

    uint8_t* bytes = (uint8_t*)arena->base;
    void* result = bytes + aligned_offset;

    arena->offset = aligned_offset + size;
    if (arena->offset > arena->peak) {
        arena->peak = arena->offset;
    }

    return result;
}

size_t memory_arena_remaining(const MemoryArena* arena) {
    if (!arena || arena->offset >= arena->size) {
        return 0;
    }

    return arena->size - arena->offset;
}

void memory_set(void* dst, uint8_t value, size_t size) {
    if (!dst) {
        return;
    }

    uint8_t* bytes = (uint8_t*)dst;
    for (size_t i = 0; i < size; i++) {
        bytes[i] = value;
    }
}

void memory_copy(void* dst, const void* src, size_t size) {
    if (!dst || !src) {
        return;
    }

    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;

    if (d < s) {
        for (size_t i = 0; i < size; i++) {
            d[i] = s[i];
        }
    } else if (d > s) {
        for (size_t i = size; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
}

int memory_compare(const void* a, const void* b, size_t size) {
    if (!a || !b) {
        return 0;
    }

    const uint8_t* lhs = (const uint8_t*)a;
    const uint8_t* rhs = (const uint8_t*)b;

    for (size_t i = 0; i < size; i++) {
        if (lhs[i] != rhs[i]) {
            return (lhs[i] < rhs[i]) ? -1 : 1;
        }
    }

    return 0;
}
