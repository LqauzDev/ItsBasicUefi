#include "input.h"
#include "uefi_lib.h"
#include "../Uefi.StdLib/include/stdlib.h"
#include "../Uefi.StdLib/include/string.h"

static EFI_SIMPLE_POINTER_PROTOCOL* g_mouse_protocol = NULL;
static EFI_SIMPLE_TEXT_INPUT_PROTOCOL* g_keyboard_protocol = NULL;

int input_init(void) {
    if (!gST || !gBS) return 0;
    
    g_keyboard_protocol = gST->ConIn;
    if (!g_keyboard_protocol) return 0;
    
    EFI_GUID mouse_guid = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
    EFI_STATUS status = gBS->LocateProtocol(&mouse_guid, NULL, (void**)&g_mouse_protocol);
    if (EFI_ERROR(status)) {
        g_mouse_protocol = NULL;
    }
    
    return 1;
}

int input_poll_event(InputEvent* event) {
    if (!event) return 0;
    
    if (g_keyboard_protocol) {
        EFI_STATUS status = g_keyboard_protocol->ReadKeyStroke(g_keyboard_protocol, (EFI_INPUT_KEY*)&event->data.key);
        if (status == EFI_SUCCESS) {
            event->type = INPUT_TYPE_KEYBOARD;
            return 1;
        }
    }
    
    if (g_mouse_protocol) {
        EFI_SIMPLE_POINTER_STATE mouse_state;
        EFI_STATUS status = g_mouse_protocol->GetState(g_mouse_protocol, &mouse_state);
        if (status == EFI_SUCCESS) {
            event->type = INPUT_TYPE_MOUSE;
            event->data.mouse.x = mouse_state.RelativeMovementX;
            event->data.mouse.y = mouse_state.RelativeMovementY;
            event->data.mouse.buttons = mouse_state.Buttons;
            event->data.mouse.wheel_x = 0;
            event->data.mouse.wheel_y = 0;
            return 1;
        }
    }
    
    return 0;
}

int input_wait_for_event(InputEvent* event) {
    if (!event) return 0;
    
    if (g_keyboard_protocol) {
        uintn_t index;
        EFI_STATUS status = gBS->WaitForEvent(1, &g_keyboard_protocol->WaitForKey, &index);
        if (!EFI_ERROR(status)) {
            status = g_keyboard_protocol->ReadKeyStroke(g_keyboard_protocol, (EFI_INPUT_KEY*)&event->data.key);
            if (status == EFI_SUCCESS) {
                event->type = INPUT_TYPE_KEYBOARD;
                return 1;
            }
        }
    }
    
    if (g_mouse_protocol) {
        uintn_t index;
        EFI_STATUS status = gBS->WaitForEvent(1, &g_mouse_protocol->WaitForInput, &index);
        if (!EFI_ERROR(status)) {
            EFI_SIMPLE_POINTER_STATE mouse_state;
            status = g_mouse_protocol->GetState(g_mouse_protocol, &mouse_state);
            if (status == EFI_SUCCESS) {
                event->type = INPUT_TYPE_MOUSE;
                event->data.mouse.x = mouse_state.RelativeMovementX;
                event->data.mouse.y = mouse_state.RelativeMovementY;
                event->data.mouse.buttons = mouse_state.Buttons;
                event->data.mouse.wheel_x = 0;
                event->data.mouse.wheel_y = 0;
                return 1;
            }
        }
    }
    
    return 0;
}

int input_is_key_pressed(uint16_t scan_code) {
    if (!g_keyboard_protocol) return 0;
    
    EFI_INPUT_KEY key;
    EFI_STATUS status = g_keyboard_protocol->ReadKeyStroke(g_keyboard_protocol, &key);
    
    if (status == EFI_SUCCESS && key.ScanCode == scan_code) {
        return 1;
    }
    
    return 0;
}

int input_wait_for_key(InputKey* key) {
    if (!key || !g_keyboard_protocol) return 0;
    
    uintn_t index;
    EFI_STATUS status = gBS->WaitForEvent(1, &g_keyboard_protocol->WaitForKey, &index);
    if (EFI_ERROR(status)) return 0;
    
    status = g_keyboard_protocol->ReadKeyStroke(g_keyboard_protocol, (EFI_INPUT_KEY*)key);
    return status == EFI_SUCCESS;
}

int input_get_mouse_position(int* x, int* y) {
    if (!x || !y || !g_mouse_protocol) return 0;
    
    static int current_x = 0, current_y = 0;
    
    EFI_SIMPLE_POINTER_STATE mouse_state;
    EFI_STATUS status = g_mouse_protocol->GetState(g_mouse_protocol, &mouse_state);
    if (EFI_ERROR(status)) return 0;
    
    current_x += mouse_state.RelativeMovementX;
    current_y += mouse_state.RelativeMovementY;
    
    if (current_x < 0) current_x = 0;
    if (current_y < 0) current_y = 0;
    
    *x = current_x;
    *y = current_y;
    
    return 1;
}

int input_is_mouse_button_pressed(uint8_t button) {
    if (!g_mouse_protocol) return 0;
    
    EFI_SIMPLE_POINTER_STATE mouse_state;
    EFI_STATUS status = g_mouse_protocol->GetState(g_mouse_protocol, &mouse_state);
    if (EFI_ERROR(status)) return 0;
    
    return (mouse_state.Buttons & button) != 0;
}

int input_get_touch_point(int* x, int* y, uint8_t* touch_id) {
    (void)x; (void)y; (void)touch_id;
    return 0;
}

void input_flush_events(void) {
    if (g_keyboard_protocol) {
        EFI_INPUT_KEY key;
        while (g_keyboard_protocol->ReadKeyStroke(g_keyboard_protocol, &key) == EFI_SUCCESS) {
            // Flush keyboard events
        }
    }
    
    if (g_mouse_protocol) {
        EFI_SIMPLE_POINTER_STATE mouse_state;
        g_mouse_protocol->GetState(g_mouse_protocol, &mouse_state);
    }
}
