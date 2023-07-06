#include <stdio.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_idr4(void)
{
      printf("SMMU_IDR4: 0x%x [IMPLEMENTATION_DEFINED]\n", smmu->SMMU_IDR4);
}

