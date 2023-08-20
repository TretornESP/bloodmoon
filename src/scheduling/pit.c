#include "pit.h"
#include "../util/printf.h"
#include "../io/io.h"
#include "scheduler.h"
#include "../bootservices/bootservices.h"

struct pit pit;

#define PIT_A 0x40
#define PIT_B 0x41
#define PIT_C 0x42
#define PIT_CONTROL 0x43

#define PIT_MASK 0xFF
#define PIT_SCALE 1193180
#define PIT_SET 0x36

#define TIMER_IRQ 0

#define SUBTICKS_PER_TICK 100

void timer_phase(int hz) {
	int divisor = PIT_SCALE / hz;
	outb(PIT_CONTROL, PIT_SET);
	outb(PIT_A, divisor & PIT_MASK);
	outb(PIT_A, (divisor >> 8) & PIT_MASK);
}

void init_pit(int hertz) {
    printf("### PIT STARTUP ###\n");
    pit.boot_epoch = get_boot_time();
    pit.timer_ticks = 0;
    pit.timer_subticks = 0;
    pit.preemption_ticks = 0;
    pit.preemption_enabled = 0;
    pit.ticks_per_second = hertz;
    
    timer_phase(hertz);
}

void tick() {
    //printf("Tick %d\n", pit.timer_ticks);
    if (++pit.timer_subticks == SUBTICKS_PER_TICK) {
        pit.timer_ticks++;
        pit.timer_subticks = 0;
    }
}

void set_preeption_ticks(uint64_t ticks) {
    pit.preemption_ticks = ticks;
}

void preempt_toggle() {
    pit.preemption_enabled = !pit.preemption_enabled;
}

uint8_t requires_preemption() {
    if (!pit.preemption_enabled || !pit.preemption_ticks) return 0;
    return pit.timer_ticks % pit.preemption_ticks == 0;
}

void enable_preemption() {
    pit.preemption_enabled = 1;
}

void sleep_ticks(uint64_t ticks) {
    uint64_t start_time = pit.timer_ticks;
    //This is obsolete in the multitasking world!!!
    while (pit.timer_ticks - start_time < ticks) {
        __asm__ volatile("hlt");
    }
}

void sleep(uint64_t seconds) {
    sleep_ticks(seconds * pit.ticks_per_second);
}

uint64_t get_ticks_since_boot() {
    return pit.timer_ticks;
}

uint64_t get_epoch() {
    return pit.boot_epoch + (pit.timer_ticks / pit.ticks_per_second);
}