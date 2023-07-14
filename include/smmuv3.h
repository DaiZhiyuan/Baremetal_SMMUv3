#ifndef __SMMUV3_H__
#define __SMMUV3_H__

#include <stdint.h>

#define SMMU_BASE_ADDR		(0x2b400000)

struct smmuv3
{
    const volatile uint32_t SMMU_IDR0;				// +0x0000 - RO
    const volatile uint32_t SMMU_IDR1;				// +0x0004 - RO
    const volatile uint32_t SMMU_IDR2;				// +0x0008 - RO
    const volatile uint32_t SMMU_IDR3;				// +0x000C - RO
    const volatile uint32_t SMMU_IDR4;				// +0x0010 - RO
    const volatile uint32_t SMMU_IDR5;				// +0x0014 - RO
    const volatile uint32_t SMMU_IIDR;				// +0x0018 - RO
    const volatile uint32_t SMMU_AIDR;				// +0x001C - RO
          volatile uint32_t SMMU_CR0;				// +0x0020 - RW
    const volatile uint32_t SMMU_CR0ACK;			// +0x0024 - RO
    	  volatile uint32_t SMMU_CR1;				// +0x0028 - RW
    	  volatile uint32_t SMMU_CR2;				// +0x002C - RW
    const volatile uint32_t padding0[4];			// +0x0030 - RESERVED
    	  volatile uint32_t SMMU_STATUSR;			// +0x0040 - RO
    	  volatile uint32_t SMMU_GBPA;				// +0x0044 - RW
    	  volatile uint32_t SMMU_AGBPA;				// +0x0048 - RW
    const volatile uint32_t padding1[1];			// +0x004C - RESERVED
          volatile uint32_t SMMU_IRQ_CTRL;      	// +0x0050 - RW
    const volatile uint32_t SMMU_IRQ_CTRLACK;   	// +0x0054 - RO
    const volatile uint32_t padding2[2];			// +0x0058 - RESERVED
    const volatile uint32_t SMMU_GERROR;	    	// +0x0060 - RO
          volatile uint32_t SMMU_GERRORN;	    	// +0x0064 - RW
          volatile uint64_t SMMU_GERROR_IRQ_CFG0;	// +0x0068 - RW
          volatile uint32_t SMMU_GERROR_IRQ_CFG1;   // +0x0070 - RW
          volatile uint32_t SMMU_GERROR_IRQ_CFG2;   // +0x0074 - RW
    const volatile uint32_t padding3[2];			// +0x0078 - RESERVED
          volatile uint64_t SMMU_STRTAB_BASE;		// +0x0080 - RW
          volatile uint32_t SMMU_STRTAB_BASE_CFG;   // +0x0088 - RW
    const volatile uint32_t padding4[1];			// +0x008C - RESERVED
          volatile uint64_t SMMU_CMDQ_BASE;		    // +0x0090 - RW
          volatile uint32_t SMMU_CMDQ_PROD;			// +0x0098 - RW
          volatile uint32_t SMMU_CMDQ_CONS;			// +0x009C - RW
          volatile uint64_t SMMU_EVENTQ_BASE;       // +0x00A0 - RW
          volatile uint32_t SMMU_EVENTQ_PROD;		// +0x00A8 - RAZ/WI
          volatile uint32_t SMMU_EVENTQ_CONS;       // +0x00AC - RAZ/WI
          volatile uint64_t SMMU_EVENTQ_IRQ_CFG0;	// +0x00B0 - RW
          volatile uint32_t SMMU_EVENTQ_IRQ_CFG1;	// +0x00B8 - RW
          volatile uint32_t SMMU_EVENTQ_IRQ_CFG2;	// +0x00BC - RW
          volatile uint64_t SMMU_PRIQ_BASE;			// +0x00C0 - RW
          volatile uint32_t SMMU_PRIQ_PROD;			// +0x00C8 - RAZ/WI
          volatile uint32_t SMMU_PRIQ_CONS;			// +0x00CC - RAZ/WI
          volatile uint64_t SMMU_PRIQ_IRQ_CFG0;		// +0x00D0 - RW
          volatile uint32_t SMMU_PRIQ_IRQ_CFG1;		// +0x00D8 - RW
          volatile uint32_t SMMU_PRIQ_IRQ_CFG2;		// +0x00DC - RW
    const volatile uint32_t padding5[8];			// +0x00E0 - RESERVED
          volatile uint32_t SMMU_GATOS_CTRL;		// +0x0100 - RW
    const volatile uint32_t padding6[1];			// +0x0104 - RESERVED
          volatile uint64_t SMMU_GATOS_SID;			// +0x0108 - RW
          volatile uint64_t SMMU_GATOS_ADDR;		// +0x0110 - RW
    const volatile uint64_t SMMU_GATOS_PAR;			// +0x0118 - RO
    const volatile uint32_t padding7[4];			// +0x0120 - RESERVED
    const volatile uint32_t SMMU_MPAMIDR;			// +0x0130 - RO
    const volatile uint32_t padding8[1];			// +0x0134 - RESERVED
    	  volatile uint32_t SMMU_GMPAM;				// +0x0138 - RW
    	  volatile uint32_t SMMU_GBPMPAM;			// +0x013C - RW
    const volatile uint32_t padding9[16];			// +0x0140 - RESERVED
    	  volatile uint32_t SMMU_VATOS_SEL;			// +0x0180 - RW
    const volatile uint32_t padding10[3];			// +0x0184 - RESERVED
    const volatile uint32_t SMMU_IDR6;				// +0x0190 - RO
    const volatile uint32_t padding11[8091];		// +0x0194 - RESERVED
    const volatile uint32_t SMMU_S_IDR0;			// +0x8000 - RO
    const volatile uint32_t SMMU_S_IDR1;			// +0x8004 - RO
    const volatile uint32_t SMMU_S_IDR2;			// +0x8008 - RO
    const volatile uint32_t SMMU_S_IDR3;			// +0x800C - RO
    const volatile uint32_t SMMU_S_IDR4;			// +0x8010 - RO
};

