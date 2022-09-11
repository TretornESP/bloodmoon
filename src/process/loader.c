#include "loader.h"
#include "../util/string.h"
#include "../util/printf.h"
#include <elf.h>

void parse_elf_file(uint8_t * buffer) {

    Elf64_Ehdr * elf_header = (Elf64_Ehdr *) buffer;
    if (memcmp(elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Invalid ELF file\n");
        return;
    }

    printf("Valid ELF Detected\n");
}