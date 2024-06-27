OUTPUT_ARCH(aarch64)

ENTRY(_start)

MEMORY
{
    ram_code      : ORIGIN = 0x80000000, LENGTH = 0x00040000
    ram_data      : ORIGIN = 0x80040000, LENGTH = 0x10FC0000
}

SECTIONS {
    . = ORIGIN(ram_code);

    .text : ALIGN_WITH_INPUT {
        *(.text)
        *(.rodata)
    } > ram_code

    .data : ALIGN_WITH_INPUT {
        *(.data)
    } > ram_data

    .bss : ALIGN_WITH_INPUT {
        *(.bss)
        *(COMMON)
    } > ram_data
}

