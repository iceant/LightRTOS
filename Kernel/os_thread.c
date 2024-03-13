#include <os_thread.h>

#include <cpu_stack.h>
#include <os_scheduler.h>
#include <assert.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
////
static void os_thread__exit(os_thread_t * thread){
    if(thread->state==OS_THREAD_STATE_EXIT) return;
    thread->state = OS_THREAD_STATE_EXIT;
    thread->remain_ticks = 0;
    thread->current_priority = OS_PRIORITY_MAX;
    os_scheduler_remove(thread);
    OS_LIST_REMOVE(&thread->wait_node);
    os_timer_remove(&thread->timer_node);
}

////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_thread_init(os_thread_t * thread
        , const char* name
        , void (*thread_entry)(void*)
        , void* parameter
        , void* stack_addr
        , os_size_t stack_size
        , os_priority_t init_priority
        , os_tick_t init_ticks)
{
    size_t name_size = strlen(name);
    name_size = OS_MIN(name_size, OS_NAME_MAX_SIZE-1);
    memcpy(thread->name, name, name_size);
    thread->name[name_size]='\0';
    
    thread->thread_entry = thread_entry;
    thread->parameter = parameter;
    thread->stack_addr = stack_addr;
    thread->stack_size = stack_size;
    thread->init_priority = init_priority;
    thread->init_ticks = init_ticks;
    thread->remain_ticks = init_ticks;
    thread->current_priority = init_priority;
    thread->thread_exit = os_thread__exit;
    thread->error = OS_THREAD_EOK;
    OS_LIST_INIT(&thread->ready_node);
    OS_LIST_INIT(&thread->wait_node);
    OS_LIST_INIT(&thread->timer_node.node);
    
    thread->state = OS_THREAD_STATE_SUSPENDED;
    
    int err = cpu_stack_init((void*)thread_entry, parameter, stack_addr, stack_size, &thread->sp);
    
    return err==0?OS_EOK:OS_ERROR;
}


os_err_t os_thread_startup(os_thread_t * thread)
{
    assert(thread);
    
    return os_scheduler_push_back(thread);
}

os_thread_t* os_thread_self(void)
{
    return (os_thread_t*)os_scheduler_current_thread();
}

os_err_t os_thread_suspend(os_thread_t * thread)
{
    os_scheduler_remove(thread);
    OS_LIST_REMOVE(&thread->wait_node);
    thread->state = OS_THREAD_STATE_SUSPENDED;
    return OS_EOK;
}

os_err_t os_thread_resume(os_thread_t* thread){
    assert(thread->state & OS_THREAD_STATE_SUSPENDED);
    return os_scheduler_push_back(thread);
}

os_err_t os_thread_exit(void)
{
    os_thread_t* thread = os_thread_self();
    if(thread->thread_exit){
        thread->thread_exit(thread);
    }
    return os_scheduler_schedule();
}

void os_thread_sleep(os_tick_t tick)
{
    os_scheduler_timed_wait(os_thread_self(), tick);
}

void os_thread_mdelay(os_size_t ms)
{
    os_scheduler_timed_wait(os_thread_self(), os_tick_from_millisecond(ms));
}

os_err_t os_thread_yield(void){
    volatile os_thread_t* current_thread = os_scheduler_current_thread();
    current_thread->state = OS_THREAD_STATE_YIELD;
    return os_scheduler_schedule();
}

