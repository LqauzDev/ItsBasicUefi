#include "filesystem.h"
#include "../Uefi.Core/include/uefi_lib.h"
#include "../Uefi.StdLib/include/stdlib.h"
#include "../Uefi.StdLib/include/string.h"

int filesystem_init(FileSystem* fs, EFI_HANDLE device) {
    if (!fs || !device) return 0;
    
    memset(fs, 0, sizeof(FileSystem));
    fs->volume = device;
    
    EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_STATUS status = gBS->HandleProtocol(device, &fs_guid, (void**)&fs->fs);
    
    if (EFI_ERROR(status)) return 0;
    
    return filesystem_open_root(fs);
}

int filesystem_open_root(FileSystem* fs) {
    if (!fs || !fs->fs) return 0;
    
    EFI_STATUS status = fs->fs->OpenVolume(fs->fs, &fs->root);
    return !EFI_ERROR(status);
}

FileHandle* filesystem_open_file(FileSystem* fs, const char* path, const char* mode) {
    if (!fs || !fs->root || !path || !mode) return NULL;
    
    FileHandle* file = malloc(sizeof(FileHandle));
    if (!file) return NULL;
    
    memset(file, 0, sizeof(FileHandle));
    
    char16_t path16[256];
    for (int i = 0; i < 255 && path[i]; i++) {
        path16[i] = (char16_t)path[i];
    }
    path16[strlen(path)] = 0;
    
    UINT64 mode_flags = 0;
    if (strchr(mode, 'r')) mode_flags |= EFI_FILE_MODE_READ;
    if (strchr(mode, 'w')) mode_flags |= EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE;
    if (strchr(mode, 'a')) mode_flags |= EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE;
    
    EFI_STATUS status = fs->root->Open(fs->root, &file->file, path16, mode_flags, 0);
    if (EFI_ERROR(status)) {
        free(file);
        return NULL;
    }
    
    if (strchr(mode, 'a')) {
        EFI_FILE_INFO info;
        UINTN info_size = sizeof(info);
        status = file->file->GetInfo(file->file, &gEfiFileInfoGuid, &info_size, &info);
        if (!EFI_ERROR(status)) {
            file->position = info.FileSize;
            file->file->SetPosition(file->file, file->position);
        }
    }
    
    filesystem_get_file_size(file, &file->size);
    return file;
}

int filesystem_close_file(FileHandle* file) {
    if (!file || !file->file) return 0;
    
    EFI_STATUS status = file->file->Close(file->file);
    free(file);
    return !EFI_ERROR(status);
}

int filesystem_read_file(FileHandle* file, void* buffer, size_t size, size_t* bytes_read) {
    if (!file || !file->file || !buffer) return 0;
    
    UINTN read_size = size;
    EFI_STATUS status = file->file->Read(file->file, &read_size, buffer);
    
    if (bytes_read) *bytes_read = read_size;
    return !EFI_ERROR(status);
}

int filesystem_write_file(FileHandle* file, const void* buffer, size_t size, size_t* bytes_written) {
    if (!file || !file->file || !buffer) return 0;
    
    UINTN write_size = size;
    EFI_STATUS status = file->file->Write(file->file, &write_size, (void*)buffer);
    
    if (bytes_written) *bytes_written = write_size;
    return !EFI_ERROR(status);
}

int filesystem_seek_file(FileHandle* file, uint64_t position) {
    if (!file || !file->file) return 0;
    
    EFI_STATUS status = file->file->SetPosition(file->file, position);
    if (!EFI_ERROR(status)) {
        file->position = position;
    }
    return !EFI_ERROR(status);
}

int filesystem_tell_file(FileHandle* file, uint64_t* position) {
    if (!file || !file->file || !position) return 0;
    
    EFI_STATUS status = file->file->GetPosition(file->file, position);
    if (!EFI_ERROR(status)) {
        file->position = *position;
    }
    return !EFI_ERROR(status);
}

int filesystem_get_file_size(FileHandle* file, uint64_t* size) {
    if (!file || !file->file || !size) return 0;
    
    uint64_t current_pos;
    if (filesystem_tell_file(file, &current_pos)) {
        EFI_STATUS status = file->file->SetPosition(file->file, 0xFFFFFFFFFFFFFFFFULL);
        if (!EFI_ERROR(status)) {
            status = file->file->GetPosition(file->file, size);
            file->file->SetPosition(file->file, current_pos);
            if (!EFI_ERROR(status)) {
                file->size = *size;
                return 1;
            }
        }
    }
    return 0;
}

int filesystem_delete_file(FileSystem* fs, const char* path) {
    if (!fs || !fs->root || !path) return 0;
    
    char16_t path16[256];
    for (int i = 0; i < 255 && path[i]; i++) {
        path16[i] = (char16_t)path[i];
    }
    path16[strlen(path)] = 0;
    
    EFI_STATUS status = fs->root->Delete(fs->root);
    return !EFI_ERROR(status);
}

int filesystem_create_directory(FileSystem* fs, const char* path) {
    if (!fs || !fs->root || !path) return 0;
    
    char16_t path16[256];
    for (int i = 0; i < 255 && path[i]; i++) {
        path16[i] = (char16_t)path[i];
    }
    path16[strlen(path)] = 0;
    
    EFI_FILE_PROTOCOL* dir = NULL;
    EFI_STATUS status = fs->root->Open(fs->root, &dir, path16, 
                                       EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 
                                       EFI_FILE_DIRECTORY);
    
    if (!EFI_ERROR(status) && dir) {
        dir->Close(dir);
        return 1;
    }
    return 0;
}

int filesystem_list_directory(FileSystem* fs, const char* path, void (*callback)(const char* name, int is_directory, void* context), void* context) {
    if (!fs || !fs->root || !callback) return 0;
    
    EFI_FILE_PROTOCOL* dir = NULL;
    if (path && strlen(path) > 0) {
        char16_t path16[256];
        for (int i = 0; i < 255 && path[i]; i++) {
            path16[i] = (char16_t)path[i];
        }
        path16[strlen(path)] = 0;
        
        EFI_STATUS status = fs->root->Open(fs->root, &dir, path16, EFI_FILE_MODE_READ, 0);
        if (EFI_ERROR(status)) return 0;
    } else {
        dir = fs->root;
    }
    
    char buffer[4096];
    UINTN buffer_size = sizeof(buffer);
    EFI_STATUS status = dir->Read(dir, &buffer_size, buffer);
    
    while (!EFI_ERROR(status) && buffer_size > 0) {
        EFI_FILE_INFO* info = (EFI_FILE_INFO*)buffer;
        
        char name8[256];
        for (int i = 0; i < 255 && info->FileName[i]; i++) {
            name8[i] = (char)info->FileName[i];
        }
        name8[info->FileNameLength / sizeof(char16_t)] = 0;
        
        if (strcmp(name8, ".") != 0 && strcmp(name8, "..") != 0) {
            callback(name8, (info->Attribute & EFI_FILE_DIRECTORY) != 0, context);
        }
        
        buffer_size = sizeof(buffer);
        status = dir->Read(dir, &buffer_size, buffer);
    }
    
    if (dir != fs->root) {
        dir->Close(dir);
    }
    
    return 1;
}
