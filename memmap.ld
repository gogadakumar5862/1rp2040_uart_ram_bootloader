MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2M
    RAM(rwx)  : ORIGIN = 0x20000000, LENGTH = 256k
}

/* Application RAM load base */
APP_RAM_START = 0x20001000;

SECTIONS
{
    .text :
    {
        KEEP(*(.boot2))
        KEEP(*(.boot.entry))
        *(.text*)
        *(.rodata*)
        *(.data*)
    } > FLASH

    .bss :
    {
        *(.bss*)
        *(COMMON)
    } > RAM
}

