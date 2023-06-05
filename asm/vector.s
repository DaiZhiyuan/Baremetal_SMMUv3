/*
 * ARMv8-A Vector Table
 */

    .section VECTORS,"ax"
    .align 12

    .global vectors

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
    B   .       // FIQ or vFIQ

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
    B   .       // FIQ or vFIQ

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
    B   .       // FIQ or vFIQ

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
    B   .       // FIQ or vFIQ

    .balign 128
serror_lower_el_aarch32:
    B   .       // SError or vSError


