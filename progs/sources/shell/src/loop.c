#include <stdio.h>
#include <stdint.h>

const char hw[] = "Hello, World!\n";

uint64_t strlen(const char * str) {
    uint64_t len = 0;
    while(str[len] != 0) {
        len++;
    }
    return len;
}

void printer(const char * str) {
    __asm__("mov $1, %%rax\n"
            "mov $1, %%rdi\n"
            "mov %0, %%rsi\n"
            "mov %1, %%rdx\n"
            "syscall" : : "r"(str), "r"(strlen(str)) : "%rax", "%rdi", "%rsi", "%rdx");
}

void atoi(int num, char * str) {
    int i = 0;
    while(num > 0) {
        str[i++] = num % 10 + '0';
        num /= 10;
    }
    str[i] = 0;
    int j = 0;
    i--;
    while(j < i) {
        char tmp = str[j];
        str[j] = str[i];
        str[i] = tmp;
        j++;
        i--;
    }
}

void printi(int num) {
    char str[20];
    atoi(num, str);
    printer(str);
}

void dummy_loop() {
    while (1) {
        //Syscall 24 yields the CPU
        __asm__("mov $24, %rax\n"
                "syscall");
    }
}

int main(int argc, char* argv[]) {
    printer(hw);
    printer("ARGC: ");
    printi(argc);
    printer("\n");

    for (int i = 0; i < argc; i++) {
        printer("ARGV[");
        printi(i);
        printer("]: ");
        printer(argv[i]);
        printer("\n");
    }

    dummy_loop();
    return 0;
}