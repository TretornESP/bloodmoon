#include "interrupts.h"
#include "idt.h"
#include "io.h"

#include "../cpus/cpus.h"
#include "../arch/gdt.h"
#include "../arch/tss.h"
#include "../arch/getcpuid.h"
#include "../arch/ints.h"
#include "../util/dbgprinter.h"
#include "../memory/memory.h"
#include "../memory/heap.h"
#include "../memory/paging.h"
#include "../util/string.h"
#include "../dev/pci/pci.h"
#include "../dev/acpi/acpi.h"
#include "../dev/apic/apic.h"
#include "../drivers/ps2/keyboard.h"
#include "../drivers/net/e1000/e1000c.h"
#include "../scheduling/scheduler.h"
#include "../scheduling/pit.h"
#include "../scheduling/sline.h"
#include "../syscall/syscall.h"
#define __UNDEFINED_HANDLER CLI(); dbg_print(__func__); (void)frame; set_debug_msg(__func__); panic("Undefined interrupt handler");

extern void* interrupt_vector[IDT_ENTRY_COUNT];
char io_tty[32] = "default\0";
char saved_tty[32];
uint8_t interrupts_ready = 0;
struct idtr idtr;
volatile int dynamic_interrupt = -1;

struct stackFrame {
    struct stackFrame * rbp;
    uint64_t rip;
};

void walk_stack(unsigned int MaxFrames) {
    struct stackFrame * frame;
    __asm__ volatile("mov %%rbp, %0" : "=r" (frame));
}

void (*dynamic_interrupt_handlers[256])(struct cpu_context* ctx, uint8_t cpuid) = {0};

void PageFault_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    uint64_t faulting_address;
    __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));
    char buffer[128];
    sprintf(buffer, "Page Fault Address: %x\n", faulting_address);
    set_debug_msg(buffer);
    panic_reboot("Page fault\n");
}

void DoubleFault_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    panic("Double fault\n");
}

void GPFault_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    panic("General protection fault\n");
}

void PCI_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    trigger_pci_interrupt();
}

void Syscall_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    ctx->rax = syscall(ctx->rax, ctx->rdi, ctx->rsi, ctx->rdx, ctx->r10, ctx->r8, ctx->r10);
    __asm__ volatile("mov %0, %%rax" : : "r"(ctx->rax));
}

//you may need save_all here
void PitInt_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    CLI();
    (void)ctx;
    (void)cpuid;
    tick();
    if (requires_wakeup()) {
        wakeup();
    }
    if (requires_preemption()) {
        yield();
    } 
    STI();
}

void Serial1Int_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    char c = inb(0x3f8);
    dbg_print("Serial1: ");
    dbg_print(itoa(c, 16));
    dbg_print("\n");
}

void Serial2Int_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    char c = inb(0x3f8);
    dbg_print("Serial2: ");
    dbg_print(itoa(c, 16));
    dbg_print("\n");
}

static void interrupt_exception_handler(struct cpu_context* ctx, uint8_t cpu_id) {
    printf("GENERIC EXCEPTION %d ON CPU %d\n", ctx->interrupt_number, cpu_id);
    panic("Exception\n");
}

struct idtdescentry * get_idt_gate(uint8_t entry_offset) {
    return (struct idtdescentry*)(idtr.offset + (entry_offset * sizeof(struct idtdescentry)));
}

void set_io_tty(const char * tty) {
    memset(io_tty, 0, 32);
    strcpy(io_tty, tty);
}

void set_idt_gate(uint64_t handler, uint8_t entry_offset, uint8_t type_attr, uint8_t ist, uint16_t selector) {
    struct idtdescentry* interrupt = (struct idtdescentry*)(idtr.offset + (entry_offset * sizeof(struct idtdescentry)));
    set_offset(interrupt, handler);
    interrupt->type_attr.raw = type_attr;
    interrupt->selector = selector;
    interrupt->ist = ist;
}

void hook_interrupt(uint8_t interrupt, void* handler) {
    if (!interrupts_ready) panic("Interrupts not ready\n");
    CLI();
    dynamic_interrupt_handlers[interrupt] = handler;
    STI();
}

