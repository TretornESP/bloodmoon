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

extern void* interrupt_vector[IDT_ENTRY_COUNT];

uint8_t interrupts_ready = 0;
struct idtr idtr;
volatile int dynamic_interrupt = -1;

void (*dynamic_interrupt_handlers[256])(struct cpu_context* ctx, uint8_t cpuid) = {0};

void PageFault_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    uint64_t faulting_address;
    __asm__ volatile("mov %%cr2, %0" : "=r" (faulting_address));
    char buffer[20];
    sprintf(buffer, "Page Fault Address: %x\n", faulting_address);
    set_debug_msg(buffer);
    panic("Page fault\n");
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

void KeyboardInt_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    uint8_t scancode = inb(0x60);
    handle_keyboard(scancode);
}

void Network_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    trigger_pci_interrupt();
}

void Syscall_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
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
void PitInt_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    tick();
    if (requires_preemption()) {
        yield();
    } else if (requires_wakeup()) {
        wakeup();
    }
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

void set_idt_gate(uint64_t handler, uint8_t entry_offset, uint8_t type_attr, uint8_t ist, uint16_t selector) {
    struct idtdescentry* interrupt = (struct idtdescentry*)(idtr.offset + (entry_offset * sizeof(struct idtdescentry)));
    set_offset(interrupt, handler);
    interrupt->type_attr.raw = type_attr;
    interrupt->selector = selector;
    interrupt->ist = ist;
}

void hook_interrupt(uint8_t interrupt, void* handler) {
    __asm__("cli");
    if (dynamic_interrupt_handlers[interrupt] != 0) {
        dbg_print("Dynamic interrupt already hooked\n");
        return;
    }
    dynamic_interrupt_handlers[interrupt] = handler;
    __asm__("sti");
}

void unhook_interrupt(uint8_t interrupt) {
    __asm__("cli");
    dynamic_interrupt_handlers[interrupt] = (void*)0;
    __asm__("sti");
}

void enable_interrupts() {
    __asm__("sti");
}

void load_interrupts_for_local_cpu() {
    if (interrupts_ready) {
        __asm__("cli");
        __asm__("lidt %0" : : "m"(idtr));
    } else {
        panic("Interrupts not ready\n");
    }
}

void init_interrupts(uint8_t pit_disable) {
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
    dynamic_interrupt_handlers[0xB] = Network_Handler;
    dynamic_interrupt_handlers[0xD] = GPFault_Handler;
    dynamic_interrupt_handlers[0xE] = PageFault_Handler;
    dynamic_interrupt_handlers[0x20] = PitInt_Handler;
    dynamic_interrupt_handlers[0x21] = KeyboardInt_Handler;
    dynamic_interrupt_handlers[0x23] = Serial2Int_Handler;
    dynamic_interrupt_handlers[0x24] = Serial1Int_Handler;
    dynamic_interrupt_handlers[0x80] = Syscall_Handler;
    
    interrupts_ready = 1;
    return;
}

void raise_interrupt(uint8_t interrupt) {
    __asm__("cli");
    dynamic_interrupt = interrupt;
    __asm__("int %0" : : "i"(DYNAMIC_HANDLER));
}

void global_interrupt_handler(struct cpu_context* ctx, uint8_t cpu_id) {
    __asm__("cli");
    
    void (*handler)(struct cpu_context* ctx, uint8_t cpu_id) = (void*)interrupt_vector[ctx->interrupt_number];
    
    if (ctx->interrupt_number == DYNAMIC_HANDLER) {
        if (dynamic_interrupt != 0 && dynamic_interrupt != DYNAMIC_HANDLER) {   
            handler = (void*)dynamic_interrupt_handlers[dynamic_interrupt];
            dynamic_interrupt = 0;
        } else {
            panic("Invalid dynamic interrupt\n");
        }
    }

    printf("Interrupt %d received on CPU %d\n", ctx->interrupt_number, cpu_id);

    if (handler == 0) {
        panic("No handler for interrupt\n");
    }

    handler(ctx, cpu_id);

    local_apic_eoi(cpu_id);
    __asm__("sti");
}
