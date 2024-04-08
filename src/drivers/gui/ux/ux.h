#ifndef __UX_H
#define __UX_H

#include <stdint.h>
#include "cursor.h"

#define UX_MAX_WINDOWS          0x10

#define UX_TYPE_BUTTON          00
#define UX_TYPE_LABEL           01
#define UX_TYPE_IMAGE           02
#define UX_TYPE_TEXT_INPUT      03
#define UX_TYPE_TEXT_AREA       04
#define UX_TYPE_CHECKBOX        05
#define UX_TYPE_RADIO           06
#define UX_TYPE_DROPDOWN        07
#define UX_TYPE_SLIDER          08
#define UX_TYPE_TAB             09
#define UX_TYPE_PANEL           10
#define UX_TYPE_WINDOW          11

#define UX_EVENT_LCLICK         0x01
#define UX_EVENT_RCLICK         0x02
#define UX_EVENT_MCLICK         0x04
#define UX_EVENT_MOUSEMOVE      0x08
#define UX_EVENT_KEYPRESS       0x10

#define UX_EVENT_DEVICE_MOUSE   0x00
#define UX_EVENT_DEVICE_KEYBOARD 0x01

#define UX_EVENT_MOUSE_LEFT_MASK 0x01
#define UX_EVENT_MOUSE_RIGHT_MASK 0x02
#define UX_EVENT_MOUSE_MIDDLE_MASK 0x04

#define UX_DATATYPE_STRING      0x01
#define UX_DATATYPE_INT         0x02
#define UX_DATATYPE_DOUBLE      0x03
#define UX_DATATYPE_BOOL        0x04

#define UX_COORDS_ABSOLUTE      0x00
#define UX_COORDS_RELATIVE      0x01
#define UX_COORDS_PERCENT       0x02

#define UX_MAX_NAME                 0x100
#define UX_MAX_SPECIFIC_DATA_NAME   0x100
#define UX_MAX_SPECIFIC_DATA        0x1000
#define UX_MAX_SPECIFIC_DATA_ITEMS  0x10

#define UX_SERIALIZE_SEPARETOR  0x05
#define UX_MAX_CHILDREN         0x10

struct ux_event {
    uint8_t device; //0 = mouse, 1 = keyboard
    uint16_t x;
    uint16_t y;
    uint8_t data; //Can be the key pressed or the button clicked
};

struct ux_component {
    char name[UX_MAX_NAME];
    char text[UX_MAX_SPECIFIC_DATA];

    uint8_t type;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint8_t coord_type;
    uint8_t has_focus;

    uint8_t specific_data_items;
    uint8_t children_count;
    
    struct cursor cursor;

    void (*mouse)(struct ux_component* component, uint16_t x, uint16_t y, uint8_t button);
    void (*write)(struct ux_component* component, char c);

    struct ux_specific_data {
        char name[UX_MAX_SPECIFIC_DATA_NAME];
        uint64_t type;
        union {
            int64_t int_data;
            double double_data;
            char string_data[UX_MAX_SPECIFIC_DATA];
            uint8_t bool_data;
        };
    } specific_data[UX_MAX_SPECIFIC_DATA_ITEMS];

    struct ux_component * children[UX_MAX_CHILDREN];
    struct ux_component * parent;
};

void load_font();
void print_bg();
void spawn_tty();

void hook_mouse(struct ux_component* component, void (*mouse)(struct ux_component* component, uint16_t x, uint16_t y, uint8_t button));
void hook_write(struct ux_component* component, void (*write)(struct ux_component* component, char c));
void unhook_keyboard(struct ux_component* component);
void unhook_keyboard(struct ux_component* component);
void event_dispatch(struct ux_event* event);
#endif





/* KEPT JUST IN CASE
void ux_write_extra_byte(struct ux_component * component, uint8_t data) {
    if (component == 0) {
        printf("Component is null\n");
        return;
    }

    if (component->specific_data == 0) {
        component->specific_data = UX_MAX_SPECIFIC_DATA;
        memset(component->specific_data, 0, UX_MAX_SPECIFIC_DATA);
    }

    if (component->specific_data_bytes >= UX_MAX_SPECIFIC_DATA) {
        printf("Max extra data reached\n");
        return;
    }

    ((uint8_t*)component->specific_data)[component->specific_data_bytes++] = data;
}

void ux_write_bytes(struct ux_component * component, uint8_t * data, uint64_t size) {
    if (component == 0) {
        printf("Component is null\n");
        return;
    }

    if (component->specific_data == 0) {
        component->specific_data = UX_MAX_SPECIFIC_DATA;
        memset(component->specific_data, 0, UX_MAX_SPECIFIC_DATA);
    }

    if (component->specific_data_bytes + size >= UX_MAX_SPECIFIC_DATA) {
        printf("Max extra data reached\n");
        return;
    }

    memcpy(((uint8_t*)component->specific_data) + component->specific_data_bytes, data, size);
    component->specific_data_bytes += size;
}

//FORMAT: [type][name_len][name][data_len][data][crc32]
//SIZES:  [   1][       2][   N][       2][   M][    4]
static void serialize_specific_data(const char* name, uint8_t type, void* data, struct ux_component * component) {
    if (component == 0) {
        printf("Component is null\n");
        return;
    }

    if (strlen(name) > 0xFFFF) {
        printf("Name too long\n");
        return;
    }

    switch (type) {
        case UX_DATATYPE_STRING: {
            uint32_t buflen = 5 + strlen(name) + strlen((char*)data);
            char* buffer = malloc(buflen);
            if (buffer == 0) {
                printf("Failed to allocate buffer\n");
                return;
            }
            memset(buffer, 0, buflen);
            char * str = (char*)data;

            uint64_t str_len = strlen(str);
            if (str_len > 0xFFFF) {
                printf("String too long\n");
                free(buffer);
                return;
            }

            str_len = strlen(name);
            memset(buffer, 0, buflen);
            buffer[0] = type;
            buffer[1] = str_len & 0xFF;
            buffer[2] = (str_len >> 8) & 0xFF;
            memcpy(buffer + 3, name, str_len);
            buffer[3 + str_len] = UX_SERIALIZE_SEPARETOR;
            str_len = strlen(str);
            buffer[4 + str_len] = str_len & 0xFF;
            buffer[5 + str_len] = (str_len >> 8) & 0xFF;
            memcpy(buffer + 6 + str_len, str, str_len);
            uint32_t crc = crc32_byte(buffer, buflen - 4);
            buffer[buflen - 4] = crc & 0xFF;
            buffer[buflen - 3] = (crc >> 8) & 0xFF;
            buffer[buflen - 2] = (crc >> 16) & 0xFF;
            buffer[buflen - 1] = (crc >> 24) & 0xFF;
            ux_write_bytes(component, buffer, buflen);
            free(buffer);
        }
    }
}
*/