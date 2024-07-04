.section .rodata.vectors,"ax"
.balign 0x800
exception_vector_table:

    /** Vector Table for when exception occurs in current EL and the stack pointer to be used is SP0*/
    current_el_sp0:
        .balign 0x80 
        mov x0, #0
        ldr x0, =(0x31d0000)
        mov w2, #0x41
        strb w2, [x0]
        mov w2, #0x30
        strb w2, [x0]
        .balign 0x80 
        mov x0, #0
        ldr x0, =(0x31d0000)
        mov w2, #0x41
        strb w2, [x0]
        mov w2, #0x31
        strb w2, [x0]
        .balign 0x80 
        mov x0, #0
        ldr x0, =(0x31d0000)
        mov w2, #0x41
        strb w2, [x0]
        mov w2, #0x32
        strb w2, [x0]
        .balign 0x80 
        mov x0, #0
        ldr x0, =(0x31d0000)
        mov w2, #0x41
        strb w2, [x0]
        mov w2, #0x33
        strb w2, [x0]

    /** Vector Table for when exception occurs in current EL and the stack pointer to be used is SPn (with n being the EL number 1, 2 or 3)*/
    current_el_spx:
        .balign 0x80 
        mov x0, #0
        ldr x0, =(0x31d0000)
        mov w2, #0x41
        strb w2, [x0]
        mov w2, #34
        strb w2, [x0]
        .balign 0x80 
        mov x0, #0
        ldr x0, =(0x31d0000)
        mov w2, #0x41
        strb w2, [x0]
        mov w2, #35
        strb w2, [x0]
        .balign 0x80 
        mov x0, #0
        ldr x0, =(0x31d0000)
        mov w2, #0x41
        strb w2, [x0]
        mov w2, #36
        strb w2, [x0]
        .balign 0x80 
        mov x0, #0
        ldr x0, =(0x31d0000)
        mov w2, #0x41
        strb w2, [x0]
        mov w2, #37
        strb w2, [x0]
    
    /** Vector Table for when exception is taken at a lower EL and the execution state of the next lower level is AARCH64*/
    lower_el_a64:
        .fill 0x200

    /** Vector Table for when exception is taken at a lower EL and the execution state of the next lower level is AARCH32*/
    lower_el_a32:
        .fill 0x200 // architecture not expected to be used.
.section .text
.global _start
.balign 0x4
_start:
    // Disable interrupts
    mov x0, #0
    ldr x0, =(0x31d0000)
    ldr x1, =stack
    mov sp, x1
    msr sp_el1, x1
    msr sp_el0, x1
    mrs x2, CurrentEL
    add x2, x2, 0x30
    strb w2, [x0]
    mov w1, 0x30 
    strb w1, [x0]
    mov w1, 0x41
    strb w1, [x0]
    
	ldr	x0, =exception_vector_table
	msr	vbar_el2, x0
    mov x0, 0x42
    bl bsp_putchar
    //mov x27, #0xc0de

    // Call kernel main
    mov x0, 0x30
    bl bsp_putchar
    bl kernel_main

    // Infinite 
hang:
    wfi
    b hang



.section .data
.balign 0x4
stack:
    .fill 0x200
