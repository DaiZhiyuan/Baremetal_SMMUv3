#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "pl011_uart.h"

// Useful defines for control/status registes
#define PL011_LCR_WORD_LENGTH_8   (0x60)
#define PL011_LCR_WORD_LENGTH_7   (0x40)
#define PL011_LCR_WORD_LENGTH_6   (0x20)
#define PL011_LCR_WORD_LENGTH_5   (0x00)

#define PL011_LCR_FIFO_ENABLE     (0x10)
#define PL011_LCR_FIFO_DISABLE    (0x00)

#define PL011_LCR_TWO_STOP_BITS   (0x08)
#define PL011_LCR_ONE_STOP_BIT    (0x00)

#define PL011_LCR_PARITY_ENABLE   (0x02)
#define PL011_LCR_PARITY_DISABLE  (0x00)

#define PL011_LCR_BREAK_ENABLE    (0x01)
#define PL011_LCR_BREAK_DISABLE   (0x00)

#define PL011_IBRD_DIV_38400      (0x27)
#define PL011_FBRD_DIV_38400      (0x09)

#define PL011_ICR_CLR_ALL_IRQS    (0x07FF)

#define PL011_FR_TXFF_FLAG        (0x20)
#define PL011_FR_RXFF_FLAG        (0x40)

#define PL011_CR_UART_ENABLE      (0x01)
#define PL011_CR_TX_ENABLE        (0x0100)
#define PL011_CR_RX_ENABLE        (0x0200)

struct pl011_uart {
        volatile unsigned int UARTDR;        // +0x00
        volatile unsigned int UARTECR;       // +0x04
  const volatile unsigned int unused0[4];    // +0x08 to +0x14 reserved
  const volatile unsigned int UARTFR;        // +0x18 - RO
  const volatile unsigned int unused1;       // +0x1C reserved
        volatile unsigned int UARTILPR;      // +0x20
        volatile unsigned int UARTIBRD;      // +0x24
        volatile unsigned int UARTFBRD;      // +0x28
        volatile unsigned int UARTLCR_H;     // +0x2C
        volatile unsigned int UARTCR;        // +0x30
        volatile unsigned int UARTIFLS;      // +0x34
        volatile unsigned int UARTIMSC;      // +0x38
  const volatile unsigned int UARTRIS;       // +0x3C - RO
  const volatile unsigned int UARTMIS;       // +0x40 - RO
        volatile unsigned int UARTICR;       // +0x44 - WO
        volatile unsigned int UARTDMACR;     // +0x48
} *uart;

void uartInit() {
    uart = (struct pl011_uart*) UART_BASE_ADDR;

    // Ensure UART is disabled
    uart->UARTCR  = 0x0;

    // Set UART 0 Registers
    uart->UARTECR   = 0x0;  // Clear the recieve status (i.e. error) register
    uart->UARTLCR_H = 0x0 | PL011_LCR_WORD_LENGTH_8 | PL011_LCR_FIFO_DISABLE | \
                      PL011_LCR_ONE_STOP_BIT | PL011_LCR_PARITY_DISABLE | PL011_LCR_BREAK_DISABLE;

    uart->UARTIBRD = PL011_IBRD_DIV_38400;
    uart->UARTFBRD = PL011_FBRD_DIV_38400;

    uart->UARTIMSC = 0x0;                     // Mask out all UART interrupts
    uart->UARTICR  = PL011_ICR_CLR_ALL_IRQS;  // Clear interrupts

    uart->UARTCR  = 0x0 | PL011_CR_UART_ENABLE | PL011_CR_TX_ENABLE | PL011_CR_RX_ENABLE;

    return;
}

static void uartPutChar(char c) {
    // Wait until FIFO or TX register has space
    while ((uart->UARTFR & PL011_FR_TXFF_FLAG) != 0x0) {}

    // Write packet into FIFO/tx register
    uart->UARTDR = c;

    // Model requires us to manually send a carriage return
    if (c == '\n') {
        while ((uart->UARTFR & PL011_FR_TXFF_FLAG) != 0x0) {}
        uart->UARTDR = '\r';
    }
}

static void uartPutString(const char *s) {
    while (*s != '\0') {
        uartPutChar(*s++);
    }
}

static void uartPutUnsigned(uint64_t value, unsigned int base, int uppercase) {
    static const char lower[] = "0123456789abcdef";
    static const char upper[] = "0123456789ABCDEF";
    const char *digits = uppercase ? upper : lower;
    char buf[32];
    int i = 0;

    if (value == 0) {
        uartPutChar('0');
        return;
    }

    while (value != 0) {
        buf[i++] = digits[value % base];
        value /= base;
    }

    while (i > 0) {
        uartPutChar(buf[--i]);
    }
}

static void uartPutSigned(int64_t value) {
    if (value < 0) {
        uartPutChar('-');
        uartPutUnsigned((uint64_t)(-value), 10, 0);
    } else {
        uartPutUnsigned((uint64_t)value, 10, 0);
    }
}

int printf(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);

    while (*fmt != '\0') {
        int is_long = 0;

        if (*fmt != '%') {
            uartPutChar(*fmt++);
            continue;
        }

        fmt++;

        if (*fmt == 'l') {
            is_long = 1;
            fmt++;
        }

        switch (*fmt) {
        case 'd':
            if (is_long) {
                uartPutSigned(va_arg(ap, long));
            } else {
                uartPutSigned(va_arg(ap, int));
            }
            break;
        case 'u':
            if (is_long) {
                uartPutUnsigned(va_arg(ap, unsigned long), 10, 0);
            } else {
                uartPutUnsigned(va_arg(ap, unsigned int), 10, 0);
            }
            break;
        case 'x':
            if (is_long) {
                uartPutUnsigned(va_arg(ap, unsigned long), 16, 0);
            } else {
                uartPutUnsigned(va_arg(ap, unsigned int), 16, 0);
            }
            break;
        case 'X':
            if (is_long) {
                uartPutUnsigned(va_arg(ap, unsigned long), 16, 1);
            } else {
                uartPutUnsigned(va_arg(ap, unsigned int), 16, 1);
            }
            break;
        case 'c':
            uartPutChar((char)va_arg(ap, int));
            break;
        case 's':
            uartPutString(va_arg(ap, const char *));
            break;
        case '%':
            uartPutChar('%');
            break;
        default:
            uartPutChar('%');
            if (*fmt != '\0') {
                uartPutChar(*fmt);
            }
            break;
        }

        if (*fmt != '\0') {
            fmt++;
        }
    }

    va_end(ap);

    return 0;
}

int fputc(int c, FILE *f) {
    (void)f;

    uartPutChar((char)c);

    return 0;
}

int _write(int fd, char *buf, int len) {
    int i;

    (void)fd;

    for (i = 0; i < len; i++) {
        uartPutChar(buf[i]);
    }

    return len;
}

void _exit(int rc)
{
    (void)rc;

    while(1) {
        __asm__ volatile("WFE");
    }
}

void __assert_func(const char *file, int line, const char *func, const char *expr)
{
    printf("ASSERT FAILED: %s:%d: %s: %s\n", file, line, func ? func : "", expr);
    _exit(1);
}

extern char __heap_start[];
extern char __heap_end[];

static char *heap_end;

void *_sbrk(ptrdiff_t incr)
{
    char *prev;

    if (heap_end == NULL) {
        heap_end = __heap_start;
    }

    prev = heap_end;

    if (incr < 0 || incr > (__heap_end - heap_end)) {
        return (void *)-1;
    }

    heap_end += incr;

    return prev;
}
