#include <os_scheduler.h>
#include <cpu_tick.h>
#include <cpu_spinlock.h>
#include <os_timer.h>

#include <assert.h>
#include <cpu_stack.h>
#include <cpu_interrupt.h>
#include <stdio.h>


////////////////////////////////////////////////////////////////////////////////
////

////////////////////////////////////////////////////////////////////////////////
////
static cpu_spinlock_t os_scheduler__tick_lock={.atomic.counter =0};
static cpu_spinlock_t os_scheduler__lock={.atomic.counter = 0};
static volatile os_tick_t os_scheduler__tick_count=0;
static volatile os_thread_t* os_scheduler__current_thread=0;
static os_list_t os_scheduler__ready_table[OS_PRIORITY_MAX];
static volatile os_bool_t os_scheduler__init_flag = OS_FALSE;
static volatile int os_scheduler__ctrl_flag = 0;
////////////////////////////////////////////////////////////////////////////////
////

#define OS_SCHEDULER_LOCK() cpu_spinlock_lock(&os_scheduler__lock)
#define OS_SCHEDULER_UNLOCK() cpu_spinlock_unlock(&os_scheduler__lock)
////////////////////////////////////////////////////////////////////////////////
////

static void os_scheduler__SysTickHandler(void){
    register volatile os_thread_t * current_thread;
    register os_bool_t tick_schedule_flag = OS_FALSE;
    register os_bool_t timer_schedule_flag = OS_FALSE;

    cpu_spinlock_lock(&os_scheduler__tick_lock);
    {
        os_scheduler__tick_count++;
        current_thread = os_scheduler__current_thread;
        
        if(current_thread->state == OS_THREAD_STATE_RUNNING){
            current_thread->remain_ticks--;
            if(current_thread->remain_ticks<=0){
                tick_schedule_flag = OS_TRUE;
                current_thread->state = OS_THREAD_STATE_YIELD;
            }
        }
        
        timer_schedule_flag = os_timer_tick();
    }
    cpu_spinlock_unlock(&os_scheduler__tick_lock);
    
    if(tick_schedule_flag || timer_schedule_flag){
        os_scheduler_schedule();
    }
}

////////////////////////////////////////////////////////////////////////////////
//// Ready Table
/* Used in os_scheduler_schedule() ONLY! */
static os_err_t os_scheduler__ready_table_next(os_thread_t** thread){
    
    os_priority_t priority;
    os_list_node_t * head;
    os_list_node_t * node;


    priority = os_priority_get_highest();
    if(priority==0 || priority>=OS_PRIORITY_MAX){
        *thread = NULL;
        return OS_EEMPTY;
    }
    assert(priority>0 && priority<OS_PRIORITY_MAX);

    head = &os_scheduler__ready_table[priority];
    node = OS_LIST_NEXT(head);
    OS_LIST_REMOVE(node);

    if(OS_LIST_IS_EMPTY(head)){
        os_priority_unmark(priority);
    }

    *thread = OS_CONTAINER_OF(node, os_thread_t, ready_node);

    return OS_EOK;
}

////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_scheduler_init(void){
    
    cpu_spinlock_lock(&os_scheduler__lock);
    
    if(os_scheduler__init_flag==OS_TRUE){
        cpu_spinlock_unlock(&os_scheduler__lock);
        return OS_EOK;
    }
    
    os_scheduler__tick_count = 0;
    os_scheduler__current_thread = 0;
    
    int i;
    for(i=0; i<OS_PRIORITY_MAX; i++){
        OS_LIST_INIT(&os_scheduler__ready_table[i]);
    }
    
    os_priority_init();
    
    os_timer_init();
    
    cpu_tick_set(os_scheduler__SysTickHandler);
    
    os_scheduler__init_flag = OS_TRUE;
    cpu_spinlock_unlock(&os_scheduler__lock);
    
    return OS_EOK;
}

void os_scheduler_ctrl(int ctrl_flag)
{
    OS_SCHEDULER_LOCK();
    os_scheduler__ctrl_flag = ctrl_flag;
    OS_SCHEDULER_UNLOCK();
}

