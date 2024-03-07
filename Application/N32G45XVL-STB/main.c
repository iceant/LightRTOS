#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <stdio.h>

#if 1
////////////////////////////////////////////////////////////////////////////////
////
#define THREAD1_STACK_SIZE 1024
#define THREAD2_STACK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t thread1_stack[THREAD1_STACK_SIZE];
static os_thread_t thread1;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t thread2_stack[THREAD2_STACK_SIZE];
static os_thread_t thread2;

////////////////////////////////////////////////////////////////////////////////
////

static void thread1_entry(void* p){
    os_size_t timeout_ms = (os_size_t)p;
    os_size_t nCount = 0;
    while(1){
        printf("Thread:%d, nCount=%d\n", os_thread_self(), nCount++);
//        os_thread_mdelay(timeout_ms);
        os_thread_sleep((os_tick_t)p);
    }
}

////////////////////////////////////////////////////////////////////////////////
////
#define USART1_RX_BUFFER_SIZE 256
static uint8_t USART1_RxBuffer[USART1_RX_BUFFER_SIZE]={0};
static os_size_t USART1_RxIdx = 0;
static os_sem_t USART1_RxSem;
__ALIGNED(OS_ALIGN_SIZE)
static uint8_t USART1_RxThread_Stack[1024];
static os_thread_t USART1_RxThread;
static void USART1_RxThreadEntry(void* p)
{
    while(1){
        os_sem_take(&USART1_RxSem, OS_WAIT_INFINITY);
        if(USART1_RxBuffer[USART1_RxIdx-2]=='\r' && USART1_RxBuffer[USART1_RxIdx-1]=='\n'){
            printf("USART1: %s\n", USART1_RxBuffer);
            USART1_RxIdx = 0;
            memset(USART1_RxBuffer, 0, USART1_RX_BUFFER_SIZE);
        }
        if(USART1_RxIdx==USART1_RX_BUFFER_SIZE){
            USART1_RxIdx = 0;
            memset(USART1_RxBuffer, 0, USART1_RX_BUFFER_SIZE);
        }
    }
    
}

void USART1_IRQHandler(void){
    if (USART_GetIntStatus(USART1, USART_INT_RXDNE) != RESET)
    {
        /* Read one byte from the receive data register */
        USART1_RxBuffer[USART1_RxIdx++] = USART_ReceiveData(USART1);
        os_sem_release(&USART1_RxSem);
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////
////

int main(void){
    
    os_sem_init(&USART1_RxSem, "USART1_RxSem", 0, OS_QUEUE_FIFO);
    
    board_init();
    
    os_kernel_init();
    
    os_thread_init(&thread1, "Thread1", thread1_entry, 10, thread1_stack, sizeof(thread1_stack), 20, 5);
    os_thread_startup(&thread1);

    os_thread_init(&thread2, "Thread2", thread1_entry, 20, thread2_stack, sizeof(thread2_stack), 20, 5);
    os_thread_startup(&thread2);

    os_thread_init(&USART1_RxThread, "USART1_RxThread", USART1_RxThreadEntry, 0, USART1_RxThread_Stack, sizeof(USART1_RxThread_Stack), 20, 10);
    os_thread_startup(&USART1_RxThread);
    
    os_kernel_startup();
    
    while(1){}
}

