        .cpu cortex-m0plus
        .thumb

/* Vector Table */
        .section .vectors, "ax"
        .align 2
        .global _vectors
_vectors:
        .word 0x20041000      /* Initial Stack Pointer (top of APP RAM) */
        .word _reset          /* Reset Handler */

/* Reset Handler */
        .thumb_func
        .global _reset
_reset:
        /* Setup Stack explicitly */
        ldr r0, =0x20041000
        mov sp, r0

        /* Release IO_BANK0 reset */
        ldr  r3, =0x4000f000
        movs r2, #32
        str  r2, [r3, #0]

        /* GPIO25 to SIO function */
        ldr  r3, =0x400140cc
        movs r2, #5
        str  r2, [r3, #0]

        /* GPIO25 Output Enable */
        ldr  r3, =0xd0000020
        movs r2, #128
        lsl  r2, r2, #18
        str  r2, [r3, #0]

        b main
        b .
.align 4

