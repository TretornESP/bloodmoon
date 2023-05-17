#include "timeout.h"
#include "../dev/smbios/smbios_interface.h"

#define DEFAULT_CPU_FREQ 2000

void timeout(uint64_t ms) {
    uint16_t * current_speed = get_smbios_cpu_current_speed();
    uint16_t default_freq = DEFAULT_CPU_FREQ;

    if (current_speed == 0 || *current_speed == 0)
        current_speed = &default_freq;

    //Frequency is in MHz, so we need to convert it to Hz
    uint64_t hertz = *current_speed * 1000000;

    uint64_t iterations = ms * hertz / 1000;
    for (uint64_t i = 0; i < iterations; i++) {
        __asm__ volatile("nop");
    }
}