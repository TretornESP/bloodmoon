#include "interrupts.h"
#include "io.h"

#include "../util/dbgprinter.h"
#include "../memory/memory.h"

struct idtr idtr;

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame * frame) {
    dbg_print("PageFault_Handler\n");
    (void)frame;
    while(1);
}

__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame * frame) {
    dbg_print("DoubleFault_Handler\n");
    (void)frame;
    while(1);
}

__attribute__((interrupt)) void GPFault_Handler(struct interrupt_frame * frame) {
    dbg_print("GPFault_Handler\n");
    (void)frame;
    while(1);
}

__attribute__((interrupt)) void KeyboardInt_Handler(struct interrupt_frame * frame) {
    dbg_print("KeyboardInt_Handler\n");
    (void)frame;
    while(1);
}

__attribute__((interrupt)) void MouseInt_Handler(struct interrupt_frame * frame) {
    dbg_print("MouseInt_Handler\n");
    (void)frame;
    while(1);
}

void set_idt_gate(uint64_t handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector) {
    struct idtdescentry* interrupt = (struct idtdescentry*)(idtr.offset + entry_offset * sizeof(struct idtdescentry));
    set_offset(interrupt, handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}


void init_interrupts() {

    __asm__("cli");

    idtr.limit = 0x0FFF;
    idtr.offset = (uint64_t)request_page();

    set_idt_gate((uint64_t)PageFault_Handler,   0xE,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)DoubleFault_Handler, 0x8,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)GPFault_Handler,     0xD,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)KeyboardInt_Handler, 0x21,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)MouseInt_Handler,    0x2C,   IDT_TA_InterruptGate, 0x28);

    __asm__("lidt %0" : : "m"(idtr));

    __asm__("sti");

}
