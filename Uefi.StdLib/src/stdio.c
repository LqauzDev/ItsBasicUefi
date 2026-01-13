#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "../Uefi.Core/include/uefi_lib.h"

static FILE stdin_file = {0};
static FILE stdout_file = {0};
static FILE stderr_file = {0};

FILE* stdin = &stdin_file;
FILE* stdout = &stdout_file;
FILE* stderr = &stderr_file;

static void print_int(int value, int base) {
    if (value < 0) {
        uefi_print("-");
        value = -value;
    }
    
    if (value >= base) {
        print_int(value / base, base);
    }
    
    char digit = "0123456789ABCDEF"[value % base];
    uefi_print((char[]){digit, '\0'});
}

static void print_uint(unsigned int value, int base) {
    if (value >= base) {
        print_uint(value / base, base);
    }
    
    char digit = "0123456789ABCDEF"[value % base];
    uefi_print((char[]){digit, '\0'});
}

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    int count = 0;
    const char* p = format;
    
    while (*p) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd':
                case 'i':
                    print_int(va_arg(args, int), 10);
                    count++;
                    break;
                case 'u':
                    print_uint(va_arg(args, unsigned int), 10);
                    count++;
                    break;
                case 'x':
                    print_uint(va_arg(args, unsigned int), 16);
                    count++;
                    break;
                case 'c':
                    {
                        char c = va_arg(args, int);
                        uefi_print((char[]){c, '\0'});
                        count++;
                    }
                    break;
                case 's':
                    {
                        char* s = va_arg(args, char*);
                        uefi_print(s);
                        count++;
                    }
                    break;
                case 'p':
                    uefi_print("0x");
                    uefi_print_hex((uint64_t)va_arg(args, void*));
                    count++;
                    break;
                case '%':
                    uefi_print("%");
                    count++;
                    break;
                default:
                    uefi_print("%");
                    uefi_print((char[]){*p, '\0'});
                    count++;
                    break;
            }
            p++;
        } else {
            uefi_print((char[]){*p, '\0'});
            p++;
            count++;
        }
    }
    
    va_end(args);
    return count;
}

int fprintf(FILE* stream, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    int count = 0;
    const char* p = format;
    
    while (*p) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd':
                case 'i':
                    print_int(va_arg(args, int), 10);
                    count++;
                    break;
                case 'u':
                    print_uint(va_arg(args, unsigned int), 10);
                    count++;
                    break;
                case 'x':
                    print_uint(va_arg(args, unsigned int), 16);
                    count++;
                    break;
                case 'c':
                    {
                        char c = va_arg(args, int);
                        uefi_print((char[]){c, '\0'});
                        count++;
                    }
                    break;
                case 's':
                    {
                        char* s = va_arg(args, char*);
                        uefi_print(s);
                        count++;
                    }
                    break;
                case 'p':
                    uefi_print("0x");
                    uefi_print_hex((uint64_t)va_arg(args, void*));
                    count++;
                    break;
                case '%':
                    uefi_print("%");
                    count++;
                    break;
                default:
                    uefi_print("%");
                    uefi_print((char[]){*p, '\0'});
                    count++;
                    break;
            }
            p++;
        } else {
            uefi_print((char[]){*p, '\0'});
            p++;
            count++;
        }
    }
    
    va_end(args);
    return count;
}

int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char buffer[1024];
    int count = 0;
    const char* p = format;
    char* dest = str;
    
    while (*p) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd':
                case 'i':
                    {
                        int value = va_arg(args, int);
                        if (value < 0) {
                            *dest++ = '-';
                            value = -value;
                        }
                        
                        char temp[32];
                        int len = 0;
                        
                        if (value == 0) {
                            temp[len++] = '0';
                        } else {
                            while (value > 0) {
                                temp[len++] = '0' + (value % 10);
                                value /= 10;
                            }
                        }
                        
                        for (int i = len - 1; i >= 0; i--) {
                            *dest++ = temp[i];
                        }
                        count++;
                    }
                    break;
                case 'u':
                    {
                        unsigned int value = va_arg(args, unsigned int);
                        char temp[32];
                        int len = 0;
                        
                        if (value == 0) {
                            temp[len++] = '0';
                        } else {
                            while (value > 0) {
                                temp[len++] = '0' + (value % 10);
                                value /= 10;
                            }
                        }
                        
                        for (int i = len - 1; i >= 0; i--) {
                            *dest++ = temp[i];
                        }
                        count++;
                    }
                    break;
                case 's':
                    {
                        char* s = va_arg(args, char*);
                        while (*s) {
                            *dest++ = *s++;
                        }
                        count++;
                    }
                    break;
                case 'c':
                    {
                        char c = va_arg(args, int);
                        *dest++ = c;
                        count++;
                    }
                    break;
                default:
                    *dest++ = '%';
                    *dest++ = *p;
                    count += 2;
                    break;
            }
            p++;
        } else {
            *dest++ = *p++;
            count++;
        }
    }
    
    *dest = '\0';
    va_end(args);
    return count;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char buffer[1024];
    int result = sprintf(buffer, format, args);
    
    if (size > 0) {
        size_t copy_len = (result < (int)size - 1) ? result : size - 1;
        memcpy(str, buffer, copy_len);
        str[copy_len] = '\0';
    }
    
    va_end(args);
    return result;
}

int scanf(const char* format, ...) {
    return 0;
}

int fscanf(FILE* stream, const char* format, ...) {
    return 0;
}

int sscanf(const char* str, const char* format, ...) {
    return 0;
}

int getchar(void) {
    uefi_wait_for_key();
    return 0;
}

int putchar(int c) {
    uefi_print((char[]){c, '\0'});
    return c;
}

char* gets(char* s) {
    return NULL;
}

int puts(const char* s) {
    uefi_print(s);
    uefi_println("");
    return 0;
}

int fgetc(FILE* stream) {
    return EOF;
}

char* fgets(char* s, int size, FILE* stream) {
    return NULL;
}

int fputc(int c, FILE* stream) {
    uefi_print((char[]){c, '\0'});
    return c;
}

int fputs(const char* s, FILE* stream) {
    uefi_print(s);
    return 0;
}

int ungetc(int c, FILE* stream) {
    return EOF;
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return 0;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return 0;
}

int fseek(FILE* stream, long offset, int whence) {
    return -1;
}

long ftell(FILE* stream) {
    return -1;
}

void rewind(FILE* stream) {
}

int fgetpos(FILE* stream, void* pos) {
    return -1;
}

int fsetpos(FILE* stream, const void* pos) {
    return -1;
}

int feof(FILE* stream) {
    return 0;
}

int ferror(FILE* stream) {
    return 0;
}

void clearerr(FILE* stream) {
}

FILE* fopen(const char* filename, const char* mode) {
    return NULL;
}

FILE* freopen(const char* filename, const char* mode, FILE* stream) {
    return NULL;
}

int fclose(FILE* stream) {
    return 0;
}

int fflush(FILE* stream) {
    return 0;
}

int setvbuf(FILE* stream, char* buffer, int mode, size_t size) {
    return 0;
}

void setbuf(FILE* stream, char* buffer) {
}

int remove(const char* filename) {
    return -1;
}

int rename(const char* oldname, const char* newname) {
    return -1;
}

FILE* tmpfile(void) {
    return NULL;
}

char* tmpnam(char* s) {
    static char temp_name[] = "tmpXXXXXX";
    if (s) {
        strcpy(s, temp_name);
        return s;
    }
    return temp_name;
}
