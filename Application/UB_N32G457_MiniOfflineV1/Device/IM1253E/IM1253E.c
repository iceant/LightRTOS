#include <IM1253E.h>
#include <sdk_ringbuffer.h>
#include <sdk_hex.h>
#include <sdk_crc16.h>
#include <stdio.h>
#include "bsp_tim2.h"

////////////////////////////////////////////////////////////////////////////////
////

//#define IM1253E_DEBUG_ENABLE

////////////////////////////////////////////////////////////////////////////////
////
static IM1253E_IO_T * IM1253E__IO = 0;

static os_mutex_t IM1253E__Mutex;
static uint8_t IM1253E__ReadAllCMD[]={0x01, 0x03, 0x00, 0x48, 0x00, 0x08, 0xC4, 0x1A};
static uint8_t IM1253E__ReadCVolMD[]={0x01, 0x03, 0x00, 0x48, 0x00, 0x01, 0x04, 0x1C};

////////////////////////////////////////////////////////////////////////////////
////

void IM1253E_Init(IM1253E_IO_T* IO){
    IM1253E__IO = IO;
    os_mutex_init(&IM1253E__Mutex);
}

////////////////////////////////////////////////////////////////////////////////
////

/*
 01 03 <BYTES> <BYTES> <CRC1><CRC2>
 */
static void IM1253E__GetVoltageHandler(sdk_ringbuffer_t * buffer, void* userdata){
    uint32_t* Voltage = userdata;
    int i;
    int start_idx = -1;

    int used = sdk_ringbuffer_used(buffer);
    if(used<5){
        /*数据不够，继续等待*/
        return;
    }

    for(i=0; i<(used-1); i++){
        if(sdk_ringbuffer_peek(buffer, i)==0x01 && sdk_ringbuffer_peek(buffer, i+1)==0x03){
            start_idx = i;
            break;
        }
    }

    if(start_idx==-1){
        /* 没有有效数据，重置 */
        sdk_ringbuffer_reset(buffer);
        IM1253E__IO->notify();
        return;
    }

    uint8_t DL = sdk_ringbuffer_peek(buffer, start_idx+2);
    #if defined(IM1253E_DEBUG_ENABLE)
    printf("start_idx: %d\n", start_idx);
    #endif
    if((start_idx + DL + 5) > used){
        /*数据没有接收完成，继续等待*/
        return;
    }

    uint16_t crc = SDK_HEX_GET_UINT16_LE(buffer->buffer, start_idx+3+DL);
    uint16_t chk_crc = sdk_crc16(buffer->buffer+start_idx, 3+DL);
    if(chk_crc!=crc){
        printf("IM1253E CRC Error: CRC=%x, CHK_CRC=%x\n", crc, chk_crc);
        /*错误的数据，返回*/
        sdk_ringbuffer_reset(buffer);
        IM1253E__IO->notify();
        return;
    }

    /* 到这里，收到的数据是正确的 */
    *Voltage = SDK_HEX_GET_UINT32_BE(buffer->buffer, start_idx+3);
    sdk_ringbuffer_reset(buffer);
    IM1253E__IO->notify();
}

os_err_t IM1253E_GetVoltage(uint32_t* Voltage, os_time_t timeout_ms)
{
    os_err_t err = OS_EOK;
    os_mutex_lock(&IM1253E__Mutex);
    IM1253E__IO->setRxHandler((void*)IM1253E__GetVoltageHandler, Voltage);
    IM1253E__IO->send(IM1253E__ReadCVolMD, sizeof(IM1253E__ReadCVolMD));
    #if defined(IM1253E_DEBUG_ENABLE)
    uint32_t start_tick = BSP_TIM2_GetTickCount();
    err = IM1253E__IO->wait(os_tick_from_millisecond(timeout_ms));
    volatile uint32_t end_tick = BSP_TIM2_GetTickCount();
    printf("IM1253E_GetVoltage: %ld, Used MS:%d, Ticks:%d\n", *Voltage, end_tick-start_tick, os_tick_from_millisecond(end_tick-start_tick));
    #else
    err = IM1253E__IO->wait(os_tick_from_millisecond(timeout_ms));
    #endif
    IM1253E__IO->setRxHandler(0, 0);
    os_mutex_unlock(&IM1253E__Mutex);
    return err;
}


