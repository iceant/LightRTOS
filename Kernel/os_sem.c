#include <os_sem.h>
#include <string.h>
#include <os_scheduler.h>
#include <assert.h>
#include <os_macros.h>
#include <os_priority.h>
////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE void os_sem__insert(os_sem_t* sem, volatile os_thread_t* thread)
{
    os_list_node_t * node;
    volatile os_thread_t * thread_p;

    if(OS_LIST_IS_EMPTY(&sem->list)){
        OS_LIST_INSERT_BEFORE(&sem->list, &thread->wait_node);
        return;
    }

    if(sem->flag & OS_QUEUE_FIFO){
        OS_LIST_INSERT_BEFORE(&sem->list, &thread->wait_node);
        return;
    }

    if(sem->flag & OS_QUEUE_PRIO){
        for(node = OS_LIST_NEXT(&sem->list); node!=&sem->list;){
            thread_p = OS_CONTAINER_OF(node, os_thread_t, wait_node);
            node = OS_LIST_NEXT(node);
            int cmp = os_priority_cmp(thread->current_priority, thread_p->current_priority);
            if(cmp == OS_PRIORITY_CMP_HIGH /* 更高优先级 */ ){
                /* 排在前面 */
                OS_LIST_INSERT_BEFORE(&thread_p->wait_node, &thread->wait_node);
                return;
            }else if(cmp==0 /* 相同优先级 */){
                /* 排在后面 */
                OS_LIST_INSERT_AFTER(&thread_p->wait_node, &thread->wait_node);
                return;
            }
        }

        /*到最后没有找到位置，插在最后面*/
        OS_LIST_INSERT_BEFORE(&sem->list, &thread->wait_node);
        return;
    }
}

__STATIC_FORCEINLINE void os_sem__notify_all(os_sem_t* sem){
    os_list_node_t * node;
    os_thread_t * thread_p;

    if(OS_LIST_IS_EMPTY(&sem->list)) return;

    for(node = OS_LIST_NEXT(&sem->list); node!=&sem->list;){
        thread_p = OS_CONTAINER_OF(node, os_thread_t, wait_node);
        node = OS_LIST_NEXT(node);
        OS_LIST_REMOVE(&thread_p->wait_node); /*从等待队列中移除*/
        os_timer_remove(&thread_p->timer_node); /* 如果挂在 TIMER 上，也移除 */
        os_scheduler_push_back(thread_p); /*  thread state is ready! */
    }
}
////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_sem_init(os_sem_t* sem, const char* name, int value, int flag)
{
    if(name){
        os_size_t name_size = strlen(name);
        memcpy(sem->name, name, name_size);
        sem->name[name_size] = '\0';
    }

    sem->value = value;
    sem->flag = flag;

    OS_LIST_INIT(&sem->list);
    
    cpu_spinlock_init(&sem->lock);
    
    return OS_EOK;
}


os_err_t os_sem_take(os_sem_t* sem, os_tick_t ticks)
{
    volatile os_thread_t * current_thread;
    while(1){
        cpu_spinlock_lock(&sem->lock);
        if(sem->value>0){
            sem->value--;
            /* 在 Take 前有 Release 发生，可以在初始化 sem 时，设置初始 Value，这样可以达到控制的目的 */
            cpu_spinlock_unlock(&sem->lock);
            return OS_EOK;
        }
        
        assert(sem->value==0);
        
        current_thread = os_scheduler_current_thread();
        current_thread->error = OS_THREAD_EOK;
        
        assert(current_thread->state & OS_THREAD_STATE_RUNNING);
        assert(OS_LIST_IS_EMPTY(&current_thread->wait_node));
        
        if(ticks==0){
            cpu_spinlock_unlock(&sem->lock);
            return OS_ETIMEOUT;
        }else if(ticks == OS_WAIT_INFINITY){
            os_sem__insert(sem, current_thread); /* 挂在 sem 上 */
            
            current_thread->state = OS_THREAD_STATE_WAIT;
            cpu_spinlock_unlock(&sem->lock);
            os_scheduler_schedule();
        }else{
            os_sem__insert(sem, current_thread); /* 挂在 sem 上 */
            cpu_spinlock_unlock(&sem->lock);
            
            /* 有等待时间，挂在 timer 上, 这里会调度 */
            os_scheduler_timed_wait((os_thread_t*)current_thread, ticks);
            
            /* 线程唤醒以后，检查是否超时 */
            if(current_thread->error == OS_THREAD_ETIMEOUT){
                return OS_ETIMEOUT;
            }
            
//            return OS_EOK;
        }
    }
}

os_err_t os_sem_release(os_sem_t* sem)
{
    cpu_spinlock_lock(&sem->lock);
    {
        sem->value++;
        os_sem__notify_all(sem);
    }
    cpu_spinlock_unlock(&sem->lock);

    /* 调度 */
    return os_scheduler_schedule();
}


