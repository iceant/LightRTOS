#ifndef INCLUDED_SVCHANDLER_H
#define INCLUDED_SVCHANDLER_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_CPU_TYPES_H
#include <cpu_types.h>
#endif /* INCLUDED_CPU_TYPES_H */


////////////////////////////////////////////////////////////////////////////////
////
extern volatile cpu_uintptr_t svc_exc_return;

typedef void (*SVC_Handler_Function)(void);

////////////////////////////////////////////////////////////////////////////////
////

void SVC_Handler_C(unsigned  int * svc_args);

void SVC_Handler_Set(uint8_t idx, SVC_Handler_Function fn);

#endif /*INCLUDED_SVCHANDLER_H*/
