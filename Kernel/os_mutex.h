#ifndef INCLUDED_OS_MUTEX_H
#define INCLUDED_OS_MUTEX_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_OS_TYPES_H
#include <os_types.h>
#endif /*INCLUDED_OS_TYPES_H*/

#ifndef INCLUDED_OS_ERROR_H
#include <os_error.h>
#endif /*INCLUDED_OS_ERROR_H*/


////////////////////////////////////////////////////////////////////////////////
////
typedef struct os_mutex_s{
    volatile os_uint_t counter;
}os_mutex_t;

////////////////////////////////////////////////////////////////////////////////
////
void os_mutex_init(os_mutex_t * mutex);

os_err_t os_mutex_lock(os_mutex_t * mutex);

os_err_t os_mutex_try_lock(os_mutex_t * mutex);

os_err_t os_mutex_unlock(os_mutex_t * mutex);


#endif /*INCLUDED_OS_MUTEX_H*/
