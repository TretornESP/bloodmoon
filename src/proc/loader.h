#ifndef _LOADER_H
#define _LOADER_H

#include <stdint.h>

struct proc_ld {
    void* at_phdr;
    char* ld_path;
};

struct auxv{
    uint64_t a_type;
    void* a_val;
};


void elf_readelf(uint8_t * buffer, uint64_t size);
uint8_t elf_load_elf(uint8_t * buffer, uint64_t size, int argc, char* argv[], char *envp[]);
#endif