/* SMMU_IDR0 field descriptions */
#define ST_LEVEL_SHIFT 27
#define ST_LEVEL_MASK (0x3 << ST_LEVEL_SHIFT)
#define TERM_MODEL_SHIFT 26
#define TERM_MODEL_MASK	(0x1 << TERM_MODEL_SHIFT)
#define STALL_MODEL_SHIFT 24
#define STALL_MODEL_MASK (0x3 << STALL_MODEL_SHIFT)
#define ATSRECERR_SHIFT 23
#define ATSRECERR_MASK (0x1 << ATSRECERR_SHIFT)
#define TTENDIAN_SHIFT 21
#define TTENDIAN_MASK (0x3 << TTENDIAN_SHIFT)
#define VATOS_SHIFT 20
#define VATOS_MASK	(0x1 << VATOS_SHIFT)
#define CD2L_SHIFT 19
#define CD2L_MASK (0x1 << CD2L_SHIFT)
#define VMID16_SHIFT 18
#define VMID16_MASK (0x1 << VMID16_SHIFT)
#define VMW_SHIFT 17
#define VMW_MASK (0x1 << VMW_SHIFT)
#define PRI_SHIFT 16
#define PRI_MASK (0x1 << PRI_SHIFT)
#define ATOS_SHIFT 15
#define ATOS_MASK (0x1 << ATOS_SHIFT)
#define SEV_SHIFT 14
#define SEV_MASK (0x1 << SEV_SHIFT)
#define MSI_SHIFT 13
#define MSI_MASK (0x1 << MSI_SHIFT)
#define ASID16_SHIFT 12
#define ASID16_MASK (0x1 << ASID16_SHIFT)
#define NS1ATS_SHIFT 11
#define NS1ATS_MASK (0x1 << NS1ATS_SHIFT)
#define ATS_SHIFT 10
#define ATS_MASK (0x1 << ATS_SHIFT)
#define HYP_SHIFT 9
#define HYP_MASK (0x1 << HYP_SHIFT)
#define DORMHINT_SHIFT 8
#define DORMHINT_MASK (0x1 << DORMHINT_SHIFT)
#define HTTU_SHIFT 6
#define HTTU_MASK (0x3 << HTTU_SHIFT)
#define BTM_SHIFT 5
#define BTM_MASK (0x1 << BTM_SHIFT)
#define COHACC_SHIFT 4
#define COHACC_MASK (0x1 << COHACC_SHIFT)
#define TTF_SHIFT 2
#define TTF_MASK (0x3 << TTF_SHIFT)
#define S1P_SHIFT 1
#define S1P_MASK (0x1 << S1P_SHIFT)
#define S2P_SHIFT 0
#define S2P_MASK (0x1 << S2P_SHIFT)

