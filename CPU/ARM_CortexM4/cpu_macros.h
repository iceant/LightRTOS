#ifndef INCLUDED_CPU_MACROS_H
#define INCLUDED_CPU_MACROS_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_CPU_TYPES_H
#include <cpu_types.h>
#endif /* INCLUDED_CPU_TYPES_H */

////////////////////////////////////////////////////////////////////////////////
////

#ifndef CPU_REG
#define CPU_REG(ADDRESS)  (*((volatile cpu_uintptr_t *)(ADDRESS)))
#endif

#ifndef SCB_ICSR
#define SCB_ICSR  0xE000ED04
#endif /*SCB_ICSR*/

#ifndef SCB_ICSR_PENDSVSET_Msk
#define SCB_ICSR_PENDSVSET_Msk 0x10000000
#endif /*SCB_ICSR_PENDSVSET_Msk*/


////////////////////////////////////////////////////////////////////////////////
////
#ifndef __ASM
#define __ASM __asm__
#endif /*__ASM*/

#ifndef   __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE __attribute__((always_inline)) static inline
#endif /*__STATIC_FORCEINLINE*/

#ifndef __svc
#define __svc(sNum) __ASM volatile("SVC %0" ::"M" (sNum))
#endif /*__svc*/

////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE void cpu_dmb(void)
{
    __ASM volatile ("dmb 0xF":::"memory");
}

__STATIC_FORCEINLINE cpu_uint_t cpu_ldrexw(volatile cpu_uint_t *addr)
{
    cpu_uint_t result;
    __ASM volatile ("ldrex %0, %1" : "=r" (result) : "Q" (*addr) );
    return(result);
}

__STATIC_FORCEINLINE cpu_uint_t cpu_strexw(cpu_uint_t value, volatile cpu_uint_t *addr)
{
    cpu_uint_t result;
    __ASM volatile ("strex %0, %2, %1" : "=&r" (result), "=Q" (*addr) : "r" (value) );
    return(result);
}

__STATIC_FORCEINLINE cpu_uint_t cpu_cas(volatile cpu_uint_t *dest, cpu_uint_t new_value, cpu_uint_t old_value){
    do
    {
        if (cpu_ldrexw(dest) != old_value) return 1; // Failure
    } while(cpu_strexw(new_value, dest));
    return 0;
}

__STATIC_FORCEINLINE cpu_uint_t cpu_get_IPSR(void)
{
    cpu_uint_t result;
    
    __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
    return(result);
}

__STATIC_FORCEINLINE void cpu_set_CONTROL(cpu_uint_t control)
{
    __ASM volatile ("MSR control, %0" : : "r" (control) : "memory");
}

__STATIC_FORCEINLINE cpu_uint_t cpu_get_CONTROL(void)
{
    cpu_uint_t result;
    
    __ASM volatile ("MRS %0, control" : "=r" (result) );
    return(result);
}

__STATIC_FORCEINLINE cpu_uint_t cpu_get_PRIMASK(void)
{
    cpu_uint_t result;
    
    __ASM volatile ("MRS %0, primask" : "=r" (result) :: "memory");
    return(result);
}

__STATIC_FORCEINLINE void cpu_set_PRIMASK(cpu_uint_t priMask)
{
    __ASM volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

#endif /* INCLUDED_CPU_MACROS_H */


