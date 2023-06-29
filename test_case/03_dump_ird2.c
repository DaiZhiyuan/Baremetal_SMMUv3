#include <stdio.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_idr2(void)
{
	  printf("SMMU_IDR2: 0x%x\n", smmu->SMMU_IDR2);
	  printf("\t BA_VATOS: %u\n", GET_FIELD(SMMU_IDR2, BA_VATOS_MASK, BA_VATOS_SHIFT));
}
