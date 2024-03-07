#ifndef INCLUDED_CPU_ATOMIC_GCC_H
#define INCLUDED_CPU_ATOMIC_GCC_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_CPU_TYPES_H
#include <cpu_types.h>
#endif /*INCLUDED_CPU_TYPES_H*/

////////////////////////////////////////////////////////////////////////////////
////

/* ignore some GCC warnings */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"

////////////////////////////////////////////////////////////////////////////////
////
typedef struct cpu_atomic_s{
    volatile cpu_uint_t counter;
}cpu_atomic_t;

////////////////////////////////////////////////////////////////////////////////
////
#ifndef   __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
#endif /**/


////////////////////////////////////////////////////////////////////////////////
////
__STATIC_FORCEINLINE void cpu_dmb(void)
{
    __asm__ volatile ("dmb 0xF":::"memory");
}

__STATIC_FORCEINLINE cpu_uint_t cpu_ldrexw(volatile cpu_uint_t *addr)
{
    cpu_uint_t result;
    __asm__ volatile ("ldrex %0, %1" : "=r" (result) : "Q" (*addr) );
    return(result);
}

__STATIC_FORCEINLINE cpu_uint_t cpu_strexw(cpu_uint_t value, volatile cpu_uint_t *addr)
{
    cpu_uint_t result;
    __asm__ volatile ("strex %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" (value) );
    return(result);
}

__STATIC_FORCEINLINE cpu_uint_t cpu_cas(volatile cpu_uint_t *dest, cpu_uint_t new_value, cpu_uint_t old_value){
    do
    {
        if (cpu_ldrexw(dest) != old_value) return 1; // Failure
    } while(cpu_strexw(new_value, dest));
    return 0;
}

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

#pragma GCC diagnostic pop

#endif /*INCLUDED_CPU_ATOMIC_GCC_H*/
