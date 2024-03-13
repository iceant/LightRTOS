#include <bsp_uart5.h>
#include <hw_gpio.h>
#include <hw_usart.h>
#include <stdio.h>
#include <sdk_hex.h>
////////////////////////////////////////////////////////////////////////////////
////
typedef struct BSP_UART5_RxHandler_Record{
    BSP_UART5_RxHandler rx_handler;
    void* userdata;
    os_sem_t lock;
}BSP_UART5_RxHandler_Record;


////////////////////////////////////////////////////////////////////////////////
////
#define UART5_RX_THREAD_STACK_SIZE 1024
#define UART5_RX_BLOCK_SIZE (10240+1024)

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t UART5_RxThreadStack[UART5_RX_THREAD_STACK_SIZE];
static os_sem_t UART5_RxSem;
static os_thread_t UART5_RxThread;
static uint8_t UART5_RxBlock[UART5_RX_BLOCK_SIZE];
static sdk_ringbuffer_t UART5_RxBuffer;
static BSP_UART5_RxHandler_Record bsp_uart5__rx_handler={.rx_handler  = 0, .userdata = 0};
static os_size_t UART5_RxBufferUsed = 0;

static void UART5_RxThreadEntry(void* p){
    while(1){
        os_sem_take(&UART5_RxSem, OS_WAIT_INFINITY);
        os_size_t used = sdk_ringbuffer_used(&UART5_RxBuffer);
        if(used>0 && used!=UART5_RxBufferUsed){
            UART5_RxBufferUsed = used;
            sdk_hex_dump("USART5_RxBuffer", UART5_RxBuffer.buffer, used);
        }
        if(bsp_uart5__rx_handler.rx_handler){
            bsp_uart5__rx_handler.rx_handler(&UART5_RxBuffer, bsp_uart5__rx_handler.userdata);
        }else{
            if(sdk_ringbuffer_is_full(&UART5_RxBuffer)){
                sdk_ringbuffer_reset(&UART5_RxBuffer);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////
void UART5_IRQHandler(void){
    if (USART_GetIntStatus(UART5, USART_INT_RXDNE) != RESET)
    {
        sdk_ringbuffer_put(&UART5_RxBuffer,  USART_ReceiveData(UART5));
        os_sem_release(&UART5_RxSem);
    }
}


////////////////////////////////////////////////////////////////////////////////
////

void BSP_UART5_Init(void){

    bsp_uart5__rx_handler.rx_handler = 0;
    bsp_uart5__rx_handler.userdata = 0;
    os_sem_init(&bsp_uart5__rx_handler.lock, "UART5_WaitSem", 0, OS_QUEUE_FIFO);

    sdk_ringbuffer_init(&UART5_RxBuffer, UART5_RxBlock, sizeof(UART5_RxBlock));
    os_sem_init(&UART5_RxSem, "UART5_RxSem", 0, OS_QUEUE_FIFO);

    hw_usart_dma_enable();
    hw_usart_configuration(UART5, 115200, GPIO_RMP2_UART5);
    hw_usart_enable_irq(UART5, UART5_IRQn, 0, HW_USART_IRQ_MODE_RX);

    os_thread_init(&UART5_RxThread, "UART5_RxThread"
                   , UART5_RxThreadEntry, 0
            , UART5_RxThreadStack, sizeof(UART5_RxThreadStack)
            , 20, 10);

    os_thread_startup(&UART5_RxThread);

}

void BSP_UART5_SetRxHandler(BSP_UART5_RxHandler rxHandler, void* userdata)
{
    printf("BSP_UART5_SetRxHandler: %x, %x\n", rxHandler, userdata);
    bsp_uart5__rx_handler.rx_handler = rxHandler;
    bsp_uart5__rx_handler.userdata = userdata;
}

os_err_t BSP_UART5_Send(uint8_t * data, os_size_t size)
{
    UART5_RxBufferUsed = 0;

    sdk_hex_dump("UART5_Send", data, size);

    sdk_ringbuffer_reset(&UART5_RxBuffer);
    hw_usart_dma_send(UART5, data, size);
//    hw_usart_send(UART5, data, size);

    return OS_EOK;
}

os_err_t BSP_UART5_TimeWait(os_tick_t ticks)
{

    os_err_t err = os_sem_take(&bsp_uart5__rx_handler.lock, ticks);
    UART5_RxBufferUsed = 0;
    printf("BSP_UART5_TimeWait: %d ticks, handler:%x return: %d\n", ticks, bsp_uart5__rx_handler.rx_handler,  err);
    return err;
}

void BSP_UART5_Notify(void){
    os_sem_release(&bsp_uart5__rx_handler.lock);
}

