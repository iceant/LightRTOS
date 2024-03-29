#include <bsp_usart1.h>
#include <hw_usart.h>

#include <hw_usart.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////
typedef struct BSP_USART1_RxHandler_Record{
    BSP_USART1_RxHandler rx_handler;
    void* userdata;
    os_sem_t lock;
}BSP_USART1_RxHandler_Record;



////////////////////////////////////////////////////////////////////////////////
////
#define USART1_RX_THREAD_STACK_SIZE 1024
#define USART1_RX_BLOCK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t USART1_RxThreadStack[USART1_RX_THREAD_STACK_SIZE];
static os_sem_t USART1_RxSem;
static os_thread_t USART1_RxThread;
static uint8_t USART1_RxBlock[USART1_RX_BLOCK_SIZE];
static sdk_ringbuffer_t USART1_RxBuffer;
static BSP_USART1_RxHandler_Record bsp_usart1__rx_handler;
static os_bool_t BSP_USART1__WaitFlag = OS_FALSE;

////////////////////////////////////////////////////////////////////////////////
////

static void USART1_RxThreadEntry(void* p){
    while(1){
        os_sem_take(&USART1_RxSem, OS_WAIT_INFINITY);
        if(bsp_usart1__rx_handler.rx_handler){
            bsp_usart1__rx_handler.rx_handler(&USART1_RxBuffer, bsp_usart1__rx_handler.userdata);
        }else{
            if(sdk_ringbuffer_is_full(&USART1_RxBuffer)){
                sdk_ringbuffer_reset(&USART1_RxBuffer);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////
void USART1_IRQHandler(void){
    if (USART_GetIntStatus(USART1, USART_INT_RXDNE) != RESET)
    {
        sdk_ringbuffer_put(&USART1_RxBuffer, USART_ReceiveData(USART1));
        os_sem_release(&USART1_RxSem);
    }
}


////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART1_Init(void){
    bsp_usart1__rx_handler.rx_handler = 0;
    bsp_usart1__rx_handler.userdata = 0;
    os_sem_init(&bsp_usart1__rx_handler.lock, "USART1_WaitSem", 0, OS_QUEUE_FIFO);

    sdk_ringbuffer_init(&USART1_RxBuffer, USART1_RxBlock, sizeof(USART1_RxBlock));
    os_sem_init(&USART1_RxSem, "USART1_RxSem", 0, OS_QUEUE_FIFO);

    hw_usart_configuration(USART1, 115200, 0);
    hw_usart_enable_irq(USART1, USART1_IRQn, 0, HW_USART_IRQ_MODE_RX);

    os_thread_init(&USART1_RxThread, "USART1_RxThread", USART1_RxThreadEntry, 0
            , USART1_RxThreadStack, sizeof(USART1_RxThreadStack)
            , 20, 10);
    os_thread_startup(&USART1_RxThread);
}

void BSP_USART1_SetRxHandler(BSP_USART1_RxHandler rxHandler, void* userdata)
{
    USART_ConfigInt(USART1, USART_INT_RXDNE, DISABLE);
    bsp_usart1__rx_handler.rx_handler = rxHandler;
    bsp_usart1__rx_handler.userdata = userdata;
    USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
}

os_err_t BSP_USART1_Send(uint8_t * data, os_size_t size)
{
    hw_usart_dma_send(USART1, data, size);
    return OS_EOK;
}

os_err_t BSP_USART1_TimeWait(os_time_t timeout_ms)
{
    os_err_t err;
    BSP_USART1__WaitFlag = OS_TRUE;
    err =  os_sem_take(&bsp_usart1__rx_handler.lock, os_tick_from_millisecond(timeout_ms));
    BSP_USART1__WaitFlag = OS_FALSE;
    return err;
}

void BSP_USART1_Notify(void)
{
    if(BSP_USART1__WaitFlag==OS_TRUE){
        os_sem_release(&bsp_usart1__rx_handler.lock);
    }
}
