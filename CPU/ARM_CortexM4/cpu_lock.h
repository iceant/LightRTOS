#ifndef INCLUDED_CPU_LOCK_H
#define INCLUDED_CPU_LOCK_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_CPU_TYPES_H
#include <cpu_types.h>
#endif /*INCLUDED_CPU_TYPES_H*/

#ifndef INCLUDED_CPU_MACROS_H
#include <cpu_macros.h>
#endif /*INCLUDED_CPU_MACROS_H*/

////////////////////////////////////////////////////////////////////////////////
////

typedef volatile cpu_uint_t cpu_lock_t;

////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE void cpu_lock_init(cpu_lock_t* lock){
    *lock = 0;
}

__STATIC_FORCEINLINE void cpu_lock_lock(cpu_lock_t* lock){
    cpu_dmb();
    cpu_cas(lock, 1, 0);
    cpu_dmb();
}

__STATIC_FORCEINLINE int cpu_lock_try_lock(cpu_lock_t * lock)
{
    cpu_dmb();
    if(cpu_ldrexw(lock) != 0){
        return -1;
    }
    if(cpu_strexw(1, lock) != 0){
        return -2;
    }
    cpu_dmb();
    return 0;
}

__STATIC_FORCEINLINE void cpu_lock_unlock(cpu_lock_t *lock){
    cpu_dmb();
    cpu_cas(lock, 0, 1);
    cpu_dmb();
}


#endif /* INCLUDED_CPU_LOCK_H */
