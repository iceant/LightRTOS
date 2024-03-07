#ifndef INCLUDED_OS_SPINLOCK_H
#define INCLUDED_OS_SPINLOCK_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_OS_TYPES_H
#include <os_types.h>
#endif /*INCLUDED_OS_TYPES_H*/

#ifndef INCLUDED_CPU_ATOMIC_H
#include <cpu_atomic.h>
#endif /*INCLUDED_CPU_ATOMIC_H*/

#ifndef INCLUDED_OS_ERROR_H
#include <os_error.h>
#endif /*INCLUDED_OS_ERROR_H*/


////////////////////////////////////////////////////////////////////////////////
////
typedef struct os_spinlock_s{
    cpu_atomic_t atomic;
}os_spinlock_t;

////////////////////////////////////////////////////////////////////////////////
////
os_err_t os_spinlock_init(os_spinlock_t * spinlock);

void os_spinlock_lock(os_spinlock_t * spinlock);

os_err_t os_spinlock_try_lock(os_spinlock_t * spinlock);

void os_spinlock_unlock(os_spinlock_t *spinlock);


#endif /*INCLUDED_OS_SPINLOCK_H*/
