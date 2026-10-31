#include "task.h"
#include <stddef.h>

/* Static pool of Task Control Blocks in .bss RAM */
static tcb_t    s_tasks[MAX_TASKS];
static uint32_t s_num_tasks = 0U;
static uint32_t s_current_index = 0U;
static volatile bool s_scheduler_running = false;

/* Pointers shared with PendSV_Handler assembly in kernel/port/port.c */
volatile tcb_t *g_current_task = NULL;
volatile tcb_t *g_next_task    = NULL;

/* =========================================================================
 * Safety Trap if a Task Function Ever Returns
 * ========================================================================= */
static void task_exit_trap(void)
{
    while (1) {
    }
}

int task_create(task_func_t entry, const char *name)
{
    if ((entry == NULL) || (s_num_tasks >= MAX_TASKS)) {
        return -1;
    }

    tcb_t *tcb = &s_tasks[s_num_tasks];
    tcb->pid   = s_num_tasks;
    tcb->name  = (name != NULL) ? name : "unnamed";

    /* Start at the top (highest memory address) of this task's stack array.
     * Mask with ~0x7UL to guarantee 8-byte stack alignment per ARM AAPCS. */
    uint32_t *sp = (uint32_t *)((uintptr_t)(&tcb->stack[TASK_STACK_WORDS]) & ~0x7UL);

    // initialize context registers
    /* ---------------------------------------------------------------------
     * 1. Forge the 8 Hardware-Saved Registers (popped automatically by CPU)
     * --------------------------------------------------------------------- */
    *(--sp) = 0x01000000UL;                  /* xPSR: Bit 24 = 1 (Thumb mode) */
    *(--sp) = (uint32_t)(uintptr_t)entry & ~0x1UL;    /* PC  : Task entry function     */
    *(--sp) = (uint32_t)(uintptr_t)task_exit_trap; /* LR: Trap if task returns */
    *(--sp) = 0x12121212UL;                  /* R12 : Initial debug marker    */
    *(--sp) = 0x03030303UL;                  /* R3  : Initial debug marker    */
    *(--sp) = 0x02020202UL;                  /* R2  : Initial debug marker    */
    *(--sp) = 0x01010101UL;                  /* R1  : Initial debug marker    */
    *(--sp) = 0x00000000UL;                  /* R0  : 1st argument OF ENTRY */

    /* ---------------------------------------------------------------------
     * 2. Forge the 8 Software-Saved Registers (popped by PendSV_Handler)
     * --------------------------------------------------------------------- */
    *(--sp) = 0x11111111UL;                  /* R11 : Initial debug marker    */
    *(--sp) = 0x10101010UL;                  /* R10 : Initial debug marker    */
    *(--sp) = 0x09090909UL;                  /* R9  : Initial debug marker    */
    *(--sp) = 0x08080808UL;                  /* R8  : Initial debug marker    */
    *(--sp) = 0x07070707UL;                  /* R7  : Initial debug marker    */
    *(--sp) = 0x06060606UL;                  /* R6  : Initial debug marker    */
    *(--sp) = 0x05050505UL;                  /* R5  : Initial debug marker    */
    *(--sp) = 0x04040404UL;                  /* R4  : Initial debug marker    */

    /* Save the final stack pointer (pointing right at R4!) into the TCB */
    tcb->sp = sp;

    s_num_tasks++;
    return (int)tcb->pid;
}

bool task_scheduler_is_running(void)
{
    return s_scheduler_running;
}

bool task_schedule_next(void)
{
    if (!s_scheduler_running || (s_num_tasks <= 1U)) {
        return false;
    }

    /* Round-robin: advance to the next task index, wrapping around */
    s_current_index = (s_current_index + 1U) % s_num_tasks;
    g_next_task     = &s_tasks[s_current_index];

    return (g_next_task != g_current_task);
}

void rtos_start(void)
{
    if (s_num_tasks == 0U) {
        while (1) {
        }
    }

    s_current_index    = 0U;
    g_current_task     = &s_tasks[0];
    g_next_task        = &s_tasks[0];
    s_scheduler_running = true;

    /* Hand off to the ARM Cortex-M3 port to switch to PSP and launch Task 0 */
    port_start_first_task();
}