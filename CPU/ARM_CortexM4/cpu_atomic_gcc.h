#ifndef INCLUDED_CPU_ATOMIC_GCC_H
#define INCLUDED_CPU_ATOMIC_GCC_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_CPU_TYPES_H
#include <cpu_types.h>
#endif /*INCLUDED_CPU_TYPES_H*/

#ifndef INCLUDED_CPU_MACROS_H
#include <cpu_macros.h>
#endif /* INCLUDED_CPU_MACROS_H */


////////////////////////////////////////////////////////////////////////////////
////
typedef struct cpu_atomic_s{
    volatile cpu_uint_t counter;
}cpu_atomic_t;

////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE cpu_int_t cpu_atomic_add_return(cpu_atomic_t* v, cpu_int_t i){
    cpu_uint_t tmp;
    cpu_int_t result;
    cpu_dmb();
    __asm__ volatile("@ cpu_atomic_add_return\n"
                     "1:    ldrex %0, [%3]\n"
                     "      add %0, %0, %4\n"
                     "      strex %1, %0, [%3]\n"
                     "      teq %1, #0\n"
                     "      bne 1b"
            : "=&r"(result), "=&r"(tmp), "+Qo"(v->counter)
            : "r"(&v->counter), "Ir"(i)
            : "cc"
            );
    cpu_dmb();
    return result;
}

__STATIC_FORCEINLINE void cpu_atomic_add(cpu_atomic_t* v, int i)
{
    cpu_atomic_add_return(v, i);
}

__STATIC_FORCEINLINE cpu_int_t cpu_atomic_sub_return(cpu_atomic_t* v, cpu_int_t i){
    cpu_uint_t tmp;
    cpu_int_t result;
    cpu_dmb();
    __asm__ __volatile__("@ cpu_atomic_sub_return\n"
                         "1:    ldrex %0, [%3]\n"
                         "      sub %0, %0, %4\n"
                         "      strex %1, %0, [%3]\n"
                         "      teq %1, #0\n"
                         "      bne 1b"
            : "=&r"(result), "=&r"(tmp), "+Qo"(v->counter)
            : "r"(&v->counter), "Ir"(i)
            : "cc"
            );
    cpu_dmb();
    return result;
}

__STATIC_FORCEINLINE void cpu_atomic_sub(cpu_atomic_t* v, cpu_int_t i){
    cpu_atomic_sub_return(v, i);
}


#endif /*INCLUDED_CPU_ATOMIC_GCC_H*/
