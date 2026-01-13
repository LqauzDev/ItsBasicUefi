#include "graphics.h"
#include "uefi_lib.h"
#include "../Uefi.StdLib/include/stdlib.h"
#include "../Uefi.StdLib/include/string.h"

static GraphicsContext g_graphics_ctx = {0};

int graphics_init(GraphicsContext* ctx) {
    if (!gST || !gST->BootServices) return 0;
    
    EFI_GUID gop_guid = {0x9072b5e0, 0x8a2f, 0x4a1a, {0xa1, 0x8c, 0xfc, 0x01, 0x89, 0x45, 0x28, 0x0a}};
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop = NULL;
    
    EFI_STATUS status = gBS->LocateProtocol(&gop_guid, NULL, (void**)&gop);
    if (EFI_ERROR(status)) return 0;
    
    ctx->width = gop->Mode->Info->HorizontalResolution;
    ctx->height = gop->Mode->Info->VerticalResolution;
    ctx->pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;
    ctx->framebuffer = (uint8_t*)gop->Mode->FrameBufferBase;
    
    g_graphics_ctx = *ctx;
    return 1;
}

void graphics_clear(GraphicsContext* ctx, Color color) {
    if (!ctx || !ctx->framebuffer) return;
    
    uint32_t pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
    
    for (uint32_t y = 0; y < ctx->height; y++) {
        uint32_t* row = (uint32_t*)(ctx->framebuffer + y * ctx->pixels_per_scanline * 4);
        for (uint32_t x = 0; x < ctx->width; x++) {
            row[x] = pixel;
        }
    }
}

void graphics_set_pixel(GraphicsContext* ctx, int x, int y, Color color) {
    if (!ctx || !ctx->framebuffer || x < 0 || y < 0 || x >= (int)ctx->width || y >= (int)ctx->height) return;
    
    uint32_t* pixel = (uint32_t*)(ctx->framebuffer + y * ctx->pixels_per_scanline * 4 + x * 4);
    *pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}

Color graphics_get_pixel(GraphicsContext* ctx, int x, int y) {
    if (!ctx || !ctx->framebuffer || x < 0 || y < 0 || x >= (int)ctx->width || y >= (int)ctx->height) {
        return COLOR_BLACK;
    }
    
    uint32_t pixel = *(uint32_t*)(ctx->framebuffer + y * ctx->pixels_per_scanline * 4 + x * 4);
    Color color;
    color.a = (pixel >> 24) & 0xFF;
    color.r = (pixel >> 16) & 0xFF;
    color.g = (pixel >> 8) & 0xFF;
    color.b = pixel & 0xFF;
    return color;
}

void graphics_draw_rect(GraphicsContext* ctx, Rectangle rect, Color color) {
    if (!ctx || !ctx->framebuffer) return;
    
    graphics_draw_line(ctx, rect.x, rect.y, rect.x + rect.width - 1, rect.y, color);
    graphics_draw_line(ctx, rect.x, rect.y + rect.height - 1, rect.x + rect.width - 1, rect.y + rect.height - 1, color);
    graphics_draw_line(ctx, rect.x, rect.y, rect.x, rect.y + rect.height - 1, color);
    graphics_draw_line(ctx, rect.x + rect.width - 1, rect.y, rect.x + rect.width - 1, rect.y + rect.height - 1, color);
}

void graphics_fill_rect(GraphicsContext* ctx, Rectangle rect, Color color) {
    if (!ctx || !ctx->framebuffer) return;
    
    for (int y = rect.y; y < rect.y + (int)rect.height && y < (int)ctx->height; y++) {
        for (int x = rect.x; x < rect.x + (int)rect.width && x < (int)ctx->width; x++) {
            if (x >= 0 && y >= 0) {
                graphics_set_pixel(ctx, x, y, color);
            }
        }
    }
}

void graphics_draw_line(GraphicsContext* ctx, int x0, int y0, int x1, int y1, Color color) {
    if (!ctx || !ctx->framebuffer) return;
    
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        graphics_set_pixel(ctx, x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void graphics_draw_circle(GraphicsContext* ctx, int cx, int cy, int radius, Color color) {
    if (!ctx || !ctx->framebuffer) return;
    
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        graphics_set_pixel(ctx, cx + x, cy + y, color);
        graphics_set_pixel(ctx, cx + y, cy + x, color);
        graphics_set_pixel(ctx, cx - y, cy + x, color);
        graphics_set_pixel(ctx, cx - x, cy + y, color);
        graphics_set_pixel(ctx, cx - x, cy - y, color);
        graphics_set_pixel(ctx, cx - y, cy - x, color);
        graphics_set_pixel(ctx, cx + y, cy - x, color);
        graphics_set_pixel(ctx, cx + x, cy - y, color);
        
        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void graphics_fill_circle(GraphicsContext* ctx, int cx, int cy, int radius, Color color) {
    if (!ctx || !ctx->framebuffer) return;
    
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                graphics_set_pixel(ctx, cx + x, cy + y, color);
            }
        }
    }
}
