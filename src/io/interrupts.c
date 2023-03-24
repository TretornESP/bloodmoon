#include "interrupts.h"
#include "io.h"

#include "../util/dbgprinter.h"
#include "../memory/memory.h"
#include "../memory/paging.h"
#include "../util/string.h"
#include "../drivers/keyboard/keyboard.h"
#include "../scheduling/pit.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

#define PIC_EOI 0x20

#define __UNDEFINED_HANDLER dbg_print(__func__); (void)frame; panic("Undefined interrupt handler");

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
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame * frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void GPFault_Handler(struct interrupt_frame * frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void KeyboardInt_Handler(struct interrupt_frame * frame) {
    (void)frame;
    uint8_t scancode = inb(0x60);
    handle_keyboard(scancode);
    pic_end_master();
}

__attribute__((interrupt)) void MouseInt_Handler(struct interrupt_frame * frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void DivByZero_Handler(struct interrupt_frame * frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void UncaughtInt_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void NMI_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void Overflow_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void BoundRangeExceeded_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void InvalidOpc_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void NoCoproc_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void CoprocSegmentOverrun_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void INVTSS_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void SegNotPresent_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void StackSegmentFault_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void x87FPE_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void AlignCheck_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void MachineCheck_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void SIMD_FPE_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void Virtualization_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

__attribute__((interrupt)) void Security_Handler(struct interrupt_frame* frame) {
    __UNDEFINED_HANDLER
}

//you may need save_all here
__attribute__((interrupt)) void PitInt_Handler(struct interrupt_frame * frame) {
    (void)frame;
    tick();
    pic_end_master();
}

struct idtr idtr;
void set_idt_gate(uint64_t handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector) {
    struct idtdescentry* interrupt = (struct idtdescentry*)(idtr.offset + (entry_offset * sizeof(struct idtdescentry)));
    set_offset(interrupt, handler);
    interrupt->type_attr = type_attr;
    interrupt->selector = selector;
}


void init_interrupts(uint8_t pit_disable) {
    dbg_print("### INTERRUPTS STARTUP ###\n");
    __asm__("cli");
    idtr.limit = 256 * sizeof(struct idtdescentry) - 1;
    idtr.offset = (uint64_t)request_page_identity();
    memset((void*)idtr.offset, 0, 256 * sizeof(struct idtdescentry));

    for (int i = 0; i < 256; i++) {
        set_idt_gate((uint64_t)UncaughtInt_Handler, i, IDT_TA_InterruptGate, 0x28);
    }

    set_idt_gate((uint64_t)PageFault_Handler,   0xE,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)DoubleFault_Handler, 0x8,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)GPFault_Handler,     0xD,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)KeyboardInt_Handler, 0x21,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)MouseInt_Handler,    0x2C,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)PitInt_Handler,      0x20,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)DivByZero_Handler,   0x0,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)NMI_Handler,         0x2,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)Overflow_Handler,    0x4,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)BoundRangeExceeded_Handler, 0x5, IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)InvalidOpc_Handler,  0x6,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)NoCoproc_Handler,    0x7,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)CoprocSegmentOverrun_Handler, 0x9, IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)INVTSS_Handler,      0xA,    IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)SegNotPresent_Handler, 0xB,  IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)StackSegmentFault_Handler, 0xC, IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)x87FPE_Handler,      0x10,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)AlignCheck_Handler,  0x11,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)MachineCheck_Handler, 0x12,  IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)SIMD_FPE_Handler,    0x13,   IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)Virtualization_Handler, 0x14, IDT_TA_InterruptGate, 0x28);
    set_idt_gate((uint64_t)Security_Handler,    0x1E,   IDT_TA_InterruptGate, 0x28);
    
    __asm__ volatile("lidt %0" : : "m"(idtr));

    remap_pic();

    init_keyboard();

    outb(PIC1_DATA, 0xf8 + pit_disable); //PIT IS DISABLED
    outb(PIC2_DATA, 0xef);
    dbg_print("Interrupts initialized\n");
    __asm__("sti");

}
