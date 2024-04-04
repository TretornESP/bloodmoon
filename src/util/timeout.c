#include "timeout.h"
#include "../dev/smbios/smbios_interface.h"

#define DEFAULT_CPU_FREQ 2000

void timeout(uint64_t ms) {
    uint16_t current_speed = get_smbios_cpu_current_speed();

    if (current_speed == 0) current_speed = DEFAULT_CPU_FREQ;

    //Frequency is in MHz, so we need to convert it to Hz
    uint64_t hertz = current_speed * 1000000;

    uint64_t iterations = ms * hertz / 1000;
    for (uint64_t i = 0; i < iterations; i++) {
        __asm__ volatile("nop");
    }
}