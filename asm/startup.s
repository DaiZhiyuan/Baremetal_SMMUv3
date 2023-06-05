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
     * Branch to scatter loading and C library init code
     */
    .global  __main
    B        __main
