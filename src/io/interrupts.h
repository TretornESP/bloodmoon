#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H
#include "idt.h"

struct interrupt_frame;
__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void GPFault_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void KeyboardInt_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void MouseInt_Handler(struct interrupt_frame *);
__attribute__((interrupt)) void PitInt_Handler(struct interrupt_frame *);

void init_interrupts(uint8_t);

#endif