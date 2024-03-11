#ifndef INCLUDED_OS_THREAD_H
#define INCLUDED_OS_THREAD_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_OS_TYPES_H
#include <os_types.h>
#endif /*INCLUDED_OS_TYPES_H*/

#ifndef INCLUDED_OS_PRIORITY_H
#include <os_priority.h>
#endif /*INCLUDED_OS_PRIORITY_H*/

#ifndef INCLUDED_OS_TICK_H
#include <os_tick.h>
#endif /*INCLUDED_OS_TICK_H*/

#ifndef INCLUDED_OS_LIST_H
#include <os_list.h>
#endif /*INCLUDED_OS_LIST_H*/

#ifndef INCLUDED_OS_ERROR_H
#include <os_error.h>
#endif /*INCLUDED_OS_ERROR_H*/

#ifndef INCLUDED_OS_TIMER_H
#include <os_timer.h>
#endif /* INCLUDED_OS_TIMER_H */
////////////////////////////////////////////////////////////////////////////////
////
#define OS_THREAD_STATE_READY       (1<<0)
#define OS_THREAD_STATE_SUSPENDED   (1<<1)
#define OS_THREAD_STATE_RUNNING     (1<<2)
#define OS_THREAD_STATE_WAIT        (1<<3)
#define OS_THREAD_STATE_YIELD       (1<<4)
#define OS_THREAD_STATE_TIMEWAIT    (1<<5)
#define OS_THREAD_STATE_EXIT        (1<<6)

#define OS_THREAD_EOK       0
#define OS_THREAD_ETIMEOUT  (1<<0)
////////////////////////////////////////////////////////////////////////////////
////

typedef void (*os_thread_entry)(void* parameter);

typedef struct os_thread_s{
    void* sp;
    char name[OS_NAME_MAX_SIZE];
    void* stack_addr;
    os_size_t stack_size;
    os_thread_entry thread_entry;
    void* parameter;
    os_priority_t init_priority;
    os_tick_t init_ticks;
    os_priority_t current_priority;
    os_tick_t remain_ticks;
    os_list_t ready_node;
    os_list_t wait_node;
    os_timer_node_t timer_node;
    void (*thread_exit)(struct os_thread_s* thread);
    os_int_t state;
    os_err_t error;
}os_thread_t;

////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_thread_init(os_thread_t * thread
        , const char name[OS_NAME_MAX_SIZE]
        , void (*thread_entry)(void*)
        , void* parameter
        , void* stack_addr
        , os_size_t stack_size
        , os_priority_t init_priority
        , os_tick_t init_ticks);

os_err_t os_thread_startup(os_thread_t * thread);

os_thread_t* os_thread_self(void);

os_err_t os_thread_suspend(os_thread_t * thread);

os_err_t os_thread_resume(os_thread_t* thread);

os_err_t os_thread_exit(void);

void os_thread_sleep(os_tick_t tick);

void os_thread_mdelay(os_size_t ms);

os_err_t os_thread_yield(void);

#endif /*INCLUDED_OS_THREAD_H*/