/* SMMU_IDR1 field descriptions */
#define ECMDQ_SHIFT 31
#define ECMDQ_MASK (0x1 << ECMDQ_SHIFT)
#define TABLES_PRESET_SHIFT 30
#define TABLES_PRESET_MASK (0x1 << TABLES_PRESET_SHIFT)
#define QUEUES_PRESET_SHIFT	29
#define QUEUES_PRESET_MASK (0x1 << QUEUES_PRESET_SHIFT)
#define REL_SHIFT 28
#define REL_MASK (0x1 << REL_SHIFT)
#define ATTR_TYPES_OVR_SHIFT 27
#define ATTR_TYPES_OVR_MASK (0x1 << ATTR_TYPES_OVR_SHIFT)
#define ATTR_PERMS_OVR_SHIFT 26
#define ATTR_PERMS_OVR_MASK (0x1 << ATTR_PERMS_OVR_SHIFT)
#define CMDQS_SHIFT 21
#define CMDQS_MASK (0x1f << CMDQS_SHIFT)
#define EVENTQS_SHIFT 16
#define EVENTQS_MASK (0x1f << EVENTQS_SHIFT)
#define PRIQS_SHIFT 11
#define PRIQS_MASK (0x1f << PRIQS_SHIFT)
#define SSIDSIZE_SHIFT 6
#define SSIDSIZE_MASK (0x1f << SSIDSIZE_SHIFT)
#define SIDSIZE_SHIFT 0
#define SIDSIZE_MASK (0x1f << SIDSIZE_SHIFT)

/* SMMU_IDR2 field descriptions */
#define BA_VATOS_SHIFT 0
#define BA_VATOS_MASK (0x1ff << BA_VATOS_SHIFT)

/* SMMU_IDR3 field descriptions */
#define PTWNNC_SHIFT 14
#define PTWNNC_MASK (0x1 << PTWNNC_SHIFT)
#define E0PD_SHIFT 13
#define E0PD_MASK (0x1 << E0PD_SHIFT)
#define BBML_SHIFT 11
#define BBML_MASK (0x3 << BBML_SHIFT)
#define RIL_SHIFT 10
#define RIL_MASK (0x1 << RIL_SHIFT)
#define STT_SHIFT 9
#define STT_MASK (0x1 << STT_SHIFT)
#define FWB_SHIFT 8
#define FWB_MASK (0x1 << FWB_SHIFT)
#define MPAM_SHIFT 7
#define MPAM_MASK (0x1 << MPAM_SHIFT)
#define PPS_SHIFT 5
#define PPS_MASK (0x1 << PPS_SHIFT)
#define XNX_SHIFT 4
#define XNX_MASK (0x1 << XNX_SHIFT)
#define PBHA_SHIFT 3
#define PBHA_MASK (0x1 << PBHA_SHIFT)
#define HAD_SHIFT 2
#define HAD_MASK (0x1 << HAD_SHIFT)

/* SMMU_IDR4 field is IMPLEMENTATION_DEFINED */

/* SMMU_IDR5 field descriptions */
#define STALL_MAX_SHIFT 16
#define STALL_MAX_MASK (0xffff << STALL_MAX_SHIFT)
#define VAX_SHIFT 10
#define VAX_MASK (0x3 << VAX_SHIFT)
#define GRAN64K_SHIFT 6
#define GRAN64K_MASK (0x1 << GRAN64K_SHIFT)
#define GRAN16K_SHIFT 5
#define GRAN16K_MASK (0x1 << GRAN16K_SHIFT)
#define GRAN4K_SHIFT 4
#define GRAN4K_MASK (0x1 << GRAN4K_SHIFT)
#define OAS_SHIFT 0
#define OAS_MASK (0x7 << OAS_SHIFT)

/* SMMU_IDR6 field descriptions */
#define CMDQ_CONTROL_PAGE_LOG2NUMP_SHIFT 24
#define CMDQ_CONTROL_PAGE_LOG2NUMP_MASK (0xf << CMDQ_CONTROL_PAGE_LOG2NUMP_SHIFT)
#define CMDQ_CONTROL_PAGE_LOG2NUMQ_SHIFT 16
#define CMDQ_CONTROL_PAGE_LOG2NUMQ_MASK (0xf << CMDQ_CONTROL_PAGE_LOG2NUMQ_SHIFT)

/* SMMU_IIDR field descriptions */
#define ProductID_SHIFT 20
#define ProductID_MASK (0xfff << ProductID_SHIFT)
#define Variant_SHIFT 16
#define Variant_MASK (0xf << Variant_SHIFT)
#define Revision_SHIFT 12
#define Revision_MASK (0xf << Revision_SHIFT)
#define Implementer_SHIFT 0
#define Implementer_MASK (0xfff << Implementer_SHIFT)

/* SMMU_AIDR field descriptions */
#define ArchMajorRev_SHIFT 4
#define ArchMajorRev_MASK (0xf << ArchMajorRev_SHIFT)
#define ArchMinorRev_SHIFT 0
#define ArchMinorRev_MASK (0xf << ArchMinorRev_SHIFT)

#define GET_FIELD(reg,mask,shift)	(((smmu->reg) & (mask)) >> (shift))
#define OFFSETOF(type, member) ((uint64_t) & ((type*)0)->member)

#endif /* __SMMUV3_H__ */
