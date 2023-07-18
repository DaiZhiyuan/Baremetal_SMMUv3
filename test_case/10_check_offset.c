#include <stdio.h>
#include <assert.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void check_offset(void)
{
	/*
		Offset  Register   Notes
		---------------------------
		0x0000 SMMU_IDR0 32-bit, RO
		0x0004 SMMU_IDR1 32-bit, RO
		0x0008 SMMU_IDR2 32-bit, RO
		0x000C SMMU_IDR3 32-bit, RO
		0x0010 SMMU_IDR4 32-bit, RO
		0x0014 SMMU_IDR5 32-bit, RO
		0x0018 SMMU_IIDR 32-bit, RO
		0x001C SMMU_AIDR 32-bit, RO
		0x0190 SMMU_IDR6 32-bit, RO
	*/

	assert(OFFSETOF(struct smmuv3, SMMU_IDR0) == 0x0000);
	assert(OFFSETOF(struct smmuv3, SMMU_IDR1) == 0x0004);
	assert(OFFSETOF(struct smmuv3, SMMU_IDR2) == 0x0008);
	assert(OFFSETOF(struct smmuv3, SMMU_IDR3) == 0x000C);
	assert(OFFSETOF(struct smmuv3, SMMU_IDR4) == 0x0010);
	assert(OFFSETOF(struct smmuv3, SMMU_IIDR) == 0x0018);
	assert(OFFSETOF(struct smmuv3, SMMU_AIDR) == 0x001C);
	assert(OFFSETOF(struct smmuv3, SMMU_IDR6) == 0x0190);

#ifdef DEBUG
	printf("SMMU_IDR0 offset:0x%lx\n", OFFSETOF(struct smmuv3, SMMU_IDR0));
	printf("SMMU_IDR1 offset:0x%lx\n", OFFSETOF(struct smmuv3, SMMU_IDR1));
	printf("SMMU_IDR2 offset:0x%lx\n", OFFSETOF(struct smmuv3, SMMU_IDR2));
	printf("SMMU_IDR3 offset:0x%lx\n", OFFSETOF(struct smmuv3, SMMU_IDR3));
	printf("SMMU_IDR4 offset:0x%lx\n", OFFSETOF(struct smmuv3, SMMU_IDR4));
	printf("SMMU_IDR5 offset:0x%lx\n", OFFSETOF(struct smmuv3, SMMU_IDR5));
	printf("SMMU_IIDR offset:0x%lx\n", OFFSETOF(struct smmuv3, SMMU_IIDR));
	printf("SMMU_AIDR offset:0x%lx\n", OFFSETOF(struct smmuv3, SMMU_AIDR));
	printf("SMMU_IDR6 offset:0x%lx\n", OFFSETOF(struct smmuv3, SMMU_IDR6));
#endif

}
