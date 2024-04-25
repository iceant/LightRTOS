#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <sdk_hex.h>
#include <sdk_ring.h>
#include <sdk_crc16.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////
#define THREAD1_STACK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t BootThread_Stack[THREAD1_STACK_SIZE];
static os_thread_t BootThread;

////////////////////////////////////////////////////////////////////////////////
////

static void BootThread_Entry(void* p){
    printf("BootThread Startup...\r\n");
    OLED_TurnOn();
    os_thread_mdelay(1000);

    char buf[128]={0};
    int y = 0;
    while(1){
        y = 0;

        BSP_Led_Blue_Blink();

        snprintf(buf, sizeof(buf), "VOL-A: %.4f", 0.0000);
        OLED_ShowString(0, y++, (uint8_t*)buf, 8);

        snprintf(buf, sizeof(buf), "VOL-B: %.4f", 0.0000);
        OLED_ShowString(0, y++, (uint8_t*)buf, 8);

        snprintf(buf, sizeof(buf), "VOL-C: %.4f", 0.0000);
        OLED_ShowString(0, y++, (uint8_t*)buf, 8);

        snprintf(buf, sizeof(buf), "CUR-A: %.4f", 0.0000);
        OLED_ShowString(0, y++, (uint8_t*)buf, 8);

        snprintf(buf, sizeof(buf), "CUR-B: %.4f", 0.0000);
        OLED_ShowString(0, y++, (uint8_t*)buf, 8);

        snprintf(buf, sizeof(buf), "CUR-C: %.4f", 0.0000);
        OLED_ShowString(0, y++, (uint8_t*)buf, 8);

        snprintf(buf, sizeof(buf), "Energy: %.4f", 257543.2304);
        OLED_ShowString(0, y++, (uint8_t*)buf, 8);

        os_thread_mdelay(1000);
    }
}


////////////////////////////////////////////////////////////////////////////////
////
static void USART1__RxHandler(sdk_ringbuffer_t * buffer, void* userdata){

    if(sdk_ringbuffer_find_str(buffer, 0, "reboot")!=-1){
        cpu_reboot();
        return;
    }
    if(sdk_ringbuffer_find_str(buffer, 0, "\r\n")!=-1){
        printf("USART1: %s\n", buffer->buffer,sdk_ringbuffer_used(buffer));
        sdk_ringbuffer_reset(buffer);
    }
}

////////////////////////////////////////////////////////////////////////////////
////

int main(void){
    board_init();

    BSP_Led_Red_On();
    BSP_USART1_SetRxHandler(USART1__RxHandler, 0);

    os_kernel_init();

    os_thread_init(&BootThread, "BootThd", BootThread_Entry, 0
                   , BootThread_Stack, sizeof(BootThread_Stack), 10, 10);
    os_thread_startup(&BootThread);

    os_kernel_startup();
    
    while(1){}
}

