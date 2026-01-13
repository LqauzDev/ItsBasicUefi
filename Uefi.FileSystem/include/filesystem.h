#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../Uefi.Core/include/uefi.h"

typedef struct {
    EFI_HANDLE volume;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
    EFI_FILE_PROTOCOL* root;
} FileSystem;

typedef struct {
    EFI_FILE_PROTOCOL* file;
    uint64_t size;
    uint64_t position;
} FileHandle;

int filesystem_init(FileSystem* fs, EFI_HANDLE device);
int filesystem_open_root(FileSystem* fs);
FileHandle* filesystem_open_file(FileSystem* fs, const char* path, const char* mode);
int filesystem_close_file(FileHandle* file);
int filesystem_read_file(FileHandle* file, void* buffer, size_t size, size_t* bytes_read);
int filesystem_write_file(FileHandle* file, const void* buffer, size_t size, size_t* bytes_written);
int filesystem_seek_file(FileHandle* file, uint64_t position);
int filesystem_tell_file(FileHandle* file, uint64_t* position);
int filesystem_get_file_size(FileHandle* file, uint64_t* size);
int filesystem_delete_file(FileSystem* fs, const char* path);
int filesystem_create_directory(FileSystem* fs, const char* path);
int filesystem_list_directory(FileSystem* fs, const char* path, void (*callback)(const char* name, int is_directory, void* context), void* context);

#endif
