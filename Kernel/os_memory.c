#include <os_memory.h>
#include <string.h>
#include <stdlib.h>
////////////////////////////////////////////////////////////////////////////////
////

void* os_memory_alloc(os_size_t nBytes, const char* file, os_size_t line)
{
    return malloc(nBytes);
}

void* os_memory_calloc(os_size_t nCount, os_size_t nBytes, const char* file, os_size_t line)
{
    return calloc(nCount, nBytes);
}

void* os_memory_realloc(void* mem, os_size_t nBytes, const char* file, os_size_t line)
{
    return realloc(mem, nBytes);
}

void os_memory_free(void* mem, const char* file, os_size_t line)
{
    if(mem){
        free(mem);
    }
}

