#ifndef INCLUDED_OS_SCHEDULER_H
#define INCLUDED_OS_SCHEDULER_H


////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_OS_TYPES_H
#include <os_types.h>
#endif /* INCLUDED_OS_TYPES_H */

#ifndef INCLUDED_OS_ERROR_H
#include <os_error.h>
#endif /* INCLUDED_OS_ERROR_H */

#ifndef INCLUDED_OS_THREAD_H
#include <os_thread.h>
#endif /* INCLUDED_OS_THREAD_H */
////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_scheduler_init(void);

os_err_t os_scheduler_schedule(void);

os_err_t os_scheduler_resume(os_thread_t* thread);

os_err_t os_scheduler_push_back(os_thread_t* thread);

os_err_t os_scheduler_push_front(os_thread_t* thread);

os_err_t os_scheduler_remove(os_thread_t* thread);

volatile os_thread_t * os_scheduler_current_thread(void);

os_err_t os_scheduler_timed_wait(os_thread_t* thread, os_tick_t tick);

volatile os_tick_t os_scheduler_get_current_tick(void);

#endif /*INCLUDED_OS_SCHEDULER_H*/