void unhook_interrupt(uint8_t interrupt) {
    if (!interrupts_ready) panic("Interrupts not ready\n");
    CLI();
    dynamic_interrupt_handlers[interrupt] = (void*)interrupt_exception_handler;
    STI();
}

void enable_interrupts() {
    if (!interrupts_ready) panic("Interrupts not ready\n");
    STI();
}

void load_interrupts_for_local_cpu() {
    if (interrupts_ready) {
        __asm__("cli");
        __asm__("lidt %0" : : "m"(idtr));
    } else {
        panic("Interrupts not ready\n");
    }
}

void init_interrupts() {
    dbg_print("### INTERRUPTS STARTUP ###\n");
    __asm__("cli");
    
    if (!check_apic()) {
        panic("APIC not found\n");
    }

    idtr.limit = 256 * sizeof(struct idtdescentry) - 1;
    idtr.offset = (uint64_t)request_current_page_identity();
    memset((void*)idtr.offset, 0, 256 * sizeof(struct idtdescentry));
    
    for (int i = 0; i < 256; i++) {
        set_idt_gate((uint64_t)interrupt_vector[i], i, IDT_TA_InterruptGate, 0, get_kernel_code_selector());
    }

    for (int i = 0; i < 32; i++) {
        dynamic_interrupt_handlers[i] = interrupt_exception_handler;
    }

    dynamic_interrupt_handlers[0x8] = DoubleFault_Handler;
    dynamic_interrupt_handlers[0xD] = GPFault_Handler;
    dynamic_interrupt_handlers[0xE] = PageFault_Handler;
    dynamic_interrupt_handlers[PCIA_IRQ] = PCI_Handler;
    dynamic_interrupt_handlers[PIT_IRQ] = PitInt_Handler;
    dynamic_interrupt_handlers[SR2_IRQ] = Serial2Int_Handler;
    dynamic_interrupt_handlers[SR1_IRQ] = Serial1Int_Handler;
    dynamic_interrupt_handlers[0x80] = Syscall_Handler;
    
    interrupts_ready = 1;
    return;
}

void raise_interrupt(uint8_t interrupt) {
    if (!interrupts_ready) panic("Interrupts not ready\n");
    dynamic_interrupt = interrupt;
    __asm__("int %0" : : "i"(DYNAMIC_HANDLER));
}

void global_interrupt_handler(struct cpu_context* ctx, uint8_t cpu_id) {
    void (*handler)(struct cpu_context* ctx, uint8_t cpu_id) = (void*)dynamic_interrupt_handlers[ctx->interrupt_number];
    
    if (ctx->interrupt_number == DYNAMIC_HANDLER) {
        if (dynamic_interrupt != 0 && dynamic_interrupt != DYNAMIC_HANDLER) {   
            handler = (void*)dynamic_interrupt_handlers[dynamic_interrupt];
            dynamic_interrupt = 0;
        } else {
            panic("Invalid dynamic interrupt\n");
        }
    }

    //Get current proces
    struct task* current_task = get_current_task();
    if (current_task != 0) {
        strcpy(saved_tty, get_current_tty());
        set_current_tty(io_tty);
    }

    //printf("Interrupt %d received on CPU %d\n", ctx->interrupt_number, cpu_id);

    if (handler == 0) {
        char buffer[32];
        sprintf(buffer, "No handler for interrupt 0x%x\n", ctx->interrupt_number);
        set_debug_msg(buffer);
        panic("No handler for interrupt !\n");
    }

    handler(ctx, cpu_id);

    if (current_task != 0) {
        set_current_tty(saved_tty);
    }

    local_apic_eoi(cpu_id);
}

void mask_interrupt(uint8_t irq) {
    if (!interrupts_ready) panic("Interrupts not ready\n");
    if (!ioapic_mask(irq, 0x0)) {
        panic("Failed to mask interrupt\n");
    }
}

void unmask_interrupt(uint8_t irq) {
    if (!interrupts_ready) panic("Interrupts not ready\n");
    printf("Unmasking interrupt %d\n", irq);
    if (!ioapic_mask(irq, 0x1)) {
        panic("Failed to unmask interrupt\n");
    }
}