#ifndef MINIRTOS_H
#define MINIRTOS_H

#include <stdint.h>
#include <stdbool.h>

/* Function signature for every RTOS task: void task_fn(void) */
typedef void (*task_func_t)(void);

/**
 * @brief Create a new task with its own private stack and forged initial context.
 *
 * @param entry  Function pointer to the task's infinite loop.
 * @param name   Human-readable task name (for debugging/crash dumps).
 * @return       Assigned task ID (PID >= 0) on success, or -1 if full.
 */
int task_create(task_func_t entry, const char *name);

/**
 * @brief Start the preemptive scheduler and launch the first task.
 *        Does not return.
 */
void rtos_start(void);

#endif /* MINIRTOS_H */