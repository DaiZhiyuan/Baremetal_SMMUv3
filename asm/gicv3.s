/*
 * GICv3 driver
 */
    .section GIC,"ax"
    .align 3

    .equ GICDbase,		        0x2F000000
    .equ GICD_CTLRoffset,		0x0
    .equ GICD_CTLR.ARE_S,		(0x1 << 4)
    .equ GICD_CTLR.ARE_NS,		(0x1 << 5)
    .equ GICD_CTLR.EnableG0,	(0x1 << 0)
    .equ GICD_CTLR.EnableG1NS,  (0x1 << 1)
    .equ GICD_CTLR.EnableG1S,   (0x1 << 2)
    .equ GICD_CTLR.DS,          (0x1 << 6)

    .equ GICRbase,	            0x2F100000
    .equ GICR_WAKERoffset,		0x14

    .equ ICC_SRE_ELn.Enable,     (0x1 << 3)
    .equ ICC_SRE_ELn.SRE,        (0x1 << 0)

    .global gicInit
    .type gicInit, "function"
gicInit:

  /*
   * Configure GIC Distributor
   */
  MOV x0, #GICDbase

  ADD x1, x0, #GICD_CTLRoffset
  MOV x2, #GICD_CTLR.ARE_NS
  ORR x2, x2, #GICD_CTLR.ARE_S
  STR w2, [x1]

  ORR x2, x2, #GICD_CTLR.EnableG0
  ORR x2, x2, #GICD_CTLR.EnableG1S
  ORR x2, x2, #GICD_CTLR.EnableG1NS
  STR w2, [x1]
  DSB SY

  /*
   * Configure Redistributor
   */
  MOV x0, #GICRbase
  MOV x1, #GICR_WAKERoffset
  ADD x1, x1, x0
  STR wzr, [x1]
  DSB SY

ChildrenAsleep:
  LDR w0, [x1]
  AND w0, w0, #0x6
  CBNZ w0, ChildrenAsleep

  /*
   * Configure CPU interface
   */
  MOV x0, #ICC_SRE_ELn.Enable
  ORR x0, x0, ICC_SRE_ELn.SRE
  MSR ICC_SRE_EL3, x0
  ISB

  MRS x1, SCR_EL3
  ORR x1, x1, #1  // Set NS bit
  MSR SCR_EL3, x1
  ISB

  MOV w0, #3
  MSR ICC_IGRPEN1_EL3, x0
  RET
