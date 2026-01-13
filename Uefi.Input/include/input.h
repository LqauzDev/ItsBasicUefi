#ifndef INPUT_H
#define INPUT_H

#include "../Uefi.Core/include/uefi.h"

typedef struct {
    uint16_t scan_code;
    uint16_t unicode_char;
} InputKey;

typedef struct {
    int x, y;
    uint8_t buttons;
    int wheel_x, wheel_y;
} InputMouse;

typedef struct {
    int x, y;
    uint8_t touch_id;
    uint8_t pressure;
} InputTouch;

typedef enum {
    INPUT_TYPE_KEYBOARD,
    INPUT_TYPE_MOUSE,
    INPUT_TYPE_TOUCH,
    INPUT_TYPE_UNKNOWN
} InputType;

typedef struct {
    InputType type;
    union {
        InputKey key;
        InputMouse mouse;
        InputTouch touch;
    } data;
} InputEvent;

int input_init(void);
int input_poll_event(InputEvent* event);
int input_wait_for_event(InputEvent* event);
int input_is_key_pressed(uint16_t scan_code);
int input_wait_for_key(InputKey* key);
int input_get_mouse_position(int* x, int* y);
int input_is_mouse_button_pressed(uint8_t button);
int input_get_touch_point(int* x, int* y, uint8_t* touch_id);
void input_flush_events(void);

#define KEY_SCAN_UP      0x01
#define KEY_SCAN_DOWN    0x02
#define KEY_SCAN_RIGHT   0x03
#define KEY_SCAN_LEFT    0x04
#define KEY_SCAN_HOME    0x05
#define KEY_SCAN_END     0x06
#define KEY_SCAN_PGUP    0x07
#define KEY_SCAN_PGDN    0x08
#define KEY_SCAN_ESC     0x17
#define KEY_SCAN_ENTER   0x1C
#define KEY_SCAN_SPACE   0x39
#define KEY_SCAN_F1      0x3B
#define KEY_SCAN_F2      0x3C
#define KEY_SCAN_F3      0x3D
#define KEY_SCAN_F4      0x3E
#define KEY_SCAN_F5      0x3F
#define KEY_SCAN_F6      0x40
#define KEY_SCAN_F7      0x41
#define KEY_SCAN_F8      0x42
#define KEY_SCAN_F9      0x43
#define KEY_SCAN_F10     0x44
#define KEY_SCAN_F11     0x45
#define KEY_SCAN_F12     0x46

#define MOUSE_BUTTON_LEFT   0x01
#define MOUSE_BUTTON_RIGHT  0x02
#define MOUSE_BUTTON_MIDDLE 0x04

#endif
