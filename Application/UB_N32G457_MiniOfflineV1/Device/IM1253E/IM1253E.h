#ifndef INCLUDED_IM1253E_H
#define INCLUDED_IM1253E_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

/*
 * 注意:
 * IM1253E 只读取电压，最少需要等待 120 毫秒，也就是每次读取电压，需要设置 timeout_ms 最少为 120, 建议 130
 * IM1253E 读取所有数据，最少需要等待 140 毫秒，也就是每次读取，需要设置 timeout_ms 最少为 140，建议 150
 */



////////////////////////////////////////////////////////////////////////////////
////
typedef struct IM1253E_IO_S{
    void (*setRxHandler)(void* handler, void* userdata);
    os_err_t (*wait)(os_tick_t ticks);
    os_err_t (*send)(uint8_t * data, os_size_t size);
    void (*notify)(void);
}IM1253E_IO_T;

typedef struct IM1253E_Data_S{
    uint32_t Voltage;
    uint32_t Current;
    uint32_t FoP; /*有功功率*/
    uint32_t FoE; /*有功总电量*/
    uint32_t Factor; /*功率因素*/
    uint32_t O2C; /*二氧化碳排放量*/
    uint32_t Temperature;
    uint32_t Frequency;
}IM1253E_Data_T;

////////////////////////////////////////////////////////////////////////////////
////
void IM1253E_Init(IM1253E_IO_T* IO);

os_err_t IM1253E_GetVoltage(uint32_t * Voltage, os_time_t timeout_ms);

os_err_t IM1253E_GetData(IM1253E_Data_T * Data, os_time_t timeout_ms);

#endif /*INCLUDED_IM1253E_H*/
