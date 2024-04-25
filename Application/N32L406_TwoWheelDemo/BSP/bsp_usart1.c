#include <bsp_usart1.h>
#include <os_kernel.h>
#include <sdk_ringbuffer.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////
#define HW_USARTx USART1
#define HW_USARTx_TxGPIO GPIOA
#define HW_USARTx_TxPIN  GPIO_PIN_9
#define HW_USARTx_RxGPIO GPIOA
#define HW_USARTx_RxPIN  GPIO_PIN_10
#define HW_USARTx_BAUDRATE  115200

#define USART1_RX_BLOCK_SIZE 1024
#define USART1_RX_THREAD_STACK_SIZE 1024

#define USART1_RX_THREAD_PRIORITY   20
#define USART1_RX_THREAD_TIMESLICE  10

static hw_usart_t HW_USART_OBJ;

static uint8_t USART1_RxBlock[USART1_RX_BLOCK_SIZE];
static sdk_ringbuffer_t USART1_RxBuffer;
static os_sem_t USART1_RxSem;
static os_sem_t USART1_NotifySem;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t USART1_RxThread_Stack[USART1_RX_THREAD_STACK_SIZE];
static os_thread_t USART1_RxThread;

static BSP_USART1_RxHandler BSP_USART1__RxHandler=0;
static void* BSP_USART1__RxHandler_Parameter=0;
static os_bool_t BSP_USART1__RxThread_Flag = OS_FALSE;
////////////////////////////////////////////////////////////////////////////////
////

static void USART1_RxThreadEntry(void* parameter){
    printf("USART1_RxThreadEntry Startup...\n");
    BSP_USART1__RxThread_Flag = OS_TRUE;
    while(1){
        os_sem_take(&USART1_RxSem, OS_WAIT_INFINITY);
        if(BSP_USART1__RxHandler){
            BSP_USART1__RxHandler(&USART1_RxBuffer, BSP_USART1__RxHandler_Parameter);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void USART1_IRQHandler(void){
    if (USART_GetIntStatus(HW_USARTx, USART_INT_RXDNE) != RESET)
    {
        uint8_t ch =  USART_ReceiveData(HW_USARTx);
        if(BSP_USART1__RxThread_Flag){
            sdk_ringbuffer_put(&USART1_RxBuffer,ch);
            os_sem_release(&USART1_RxSem);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART1_Init(void)
{
    sdk_ringbuffer_init(&USART1_RxBuffer, USART1_RxBlock, sizeof(USART1_RxBlock));
    os_sem_init(&USART1_RxSem, "USART1_RxSem", 0, OS_QUEUE_FIFO);
    
    hw_usart_enable_dma();
    hw_usart_init(&HW_USART_OBJ, HW_USARTx, HW_USARTx_TxGPIO, HW_USARTx_TxPIN, HW_USARTx_RxGPIO, HW_USARTx_RxPIN, HW_USARTx_BAUDRATE);
    hw_usart_enable_irq(HW_USARTx, USART1_IRQn, 0, HW_USART_IRQ_MODE_RX);
    
    os_sem_init(&USART1_NotifySem, "USART1_NtySem", 0, OS_QUEUE_FIFO);
    os_thread_init(&USART1_RxThread, "USART1_RxThd", USART1_RxThreadEntry, 0
                   , USART1_RxThread_Stack, sizeof(USART1_RxThread_Stack)
                   , USART1_RX_THREAD_PRIORITY
                   , USART1_RX_THREAD_TIMESLICE
                   );
    os_thread_startup(&USART1_RxThread);
}

void BSP_USART1_SetRxHandler(BSP_USART1_RxHandler rxHandler, void* userdata){
    BSP_USART1__RxHandler= rxHandler;
    BSP_USART1__RxHandler_Parameter = userdata;
}

int BSP_USART1_Send(uint8_t* data, int size){
    hw_usart_send(HW_USARTx, data, size);
}

int BSP_USART1_TimeWait(uint32_t timeout_ms){
    os_sem_take(&USART1_NotifySem, os_tick_from_millisecond(timeout_ms));
}

void BSP_USART1_Notify(void){
    os_sem_release(&USART1_NotifySem);
}

