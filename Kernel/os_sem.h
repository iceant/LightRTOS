#ifndef INCLUDED_OS_SEM_H
#define INCLUDED_OS_SEM_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_OS_TYPES_H
#include <os_types.h>
#endif /* INCLUDED_OS_TYPES_H */

#ifndef INCLUDED_OS_ERROR_H
#include <os_error.h>
#endif /* INCLUDED_OS_ERROR_H */

#ifndef INCLUDED_OS_LIST_H
#include <os_list.h>
#endif /* INCLUDED_OS_LIST_H */

#ifndef INCLUDED_OS_TICK_H
#include <os_tick.h>
#endif /* INCLUDED_OS_TICK_H */

#ifndef INCLUDED_CPU_LOCK_H
#include <cpu_lock.h>
#endif /*INCLUDED_CPU_LOCK_H*/

#ifndef INCLUDED_OS_THREAD_H
#include <os_thread.h>
#endif /*INCLUDED_OS_THREAD_H*/


////////////////////////////////////////////////////////////////////////////////
////

typedef struct os_sem_s{
    int value;
    char name[OS_NAME_MAX_SIZE];
    int flag;
    os_list_t list;
    cpu_lock_t lock;
}os_sem_t;

////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_sem_init(os_sem_t* sem, const char* name, int value, int flag);

os_err_t os_sem_take(os_sem_t* sem, os_tick_t ticks);

os_err_t os_sem_assert_take(os_sem_t * sem, os_thread_t * assert_owner, os_tick_t ticks);

os_err_t os_sem_release(os_sem_t* sem);


#endif /* INCLUDED_OS_SEM_H */