os_err_t os_scheduler_schedule(void)
{
    register volatile os_thread_t * curr_thread;
    os_thread_t * next_thread;
    register volatile void** curr_stack_p = 0;
    register volatile void** next_stack_p = 0;

    if(os_scheduler__ctrl_flag==OS_SCHEDULER_CTRL_STOP){
        return OS_ESTOP;
    }

    OS_SCHEDULER_LOCK();
    if(os_scheduler__ctrl_flag==OS_SCHEDULER_CTRL_STOP){
        return OS_ESTOP;
    }

    curr_thread = os_scheduler__current_thread;
    
    if(curr_thread!=0 && (curr_thread->state & OS_THREAD_STATE_RUNNING) && (curr_thread->remain_ticks>0)){
        /*线程的时间片还没有用完，继续*/
        OS_SCHEDULER_UNLOCK();
        return OS_EOK;
    }
    
    /*取出下一个任务*/
    os_err_t  err = os_scheduler__ready_table_next(&next_thread);
    if(err!=OS_EOK){
        /* 没有任务需要调度 */
        OS_SCHEDULER_UNLOCK();
        return err;
    }
    
    if(curr_thread==0){
        /* first time schedule */
        curr_stack_p = 0;
        next_stack_p = (volatile void** )&next_thread->sp;
    }else{
        curr_stack_p = (volatile void** )&curr_thread->sp;
        next_stack_p = (volatile void** )&next_thread->sp;
        if(curr_thread->state & OS_THREAD_STATE_YIELD){
            /* push back into priority table */
            OS_LIST_INSERT_BEFORE(&os_scheduler__ready_table[curr_thread->current_priority], &curr_thread->ready_node);
            curr_thread->remain_ticks = curr_thread->init_ticks;
            curr_thread->state = OS_THREAD_STATE_READY;
            os_priority_mark(curr_thread->current_priority);
        }
    }
    
    next_thread->state = OS_THREAD_STATE_RUNNING;
    os_scheduler__current_thread = next_thread;
    OS_SCHEDULER_UNLOCK();
    
    if(cpu_stack_switch((void**)curr_stack_p, (void**)next_stack_p)!=0){
        /*没有被调度，加入就绪表，下次调度*/
        os_scheduler_push_front(next_thread);  /*下次调度优先调度这个任务*/
    }
    
    return OS_EOK;
}


os_err_t os_scheduler_push_back(os_thread_t* thread)
{
    if(os_scheduler__init_flag!=OS_TRUE){
        os_scheduler_init();
    }
    
    OS_SCHEDULER_LOCK();
    {
        OS_LIST_INSERT_BEFORE(&os_scheduler__ready_table[thread->current_priority], &thread->ready_node);
        thread->remain_ticks = thread->init_ticks;
        thread->state = OS_THREAD_STATE_READY;
        os_priority_mark(thread->current_priority);
    }
    OS_SCHEDULER_UNLOCK();
    
    return OS_EOK;
}

os_err_t os_scheduler_push_front(os_thread_t* thread)
{
    OS_SCHEDULER_LOCK();
    {
        OS_LIST_INSERT_AFTER(&os_scheduler__ready_table[thread->current_priority], &thread->ready_node);
        thread->remain_ticks = thread->init_ticks;
        thread->state = OS_THREAD_STATE_READY;
        os_priority_mark(thread->current_priority);
    }
    OS_SCHEDULER_UNLOCK();
    
    return OS_EOK;
}

volatile os_thread_t * os_scheduler_current_thread(void)
{
    return os_scheduler__current_thread;
}


os_err_t os_scheduler_remove(os_thread_t* thread)
{
    assert(thread);
    
    os_thread_t * curr_thread = thread;
    os_list_node_t *head = &os_scheduler__ready_table[curr_thread->current_priority];
    
    OS_SCHEDULER_LOCK();
    {
        OS_LIST_REMOVE(&curr_thread->ready_node);
        if(OS_LIST_IS_EMPTY(head)){
            os_priority_unmark(curr_thread->current_priority);
        }
    }
    OS_SCHEDULER_UNLOCK();
    return OS_EOK;
}

static void os_scheduler__timeout(os_time_t time, void* userdata){
    os_thread_t* thread = (os_thread_t*)userdata;
    
    thread->error = OS_THREAD_ETIMEOUT;
    OS_LIST_REMOVE(&thread->wait_node);             /* 如果之前挂在别的等待对象上，清除 */
    os_timer_remove(&thread->timer_node);
    if(thread->timer_node.flag & OS_TIMER_TYPE_ONCE){
        os_scheduler_push_back(thread);                 /* 添加到就绪表中 */
    }
}

os_err_t os_scheduler_timed_wait(os_thread_t* thread, os_tick_t tick){
    
    thread->state = OS_THREAD_STATE_TIMEWAIT;
    thread->remain_ticks = 0;
    os_timer_add(&thread->timer_node, os_scheduler__timeout, thread, tick, OS_TIMER_TYPE_ONCE);
    
    return os_scheduler_schedule();
}

volatile os_tick_t os_scheduler_get_current_tick(void){
    return os_scheduler__tick_count;
}

os_bool_t os_scheduler_is_ready(void){
    return (os_scheduler__init_flag==OS_TRUE && os_scheduler__current_thread!=0)?OS_TRUE:OS_FALSE;
}