////////////////////////////////////////////////////////////////////////////////
////

static void IM1253E_GetDataHandler(sdk_ringbuffer_t * buffer, void* userdata){
    IM1253E_Data_T* Data = userdata;
    int i;
    int start_idx = -1;

    int used = sdk_ringbuffer_used(buffer);
    if(used<5){
        /*数据不够，继续等待*/
        return;
    }

    for(i=0; i<(used-1); i++){
        if(sdk_ringbuffer_peek(buffer, i)==0x01 && sdk_ringbuffer_peek(buffer, i+1)==0x03){
            start_idx = i;
            break;
        }
    }

    if(start_idx==-1){
        /* 没有有效数据，重置 */
        sdk_ringbuffer_reset(buffer);
        IM1253E__IO->notify();
        return;
    }

    uint8_t DL = sdk_ringbuffer_peek(buffer, start_idx+2);
    if((start_idx + DL + 5) > used){
        /*数据没有接收完成，继续等待*/
        return;
    }

    uint16_t crc = SDK_HEX_GET_UINT16_LE(buffer->buffer, start_idx+3+DL);
    uint16_t chk_crc = sdk_crc16(buffer->buffer+start_idx, 3+DL);
    if(chk_crc!=crc){
        printf("IM1253E CRC Error: CRC=%x, CHK_CRC=%x\n", crc, chk_crc);
        /*错误的数据，返回*/
        sdk_ringbuffer_reset(buffer);
        IM1253E__IO->notify();
        return;
    }

    /* 到这里，收到的数据是正确的 */
    int idx = start_idx+3;
    Data->Voltage = SDK_HEX_GET_UINT32_BE(buffer->buffer, idx); idx+=4;
    Data->Current = SDK_HEX_GET_UINT32_BE(buffer->buffer, idx); idx+=4;
    Data->FoP = SDK_HEX_GET_UINT32_BE(buffer->buffer, idx); idx+=4;
    Data->FoE = SDK_HEX_GET_UINT32_BE(buffer->buffer, idx); idx+=4;
    Data->Factor = SDK_HEX_GET_UINT32_BE(buffer->buffer, idx); idx+=4;
    Data->O2C = SDK_HEX_GET_UINT32_BE(buffer->buffer, idx); idx+=4;
    Data->Temperature = SDK_HEX_GET_UINT32_BE(buffer->buffer, idx); idx+=4;
    Data->Frequency = SDK_HEX_GET_UINT32_BE(buffer->buffer, idx); idx+=4;

    sdk_ringbuffer_reset(buffer);
    IM1253E__IO->notify();
}

os_err_t IM1253E_GetData(IM1253E_Data_T * Data, os_time_t timeout_ms)
{
    os_err_t err = OS_EOK;
    os_mutex_lock(&IM1253E__Mutex);
    IM1253E__IO->setRxHandler((void*)IM1253E_GetDataHandler, Data);
    IM1253E__IO->send(IM1253E__ReadAllCMD, sizeof(IM1253E__ReadAllCMD));
    
    #if defined(IM1253E_DEBUG_ENABLE)
    uint32_t start_tick = BSP_TIM2_GetTickCount();
    #endif
    
    err = IM1253E__IO->wait(os_tick_from_millisecond(timeout_ms));
    
    #if defined(IM1253E_DEBUG_ENABLE)
    volatile uint32_t end_tick = BSP_TIM2_GetTickCount();
    printf("IM1253E_GetData: %ld, Used MS:%d, Ticks:%d\n", Data->Voltage, end_tick-start_tick, os_tick_from_millisecond(end_tick-start_tick));
    #endif
    
    IM1253E__IO->setRxHandler(0, 0);
    os_mutex_unlock(&IM1253E__Mutex);
    return err;
}







