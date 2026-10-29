#include "uart.h"
#include "kprintf.h"
#include "systick.h"
#include <stdint.h>

/* Deliberate crash function — marked noinline so it has a real address in Flash
 * and sets LR to the exact call site inside main() instead of copying it to its call */
__attribute__((noinline))
static void trigger_deliberate_crash(uint32_t arg0, uint32_t arg1)
{
    /* In the ARM C calling convention (AAPCS):
     *   arg0 is placed in R0 (0xCAFEBABE)
     *   arg1 is placed in R1 (0xDEADBEEF)
     * Executing the architectural Undefined Instruction (udf #0) immediately
     * triggers a fault (escalated to HardFault) at the exact PC of udf #0! */
    (void)arg0;
    (void)arg1;
    __asm volatile ("udf #0");
}

int main(void)
{
    uart_init();
    kprintf_init(uart_putc);
    systick_init();

    kprintf("========================================\n");
    kprintf("Booting miniRTOS - Milestone %d\n", 3);
    kprintf("SysTick: 1000 Hz (1 ms heartbeat)\n");
    kprintf("Press any key to echo, or 'c' to trigger a HardFault crash!\n");
    kprintf("========================================\n");

    uint32_t last_print_tick = 0;

    while (1) {
        /* 1. Print uptime every 1,000 ticks (1 second) */
        uint32_t now = systick_get_ticks();
        if ((now - last_print_tick) >= 1000U) {
            last_print_tick += 1000U;
            kprintf("[Uptime: %u s | %u ticks]\n", now / 1000U, now);
        }

        /* 2. Echo keys typed in the console (or crash on 'c') */
        char c;
        if (uart_getc(&c)) {
            if (c == 'c') {
                kprintf("\nTriggering deliberate crash at %u ms...\n", systick_get_ticks());
                trigger_deliberate_crash(0xCAFEBABEU, 0xDEADBEEFU);
            } else if (c == '\r') {
                uart_putc('\n');
            } else {
                uart_putc(c);
            }
        }
    }

    return 0;
}