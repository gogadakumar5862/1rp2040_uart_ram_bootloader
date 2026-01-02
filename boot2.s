.cpu cortex-m0plus
.thumb

.section .boot2, "ax"

    ldr r0, =XIP_SSI_SSIENR
    movs r1, #0
    str r1, [r0]

    ldr r0, =XIP_SSI_CTRLR0
    ldr r1, =0x001F0300
    str r1, [r0]

    ldr r0, =XIP_SSI_BAUDR
    ldr r1, =0x00000008
    str r1, [r0]

    ldr r0, =XIP_SSI_SPI_CTRLR0
    ldr r1, =0x03000218
    str r1, [r0]

    ldr r0, =XIP_SSI_CTRLR1
    movs r1, #0
    str r1, [r0]

    ldr r0, =XIP_SSI_SSIENR
    movs r1, #1
    str r1, [r0]

    /*  IMPORTANT 
       NOW FLASH IS READY
       DIRECTLY JUMP TO MAIN BOOTLOADER IN FLASH
       Bootloader FLASH location = 0x10000100
       Thumb mode → +1
    */
    /* Set Bootloader stack pointer (top of last 4KB RAM) */
    
    ldr r0, =0x20042000
    msr msp, r0


    ldr r0, =0x10000101
    bx  r0

.set XIP_SSI_BASE,       0x18000000
.set XIP_SSI_CTRLR0,     XIP_SSI_BASE + 0x00
.set XIP_SSI_CTRLR1,     XIP_SSI_BASE + 0x04
.set XIP_SSI_SSIENR,     XIP_SSI_BASE + 0x08
.set XIP_SSI_BAUDR,      XIP_SSI_BASE + 0x14
.set XIP_SSI_SPI_CTRLR0, XIP_SSI_BASE + 0xF4

.end

