#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <stdio.h>

#include <Console.h>
#include <NTP.h>
#include <SystemDateTime.h>
////////////////////////////////////////////////////////////////////////////////
////
#define THREAD1_STACK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t Boot_Thread_stack[THREAD1_STACK_SIZE];
static os_thread_t Boot_Thread;


////////////////////////////////////////////////////////////////////////////////
////

static void Boot_Thread_Entry(void* p){
    printf("Boot Thread Entry Enter...\n");

    Console_Init();
    
#if defined(NETWORK_ENABLE)
    printf("A7670C Startup...\n");
    A7670C_Result A7670C_StartupResult = A7670C_Startup();
    if(A7670C_StartupResult!=kA7670C_Result_OK){
        __svc(1);
    }

    NTP_Init();

    printf("SystemDateTime Startup...\n");
    SystemDateTime_Init();
#endif

    printf("MQTT Init in Boot...\n");
    MQTT_Init();

#if 1

    
    /* 用到内存，需要先初始化内存模块 */
//    EnergyMeter_Init();

    int nCount = 0;
    char * Message = "Send From Chen Peng";

    while(1){
        SystemDateTime_T dateTime;
        SystemDateTime_Get(&dateTime);
        printf("%04d-%02d-%02d %02d:%02d:%02d.%03d, Ticks:%ld\n"
               , dateTime.year
               , dateTime.month
               , dateTime.date
               , dateTime.hour
               , dateTime.minute
               , dateTime.second
               , dateTime.millisecond
               , BSP_TIM2_GetTickCount()
               );
        
        long Current = CHCS_CAB_500A_GetCurrent();
        printf("CHCS_CAB_500A Current: %ld\n", Current);

//        uint32_t Voltage = 0;
//        os_err_t err = IM1253E_GetVoltage(&Voltage, 120);
//        if(err!=OS_EOK){
//            printf("IM1253E_Voltage: %ld\n", Voltage);
//        }else{
//            printf("IM1253E_Voltage - Error %d\n", err);
//        }
//
//        IM1253E_Data_T IM1253E_Data;
//        IM1253E_GetData(&IM1253E_Data, 140);
//        printf("IM1253E_DATA Voltage: %ld\n", Voltage);

//        if(nCount++==5){
//            nCount = 0;
//            MQTT_Publish(Message, strlen(Message));
//        }

        os_thread_mdelay(5000);
    }
#endif
    while(1){
        os_thread_mdelay(1000);
    }

}

////////////////////////////////////////////////////////////////////////////////
////

int main(void){
    
    
    board_init();
    
    printf("CPU_UID: %s\n", BSP_CPUID_GetUIDStr());
    printf("CPU_UCID: %s\n", BSP_CPUID_GetUCIDStr());
    printf("MQTT: /DEVICES/%s/UPSTREAM\n", BSP_CPUID_GetUIDStr());

    
    os_kernel_init();

    Key_Init(); /* 先设置 KEY, 确保第一时间能对按键进行响应 */

    os_thread_init(&Boot_Thread, "BootThd", Boot_Thread_Entry, 0, Boot_Thread_stack, sizeof(Boot_Thread_stack), 10, 10);
    os_thread_startup(&Boot_Thread);

    os_kernel_startup();

#if 0
    int nCount = 0;
    char * Message = "Send From Chen Peng";
    while(1){
        nCount++;
        if(nCount==10){
            MQTT_Publish(Message, strlen(Message));
        }
        os_thread_mdelay(1000);
    }
#endif
}

