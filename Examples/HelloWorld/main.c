#include "uefi_lib.h"
#include "../../Uefi.StdLib/include/stdio.h"
#include "../../Uefi.StdLib/include/stdlib.h"

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    uefi_init(SystemTable);
    
    printf("Hello, UEFI World!\r\n");
    printf("Welcome to ItsBasicUefi Framework\r\n");
    printf("==================================\r\n");
    
    printf("Testing basic functionality:\r\n");
    printf("- Integer: %d\r\n", 42);
    printf("- Hex: 0x%x\r\n", 255);
    printf("- String: %s\r\n", "UEFI is working!");
    
    printf("\r\nPress any key to exit...\r\n");
    uefi_wait_for_key();
    
    printf("Goodbye!\r\n");
    
    return EFI_SUCCESS;
}
