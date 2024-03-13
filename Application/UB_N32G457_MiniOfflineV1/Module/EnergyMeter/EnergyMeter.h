#ifndef INCLUDED_ENERGYMETER_H
#define INCLUDED_ENERGYMETER_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_SDK_AP_H
#include <sdk_ap.h>
#endif /*INCLUDED_SDK_AP_H*/

#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

////////////////////////////////////////////////////////////////////////////////
////
typedef struct EnergyMeter_Data_S{
    uint32_t Current;
    uint32_t Voltage;
    uint32_t TimeMS;
}EnergyMeter_Data_T;

////////////////////////////////////////////////////////////////////////////////
////

void EnergyMeter_Init(void);



#endif /* INCLUDED_ENERGYMETER_H */
