#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "uefi.h"

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t pixels_per_scanline;
    uint8_t* framebuffer;
} GraphicsContext;

typedef struct {
    uint8_t b, g, r, a;
} Color;

typedef struct {
    int x, y;
    uint32_t width, height;
} Rectangle;

int graphics_init(GraphicsContext* ctx);
void graphics_clear(GraphicsContext* ctx, Color color);
void graphics_set_pixel(GraphicsContext* ctx, int x, int y, Color color);
Color graphics_get_pixel(GraphicsContext* ctx, int x, int y);
void graphics_draw_rect(GraphicsContext* ctx, Rectangle rect, Color color);
void graphics_fill_rect(GraphicsContext* ctx, Rectangle rect, Color color);
void graphics_draw_line(GraphicsContext* ctx, int x0, int y0, int x1, int y1, Color color);
void graphics_draw_circle(GraphicsContext* ctx, int cx, int cy, int radius, Color color);
void graphics_fill_circle(GraphicsContext* ctx, int cx, int cy, int radius, Color color);

#define COLOR_BLACK   ((Color){0, 0, 0, 255})
#define COLOR_WHITE   ((Color){255, 255, 255, 255})
#define COLOR_RED     ((Color){255, 0, 0, 255})
#define COLOR_GREEN   ((Color){0, 255, 0, 255})
#define COLOR_BLUE    ((Color){0, 0, 255, 255})
#define COLOR_YELLOW  ((Color){255, 255, 0, 255})
#define COLOR_CYAN    ((Color){0, 255, 255, 255})
#define COLOR_MAGENTA ((Color){255, 0, 255, 255})

#endif
