#include <stdio.h>

#include "pl011_uart.h"
#include "testcase.h"

extern void gicInit(void);

int main(void)
{
	/* Infrastructure initialization segment */
    uartInit();
    gicInit();

    /* The actual testcase segment */
    dump_smmu_idr0();
    dump_smmu_idr1();

    return 0;
}
