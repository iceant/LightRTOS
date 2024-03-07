#ifndef INCLUDED_OS_TYPES_H
#define INCLUDED_OS_TYPES_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif /*INCLUDED_STDINT_H*/

#ifndef INCLUDED_STDDEF_H
#define INCLUDED_STDDEF_H
#include <stddef.h>
#endif /*INCLUDED_STDDEF_H*/

#ifndef INCLUDED_STDBOOL_H
#define INCLUDED_STDBOOL_H
#include <stdbool.h>
#endif /*INCLUDED_STDBOOL_H*/

#ifndef INCLUDED_INTTYPES_H
#define INCLUDED_INTTYPES_H
#include <inttypes.h>
#endif /*INCLUDED_INTTYPES_H*/

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_OS_CONFIG_H
#include <os_config.h>
#endif /*INCLUDED_OS_CONFIG_H*/

#ifndef INCLUDED_OS_COMPILER_H
#include <os_compiler.h>
#endif /*INCLUDED_OS_COMPILER_H*/

#ifndef INCLUDED_OS_MACROS_H
#include <os_macros.h>
#endif /*INCLUDED_OS_MACROS_H*/


////////////////////////////////////////////////////////////////////////////////
////

#if defined(CPU_8BIT)
    #define OS_CPU_BIT 8
    #define OS_ALIGN_SIZE 1
#endif

#if defined(CPU_16BIT)
    #define OS_CPU_BIT 16
    #define OS_ALIGN_SIZE 2
#endif

#if defined(CPU_32BIT)
    #define OS_CPU_BIT 32
    #define OS_ALIGN_SIZE 4
#endif

#if defined(CPU_64BIT)
    #define OS_CPU_BIT 64
    #define OS_ALIGN_SIZE 8
#endif

typedef CONCAT(int, CONCAT(OS_CPU_BIT, _t)) os_int_t;
typedef CONCAT(uint, CONCAT(OS_CPU_BIT, _t)) os_uint_t;
typedef CONCAT(uint, CONCAT(OS_CPU_BIT, _t)) os_size_t;
typedef CONCAT(uint, CONCAT(OS_CPU_BIT, _t)) os_uintptr_t;
typedef CONCAT(int, CONCAT(OS_CPU_BIT, _t)) os_intptr_t;

#define OS_PRId CONCAT(PRId, OS_CPU_BIT)
#define OS_PRIi CONCAT(PRIi, OS_CPU_BIT)
#define OS_PRIo CONCAT(PRIo, OS_CPU_BIT)
#define OS_PRIu CONCAT(PRIu, OS_CPU_BIT)
#define OS_PRIx CONCAT(PRIx, OS_CPU_BIT)
#define OS_PRIX CONCAT(PRIX, OS_CPU_BIT)

////////////////////////////////////////////////////////////////////////////////
////

#define os_bool_t bool
#define OS_TRUE   true
#define OS_FALSE  false

////////////////////////////////////////////////////////////////////////////////
////

////////////////////////////////////////////////////////////////////////////////
//// CONSTANTS

#define OS_WAIT_INFINITY (-1U)

#define OS_QUEUE_FIFO (1<<0)
#define OS_QUEUE_PRIO (1<<1)






#endif /*INCLUDED_OS_TYPES_H*/
