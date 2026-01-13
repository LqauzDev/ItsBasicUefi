#include "stdlib.h"
#include "string.h"
#include "../Uefi.Core/include/uefi_lib.h"

static unsigned int next_random = 1;

void* malloc(size_t size) {
    return uefi_allocate_pool(EfiLoaderData, size);
}

void* calloc(size_t nmemb, size_t size) {
    size_t total_size = nmemb * size;
    void* ptr = malloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    void* new_ptr = malloc(size);
    if (new_ptr && ptr) {
        memcpy(new_ptr, ptr, size);
        free(ptr);
    }
    return new_ptr;
}

void free(void* ptr) {
    uefi_free_pool(ptr);
}

void exit(int status) {
    uefi_print("Exiting with status: ");
    uefi_print_hex(status);
    uefi_println("");
    
    if (gRT) {
        gRT->ResetSystem(EfiResetShutdown, status, 0, NULL);
    }
    
    while (1) {
        uefi_delay(1000000);
    }
}

void abort(void) {
    uefi_println("Program aborted!");
    exit(1);
}

int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    
    while (*str == ' ') str++;
    
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

long atol(const char* str) {
    long result = 0;
    int sign = 1;
    
    while (*str == ' ') str++;
    
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

long long atoll(const char* str) {
    long long result = 0;
    int sign = 1;
    
    while (*str == ' ') str++;
    
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

double atof(const char* str) {
    return 0.0;
}

int rand(void) {
    next_random = next_random * 1103515245 + 12345;
    return (next_random / 65536) % 32768;
}

void srand(unsigned int seed) {
    next_random = seed;
}

void qsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
    if (nmemb <= 1) return;
    
    char* arr = (char*)base;
    char* pivot = arr + (nmemb - 1) * size;
    
    size_t i = 0;
    for (size_t j = 0; j < nmemb - 1; j++) {
        if (compar(arr + j * size, pivot) <= 0) {
            if (i != j) {
                char temp[256];
                memcpy(temp, arr + j * size, size);
                memcpy(arr + j * size, arr + i * size, size);
                memcpy(arr + i * size, temp, size);
            }
            i++;
        }
    }
    
    if (i != nmemb - 1) {
        char temp[256];
        memcpy(temp, pivot, size);
        memcpy(pivot, arr + i * size, size);
        memcpy(arr + i * size, temp, size);
    }
    
    qsort(arr, i, size, compar);
    qsort(arr + (i + 1) * size, nmemb - i - 1, size, compar);
}

void* bsearch(const void* key, const void* base, size_t nmemb, size_t size,
              int (*compar)(const void*, const void*)) {
    char* arr = (char*)base;
    size_t low = 0, high = nmemb;
    
    while (low < high) {
        size_t mid = low + (high - low) / 2;
        void* mid_elem = arr + mid * size;
        int cmp = compar(key, mid_elem);
        
        if (cmp == 0) {
            return mid_elem;
        } else if (cmp < 0) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }
    
    return NULL;
}

div_t div(int numer, int denom) {
    div_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}

ldiv_t ldiv(long numer, long denom) {
    ldiv_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}

lldiv_t lldiv(long long numer, long long denom) {
    lldiv_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}
