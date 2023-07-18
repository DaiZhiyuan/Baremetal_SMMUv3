#include <stdio.h>
#include <assert.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_idr6(void)
{
	/*
	 * FVP model:
	 * 	 pci.pci_smmuv3.mmu.SMMU_S_IDR6=0
	 */
	assert(smmu->SMMU_IDR6 == 0x0);

#ifdef DEBUG
	  printf("SMMU_IDR6: 0x%x\n", smmu->SMMU_IDR6);
	  printf("\t CMDQ_CONTROL_PAGE_LOG2NUMP: %u\n", \
			  GET_FIELD(SMMU_IDR6, CMDQ_CONTROL_PAGE_LOG2NUMP_MASK, CMDQ_CONTROL_PAGE_LOG2NUMP_SHIFT));
	  printf("\t CMDQ_CONTROL_PAGE_LOG2NUMQ: %u\n", \
			  GET_FIELD(SMMU_IDR6, CMDQ_CONTROL_PAGE_LOG2NUMQ_MASK, CMDQ_CONTROL_PAGE_LOG2NUMQ_SHIFT));
#endif

}
