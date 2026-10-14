OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
MEMORY {
    EWRAM (rwx) : ORIGIN = 0x02000000, LENGTH = 4M
}
SECTIONS {
    . = 0x02000000;
    .text : { *(.text*) } > EWRAM
    .rodata : { *(.rodata*) } > EWRAM
    .data : { *(.data*) } > EWRAM
    .bss : { *(.bss*) *(COMMON) } > EWRAM
    . = ALIGN(4);
    __bss_end = .;
}
