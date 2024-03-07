#ifndef INCLUDED_CPU_SPINLOCK_H
#define INCLUDED_CPU_SPINLOCK_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_CPU_ATOMIC_H
#include <cpu_atomic.h>
#endif /* INCLUDED_CPU_ATOMIC_H */

////////////////////////////////////////////////////////////////////////////////
////
typedef struct cpu_spinlock_s{
    cpu_atomic_t atomic;
}cpu_spinlock_t;

////////////////////////////////////////////////////////////////////////////////
////
__STATIC_FORCEINLINE void cpu_spinlock_init(cpu_spinlock_t * spinlock){
    spinlock->atomic.counter = 0;
}

__STATIC_FORCEINLINE void cpu_spinlock_lock(cpu_spinlock_t * spinlock){
    cpu_dmb();
    cpu_cas(&spinlock->atomic.counter, 1, 0);
    cpu_dmb();
}

__STATIC_FORCEINLINE int cpu_spinlock_try_lock(cpu_spinlock_t * spinlock)
{
    cpu_dmb();
    if(cpu_ldrexw(&spinlock->atomic.counter)!=0){
        return -1;
    }
    if(cpu_strexw(1, &spinlock->atomic.counter)!=0){
        return -2;
    }
    cpu_dmb();
    return 0;
}

__STATIC_FORCEINLINE void cpu_spinlock_unlock(cpu_spinlock_t *spinlock){
    cpu_dmb();
    cpu_cas(&spinlock->atomic.counter, 0, 1);
    cpu_dmb();
}

#endif /* INCLUDED_CPU_SPINLOCK_H */
