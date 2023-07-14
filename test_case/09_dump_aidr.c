#include <stdio.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_aidr(void)
{
	  printf("SMMU_AIDR: 0x%x\n", smmu->SMMU_AIDR);
	  printf("\t ArchMajorRev: %u\n", GET_FIELD(SMMU_AIDR, ArchMajorRev_MASK, ArchMajorRev_SHIFT));
	  printf("\t ArchMinorRev: %u\n", GET_FIELD(SMMU_AIDR, ArchMinorRev_MASK, ArchMinorRev_SHIFT));
}
