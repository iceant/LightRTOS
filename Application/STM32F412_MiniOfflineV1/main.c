#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <stdio.h>
#include <sdk_ringbuffer.h>



////////////////////////////////////////////////////////////////////////////////
////
#define USART1_RX_BLOCK_SZ 256
#define USART1_RX_THREAD_STACK_SIZE 1024

static uint8_t USART1_RxBlock[USART1_RX_BLOCK_SZ];
static sdk_ringbuffer_t USART1_RxBuffer;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t USART1_RxThreadStack[USART1_RX_THREAD_STACK_SIZE];
static os_thread_t USART1_RxThread;
static os_sem_t USART1_RxSem;

static void USART1_RxCallback(uint8_t data, void* userdata)
{
    sdk_ringbuffer_put(&USART1_RxBuffer, data);
    os_sem_release(&USART1_RxSem);
}

static void USART1_RxThreadEntry(void* parameter){
    while(1){
        os_sem_take(&USART1_RxSem, OS_WAIT_INFINITY);
        if(sdk_ringbuffer_find_str(&USART1_RxBuffer, 0, "\r\n")!=-1){
            printf("[USART1] %s\r\n", USART1_RxBuffer.buffer);
            sdk_ringbuffer_reset(&USART1_RxBuffer);
        }
        if(sdk_ringbuffer_is_full(&USART1_RxBuffer)){
            sdk_ringbuffer_reset(&USART1_RxBuffer);
        }
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
    /* 初始化接收线程 */
    sdk_ringbuffer_init(&USART1_RxBuffer, USART1_RxBlock, USART1_RX_BLOCK_SZ);
    os_sem_init(&USART1_RxSem, "USART1_RxSem", 0, OS_QUEUE_FIFO);
    BSP_USART1_SetReceiveCallback(USART1_RxCallback, 0);

    os_thread_init(&USART1_RxThread, "USART1_RxThd", USART1_RxThreadEntry, 0, USART1_RxThreadStack, sizeof(USART1_RxThreadStack), 20, 10);
    os_thread_startup(&USART1_RxThread);
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
    BSP_USART1_SendString(message);
//    BSP_USART1_DMA_Send((uint8_t *)message, strlen(message));

    os_kernel_init();

    os_thread_init(&Boot_Thread, "BootThd", Boot_Thread_Entry, 0, Boot_Thread_stack, sizeof(Boot_Thread_stack), 10, 10);
    os_thread_startup(&Boot_Thread);

    os_kernel_startup();

}

