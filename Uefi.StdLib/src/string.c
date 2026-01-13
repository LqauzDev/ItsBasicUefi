#include "string.h"

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    
    while (n--) {
        *d++ = *s++;
    }
    
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    
    if (d <= s || d >= s + n) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        d += n - 1;
        s += n - 1;
        while (n--) {
            *d-- = *s--;
        }
    }
    
    return dest;
}

void* memset(void* s, int c, size_t n) {
    char* ptr = (char*)s;
    
    while (n--) {
        *ptr++ = (unsigned char)c;
    }
    
    return s;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    
    return 0;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    
    while ((*d++ = *src++) != '\0') {
        ;
    }
    
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    
    while (n-- && (*d++ = *src++) != '\0') {
        ;
    }
    
    while (n--) {
        *d++ = '\0';
    }
    
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* d = dest;
    
    while (*d) {
        d++;
    }
    
    while ((*d++ = *src++) != '\0') {
        ;
    }
    
    return dest;
}

char* strncat(char* dest, const char* src, size_t n) {
    char* d = dest;
    
    while (*d) {
        d++;
    }
    
    while (n-- && (*d++ = *src++) != '\0') {
        ;
    }
    
    *d = '\0';
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n-- && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    
    return n == SIZE_MAX ? 0 : *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strcoll(const char* s1, const char* s2) {
    return strcmp(s1, s2);
}

size_t strxfrm(char* dest, const char* src, size_t n) {
    size_t len = strlen(src);
    if (n > 0) {
        strncpy(dest, src, n - 1);
        dest[n - 1] = '\0';
    }
    return len;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) {
            return (char*)s;
        }
        s++;
    }
    
    return c == '\0' ? (char*)s : NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    
    while (*s) {
        if (*s == (char)c) {
            last = s;
        }
        s++;
    }
    
    return c == '\0' ? (char*)s : (char*)last;
}

size_t strcspn(const char* s, const char* reject) {
    const char* p;
    const char* r;
    
    for (p = s; *p; p++) {
        for (r = reject; *r; r++) {
            if (*p == *r) {
                return p - s;
            }
        }
    }
    
    return p - s;
}

size_t strspn(const char* s, const char* accept) {
    const char* p;
    const char* a;
    
    for (p = s; *p; p++) {
        for (a = accept; *a; a++) {
            if (*p == *a) {
                break;
            }
        }
        if (*a == '\0') {
            return p - s;
        }
    }
    
    return p - s;
}

char* strpbrk(const char* s, const char* accept) {
    const char* p;
    const char* a;
    
    for (p = s; *p; p++) {
        for (a = accept; *a; a++) {
            if (*p == *a) {
                return (char*)p;
            }
        }
    }
    
    return NULL;
}

char* strstr(const char* haystack, const char* needle) {
    size_t needle_len = strlen(needle);
    
    if (needle_len == 0) {
        return (char*)haystack;
    }
    
    while (*haystack) {
        if (strncmp(haystack, needle, needle_len) == 0) {
            return (char*)haystack;
        }
        haystack++;
    }
    
    return NULL;
}

static char* strtok_str = NULL;

char* strtok(char* str, const char* delim) {
    char* token;
    
    if (str) {
        strtok_str = str;
    } else if (!strtok_str) {
        return NULL;
    }
    
    token = strtok_str;
    strtok_str += strcspn(strtok_str, delim);
    
    if (*strtok_str) {
        *strtok_str++ = '\0';
    } else {
        strtok_str = NULL;
    }
    
    return token;
}

size_t strlen(const char* s) {
    const char* p = s;
    while (*p) {
        p++;
    }
    return p - s;
}

char* strerror(int errnum) {
    static char* error_msgs[] = {
        "Success",
        "Operation not permitted",
        "No such file or directory",
        "No such process",
        "Interrupted system call",
        "Input/output error",
        "No such device or address",
        "Argument list too long",
        "Exec format error",
        "Bad file number",
        "No child processes",
        "Try again",
        "Out of memory",
        "Permission denied",
        "Bad address",
        "Block device required",
        "Device or resource busy",
        "File exists",
        "Cross-device link",
        "No such device",
        "Not a directory",
        "Is a directory",
        "Invalid argument",
        "File table overflow",
        "Too many open files",
        "Not a typewriter",
        "Text file busy",
        "File too large",
        "No space left on device",
        "Illegal seek",
        "Read-only file system",
        "Too many links",
        "Broken pipe",
        "Math argument out of domain of func",
        "Math result not representable"
    };
    
    if (errnum >= 0 && errnum < (int)(sizeof(error_msgs) / sizeof(error_msgs[0]))) {
        return error_msgs[errnum];
    }
    
    return "Unknown error";
}

void* memchr(const void* s, int c, size_t n) {
    const unsigned char* p = (const unsigned char*)s;
    
    while (n--) {
        if (*p == (unsigned char)c) {
            return (void*)p;
        }
        p++;
    }
    
    return NULL;
}

char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) {
        memcpy(dup, s, len);
    }
    return dup;
}

char* strndup(const char* s, size_t n) {
    size_t len = strlen(s);
    if (len > n) {
        len = n;
    }
    
    char* dup = malloc(len + 1);
    if (dup) {
        memcpy(dup, s, len);
        dup[len] = '\0';
    }
    return dup;
}
