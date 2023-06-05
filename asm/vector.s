/*
 * ARMv8-A Vector Table
 */

    .section VECTORS,"ax"
    .align 12
    .global vectors
    .global fiqHandler

vectors:

/*
 * 0x000 Current Exception level with SP_EL0
 */
    .balign 128
sync_current_el_sp0:
    B   .       // Synchronous

    .balign 128
irq_current_el_sp0:
    B   .       // IRQ or vIRQ

    .balign 128
fiq_current_el_sp0:
    B   fiqHighLevelHandler       // FIQ or vFIQ

    .balign 128
serror_current_el_sp0:
    B   .       // SError or vSError

/*
 * 0x200 Current Exception level with SP_ELx, x>0.
 */
    .balign 128
sync_current_el_spx:
    B   .       // Synchronous

    .balign 128
irq_current_el_spx:
    B   .       // IRQ or vIRQ

    .balign 128
fiq_current_el_spx:
    B   fiqHighLevelHandler       // FIQ or vFIQ

    .balign 128
serror_current_el_spx:
    B   .       // SError or vSError

/*
 * 0x400 Lower Exception level with target using AArch64.
 */
    .balign 128
sync_lower_el_aarch64:
    B   .       // Synchronous

    .balign 128
irq_lower_el_aarch64:
    B   .       // IRQ or vIRQ

    .balign 128
fiq_lower_el_aarch64:
    B   fiqHighLevelHandler       // FIQ or vFIQ

    .balign 128
serror_lower_el_aarch64:
    B   .       // SError or vSError

/*
 * 0x600 Lower Exception level with target using AArch32.
 */
    .balign 128
sync_lower_el_aarch32:
    B   .       // Synchronous

    .balign 128
irq_lower_el_aarch32:
    B   .       // IRQ or vIRQ

    .balign 128
fiq_lower_el_aarch32:
    B   fiqHighLevelHandler       // FIQ or vFIQ

    .balign 128
serror_lower_el_aarch32:
    B   .       // SError or vSError

fiqHighLevelHandler:
    STP x29, x30, [sp, #-16]!
    STP x18, x19, [sp, #-16]!
    STP x16, x17, [sp, #-16]!
    STP x14, x15, [sp, #-16]!
    STP x12, x13, [sp, #-16]!
    STP x10, x11, [sp, #-16]!
    STP  x8,  x9, [sp, #-16]!
    STP  x6,  x7, [sp, #-16]!
    STP  x4,  x5, [sp, #-16]!
    STP  x2,  x3, [sp, #-16]!
    STP  x0,  x1, [sp, #-16]!

    BL fiqHandler

    LDP  x0,  x1, [sp], #16
    LDP  x2,  x3, [sp], #16
    LDP  x4,  x5, [sp], #16
    LDP  x6,  x7, [sp], #16
    LDP  x8,  x9, [sp], #16
    LDP x10, x11, [sp], #16
    LDP x12, x13, [sp], #16
    LDP x14, x15, [sp], #16
    LDP x16, x17, [sp], #16
    LDP x18, x19, [sp], #16
    LDP x29, x30, [sp], #16
    ERET

