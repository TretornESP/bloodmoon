#ifndef _IDT_H
#define _IDT_H
#include <stdint.h>

#define IDT_TA_InterruptGate 0x8E
#define IDT_TA_CallGate      0x8C
#define IDT_TA_TrapGate      0x8F

struct idtdescentry {
    uint16_t offset0;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t ignore;
} __attribute__((packed));

void set_offset(struct idtdescentry *, uint64_t);
uint64_t get_offset(struct idtdescentry *);

struct idtr {
    uint16_t limit;
    uint64_t offset;
} __attribute__((packed));
#endif