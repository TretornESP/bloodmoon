#include "pit.h"
#include "../util/printf.h"
#include "../io/io.h"

struct pit pit;

void init_pit(uint64_t start_epoch) {
    printf("### PIT STARTUP ###\n");
    printf("WARNING: PIT TIMER CALIBRATION IS NOT IMPLEMENTED\n");
    printf("!!!DONT RELY ON PRECISE TIMING!!!\n");
    pit.divisor = 20; //1 KHz measured by trial-and-error
    pit.base_frequency = 1193182;
    pit.ticks_since_boot = 0;
    pit.boot_epoch = start_epoch;
    outb(0x40, (uint8_t)(pit.divisor & 0x00ff));
    io_wait();
    outb(0x40, (uint8_t)((pit.divisor & 0xff00) >> 8));
}

void tick() {
    pit.ticks_since_boot += 1;
}

void sleep_millis(uint64_t millis) {
    uint64_t start_time = pit.ticks_since_boot;

    while (pit.ticks_since_boot - start_time < millis) {
        __asm__ volatile("hlt");
    }
}

void sleep(uint64_t seconds) {
    sleep_millis(seconds * 1000);
}

uint64_t get_ticks_since_boot() {
    return pit.ticks_since_boot;
}

uint64_t get_epoch() {
    return pit.boot_epoch + (pit.ticks_since_boot / 1000);
}