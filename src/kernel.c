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

void _start(void) {

    init_memory();
    init_paging();
    init_heap();
    init_pit();
    init_interrupts();
    init_smbios_interface();

    dump_smbios();
    while(1);
}