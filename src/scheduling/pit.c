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
    if (hertz < 100 || hertz % SUBTICKS_PER_TICK || (hertz < SUBTICKS_PER_TICK)) {
        printf("Invalid PIT frequency %d\n", hertz);
        return;
    }
    pit.boot_epoch = get_boot_time();
    pit.timer_ticks = 0;
    pit.timer_subticks = 0;
    pit.preemption_ticks = 0;
    pit.preemption_enabled = 0;
    pit.hertz = hertz;
    pit.wakeup_handler = (void*)0x0;
    pit.wakeup_ticks = 0;

    printf("Hertz: %d\n", hertz);
    
    timer_phase(hertz);
}

void tick() {
    //printf("Tick %d\n", pit.timer_ticks);
    if (++pit.timer_subticks == SUBTICKS_PER_TICK) {
        pit.timer_ticks++;
        pit.timer_subticks = 0;
        if (pit.wakeup_handler != 0x0 && pit.wakeup_ticks != 0 && pit.timer_ticks % pit.wakeup_ticks) {
            pit.wakeup_handler();
        }
    }
}

uint64_t seconds_to_ticks(uint64_t seconds) {
    return seconds * pit.hertz;
}

uint64_t ticks_to_seconds(uint64_t ticks) {
    if (pit.hertz == 0) return 0;
    if (ticks == 0) return 0;
    if (ticks < pit.hertz) return 1;
    return ticks / pit.hertz;
}

uint64_t ticks_to_ms(uint64_t ticks) {
    if (pit.hertz == 0) return 0;
    if (ticks == 0) return 0;
    if ((ticks * 1000)< pit.hertz) return 1000;
    return (ticks * 1000) / pit.hertz;
}

uint64_t ms_to_ticks(uint64_t ms) {
    printf("MS: %d PITH: %ld\n", ms, pit.hertz);
    uint64_t cuak = ms * pit.hertz;
    printf("CUAK: %d\n", cuak);
    return cuak / 1000;
}

void set_wakeup_call(void (* handler)(), uint64_t ticks) {
    if (handler == 0x0) {
        pit.wakeup_handler = (void*)0x0;
        pit.wakeup_ticks = 0;
    } else {
        pit.wakeup_handler = handler;
        pit.wakeup_ticks = ticks;
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
        yield();
    }
}

void sleep(uint64_t seconds) {
    sleep_ticks(seconds_to_ticks(seconds));
}

uint64_t get_ticks_since_boot() {
    return pit.timer_ticks;
}


uint64_t get_epoch() {
    return pit.boot_epoch + ticks_to_seconds(pit.timer_ticks);
}