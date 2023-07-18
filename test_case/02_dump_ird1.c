#include <stdio.h>
#include <assert.h>
#include "smmuv3.h"

static struct smmuv3* smmu = (struct smmuv3*)SMMU_BASE_ADDR;

void dump_smmu_idr1(void)
{
	/*
	 * FVP model:
	 * 	 pci.pci_smmuv3.mmu.SMMU_IDR1=242457872
	 */
	assert(smmu->SMMU_IDR1 == 0xe739d10);

#ifdef DEBUG
	  printf("SMMU_IDR1: 0x%x\n", smmu->SMMU_IDR1);
	  printf("\t ECMDQ: %u\n", GET_FIELD(SMMU_IDR0, ECMDQ_MASK, ECMDQ_SHIFT));
	  printf("\t TABLES_PRESET: %u\n", GET_FIELD(SMMU_IDR1, TABLES_PRESET_MASK, TABLES_PRESET_SHIFT));
	  printf("\t QUEUES_PRESET: %u\n", GET_FIELD(SMMU_IDR1, QUEUES_PRESET_MASK, QUEUES_PRESET_SHIFT));
	  printf("\t REL: %u\n", GET_FIELD(SMMU_IDR0, REL_MASK, REL_SHIFT));
	  printf("\t ATTR_TYPES_OVR: %u\n", GET_FIELD(SMMU_IDR1, ATTR_TYPES_OVR_MASK, ATTR_TYPES_OVR_SHIFT));
	  printf("\t ATTR_PERMS_OVR: %u\n", GET_FIELD(SMMU_IDR1, ATTR_PERMS_OVR_MASK, ATTR_PERMS_OVR_SHIFT));
	  printf("\t CMDQS: %u\n", GET_FIELD(SMMU_IDR1, CMDQS_MASK, CMDQS_SHIFT));
	  printf("\t EVENTQS: %u\n", GET_FIELD(SMMU_IDR1, EVENTQS_MASK, EVENTQS_SHIFT));
	  printf("\t PRIQS: %u\n", GET_FIELD(SMMU_IDR1, PRIQS_MASK, PRIQS_SHIFT));
	  printf("\t SSIDSIZE: %u\n", GET_FIELD(SMMU_IDR1, SSIDSIZE_MASK, SSIDSIZE_SHIFT));
	  printf("\t SIDSIZE: %u\n", GET_FIELD(SMMU_IDR1, SIDSIZE_MASK, SIDSIZE_SHIFT));
#endif

}
