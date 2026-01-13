#include "../../Uefi.Core/include/uefi_lib.h"
#include "../../Uefi.StdLib/include/stdio.h"
#include "../../Uefi.StdLib/include/stdlib.h"
#include "../../Uefi.Ovmf/include/ovmf.h"
#include "../../Uefi.Timer/include/timer.h"

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    uefi_init(SystemTable);
    
    printf("OVMF Detection Example\r\n");
    printf("======================\r\n\r\n");
    
    OvmfInfo ovmf_info;
    if (ovmf_init(&ovmf_info)) {
        printf("OVMF Environment Detected!\r\n");
        printf("Firmware Vendor: %s\r\n", ovmf_info.vendor);
        printf("Firmware Version: %s\r\n", ovmf_info.version);
        printf("Capabilities: 0x%x\r\n", ovmf_info.capabilities);
        
        printf("\nCapabilities Breakdown:\r\n");
        if (ovmf_info.capabilities & OVMF_CAPABILITY_GRAPHICS) {
            printf("- Graphics Support\r\n");
        }
        if (ovmf_info.capabilities & OVMF_CAPABILITY_NETWORK) {
            printf("- Network Support\r\n");
        }
        if (ovmf_info.capabilities & OVMF_CAPABILITY_USB) {
            printf("- USB Support\r\n");
        }
        if (ovmf_info.capabilities & OVMF_CAPABILITY_ACPI) {
            printf("- ACPI Support\r\n");
        }
        if (ovmf_info.capabilities & OVMF_CAPABILITY_SMBIOS) {
            printf("- SMBIOS Support\r\n");
        }
        
        printf("\nVirtualization Detection:\r\n");
        if (ovmf_is_qemu_environment()) {
            printf("- Running in QEMU\r\n");
        }
        if (ovmf_is_virtualbox_environment()) {
            printf("- Running in VirtualBox\r\n");
        }
        if (ovmf_is_vmware_environment()) {
            printf("- Running in VMware\r\n");
        }
        
        void* memory_map;
        size_t map_size;
        if (ovmf_get_memory_map(&memory_map, &map_size)) {
            printf("\nMemory Map Size: %d bytes\r\n", (int)map_size);
            uefi_free_pool(memory_map);
        }
        
    } else {
        printf("OVMF Environment Not Detected\r\n");
        printf("This may be running on physical hardware or a different UEFI firmware\r\n");
    }
    
    printf("\nTimer Test:\r\n");
    timer_init();
    
    Timer timer;
    timer_start(&timer);
    timer_delay_milliseconds(1000);
    timer_stop(&timer);
    
    printf("Elapsed Time: %d ms\r\n", (int)timer_get_elapsed_milliseconds(&timer));
    
    DateTime current_time;
    if (datetime_get_current(&current_time)) {
        char time_str[32];
        datetime_format_string(&current_time, time_str, sizeof(time_str));
        printf("Current Time: %s\r\n", time_str);
    }
    
    printf("\nPress any key to exit...\r\n");
    uefi_wait_for_key();
    
    return EFI_SUCCESS;
}
