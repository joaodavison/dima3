#include "console_driver/console.h"

void kernel_main() {
    // Your kernel code here
    console_init();
    //bsp_putchar('t');
    // bsp_putchar('e');
    // bsp_putchar('s');
    // bsp_putchar('t');
    // bsp_putchar('\n');

    while (1) {
        // Infinite loop to halt the kernel
    }
}

void kernel_second() {
    while(1){}
} 
