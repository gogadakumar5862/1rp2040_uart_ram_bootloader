MEMORY
{
    RAM (rx) : ORIGIN = 0x20001000, LENGTH = 240K
}

SECTIONS
{
    . = ORIGIN(RAM);

    .text :
    {
        *(.vectors)     /* MUST be first: MSP + Reset Handler */
        *(.entry)
        *(.text*)
        *(.rodata*)
    } > RAM
}

