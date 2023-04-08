#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H
#include "idt.h"

#define DYNAMIC_HANDLER 0x90

struct interrupt_frame;
__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void GPFault_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void KeyboardInt_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void MouseInt_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void PitInt_Handler(struct interrupt_frame *);

void init_interrupts(uint8_t);
void hook_interrupt(uint8_t interrupt, void* handler, uint8_t dynamic);
void unhook_interrupt(uint8_t interrupt, uint8_t dynamic);
void pic_eoi(unsigned char irq);
void raise_interrupt(uint8_t interrupt);
#endif