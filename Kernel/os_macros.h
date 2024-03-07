#ifndef INCLUDED_OS_MACROS_H
#define INCLUDED_OS_MACROS_H

////////////////////////////////////////////////////////////////////////////////
////

#define CONCAT_(prefix, suffix) prefix##suffix
/// Concatenate `prefix, suffix` into `prefixsuffix`

#define CONCAT(prefix, suffix) CONCAT_(prefix, suffix)

/// Make a unique variable name containing the line number at the end of the
/// name. Ex: `uint64_t MAKE_UNIQUE_VARIABLE_NAME(counter) = 0;` would
/// produce `uint64_t counter_7 = 0` if the call is on line 7!
#define MAKE_UNIQUE_VARIABLE_NAME(prefix) CONCAT(prefix##_, __LINE__)

////////////////////////////////////////////////////////////////////////////////
////

#define OS_ALIGN_DOWN(x,align) (x & ~(align-1))
#define OS_ALIGN_UP(x,align) ((x + （align-1)) & ~(align-1))

#define OS_CONTAINER_OF(ptr, type, member) ((type*)(((char*)ptr) - (os_uintptr_t)(&(((type*)0)->member))))

#define OS_MAX(a, b) ((a)>(b)?(a):(b))
#define OS_MIN(a, b) ((a)>(b)?(b):(a))

#define OS_PAGE(T, SZ) (((T) + (SZ) -1)/(SZ))


#endif /*INCLUDED_OS_MACROS_H*/
