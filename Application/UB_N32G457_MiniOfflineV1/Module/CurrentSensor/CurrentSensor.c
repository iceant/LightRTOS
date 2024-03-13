#include <CurrentSensor.h>
#include <bsp_can2.h>
#include <bsp_tim2.h>
#include <sdk_hex.h>
#include <stdio.h>

/*
 1078 ms - 110条数据，平均 10ms/1条 数据
 */

////////////////////////////////////////////////////////////////////////////////
////
static os_size_t nCount = 0;
static os_size_t nStartTick = 0;

static void CurrentSensor__DataHandler(sdk_ring_t * ring, void* userdata){
    for(os_size_t i=0; i<sdk_ring_used(ring); i++){
        CanRxMessage * Message = sdk_ring_pop(ring);
        uint32_t Current = SDK_HEX_GET_UINT32_BE(Message->Data, 0);
        Current = Current - 0x80000000;

        nCount++;
//        if(nCount%110==0){
//            printf("nCount:%ld, Tick:%ld, Current:%ld\n", nCount, BSP_TIM2_TickCount, Current);
//        }
        if(nCount%1000==0){
            printf("nCount:%ld, Tick:%ld, Current:%ld\n", nCount, BSP_TIM2_TickCount, Current);
        }
    }
    
    
}

////////////////////////////////////////////////////////////////////////////////
////


void CurrentSensor_Init(void)
{
    BSP_CAN2_SetRxHandler(CurrentSensor__DataHandler, 0);
}

