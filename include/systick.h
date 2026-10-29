#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>


//initialize the Cortex-M3 SysTick timer for a 1 ms (1000 Hz) heartbeat.
void systick_init(void);

//Return the number of 1 ms ticks elapsed since systick_init() was called.
uint32_t systick_get_ticks(void);

#endif /* SYSTICK_H */