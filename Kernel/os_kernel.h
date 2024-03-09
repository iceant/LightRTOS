#ifndef INCLUDED_OS_KERNEL_H
#define INCLUDED_OS_KERNEL_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_OS_COMPILER_H
#include <os_compiler.h>
#endif /*INCLUDED_OS_COMPILER_H*/

#ifndef INCLUDED_OS_TYPES_H
#include <os_types.h>
#endif /*INCLUDED_OS_TYPES_H*/

#ifndef INCLUDED_OS_MACROS_H
#include <os_macros.h>
#endif /*INCLUDED_OS_MACROS_H*/

#ifndef INCLUDED_OS_ERROR_H
#include <os_error.h>
#endif /*INCLUDED_OS_ERROR_H*/

#ifndef INCLUDED_OS_MEMORY_H
#include <os_memory.h>
#endif /*INCLUDED_OS_MEMORY_H*/

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_OS_THREAD_H
#include <os_thread.h>
#endif /*INCLUDED_OS_THREAD_H*/

#ifndef INCLUDED_OS_SCHEDULER_H
#include <os_scheduler.h>
#endif /*INCLUDED_OS_SCHEDULER_H*/

#ifndef INCLUDED_OS_SEM_H
#include <os_sem.h>
#endif /* INCLUDED_OS_SEM_H */

#ifndef INCLUDED_OS_MUTEX_H
#include <os_mutex.h>
#endif /*INCLUDED_OS_MUTEX_H*/

////////////////////////////////////////////////////////////////////////////////
////

#define OS_KERNEL_EMODULE_MEMORY        (-1001)
#define OS_KERNEL_EMODULE_SCHEDULER     (-1002)
////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_kernel_init(void);

os_err_t  os_kernel_startup(void);


#endif /*INCLUDED_OS_KERNEL_H*/
