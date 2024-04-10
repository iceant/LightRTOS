#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////
#define USART3_THREAD_STACK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t USART3_ThreadStack[USART3_THREAD_STACK_SIZE];
static os_thread_t USART3_Thread;

static void USART3_ThreadEntry(void* parameter)
{
    int nCount = 0;
    const char * message = "Hello From STM32H750\r\n";
    while(1){
//        BSP_USART3_SendString("Hello From STM32H750\r\n");
        BSP_USART3_DMA_Send((uint8_t*)message, strlen(message));
        printf("USART3 Send %d...\r\n", nCount++);
        os_thread_mdelay(1000);
    }
}

////////////////////////////////////////////////////////////////////////////////
////
#define THREAD1_STACK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t Boot_Thread_stack[THREAD1_STACK_SIZE];
static os_thread_t Boot_Thread;

////////////////////////////////////////////////////////////////////////////////
////

static void Boot_Thread_Entry(void* p){
//    BSP_USART1_SendString("Boot Thread Entry Enter...\n");
    const char* message = "Boot Thread Entry Enter...\n";
    BSP_USART1_SendString(message);
//    BSP_USART1_DMA_Send((uint8_t *)message, strlen(message));

    /* -------------------------------------------------------------------------------------------------------------- */
    /* Startup USART3 Thread */
    /* -------------------------------------------------------------------------------------------------------------- */
    os_thread_init(&USART3_Thread, "USART3_Thd", USART3_ThreadEntry, 0
                   , USART3_ThreadStack, sizeof(USART3_ThreadStack), 20, 10);
    os_thread_startup(&USART3_Thread);


    /* -------------------------------------------------------------------------------------------------------------- */
    /* Boot Thread Loop */
    /* -------------------------------------------------------------------------------------------------------------- */

    int nCount = 0;
    while(1){
        printf("nCount:%d \r\n", nCount++);
        os_thread_mdelay(1000);
    }
}

////////////////////////////////////////////////////////////////////////////////
////

int main(void){
    board_init();

    const char* message = "Board Init Finished!\r\n";
//    BSP_USART1_SendString(message);
    BSP_USART1_DMA_Send((uint8_t *)message, strlen(message));

    os_kernel_init();

    os_thread_init(&Boot_Thread, "BootThd", Boot_Thread_Entry, 0, Boot_Thread_stack, sizeof(Boot_Thread_stack), 10, 10);
    os_thread_startup(&Boot_Thread);

    os_kernel_startup();

}

