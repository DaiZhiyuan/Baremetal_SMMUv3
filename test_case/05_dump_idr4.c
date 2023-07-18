#include <stdio.h>
#include <assert.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_idr4(void)
{
	/*
	 * FVP model:
	 * 	 pci.pci_smmuv3.mmu.SMMU_IDR4=0
	 */
	assert(smmu->SMMU_IDR4 == 0x0);

#ifdef DEBUG
      printf("SMMU_IDR4: 0x%x [IMPLEMENTATION_DEFINED]\n", smmu->SMMU_IDR4);
#endif

}

