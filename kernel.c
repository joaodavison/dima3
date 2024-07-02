#include "console_driver/console.h"

void kernel_main() {
    // Your kernel code here
    bsp_putchar('D');
    bsp_putchar('I');
    bsp_putchar('M');
    bsp_putchar('A');
    bsp_putchar('\n');

    while (1) {
        // Infinite loop to halt the kernel
    }
}

void kernel_second() {
    while(1){}
} 
