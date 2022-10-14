#include "interrupts.h"
#include "io.h"

#include "../util/dbgprinter.h"
#include "../memory/memory.h"
#include "../drivers/keyboard.h"
#include "../scheduling/pit.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

#define PIC_EOI 0x20

struct idtr idtr;

void pic_end_master() {
    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_end_slave() {
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void remap_pic() {
    uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);
    io_wait();
}


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
    (void)frame;
    uint8_t scancode = inb(0x60);
    handle_keyboard(scancode);
    pic_end_master();
}

__attribute__((interrupt)) void MouseInt_Handler(struct interrupt_frame * frame) {
    dbg_print("MouseInt_Handler\n");
    (void)frame;
    while(1);
}

__attribute__((interrupt)) void DivByZero_Handler(struct interrupt_frame * frame) {
    dbg_print("DivByZero_Handler\n");
    (void)frame;
    while(1);
}

//you may need save_all here
__attribute__((interrupt)) void PitInt_Handler(struct interrupt_frame * frame) {
    (void)frame;
    tick();
    pic_end_master();
}

void set_idt_gate(uint64_t handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector) {
    struct idtdescentry* interrupt = (struct idtdescentry*)(idtr.offset + entry_offset * sizeof(struct idtdescentry));
    set_offset(interrupt, handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}


void init_interrupts(uint8_t pit_disable) {

    __asm__("cli");

    idtr.limit = 0x0FFF;
    idtr.offset = (uint64_t)request_page();

    set_idt_gate((uint64_t)PageFault_Handler,   0xE,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)DoubleFault_Handler, 0x8,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)GPFault_Handler,     0xD,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)KeyboardInt_Handler, 0x21,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)MouseInt_Handler,    0x2C,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)PitInt_Handler,      0x20,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)DivByZero_Handler,   0x0,    IDT_TA_InterruptGate, 0x28);
    __asm__("lidt %0" : : "m"(idtr));

    remap_pic();

    init_keyboard();

    outb(PIC1_DATA, 0xf8 + pit_disable); //PIT IS DISABLED
    outb(PIC2_DATA, 0xef);

    __asm__("sti");

}
