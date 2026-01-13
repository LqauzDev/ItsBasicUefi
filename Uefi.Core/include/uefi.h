#ifndef UEFI_H
#define UEFI_H

#include <stdint.h>
#include <stddef.h>

#ifndef EFIAPI
#define EFIAPI
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

typedef struct {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t HeaderSize;
    uint32_t CRC32;
    uint32_t Reserved;
} EFI_TABLE_HEADER;

typedef struct {
    uint8_t B;
    uint8_t G;
    uint8_t R;
    uint8_t Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef enum {
    EfiGdtAccessTypeCodeExecuteRead = 0xA,
    EfiGdtAccessTypeCodeExecuteReadWrite = 0xF,
    EfiGdtAccessTypeDataRead = 0x2,
    EfiGdtAccessTypeDataReadWrite = 0x3
} EFI_GDT_ACCESS_TYPE;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    void* RaiseTPL;
    void* RestoreTPL;
    void* AllocatePages;
    void* FreePages;
    void* GetMemoryMap;
    void* AllocatePool;
    void* FreePool;
    void* CreateEvent;
    void* SetTimer;
    void* WaitForEvent;
    void* SignalEvent;
    void* CloseEvent;
    void* CheckEvent;
    void* InstallProtocolInterface;
    void* ReinstallProtocolInterface;
    void* UninstallProtocolInterface;
    void* HandleProtocol;
    void* RegisterProtocolNotify;
    void* LocateHandle;
    void* LocateDevicePath;
    void* InstallConfigurationTable;
    void* LoadImage;
    void* StartImage;
    void* Exit;
    void* UnloadImage;
    void* ExitBootServices;
    void* GetNextMonotonicCount;
    void* Stall;
    void* SetWatchdogTimer;
    void* ConnectController;
    void* DisconnectController;
    void* OpenProtocol;
    void* CloseProtocol;
    void* OpenProtocolInformation;
    void* ProtocolsPerHandle;
    void* LocateHandleBuffer;
    void* LocateProtocol;
    void* InstallMultipleProtocolInterfaces;
    void* UninstallMultipleProtocolInterfaces;
    void* CalculateCrc32;
    void* CopyMem;
    void* SetMem;
    void* CreateEventEx;
} EFI_BOOT_SERVICES;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    void* GetTime;
    void* SetTime;
    void* GetWakeupTime;
    void* SetWakeupTime;
    void* SetVirtualAddressMap;
    void* ConvertPointer;
    void* GetVariable;
    void* GetNextVariableName;
    void* SetVariable;
    void* GetNextHighMonotonicCount;
    void* ResetSystem;
    void* UpdateCapsule;
    void* QueryCapsuleCapabilities;
    void* QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

typedef struct {
    void* LoadImage;
    void* StartImage;
    void* Exit;
    void* UnloadImage;
    void* ExitBootServices;
} EFI_IMAGE_HANDLE;

typedef struct {
    EFI_TABLE_HEADER Hdr;
    void* RaiseTPL;
    void* RestoreTPL;
    void* AllocatePages;
    void* FreePages;
    void* GetMemoryMap;
    void* AllocatePool;
    void* FreePool;
    void* CreateEvent;
    void* SetTimer;
    void* WaitForEvent;
    void* SignalEvent;
    void* CloseEvent;
    void* CheckEvent;
    void* InstallProtocolInterface;
    void* ReinstallProtocolInterface;
    void* UninstallProtocolInterface;
    void* HandleProtocol;
    void* RegisterProtocolNotify;
    void* LocateHandle;
    void* LocateDevicePath;
    void* InstallConfigurationTable;
    void* LoadImage;
    void* StartImage;
    void* Exit;
    void* UnloadImage;
    void* ExitBootServices;
    void* GetNextMonotonicCount;
    void* Stall;
    void* SetWatchdogTimer;
    void* ConnectController;
    void* DisconnectController;
    void* OpenProtocol;
    void* CloseProtocol;
    void* OpenProtocolInformation;
    void* ProtocolsPerHandle;
    void* LocateHandleBuffer;
    void* LocateProtocol;
    void* InstallMultipleProtocolInterfaces;
    void* UninstallMultipleProtocolInterfaces;
    void* CalculateCrc32;
    void* CopyMem;
    void* SetMem;
    void* CreateEventEx;
} EFI_SYSTEM_TABLE;

typedef struct {
    uint16_t Length;
    uint16_t Revision;
    uint32_t ParentHandle;
    uint16_t SystemTable;
    uint16_t DeviceHandle;
    uint32_t Reserved;
    uint32_t LoadOptionsSize;
    uint32_t LoadOptions;
    uint32_t ImageBase;
    uint64_t ImageSize;
    uint32_t ImageCodeType;
    uint32_t ImageDataType;
    uint32_t Unload;
} EFI_LOADED_IMAGE_PROTOCOL;

extern EFI_SYSTEM_TABLE* gST;
extern EFI_BOOT_SERVICES* gBS;
extern EFI_RUNTIME_SERVICES* gRT;

typedef uint64_t EFI_STATUS;
typedef uint64_t EFI_HANDLE;
typedef void* EFI_EVENT;
typedef uint64_t EFI_TPL;
typedef uint64_t EFI_LBA;
typedef uint64_t EFI_PHYSICAL_ADDRESS;
typedef uint64_t EFI_VIRTUAL_ADDRESS;

#define EFI_SUCCESS                     0
#define EFI_LOAD_ERROR                  1
#define EFI_INVALID_PARAMETER           2
#define EFI_UNSUPPORTED                 3
#define EFI_BAD_BUFFER_SIZE             4
#define EFI_BUFFER_TOO_SMALL            5
#define EFI_NOT_READY                   6
#define EFI_DEVICE_ERROR                7
#define EFI_WRITE_PROTECTED             8
#define EFI_OUT_OF_RESOURCES            9
#define EFI_VOLUME_CORRUPTED            10
#define EFI_VOLUME_FULL                 11
#define EFI_NO_MEDIA                    12
#define EFI_MEDIA_CHANGED               13
#define EFI_NOT_FOUND                   14
#define EFI_ACCESS_DENIED               15
#define EFI_NO_RESPONSE                 16
#define EFI_NO_MAPPING                  17
#define EFI_TIMEOUT                     18
#define EFI_NOT_STARTED                 19
#define EFI_ALREADY_STARTED             20
#define EFI_ABORTED                     21
#define EFI_ICMP_ERROR                  22
#define EFI_TFTP_ERROR                  23
#define EFI_PROTOCOL_ERROR              24
#define EFI_INCOMPATIBLE_VERSION        25
#define EFI_SECURITY_VIOLATION          26
#define EFI_CRC_ERROR                   27
#define EFI_END_OF_MEDIA                28
#define EFI_END_OF_FILE                 31
#define EFI_INVALID_LANGUAGE            32
#define EFI_COMPROMISED_DATA            33
#define EFI_IP_ADDRESS_CONFLICT         34
#define EFI_HTTP_ERROR                  35

#define EFI_WARN_UNKNOWN_GLYPH          1
#define EFI_WARN_DELETE_FAILURE         2
#define EFI_WARN_WRITE_FAILURE          3
#define EFI_WARN_BUFFER_TOO_SMALL       4
#define EFI_WARN_STALE_DATA             5
#define EFI_WARN_FILE_SYSTEM            6
#define EFI_WARN_RESET_REQUIRED         7

#endif
