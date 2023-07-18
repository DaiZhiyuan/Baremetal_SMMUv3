#include <stdio.h>
#include <assert.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_idr0(void)
{
	/*
	 * FVP model:
	 * 	 pci.pci_smmuv3.mmu.SMMU_IDR0=135259839
	 */
	assert(smmu->SMMU_IDR0 == 0x80fe6bf);

#ifdef DEBUG
	printf("SMMU_IDR0: 0x%x\n", smmu->SMMU_IDR0);
	printf("\t ST_LEVEL: %u\n", GET_FIELD(SMMU_IDR0, ST_LEVEL_MASK, ST_LEVEL_SHIFT));
	printf("\t TERM_MODEL: %u\n", GET_FIELD(SMMU_IDR0, TERM_MODEL_MASK, TERM_MODEL_SHIFT));
	printf("\t STALL_MODEL: %u\n", GET_FIELD(SMMU_IDR0, STALL_MODEL_MASK, STALL_MODEL_SHIFT));
	printf("\t ATSRECERR: %u\n", GET_FIELD(SMMU_IDR0, ATSRECERR_MASK, ATSRECERR_SHIFT));
	printf("\t TTENDIAN: %u\n", GET_FIELD(SMMU_IDR0, TTENDIAN_MASK, TTENDIAN_SHIFT));
	printf("\t VATOS: %u\n", GET_FIELD(SMMU_IDR0, VATOS_MASK, VATOS_SHIFT));
	printf("\t CD2L: %u\n", GET_FIELD(SMMU_IDR0, CD2L_MASK, CD2L_SHIFT));
	printf("\t VMID16: %u\n", GET_FIELD(SMMU_IDR0, VMID16_MASK, VMID16_SHIFT));
	printf("\t VMW: %u\n", GET_FIELD(SMMU_IDR0, VMW_MASK, VMW_SHIFT));
	printf("\t PRI: %u\n", GET_FIELD(SMMU_IDR0, PRI_MASK, PRI_SHIFT));
	printf("\t ATOS: %u\n", GET_FIELD(SMMU_IDR0, ATOS_MASK, ATOS_SHIFT));
	printf("\t SEV: %u\n", GET_FIELD(SMMU_IDR0, SEV_MASK, SEV_SHIFT));
	printf("\t MSI: %u\n", GET_FIELD(SMMU_IDR0, MSI_MASK, MSI_SHIFT));
	printf("\t ASID16: %u\n", GET_FIELD(SMMU_IDR0, ASID16_MASK, ASID16_SHIFT));
	printf("\t NS1ATS: %u\n", GET_FIELD(SMMU_IDR0, NS1ATS_MASK, NS1ATS_SHIFT));
	printf("\t ATS: %u\n", GET_FIELD(SMMU_IDR0, ATS_MASK, ATS_SHIFT));
	printf("\t HYP: %u\n", GET_FIELD(SMMU_IDR0, HYP_MASK, HYP_SHIFT));
	printf("\t HTTU: %u\n", GET_FIELD(SMMU_IDR0, HTTU_MASK, HTTU_SHIFT));
	printf("\t BTM: %u\n", GET_FIELD(SMMU_IDR0, BTM_MASK, BTM_SHIFT));
	printf("\t COHACC: %u\n", GET_FIELD(SMMU_IDR0, COHACC_MASK, COHACC_SHIFT));
	printf("\t TTF: %u\n", GET_FIELD(SMMU_IDR0, TTF_MASK, TTF_SHIFT));
	printf("\t S1P: %u\n", GET_FIELD(SMMU_IDR0, S1P_MASK, S1P_SHIFT));
	printf("\t S2P: %u\n", GET_FIELD(SMMU_IDR0, S2P_MASK, S2P_SHIFT));
#endif

}
