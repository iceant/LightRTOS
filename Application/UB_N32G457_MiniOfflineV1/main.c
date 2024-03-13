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
    
    Console_Init();
    
    A7670C_Startup();

    NTP_Init();

    SystemDateTime_Init();
    
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
        os_thread_mdelay(1000);
    }
}

////////////////////////////////////////////////////////////////////////////////
////

int main(void){
    board_init();

    printf("CPU_UID: %s\n", BSP_CPUID_GetUIDStr());
    printf("CPU_UCID: %s\n", BSP_CPUID_GetUCIDStr());

    os_kernel_init();

    os_thread_init(&Boot_Thread, "BootThd", Boot_Thread_Entry, 0, Boot_Thread_stack, sizeof(Boot_Thread_stack), 10, 10);
    os_thread_startup(&Boot_Thread);

    os_kernel_startup();
    
    while(1){}
}

