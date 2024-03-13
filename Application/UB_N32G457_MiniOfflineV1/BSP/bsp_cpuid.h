#ifndef INCLUDED_BSP_CPUID_H
#define INCLUDED_BSP_CPUID_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32G45X_H
#define INCLUDED_N32G45X_H
#include <n32g45x.h>
#endif /*INCLUDED_N32G45X_H*/



////////////////////////////////////////////////////////////////////////////////
////
typedef struct BSP_CPUID_UID_S{
    uint8_t value[12];
}BSP_CPUID_UID_T;

typedef struct BSP_CPUID_UCID_S{
    uint8_t value[16];
}BSP_CPUID_UCID_T;


////////////////////////////////////////////////////////////////////////////////
////

void BSP_CPUID_GetUID(BSP_CPUID_UID_T* UID);

void BSP_CPUID_GetUCID(BSP_CPUID_UCID_T* UID);

const char* BSP_CPUID_GetUIDStr(void);
const char* BSP_CPUID_GetUCIDStr(void);

#endif /*INCLUDED_BSP_CPUID_H*/
