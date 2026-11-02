ENTRY(start64)

MEMORY
{
    ROM   (rx)  : ORIGIN = 0x00000000, LENGTH = 0x04000000
    RAM   (rwx) : ORIGIN = 0x04000000, LENGTH = 0x00010000
    STACK (rw)  : ORIGIN = 0x04010000, LENGTH = 0x00010000
}

SECTIONS
{
    .text : ALIGN(8)
    {
        KEEP(*(BOOT))
        . = ALIGN(4096);
        KEEP(*(VECTORS))
        *(.text*)
        *(.rodata*)
        *(.ARM.extab*)
        *(.gnu.linkonce.armextab.*)
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        *(.eh_frame*)
        *(.init_array*)
        *(.fini_array*)
        . = ALIGN(8);
    } > ROM

    .data : ALIGN(8)
    {
        __data_start = .;
        *(.data*)
        . = ALIGN(8);
        __data_end = .;
    } > RAM AT > ROM

    __data_load_start = LOADADDR(.data);

    .bss (NOLOAD) : ALIGN(8)
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    } > RAM

    __heap_start = __bss_end;
    __heap_end = ORIGIN(STACK) + LENGTH(STACK) - 0x8000;
    __stack_top = ORIGIN(STACK) + LENGTH(STACK);
}
