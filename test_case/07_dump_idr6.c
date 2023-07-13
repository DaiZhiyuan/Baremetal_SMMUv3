#include <stdio.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_idr6(void)
{
	  printf("SMMU_IDR6: 0x%x\n", smmu->SMMU_IDR6);
	  printf("\t CMDQ_CONTROL_PAGE_LOG2NUMP: %u\n", \
			  GET_FIELD(SMMU_IDR6, CMDQ_CONTROL_PAGE_LOG2NUMP_MASK, CMDQ_CONTROL_PAGE_LOG2NUMP_SHIFT));
	  printf("\t CMDQ_CONTROL_PAGE_LOG2NUMQ: %u\n", \
			  GET_FIELD(SMMU_IDR6, CMDQ_CONTROL_PAGE_LOG2NUMQ_MASK, CMDQ_CONTROL_PAGE_LOG2NUMQ_SHIFT));
}
