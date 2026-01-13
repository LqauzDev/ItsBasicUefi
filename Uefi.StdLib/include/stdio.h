#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>

typedef struct {
    void* _ptr;
    int _cnt;
    void* _base;
    int _bufsiz;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz2;
    void* _tmpfname;
} FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int printf(const char* format, ...);
int fprintf(FILE* stream, const char* format, ...);
int sprintf(char* str, const char* format, ...);
int snprintf(char* str, size_t size, const char* format, ...);

int scanf(const char* format, ...);
int fscanf(FILE* stream, const char* format, ...);
int sscanf(const char* str, const char* format, ...);

int getchar(void);
int putchar(int c);
char* gets(char* s);
int puts(const char* s);

int fgetc(FILE* stream);
char* fgets(char* s, int size, FILE* stream);
int fputc(int c, FILE* stream);
int fputs(const char* s, FILE* stream);

int ungetc(int c, FILE* stream);

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);

int fseek(FILE* stream, long offset, int whence);
long ftell(FILE* stream);
void rewind(FILE* stream);

int fgetpos(FILE* stream, void* pos);
int fsetpos(FILE* stream, const void* pos);

int feof(FILE* stream);
int ferror(FILE* stream);
void clearerr(FILE* stream);

FILE* fopen(const char* filename, const char* mode);
FILE* freopen(const char* filename, const char* mode, FILE* stream);
int fclose(FILE* stream);

int fflush(FILE* stream);
int setvbuf(FILE* stream, char* buffer, int mode, size_t size);
void setbuf(FILE* stream, char* buffer);

int remove(const char* filename);
int rename(const char* oldname, const char* newname);
FILE* tmpfile(void);
char* tmpnam(char* s);

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define _IONBF 0
#define _IOLBF 1
#define _IOFBF 2

#define BUFSIZ 256
#define EOF (-1)

#endif
