.section .text
.global _start
_start:
    // Disable interrupts
    msr daifset, #0xf

    // Call kernel main
    bl kernel_main

    // Infinite loop
hang:
    wfi
    b hang

