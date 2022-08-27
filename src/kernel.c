#include "bootservices/bootservices.h"

#include "util/dbgprinter.h"
#include "util/printf.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "io/interrupts.h"
#include "util/string.h"
#include "memory/heap.h"
#include "drivers/keyboard.h"
#include "dev/acpi.h"
#include "scheduling/pit.h"
#include "dev/smbios_interface.h"
#include "process/process.h"

CPU_CONTEXT saved_context;
int ready = 0;

void t1() {
    int i = 10;
    while (i--)
        kwritest('a');
}

void t2() {
    int i = 10;
    while (i--)
        kwritest('b');
}

void kyieldtest() {
    CPU_CONTEXT context;
    SAVE_CONTEXT(&context);
    if (ready) {
        CPU_CONTEXT saved;
        memcpy(&saved, &saved_context, sizeof(CPU_CONTEXT));
        memcpy(&saved_context, &context, sizeof(CPU_CONTEXT));
        setContext(&saved);
    } else {
        memcpy(&saved_context, &context, sizeof(CPU_CONTEXT));
        ready = 1;
        t2();
    }
}

void kwritest(const char chr) {
    kyieldtest();
    printf("%c\n", chr);
}

void _start(void) {
    init_memory();
    init_paging();
    init_heap();
    init_pit();
    init_interrupts();
    init_smbios_interface();
    t1();
    while(1);
}