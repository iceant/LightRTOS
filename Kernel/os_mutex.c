#include <os_mutex.h>
#include <cpu_macros.h>

void os_mutex_init(os_mutex_t * mutex) {
    mutex->counter = 0;
}

os_err_t os_mutex_lock(os_mutex_t * mutex){
    cpu_dmb();
    cpu_cas(&mutex->counter, 1, 0);
    cpu_dmb();
    return OS_EOK;
}

os_err_t os_mutex_try_lock(os_mutex_t * mutex){
    cpu_dmb();
    if(cpu_ldrexw(&mutex->counter)!=0){
        return -1;
    }
    if(cpu_strexw(1, &mutex->counter)!=0){
        return -2;
    }
    cpu_dmb();
    return OS_EOK;
}

os_err_t os_mutex_unlock(os_mutex_t * mutex){
    cpu_dmb();
    cpu_cas(&mutex->counter, 0, 1);
    cpu_dmb();
    return OS_EOK;
}
