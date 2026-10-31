#include "minirtos.h"
#include "uart.h"
#include "kprintf.h"
#include "systick.h"
#include <stdint.h>

/* Simple busy delay so the terminal doesn't scroll at 10,000 lines/sec,
 * while still letting the 1 ms preemptive context switch interrupt tasks mid-print! */
static void spin_delay(volatile uint32_t count)
{
    while (count > 0U) {
        count--;
    }
}

static void task_a(void)
{
    uint32_t counter = 0;
    while (1) {
        kprintf("[Task A | tick %u] count = %u\n", systick_get_ticks(), counter++);
        spin_delay(200000U);
    }
}

static void task_b(void)
{
    uint32_t counter = 0;
    while (1) {
        kprintf("[Task B | tick %u] count = %u\n", systick_get_ticks(), counter++);
        spin_delay(200000U);
    }
}

static void task_c(void)
{
    uint32_t counter = 0;
    while (1) {
        kprintf("[Task C | tick %u] count = %u\n", systick_get_ticks(), counter++);
        spin_delay(200000U);
    }
}

int main(void)
{
    uart_init();
    kprintf_init(uart_putc);

    kprintf("========================================\n");
    kprintf("Booting miniRTOS - Milestone %d\n", 4);
    kprintf("Creating 3 preemptive tasks...\n");
    kprintf("========================================\n");

    task_create(task_a, "Task A");
    task_create(task_b, "Task B");
    task_create(task_c, "Task C");

    /* Launch the scheduler (never returns) */
    rtos_start();

    return 0;
}