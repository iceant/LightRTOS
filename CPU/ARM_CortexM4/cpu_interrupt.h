#ifndef INCLUDED_CPU_INTERRUPT_H
#define INCLUDED_CPU_INTERRUPT_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_CPU_TYPES_H
#include <cpu_types.h>
#endif /* INCLUDED_CPU_TYPES_H */

#ifndef INCLUDED_CPU_ATOMIC_H
#include <cpu_atomic.h>
#endif /* INCLUDED_CPU_ATOMIC_H */
////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE cpu_uint_t cpu_interrupt_disable(void){
    return cpu_get_PRIMASK();
}

__STATIC_FORCEINLINE void cpu_interrupt_enable(cpu_uint_t level){
    cpu_set_PRIMASK(level);
}



#endif /* INCLUDED_CPU_INTERRUPT_H */
