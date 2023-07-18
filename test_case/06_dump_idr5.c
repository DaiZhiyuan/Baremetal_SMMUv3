#include <stdio.h>
#include <assert.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_idr5(void)
{
	/*
	 * FVP model:
	 * 	 pci.pci_smmuv3.mmu.SMMU_IDR5=65629
	 */
	assert(smmu->SMMU_IDR5 == 0x1005d);

#ifdef DEBUG
	  printf("SMMU_IDR5: 0x%x\n", smmu->SMMU_IDR5);
	  printf("\t STALL_MAX: %u\n", GET_FIELD(SMMU_IDR5, STALL_MAX_MASK, STALL_MAX_SHIFT));
	  printf("\t VAX: %u\n", GET_FIELD(SMMU_IDR5, VAX_MASK, VAX_SHIFT));
	  printf("\t GRAN64K: %u\n", GET_FIELD(SMMU_IDR5, GRAN64K_MASK, GRAN64K_SHIFT));
	  printf("\t GRAN16K: %u\n", GET_FIELD(SMMU_IDR5, GRAN16K_MASK, GRAN16K_SHIFT));
	  printf("\t GRAN4K: %u\n", GET_FIELD(SMMU_IDR5, GRAN4K_MASK, GRAN4K_SHIFT));
	  printf("\t OAS: %u\n", GET_FIELD(SMMU_IDR5, OAS_MASK, OAS_SHIFT));
#endif

}
