MEMORY
{
    RAM (rx) : ORIGIN = 0x20000000, LENGTH = 0x41000
}

SECTIONS
{
    . = ORIGIN(RAM);

    .text :
    {
        *(.vectors)     /* MUST BE FIRST: MSP + Reset Handler */
        *(.entry)
        *(.text*)
        *(.rodata*)
    } > RAM
}

