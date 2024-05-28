#include "interrupts.h"
#include "idt.h"
#include "io.h"

#include "../cpus/cpus.h"
#include "../arch/gdt.h"
#include "../arch/tss.h"
#include "../arch/getcpuid.h"
#include "../util/dbgprinter.h"
#include "../memory/memory.h"
#include "../memory/heap.h"
#include "../memory/paging.h"
#include "../util/string.h"
#include "../devices/pci/pci.h"
#include "../devices/acpi/acpi.h"
#include "../devices/apic/apic.h"
#include "../drivers/ps2/keyboard.h"
#include "../drivers/net/e1000/e1000c.h"
#include "../sched/scheduler.h"
#include "../sched/pit.h"
#include "../sched/sline.h"
#include "../syscall/syscall.h"
#define __UNDEFINED_HANDLER __asm__("cli"); dbg_print(__func__); (void)frame; panic("Undefined interrupt handler");

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

void walk_stack(struct cpu_context* ctx, uint8_t cpuid, const unsigned int MaxFrames) {
    struct stackFrame * frame = (struct stackFrame *)ctx->rbp;
    dbg_print("Walking stack for CPU:");
    dbg_print(itoa(cpuid, 10));
    dbg_print("\n");
    uint64_t i = 0;
    while (frame && i < MaxFrames) {
        dbg_print("Frame: ");
        dbg_print(itoa(i, 10));
        dbg_print(" RBP: ");
        dbg_print(itoa((uint64_t)frame, 16));
        dbg_print(" RIP: ");
        dbg_print(itoa(frame->rip, 16));
        dbg_print("\n");
        frame = frame->rbp;
        i++;
    }
    dbg_print("End of stack walk\n");
}

void (*dynamic_interrupt_handlers[256])(struct cpu_context* ctx, uint8_t cpuid) = {0};

void PageFault_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    uint64_t faulting_address;
    __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));
    dbg_print("Page Fault Address: ");
    dbg_print(itoa((uint64_t)faulting_address, 16));
    dbg_print("\n");
    walk_stack(ctx, cpuid, 32);
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
    (void)ctx;
    (void)cpuid;
    tick();
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
    dynamic_interrupt_handlers[interrupt] = handler;
}

void unhook_interrupt(uint8_t interrupt) {
    if (!interrupts_ready) panic("Interrupts not ready\n");
    dynamic_interrupt_handlers[interrupt] = (void*)interrupt_exception_handler;
}

void load_interrupts_for_local_cpu() {
    if (interrupts_ready) {
        __asm__("lidt %0" : : "m"(idtr));
    } else {
        panic("Interrupts not ready\n");
    }
}

void init_interrupts() {
    dbg_print("### INTERRUPTS STARTUP ###\n");
    
    if (!check_apic()) {
        panic("APIC not found\n");
    }

    idtr.limit = 256 * sizeof(struct idtdescentry) - 1;
    idtr.offset = (uint64_t)TO_KERNEL_MAP(request_page());
    memset((void*)idtr.offset, 0, 256 * sizeof(struct idtdescentry));
    mprotect_current((void*)idtr.offset, 256 * sizeof(struct idtdescentry), PAGE_USER_BIT | PAGE_WRITE_BIT);

    for (int i = 0; i < 256; i++) {
        set_idt_gate((uint64_t)interrupt_vector[i], i, IDT_TA_InterruptGate, 1, get_kernel_code_selector());
    }

    set_idt_gate((uint64_t)DoubleFault_Handler, 8, IDT_TA_InterruptGate, 1, get_kernel_code_selector());
    mprotect_current((void*)idtr.offset, 256 * sizeof(struct idtdescentry), PAGE_USER_BIT);


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

const char * get_io_tty() {
    return io_tty;
}

void global_interrupt_handler(struct cpu_context* ctx, uint8_t cpu_id) {
    notify_eoi_required(ctx->interrupt_number);
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
        current_task->tty = current_task->io_tty;
    }

    //printf("Interrupt %d received on CPU %d\n", ctx->interrupt_number, cpu_id);

    if (handler == 0) {
        dbg_print("No handler for interrupt ");
        dbg_print(itoa(ctx->interrupt_number, 16));
        dbg_print("\n");
        panic("No handler for interrupt !\n");
    }

    handler(ctx, cpu_id);

    if (current_task != 0) {
        current_task->tty = current_task->regular_tty;
    }

    if (ctx->interrupt_number == PIT_IRQ) {
        if (requires_wakeup()) {
            wakeup();
            local_apic_eoi(cpu_id, ctx->interrupt_number);
        } else if (requires_preemption()) {
            local_apic_eoi(cpu_id, ctx->interrupt_number);
            yield();
        } else {
            local_apic_eoi(cpu_id, ctx->interrupt_number);
        }
    } else {
        local_apic_eoi(cpu_id, ctx->interrupt_number);
    }
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