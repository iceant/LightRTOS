#ifndef INCLUDED_CPU_TICK_H
#define INCLUDED_CPU_TICK_H

typedef void (*cpu_tick_function_t)(void);

void cpu_tick_set(cpu_tick_function_t function);

#endif /* INCLUDED_CPU_TICK_H */
