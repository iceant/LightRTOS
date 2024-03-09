#include <os_mutex.h>
#include <cpu_macros.h>
#include <os_thread.h>

void os_mutex_init(os_mutex_t * mutex) {
    mutex->counter = 0;
    mutex->owner = 0;
    mutex->owner_original_priority = OS_PRIORITY_LOWEST; /*最低优先级*/
}

os_err_t os_mutex_lock(os_mutex_t * mutex){
//    cpu_dmb();
//    cpu_cas(&mutex->counter, 1, 0);
//    cpu_dmb();
    os_err_t err;
    while(1){
        err = os_mutex_try_lock(mutex);
        if(err!=OS_EOK){
            /* 没有获得锁, 让出当前CPU时间 */
            if(mutex->owner){
                os_thread_t* current_thread = os_thread_self();
                if(os_priority_cmp(current_thread->current_priority, mutex->owner->current_priority )==OS_PRIORITY_CMP_HIGH){
                    /*提升当前Owner线程的优先级，避免调度时，拥有锁的线程优先级太低，得不到运行时间*/
                    mutex->owner->current_priority = current_thread->current_priority;
                }
            }
            os_thread_yield();
        }else{
            mutex->owner = os_thread_self();
            mutex->owner_original_priority = mutex->owner->current_priority;
            break;
        }
    }
    
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
    mutex->owner->current_priority = mutex->owner_original_priority;
    mutex->owner = 0;
    mutex->owner_original_priority = OS_PRIORITY_LOWEST;
    
    cpu_dmb();
    cpu_cas(&mutex->counter, 0, 1);
    cpu_dmb();
    return OS_EOK;
}
