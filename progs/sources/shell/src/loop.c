#include <stdio.h>

const char hw[] = "Hello, World!\n";

int main(int argc, char* argv) {
    (void)argc;
    (void)argv;
    __asm__("mov $1, %%rax\n"
            "mov $1, %%rdi\n"
            "mov %0, %%rsi\n"
            "mov $14, %%rdx\n"
            "syscall" : : "r"(hw) : "%rax", "%rdi", "%rsi", "%rdx");
    while(1) {
        __asm__("mov $24, %%rax\n"
                "syscall" : : : "%rax");
    }
}