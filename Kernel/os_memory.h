#ifndef INCLUDED_OS_MEMORY_H
#define INCLUDED_OS_MEMORY_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_OS_TYPES_H
#include <os_types.h>
#endif /*INCLUDED_OS_TYPES_H*/

#ifndef INCLUDED_STRING_H
#define INCLUDED_STRING_H
#include <string.h>
#endif /*INCLUDED_STRING_H*/

////////////////////////////////////////////////////////////////////////////////
////
void* os_memory_alloc(os_size_t nBytes, const char* file, os_size_t line);
void* os_memory_calloc(os_size_t nCount, os_size_t nBytes, const char* file, os_size_t line);
void* os_memory_realloc(void* mem, os_size_t nBytes, const char* file, os_size_t line);
void os_memory_free(void* mem, const char* file, os_size_t line);

////////////////////////////////////////////////////////////////////////////////
////
#define OS_ALLOC(SZ) os_memory_alloc((SZ), __FILE__, __LINE__)
#define OS_CALLOC(N, SZ) os_memory_calloc((N), (SZ), __FILE__, __LINE__)
#define OS_REALLOC(P, SZ) os_memory_realloc((P), (SZ), __FILE__, __LINE__)
#define OS_RESIZE(P, SZ) (P)=os_memory_realloc((P), (SZ), __FILE__, __LINE__)
#define OS_FREE(P) (os_memory_free((P), __FILE__, __LINE__), (P)=0)

#define OS_NEW(P) (P) = OS_ALLOC(sizeof(*(P)))
#define OS_NEW0(P) (P) = OS_CALLOC(1, sizeof(*(P)))


#endif /*INCLUDED_OS_MEMORY_H*/
