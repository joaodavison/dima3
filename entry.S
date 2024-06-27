.section .text
.global _start
.balign 0x4
_start:
    // Disable interrupts
    mov x0, #0
    ldr x0, =(0x00000000FFFFFFFF & 0x31d0000)
    mrs x2, CurrentEL
    add x2, x2, 0x30
    strb w2, [x0]
    mov w1, 0x30 
    strb w1, [x0]
    mov w1, 0x41
    strb w1, [x0]
    
     
    //mov x27, #0xc0de

    // Call kernel main
     //mov x0, 0x30
     //bl bsp_putchar
    //bl kernel_main

    // Infinite 
hang:
    wfi
    b hang

.section .data
.balign 0x4
stack:
    .fill 0x800

