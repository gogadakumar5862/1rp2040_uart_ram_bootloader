MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2M

    RAM_APP(rwx)  : ORIGIN = 0x20000000, LENGTH = 0x41000   /* 264KB - 4KB */
    RAM_BOOT(rwx) : ORIGIN = 0x20041000, LENGTH = 0x1000    /* Last 4KB */
}

/* Application RAM base */
APP_RAM_START = 0x20000000;

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
    } > RAM_BOOT
}

