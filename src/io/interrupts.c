#include "interrupts.h"
#include "io.h"

#include "../arch/gdt.h"
#include "../arch/tss.h"
#include "../arch/getcpuid.h"
#include "../util/dbgprinter.h"
#include "../memory/memory.h"
#include "../memory/heap.h"
#include "../memory/paging.h"
#include "../util/string.h"
#include "../dev/pci/pci.h"
#include "../dev/acpi/acpi.h"
#include "../dev/apic/apic.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/net/e1000/e1000c.h"
#include "../scheduling/scheduler.h"
#include "../scheduling/pit.h"
#include "../scheduling/sline.h"
#include "../syscall/syscall.h"

#define __UNDEFINED_HANDLER  __asm__ ("cli"); dbg_print(__func__); (void)frame; set_debug_msg(__func__); panic("Undefined interrupt handler");

struct idtr idtr;
volatile int dynamic_interrupt = -1;

void (*dynamic_interrupt_handlers[256])(struct interrupt_frame* frame) = {0};

void pic_eoi(unsigned char irq) {
    if (irq >= 12) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

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
    outb(PIC2_DATA, get_kernel_code_selector());
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
   uint64_t faulting_address;
   __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));
   char buffer[20];
   sprintf(buffer, "Page Fault Address: %x\n", faulting_address);
   set_debug_msg(buffer);
   panic("Page fault\n");
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

__attribute__((interrupt)) void Network_Handler(struct interrupt_frame* frame) {
    (void)frame;
    __asm__ volatile("cli");
    trigger_pci_interrupt();
    __asm__ volatile("sti");
    pic_end_master();
}

__attribute__((interrupt)) void Syscall_Handler(struct interrupt_frame* frame) {
    volatile uint64_t syscall_number, arg1, arg2, arg3, arg4, arg5, arg6;
    __asm__ volatile("mov %%rax, %0" : "=r"(syscall_number));
    __asm__ volatile("mov %%rdi, %0" : "=r"(arg1));
    __asm__ volatile("mov %%rsi, %0" : "=r"(arg2));
    __asm__ volatile("mov %%rdx, %0" : "=r"(arg3));
    __asm__ volatile("mov %%r10, %0" : "=r"(arg4));
    __asm__ volatile("mov %%r8, %0" : "=r"(arg5));
    __asm__ volatile("mov %%r9, %0" : "=r"(arg6));
    volatile uint64_t ret = syscall(syscall_number, arg1, arg2, arg3, arg4, arg5, arg6);
    __asm__ volatile("mov %0, %%rax" : : "r"(ret));
}

//you may need save_all here
__attribute__((interrupt)) void PitInt_Handler(struct interrupt_frame * frame) {
    __asm__ volatile("cli");
    tick();
    pic_end_master();
    if (requires_preemption()) {
        yield();
    } else if (requires_wakeup()) {
        wakeup();
    }
    __asm__ volatile("sti");
}

__attribute__((interrupt)) void Serial1Int_Handler(struct interrupt_frame * frame) {
    (void)frame;
    char c = inb(0x3f8);
    dbg_print("Serial1: ");
    dbg_print(itoa(c, 16));
    dbg_print("\n");
    pic_end_master();

}

__attribute__((interrupt)) void Serial2Int_Handler(struct interrupt_frame * frame) {
    (void)frame;
    char c = inb(0x3f8);
    dbg_print("Serial2: ");
    dbg_print(itoa(c, 16));
    dbg_print("\n");
    pic_end_master();
}

__attribute__((interrupt)) void KReturn_Handler(struct interrupt_frame_error * frame) {
    __asm__ volatile("cli");
    panic("NOT IMPLEMENTED");
    //return_from_kernel(frame);
    __asm__ volatile("sti");
}

__attribute__((interrupt)) void DynamicInt_Handler(struct interrupt_frame * frame) {
    (void)frame;
    if (dynamic_interrupt < 0) {
        dbg_print("Dynamic interrupt called without being hooked\n");
        return;
    }
    
    void (*handler)(struct interrupt_frame * frame) = (void*)dynamic_interrupt_handlers[dynamic_interrupt];
    if (handler == 0 || handler == (void*)DynamicInt_Handler) {
        dbg_print("Dynamic interrupt called without being hooked\n");
        return;
    }

    handler(frame);
}

struct idtdescentry * get_idt_gate(uint8_t entry_offset) {
    return (struct idtdescentry*)(idtr.offset + (entry_offset * sizeof(struct idtdescentry)));
}

void irq_set_mask(unsigned char interrupt) {
    uint16_t port;
    uint8_t value;
 
    if(interrupt < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        interrupt -= 8;
    }
    value = inb(port) | (1 << interrupt);
    outb(port, value);        
}
 
