#include <stdint.h>

void main() {
    uint32_t number = 0xdeadbeef;
    uint32_t target = 0xdb;
    uint8_t p = 0;
    uint8_t k = 1;

    uint32_t result = (((1 << k) - 1) & (number >> (p)));

    if (result == target)
        printf("Test okay\n");
    
    printf("Number = %x\n", number);
    printf("Result = %x\n", result);
    printf("Target = %x\n", target);
}