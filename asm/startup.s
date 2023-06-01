/*
 * Armv8-A Single-core EL3 AArch64 Startup Code
 */
  .section  BOOT,"ax"
  .align 3

  .global start64
  .type start64, "function"

start64:
  MRS      x0, MPIDR_EL1
  AND      x0, x0, #0xFFFF
  CBZ      x0, boot
sleep:
  WFI
  B        sleep

boot:
  // Disable trapping of CPTR_EL3 accesses or use of Adv.SIMD/FPU
  // -------------------------------------------------------------
  MSR      CPTR_EL3, xzr // Clear all trap bits

  // Branch to scatter loading and C library init code
  .global  __main
  B        __main
