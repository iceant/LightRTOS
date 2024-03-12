#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <stdio.h>
#include <sdk_hex.h>
#include <hw_gpio.h>

#if 1
////////////////////////////////////////////////////////////////////////////////
////
#define THREAD1_STACK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t thread1_stack[THREAD1_STACK_SIZE];
static os_thread_t thread1;


////////////////////////////////////////////////////////////////////////////////
////

static void thread1_entry(void* p){

    while(1){
        printf("Thread: %s\n", os_thread_self()->name);
        os_thread_mdelay(1000);
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////
////

static void UART5__RxHandler(sdk_ringbuffer_t * buffer, void* userdata){
    if(sdk_ringbuffer_find_str(buffer, 0, "\r\n")!=-1){
        sdk_hex_dump("UART5 RX", buffer->buffer, sdk_ringbuffer_used(buffer));

        if(sdk_ringbuffer_find_str(buffer,0, "reboot")!=-1){
            __svc(1);
        }

        if(sdk_ringbuffer_find_str(buffer,0, "led on")!=-1){
            printf("LED ON ...\r\n");
            hw_gpio_init(GPIOB, GPIO_PIN_1, GPIO_Mode_AF_PP, GPIO_Pull_Up, GPIO_DC_2mA, GPIO_Slew_Rate_High, GPIO_AF6_UART4);

        }

        if(sdk_ringbuffer_find_str(buffer,0, "led off")!=-1){
            printf("LED OFF ...\r\n");
            hw_gpio_init(GPIOB, GPIO_PIN_1, GPIO_Mode_AF_PP, GPIO_No_Pull, GPIO_DC_2mA, GPIO_Slew_Rate_High, GPIO_NO_AF);
        }

        sdk_ringbuffer_reset(buffer);
    }
}

////////////////////////////////////////////////////////////////////////////////
////

int main(void){
    board_init();
    
    BSP_UART5_SetRxHandler(UART5__RxHandler, 0);
    
    os_kernel_init();

    os_thread_init(&thread1, "Thread1", thread1_entry, 0
                   , thread1_stack, sizeof(thread1_stack)
                   , 10, 10);
    os_thread_startup(&thread1);

    os_kernel_startup();
    
    while(1){}
}

