#include "uart.h"
#include "ringbuffer.h"
#include <stdint.h>

// The board uses MMIO (memory mapped I/O) to access the UART peripheral.
// The UART0 peripheral is located at address 0x40004000 in the QEMU mps2-an385 memory map.
// The UART0 peripheral has a set of registers that can be accessed by reading and writing to specific memory addresses. 
// The following code defines a struct that represents the UART0 registers and provides functions to initialize the UART, send and receive characters, and handle interrupts.
typedef struct {
    volatile uint32_t DATA;     /* Offset 0x00: Data Register (R/W) */
    volatile uint32_t STATE;    /* Offset 0x04: Status Register (R/W) */
    volatile uint32_t CTRL;     /* Offset 0x08: Control Register (R/W) */
    volatile uint32_t INTCLEAR; /* Offset 0x0C: Interrupt Clear Register (W) */
    volatile uint32_t BAUDDIV;  /* Offset 0x10: Baudrate Divider Register (R/W) */
} cmsdk_uart_t;

#define UART0_BASE          (0x40004000UL)
#define UART0               ((cmsdk_uart_t *)UART0_BASE) // treat UART0 as a pointer to the struct cmsdk_uart_t and forces C to put the struct at UART0_BASE address

// bitmasks declarations
/* STATE register */
#define UART_STATE_TX_FULL  (1UL << 0)

/* CTRL register */
#define UART_CTRL_TX_EN     (1UL << 0)
#define UART_CTRL_RX_EN     (1UL << 1) 
#define UART_CTRL_RX_IRQ_EN (1UL << 3)

/* INTCLEAR register */
#define UART_INTCLEAR_TX    (1UL << 0)
#define UART_INTCLEAR_RX    (1UL << 1)

/* Cortex-M3 NVIC Interrupt Set-Enable Register 0 (controls IRQs 0..31) */
#define NVIC_ISER0          (*(volatile uint32_t *)0xE000E100UL) // used to write directly to 0xE000E100 address (enabling IRQ 0))
#define UART0_RX_IRQn       (0U) /* IRQ 0 in startup.c vector_table[16] */

/* --- Private RX Ring Buffer --- */
#define UART_RX_BUF_SIZE    (64U)
static uint8_t s_rx_buf[UART_RX_BUF_SIZE]; // the actual buffer
static ringbuf_t s_rx_rb; // the struct that holds the ring buffer state (head, tail, capacity, pointer to actual buffer)

void uart_init(void)
{
    /* initialize RX ring buffer */
    ringbuf_init(&s_rx_rb, s_rx_buf, UART_RX_BUF_SIZE);

    /* baud rate divider (25 MHz system clock / 115200 baud = 217) */
    UART0->BAUDDIV = 217UL;

    /* clear any pending RX interrupt flag */
    UART0->INTCLEAR = UART_INTCLEAR_RX;

    /* enable TX, RX, and the RX interrupt in the UART peripheral */
    UART0->CTRL = UART_CTRL_TX_EN | UART_CTRL_RX_EN | UART_CTRL_RX_IRQ_EN;

    /* enable IRQ 0 (UART0RX) inside the ARM Cortex-M3 NVIC */
    NVIC_ISER0 = (1UL << UART0_RX_IRQn);
}

void uart_putc(char c)
{
    /* Convert '\n' to "\r\n" so terminal lines return to column 0 properly */
    if (c == '\n') {
        while (UART0->STATE & UART_STATE_TX_FULL) {
        }
        UART0->DATA = (uint32_t)'\r';
    }

    /* wait until the UART transmit buffer is not full */
    while (UART0->STATE & UART_STATE_TX_FULL) {
    }

    /* write character to the hardware data register */
    UART0->DATA = (uint32_t)(uint8_t)c;
}

bool uart_getc(char *c)
{
    if (!c) {
        return false;
    }
    return ringbuf_pop(&s_rx_rb, (uint8_t *)c);
}

/* Hardware Interrupt Service Routine (overrides weak alias in startup.c). Is called when UART0RX interrupt is received */
void UART0RX_Handler(void)
{
    /* read the received byte from the hardware DATA register */
    uint8_t data = (uint8_t)(UART0->DATA & 0xFFUL);

    /* clear the RX interrupt flag in the UART peripheral */
    UART0->INTCLEAR = UART_INTCLEAR_RX;

    /* ush the byte into the ring buffer for main() to consume later */
    (void)ringbuf_push(&s_rx_rb, data);
}