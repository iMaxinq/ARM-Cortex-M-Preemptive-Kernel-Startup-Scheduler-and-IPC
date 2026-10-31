#include "task.h"
#include "systick.h"
#include <stdint.h>

/* =========================================================================
 * ARM Cortex-M3 System Control Block (SCB) Registers for Context Switching
 * ========================================================================= */
/* Interrupt Control and State Register (used to pend PendSV) */
#define SCB_ICSR            (*(volatile uint32_t *)0xE000ED04UL)
#define SCB_ICSR_PENDSVSET  (1UL << 28)

/* System Handler Priority Register 3 (controls PendSV and SysTick priority) */
#define SCB_SHPR3           (*(volatile uint32_t *)0xE000ED20UL)
#define PENDSV_LOWEST_PRIO  (0xFFUL << 16) /* Bits [23:16] = PendSV Priority */

void port_trigger_pendsv(void)
{
    SCB_ICSR = SCB_ICSR_PENDSVSET;
}

void port_start_first_task(void)
{
    /* PendSV is asynchronous: pending it while masked just leaves it pending,
     * so PRIMASK is fine here (unlike 'svc', which would HardFault). */
    __asm volatile ("cpsid i" ::: "memory");

    SCB_SHPR3 |= PENDSV_LOWEST_PRIO;
    systick_init();

    __asm volatile (
        "mov r0, #0                 \n"
        "msr psp, r0                \n" /* PSP = 0 marks "no outgoing task"   */
        "ldr r0, =_estack           \n"
        "msr msp, r0                \n" /* Reclaim main()'s stack for ISRs    */
        ::: "r0", "memory"
    );

    port_trigger_pendsv();

    /* PendSV (and a pending SysTick, if any) fire here and never return */
    __asm volatile ("cpsie i \n isb" ::: "memory");

    while (1) {
    }
}

__attribute__((naked))
void PendSV_Handler(void)
{
    __asm volatile (
        "cpsid i                    \n"

        /* 1. Save the outgoing task — skipped on the very first switch */
        "mrs r0, psp                \n"
        "cbz r0, 1f                 \n" /* PSP == 0: no task has run yet      */
        "stmdb r0!, {r4-r11}        \n"
        "ldr r1, =g_current_task    \n"
        "ldr r2, [r1]               \n"
        "str r0, [r2]               \n" /* g_current_task->sp = r0            */

        /* 2. g_current_task = g_next_task */
        "1:                         \n"
        "ldr r1, =g_current_task    \n"
        "ldr r3, =g_next_task       \n"
        "ldr r2, [r3]               \n"
        "str r2, [r1]               \n"

        /* 3. Restore the incoming task */
        "ldr r0, [r2]               \n" /* r0 = g_next_task->sp               */
        "ldmia r0!, {r4-r11}        \n"
        "msr psp, r0                \n"

        /* 4. Always return to Thread mode on PSP. On the first switch PendSV
         *    interrupted main() on MSP, so LR is 0xFFFFFFF9; force bit 2. */
        "orr lr, lr, #0x04          \n"
        "cpsie i                    \n"
        "bx lr                      \n"
    );
}