#ifndef UART_H
#define UART_H

#include <stdbool.h>

/* Initialize UART0 hardware for 115200 baud and enable RX interrupt */
void uart_init(void);

/* Send a single character over UART0 (blocks if TX buffer is full) */
void uart_putc(char c);

/* Non-blocking read: pops one received character from the RX ring buffer.
 * Returns true if a character was read into *c, or false if no key is waiting. */
bool uart_getc(char *c);

#endif /* UART_H */