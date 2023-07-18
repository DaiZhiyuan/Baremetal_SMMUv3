#include <stdio.h>
#include <stdint.h>

#include "pl011_uart.h"
#include "testcase.h"

extern void gicInit(void);

int get_currentEL(void)
{
	uint64_t current_EL;

#define EL_SHIFT 2
#define EL_MASK (0x3 << EL_SHIFT)

	__asm volatile ("MRS %[result], CurrentEL"
			: [result] "=r" (current_EL)
			:
			:
	);

	return (current_EL & EL_MASK) >> EL_SHIFT;
}

int main(void)
{

    /* Infrastructure initialization segment */
    uartInit();
    gicInit();

#ifdef DEBUG
    printf("Current Exception Level: [%d]\n\n", get_currentEL());
#endif

    /* The actual testcase segment */
    dump_smmu_idr0();
    dump_smmu_idr1();
    dump_smmu_idr2();
    dump_smmu_idr3();
    dump_smmu_idr4();
    dump_smmu_idr5();
    dump_smmu_idr6();
    dump_smmu_iidr();
    dump_smmu_aidr();

    check_offset();

#ifdef DEBUG
    printf("=== ALL TEST CASE PASS ===\n");
#endif

    return 0;
}
