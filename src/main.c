#include <stdio.h>

#include "pl011_uart.h"

extern void gicInit(void);

int main(void)
{
    uartInit((void*)(0x1C090000));
    gicInit();

    printf("This is ARMv8 bare-metal program.\n");

    return 0;
}
