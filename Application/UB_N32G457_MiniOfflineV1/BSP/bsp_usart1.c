#include <bsp_usart1.h>
#include <hw_usart.h>

////////////////////////////////////////////////////////////////////////////////
////
typedef struct bsp_usart1_rx_handler_s{
    BSP_USART1_RxHandler rx_handler;
    void* userdata;
    os_sem_t wait_sem;
}bsp_usart1_rx_handler_t;

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
static bsp_usart1_rx_handler_t bsp_usart1__rx_handler={.rx_handler = 0, .userdata = 0};

static void USART1_RxThreadEntry(void* p){
    while(1){
        os_sem_take(&USART1_RxSem, OS_WAIT_INFINITY);
        if(bsp_usart1__rx_handler.rx_handler){
            int result = bsp_usart1__rx_handler.rx_handler(&USART1_RxBuffer, bsp_usart1__rx_handler.userdata);
            if(result==BSP_USART1_RX_STATE_DONE || result==BSP_USART1_RX_STATE_RESET){
                sdk_ringbuffer_reset(&USART1_RxBuffer);
                os_sem_release(&bsp_usart1__rx_handler.wait_sem);
            }
        }
        if(sdk_ringbuffer_is_full(&USART1_RxBuffer)){
            sdk_ringbuffer_reset(&USART1_RxBuffer);
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
    sdk_ringbuffer_init(&USART1_RxBuffer, USART1_RxBlock, sizeof(USART1_RxBlock));
    os_sem_init(&USART1_RxSem, "USART1_RxSem", 0, OS_QUEUE_FIFO);
    os_sem_init(&bsp_usart1__rx_handler.wait_sem, "USART1_WaitSem", 0, OS_QUEUE_FIFO);

    hw_usart_configuration(USART1, 115200, 0);
    hw_usart_enable_irq(USART1, USART1_IRQn, 0, HW_USART_IRQ_MODE_RX);

    os_thread_init(&USART1_RxThread, "USART1_RxThread", USART1_RxThreadEntry, 0
                   , USART1_RxThreadStack, sizeof(USART1_RxThreadStack)
                   , 20, 10);
    os_thread_startup(&USART1_RxThread);
}

os_err_t BSP_USART1_Receive(BSP_USART1_RxHandler handler, void* userdata, os_time_t timeout_ms){
    bsp_usart1__rx_handler.rx_handler = handler;
    bsp_usart1__rx_handler.userdata = userdata;
    os_err_t err = os_sem_take(&bsp_usart1__rx_handler.wait_sem, os_tick_from_millisecond(timeout_ms));
    if(err==OS_ETIMEOUT){
        return err;
    }
    return OS_EOK;
}

os_err_t BSP_USART1_Send(uint8_t * data, os_size_t size)
{
    hw_usart_dma_send(USART1, data, size);
    return OS_EOK;
}

