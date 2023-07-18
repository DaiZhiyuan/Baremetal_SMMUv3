#include <stdio.h>
#include <assert.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_idr3(void)
{
	/*
	 * FVP model:
	 * 	 pci.pci_smmuv3.mmu.SMMU_IDR3=20
	 */
	assert(smmu->SMMU_IDR3 == 0x14);

#ifdef DEBUG
	  printf("SMMU_IDR3: 0x%x\n", smmu->SMMU_IDR3);
	  printf("\t PTWNNC: %u\n", GET_FIELD(SMMU_IDR3, PTWNNC_MASK, PTWNNC_SHIFT));
	  printf("\t E0PD: %u\n", GET_FIELD(SMMU_IDR3, E0PD_MASK, E0PD_SHIFT));
	  printf("\t BBML: %u\n", GET_FIELD(SMMU_IDR3, BBML_MASK, BBML_SHIFT));
	  printf("\t RIL: %u\n", GET_FIELD(SMMU_IDR3, RIL_MASK, RIL_SHIFT));
	  printf("\t STT: %u\n", GET_FIELD(SMMU_IDR3, STT_MASK, STT_SHIFT));
	  printf("\t FWB: %u\n", GET_FIELD(SMMU_IDR3, FWB_MASK, FWB_SHIFT));
	  printf("\t MPAM: %u\n", GET_FIELD(SMMU_IDR3, MPAM_MASK, MPAM_SHIFT));
	  printf("\t PSS: %u\n", GET_FIELD(SMMU_IDR3, PPS_MASK, PPS_SHIFT));
	  printf("\t XNX: %u\n", GET_FIELD(SMMU_IDR3, XNX_MASK, XNX_SHIFT));
	  printf("\t PBHA: %u\n", GET_FIELD(SMMU_IDR3, PBHA_MASK, PBHA_SHIFT));
	  printf("\t HAD: %u\n", GET_FIELD(SMMU_IDR3, HAD_MASK, HAD_SHIFT));
#endif

}

