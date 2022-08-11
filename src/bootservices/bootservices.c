#include "bootservices.h"
#include "limine_requests.h"

#define CALL_SERVICE(x) (x.response)

#define DEFAULT_TERMINAL_WRITER       0x0
uint64_t terminal_writer = DEFAULT_TERMINAL_WRITER;

void service_error() {
    while (1) {
        __asm__("hlt");
    }
}

void generic_terminal_callback(struct limine_terminal* terminal, uint64_t type, uint64_t p1, uint64_t p2, uint64_t p3) {
    (void)terminal;
    (void)type;
    (void)p1;
    (void)p2;
    (void)p3;
    //TODO
}

void set_terminal_extra_handler() {} //TODO

void generic_terminal_writer(const char* str, uint64_t length) {
    struct limine_terminal *terminal = CALL_SERVICE(TERMINAL)->terminals[terminal_writer];
    terminal_request.response->write(terminal, str, length);
}

void (*get_terminal_writer())(const char*, uint64_t) {
    return generic_terminal_writer;
}

char* get_bootloader_name() {
    return CALL_SERVICE(BOOTLOADER)->name;
}

char* get_bootloader_version() {
    return CALL_SERVICE(BOOTLOADER)->version;
}

uint64_t get_hhdm_offset() {
    return CALL_SERVICE(HHDM)->offset;
}

uint64_t get_terminal_count() {
    return CALL_SERVICE(TERMINAL)->terminal_count;
}

uint64_t get_current_terminal() {
    return terminal_writer;
}

uint64_t get_memory_map_entries() {
    return CALL_SERVICE(MEMMAP)->entry_count;
}

uint64_t get_memory_map_base(uint64_t index) {
    return CALL_SERVICE(MEMMAP)->entries[index]->base;
}
uint64_t get_memory_map_length(uint64_t index) {
    return CALL_SERVICE(MEMMAP)->entries[index]->length;
}
uint64_t get_memory_map_type(uint64_t index) {
    return CALL_SERVICE(MEMMAP)->entries[index]->type;
}

void set_terminal_writer(uint64_t terminal) {
    if (terminal >= get_terminal_count()) {
        service_error();
    }
    terminal_writer = terminal;
}
