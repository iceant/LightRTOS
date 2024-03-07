#include <os_spinlock.h>

////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_spinlock_init(os_spinlock_t * spinlock){
    spinlock->atomic.counter = 0;
    return OS_EOK;
}

void os_spinlock_lock(os_spinlock_t * spinlock){
    cpu_dmb();
    cpu_cas(&spinlock->atomic.counter, 1, 0);
    cpu_dmb();
}

os_err_t os_spinlock_try_lock(os_spinlock_t * spinlock){
    cpu_dmb();
    if(cpu_ldrexw(&spinlock->atomic.counter)!=0){
        return OS_ERROR;
    }
    if(cpu_strexw(1, &spinlock->atomic.counter)!=0){
        return OS_ERROR;
    }
    cpu_dmb();
    return OS_EOK;
}

void os_spinlock_unlock(os_spinlock_t *spinlock){
    cpu_dmb();
    cpu_cas(&spinlock->atomic.counter, 0, 1);
    cpu_dmb();
}
