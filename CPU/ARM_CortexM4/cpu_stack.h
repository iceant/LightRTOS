#ifndef INCLUDED_CPU_STACK_H
#define INCLUDED_CPU_STACK_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_CPU_TYPES_H
#include <cpu_types.h>
#endif /* INCLUDED_CPU_TYPES_H */

////////////////////////////////////////////////////////////////////////////////
////

int cpu_stack_init(void *thread_entry, void *parameter, void *stack_addr, int stack_size, void **cpu_sp);

int cpu_stack_switch(void** from_stack_addr_p, void** to_stack_addr_p);

#endif /* INCLUDED_CPU_STACK_H */
