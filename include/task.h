#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "minirtos.h"

#define MAX_TASKS           8U
#define TASK_STACK_WORDS    256U   /* 256 * 4 bytes = 1 KB stack per task */

/* Task Control Block (TCB) */
typedef struct tcb {
    uint32_t    *sp;                       /* MUST be first (offset 0): Saved PSP */
    uint32_t     pid;                      /* Unique Task ID (0 .. MAX_TASKS-1)   */
    const char  *name;                     /* Task name string                    */
    uint32_t     stack[TASK_STACK_WORDS];  /* Private 1 KB stack in RAM           */
} tcb_t;

/* Global pointers read/updated by PendSV_Handler in kernel/port/port.c */
extern volatile tcb_t *g_current_task;
extern volatile tcb_t *g_next_task;

/**
 * @brief Called by SysTick_Handler every 1 ms.
 *        Selects the next task in round-robin order and returns true
 *        if a context switch should be pended.
 */
bool task_schedule_next(void);

/**
 * @brief Returns true once rtos_start() has initialized the scheduler.
 */
bool task_scheduler_is_running(void);

/**
 * @brief Marks the scheduler as active and sets up g_current_task = &s_tasks[0].
 */
void task_scheduler_start_PortInit(void);

/* Architecture port function implemented in kernel/port/port.c */
void port_start_first_task(void);
void port_trigger_pendsv(void);

#endif /* TASK_H */