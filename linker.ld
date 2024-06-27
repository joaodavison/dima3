ENTRY(_start)

SECTIONS {
    . = 0x40000000; /* Adjust this load address as needed */

    .header ALIGN(4) : {
        *(.header)
    }

    .text ALIGN(4) : {
        *(.text)
    }

    .rodata ALIGN(4) : {
        *(.rodata)
    }

    .data ALIGN(4) : {
        *(.data)
    }

    .bss ALIGN(4) : {
        *(.bss)
        *(COMMON)
    }
}

