#include <stdio.h>

#include "pl011_uart.h"

int main(void)
{
    uartInit((void*)(0x1C090000));

	printf("This is ARMv8 bare-metal program.\n");

	return 0;
}
