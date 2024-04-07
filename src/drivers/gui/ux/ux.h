#ifndef __UX_H
#define __UX_H

#include <stdint.h>

#define UX_MAX_WINDOWS          0x10

#define UX_TYPE_BUTTON          0x00
#define UX_TYPE_LABEL           0x01
#define UX_TYPE_IMAGE           0x02
#define UX_TYPE_TEXT_INPUT      0x03
#define UX_TYPE_TEXT_AREA       0x04
#define UX_TYPE_CHECKBOX        0x05
#define UX_TYPE_RADIO           0x06
#define UX_TYPE_DROPDOWN        0x07
#define UX_TYPE_SLIDER          0x08
#define UX_TYPE_TAB             0x09
#define UX_TYPE_PANEL           0x0A
#define UX_TYPE_WINDOW          0x0B

#define UX_DATATYPE_STRING      0x01
#define UX_DATATYPE_INT         0x02
#define UX_DATATYPE_DOUBLE      0x03
#define UX_DATATYPE_BOOL        0x04

#define UX_COORDS_ABSOLUTE      0x00
#define UX_COORDS_RELATIVE      0x01

#define UX_MAX_EXTRA_DATA       0x1000

#define UX_SERIALIZE_SEPARETOR  0x05

struct ux_component {
    uint8_t type;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint8_t coord_type;
    void * specific_data;
    uint64_t specific_data_bytes;
    uint64_t children_items;
    
    struct ux_component * parent;
    struct ux_component ** children;
};

void print_bg();
void spawn_tty();
#endif