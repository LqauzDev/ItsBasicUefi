#include "../../Uefi.Core/include/uefi_lib.h"
#include "../../Uefi.StdLib/include/stdio.h"
#include "../../Uefi.StdLib/include/stdlib.h"
#include "../../Uefi.Graphics/include/graphics.h"
#include "../../Uefi.Input/include/input.h"
#include "../../Uefi.Timer/include/timer.h"

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    uefi_init(SystemTable);
    
    printf("Graphics and Input Demo\r\n");
    printf("========================\r\n\r\n");
    
    GraphicsContext ctx;
    if (graphics_init(&ctx)) {
        printf("Graphics initialized: %dx%d\r\n", ctx.width, ctx.height);
        
        graphics_clear(&ctx, COLOR_BLACK);
        
        Rectangle rect = {100, 100, 200, 150};
        graphics_fill_rect(&ctx, rect, COLOR_BLUE);
        
        graphics_draw_circle(&ctx, 400, 200, 50, COLOR_RED);
        graphics_fill_circle(&ctx, 400, 200, 30, COLOR_YELLOW);
        
        graphics_draw_line(&ctx, 50, 50, 600, 400, COLOR_GREEN);
        
        printf("Graphics demo complete\r\n");
    } else {
        printf("Graphics initialization failed\r\n");
    }
    
    if (input_init()) {
        printf("Input system initialized\r\n");
        printf("Press ESC to exit, arrow keys to move\r\n");
        
        int x = ctx.width / 2;
        int y = ctx.height / 2;
        
        while (1) {
            InputEvent event;
            if (input_wait_for_event(&event)) {
                if (event.type == INPUT_TYPE_KEYBOARD) {
                    if (event.data.key.scan_code == KEY_SCAN_ESC) {
                        break;
                    } else if (event.data.key.scan_code == KEY_SCAN_UP) {
                        y -= 10;
                    } else if (event.data.key.scan_code == KEY_SCAN_DOWN) {
                        y += 10;
                    } else if (event.data.key.scan_code == KEY_SCAN_LEFT) {
                        x -= 10;
                    } else if (event.data.key.scan_code == KEY_SCAN_RIGHT) {
                        x += 10;
                    }
                    
                    if (x < 0) x = 0;
                    if (x >= (int)ctx.width) x = ctx.width - 1;
                    if (y < 0) y = 0;
                    if (y >= (int)ctx.height) y = ctx.height - 1;
                    
                    graphics_set_pixel(&ctx, x, y, COLOR_WHITE);
                }
            }
        }
    } else {
        printf("Input initialization failed\r\n");
        timer_delay_milliseconds(2000);
    }
    
    return EFI_SUCCESS;
}
