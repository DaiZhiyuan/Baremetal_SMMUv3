#include <stdio.h>

#include "pl011_uart.h"

extern void gicInit(void);
extern void dump_smmu_idr0(void);

int main(void)
{
    uartInit((void*)(0x1C090000));
    gicInit();

    dump_smmu_idr0();

    return 0;
}