void irq_clear_mask(unsigned char interrupt) {
    uint16_t port;
    uint8_t value;
 
    if(interrupt < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        interrupt -= 8;
    }
    value = inb(port) & ~(1 << interrupt);
    outb(port, value);        
}

void set_idt_gate(uint64_t handler, uint8_t entry_offset, uint8_t type_attr, uint8_t ist, uint16_t selector) {
    struct idtdescentry* interrupt = (struct idtdescentry*)(idtr.offset + (entry_offset * sizeof(struct idtdescentry)));
    set_offset(interrupt, handler);
    interrupt->type_attr.raw = type_attr;
    interrupt->selector = selector;
    interrupt->ist = ist;
}

void hook_interrupt(uint8_t interrupt, void* handler, uint8_t dynamic) {
    __asm__("cli");
    if (dynamic) {
        if (dynamic_interrupt_handlers[interrupt] != 0) {
            dbg_print("Dynamic interrupt already hooked\n");
            return;
        }
        dynamic_interrupt_handlers[interrupt] = handler;
    } else {
        set_idt_gate((uint64_t)handler, interrupt, IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
        irq_clear_mask(interrupt);
    }
    __asm__("sti");
}

void unhook_interrupt(uint8_t interrupt, uint8_t dynamic) {
    __asm__("cli");
    if (dynamic) {
        dynamic_interrupt_handlers[interrupt] = (void*)0;
    } else {
        set_idt_gate((uint64_t)UncaughtInt_Handler, interrupt, IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
        irq_set_mask(interrupt);
    }
    __asm__("sti");
}

void init_interrupts(uint8_t pit_disable) {
    dbg_print("### INTERRUPTS STARTUP ###\n");
    __asm__("cli");

    
    if (check_apic()) {

        struct madt_header* madt = get_acpi_madt();
        if (madt != 0) {
            register_apic(madt, 0x0);
        }

        __asm__("sti");
        return;
    }

    idtr.limit = 256 * sizeof(struct idtdescentry) - 1;
    idtr.offset = (uint64_t)request_current_page_identity();
    memset((void*)idtr.offset, 0, 256 * sizeof(struct idtdescentry));

    for (int i = 0; i < 256; i++) {
        set_idt_gate((uint64_t)UncaughtInt_Handler, i, IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    }

    set_idt_gate((uint64_t)PageFault_Handler,   0xE,    IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)DoubleFault_Handler, 0x8,    IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)GPFault_Handler,     0xD,    IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)KeyboardInt_Handler, 0x21,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)MouseInt_Handler,    0x2C,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)Serial1Int_Handler,   0x24,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)Serial2Int_Handler,  0x23,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)PitInt_Handler,      0x20,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)DivByZero_Handler,   0x0,    IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)NMI_Handler,         0x2,    IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)Overflow_Handler,    0x4,    IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)BoundRangeExceeded_Handler, 0x5, IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)InvalidOpc_Handler,  0x6,    IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)NoCoproc_Handler,    0x7,    IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)CoprocSegmentOverrun_Handler, 0x9, IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)INVTSS_Handler,      0xA,    IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)Network_Handler, 0xB,  IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)StackSegmentFault_Handler, 0xC, IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)x87FPE_Handler,      0x10,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)AlignCheck_Handler,  0x11,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)MachineCheck_Handler, 0x12,  IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)SIMD_FPE_Handler,    0x13,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)Virtualization_Handler, 0x14, IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)Security_Handler,    0x1E,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)Syscall_Handler,     0x80,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)DynamicInt_Handler,  0x90,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    set_idt_gate((uint64_t)KReturn_Handler,  0x8A,   IDT_TA_InterruptGate, IST_Interrupts, get_kernel_code_selector());
    

    uint64_t stackinterrupts = (uint64_t)stackalloc(STACKSIZE) + STACKSIZE;
    tss_set_ist(0, IST_Interrupts, stackinterrupts);

    __asm__ volatile("lidt %0" : : "m"(idtr));

    remap_pic();

    init_keyboard();
    
    //0xe is on binary 1110, so we mask the first 4 bits
    outb(PIC1_DATA, 0xe0 + pit_disable); //PIT IS DISABLED
    outb(PIC2_DATA, 0xef);

    irq_clear_mask(0xb);

    dbg_print("Interrupts initialized\n");
    __asm__("sti");

}

void raise_interrupt(uint8_t interrupt) {
    dynamic_interrupt = interrupt;
    __asm__("int %0" : : "i"(DYNAMIC_HANDLER));
}