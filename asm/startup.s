/*
 * Armv8-A Single-core EL3 AArch64 Startup Code
 */
    .section  BOOT,"ax"
    .align 3

    .global start64
    .type start64, "function"
    .global vectors

start64:
    MRS x0, MPIDR_EL1
    AND x0, x0, #0xFFFF
    CBZ x0, boot

sleep:
    WFI
    B sleep

boot:
    /*
     * Disable trapping of CPTR_EL3 accesses or use of Adv.SIMD/FPU
     */
    MSR      CPTR_EL3, xzr

    /*
     * Configure SCR_EL3
     */
    MOV w1, #0
    ORR w1, w1, #(1 << 3)
    ORR w1, w1, #(1 << 2)
    ORR w1, w1, #(1 << 1)
    MSR SCR_EL3, x1

    /*
     * Install vector table
     */
    LDR x0, =vectors
    MSR VBAR_EL3, x0
    ISB

    /*
     * Clear interrupt masks
     */
    MSR DAIFClr, #0xF

    /*
     * Set up the stack
     */
    LDR x0, =__stack_top
    MOV sp, x0

    /*
     * Copy .data from ROM to RAM
     */
    LDR x0, =__data_load_start
    LDR x1, =__data_start
    LDR x2, =__data_end
copy_data:
    CMP x1, x2
    B.HS copy_data_done
    LDR x3, [x0], #8
    STR x3, [x1], #8
    B copy_data
copy_data_done:

    /*
     * Zero .bss
     */
    LDR x0, =__bss_start
    LDR x1, =__bss_end
zero_bss:
    CMP x0, x1
    B.HS zero_bss_done
    STR xzr, [x0], #8
    B zero_bss
zero_bss_done:

    /*
     * Branch to C entry point
     */
    BL main

hang:
    WFI
    B hang
