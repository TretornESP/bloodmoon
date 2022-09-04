#include "loader.h"
#include "../util/string.h"
#include <elf.h>

void parse_elf_file(uint8_t * buffer, uint64_t size) {

    Elf64_Ehdr * elf_header = (Elf64_Ehdr *) buffer;
    if (memcmp(elf_header->e_ident, ELFMAG, SELFMAG) != 0) {
        // Not an ELF file
        return;
    }
}