#include <stdio.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_iidr(void)
{
	  printf("SMMU_IIDR: 0x%x\n", smmu->SMMU_IIDR);
	  printf("\t ProductID: %u\n", GET_FIELD(SMMU_IIDR, ProductID_MASK, ProductID_SHIFT));
	  printf("\t Variant: %u\n", GET_FIELD(SMMU_IIDR, Variant_MASK, Variant_SHIFT));
	  printf("\t Revision: %u\n", GET_FIELD(SMMU_IIDR, Revision_MASK, Revision_SHIFT));
	  printf("\t Implementer: %u\n", GET_FIELD(SMMU_IIDR, Implementer_MASK, Implementer_SHIFT));
}
