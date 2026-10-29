#include "uart.h"
#include "kprintf.h"

int main(void)
{
    /* 1. Initialize UART0 hardware and enable RX interrupt (IRQ 0) */
    uart_init();

    /* 2. Connect kprintf() to uart_putc() */
    kprintf_init(uart_putc);

    /* 3. Print the Milestone 2 startup banner (uses TX pipeline) */
    kprintf("========================================\n");
    kprintf("Booting miniRTOS - Milestone %d\n", 2);
    kprintf("UART0 base: %x | Type keys to echo:\n", 0x40004000U);
    kprintf("========================================\n");

    /* 4. Main loop: check the RX ring buffer and echo every key typed */
    while (1) {
        char c;
        if (uart_getc(&c)) {
            /* A key was popped from the ring buffer! Echo it back to the screen: */
            if (c == '\r') {
                /* In QEMU terminal, pressing Enter sends '\r'. Print a newline: */
                uart_putc('\n');
            } else {
                uart_putc(c);
            }
        }
    }

    return 0;
}