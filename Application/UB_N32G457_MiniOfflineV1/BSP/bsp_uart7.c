#include <bsp_uart7.h>
#include <hw_usart.h>
#include <stdio.h>
#include <sdk_hex.h>

////////////////////////////////////////////////////////////////////////////////
////
typedef struct BSP_UART7_RxHandler_Record{
    BSP_UART7_RxHandler rx_handler;
    void* userdata;
    os_sem_t lock;
}BSP_UART7_RxHandler_Record;


////////////////////////////////////////////////////////////////////////////////
////
//#define BSP_UART7_DEBUG_ENABLE
#define BSP_UARTx           UART7
#define BSP_UARTx_REMAP     GPIO_RMP1_UART7
#define BSP_UARTx_BAUDRATE  4800

#define UART7_RX_THREAD_STACK_SIZE 1024
#define UART7_RX_BLOCK_SIZE (1024)

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t UART7_RxThreadStack[UART7_RX_THREAD_STACK_SIZE];
static os_sem_t UART7_RxSem;
static os_thread_t UART7_RxThread;
static uint8_t UART7_RxBlock[UART7_RX_BLOCK_SIZE];
static sdk_ringbuffer_t UART7_RxBuffer;
static BSP_UART7_RxHandler_Record bsp_uart7__rx_handler={.rx_handler  = 0, .userdata = 0};

static os_bool_t UART7_RxThreadFlag = OS_FALSE;
static os_bool_t UART7_TimeWaitFlag = OS_FALSE;

#if defined(BSP_UART7_DEBUG_ENABLE)
static os_size_t UART7_RxBufferUsed = 0;
#endif

////////////////////////////////////////////////////////////////////////////////
////

static void UART7_RxThreadEntry(void* p){
    UART7_RxThreadFlag = OS_TRUE;
    while(1){
        os_sem_take(&UART7_RxSem, OS_WAIT_INFINITY);
#if defined(BSP_UART7_DEBUG_ENABLE)
        os_size_t used = sdk_ringbuffer_used(&UART7_RxBuffer);
        if(used>0 && used!=UART7_RxBufferUsed){
            UART7_RxBufferUsed = used;
            sdk_hex_dump("UART7_RxBuffer", UART7_RxBuffer.buffer, used);
        }
#endif
        if(bsp_uart7__rx_handler.rx_handler){
            bsp_uart7__rx_handler.rx_handler(&UART7_RxBuffer, bsp_uart7__rx_handler.userdata);
        }else{
            if(sdk_ringbuffer_is_full(&UART7_RxBuffer)){
                sdk_ringbuffer_reset(&UART7_RxBuffer);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////
void UART7_IRQHandler(void){
    if (USART_GetIntStatus(BSP_UARTx, USART_INT_RXDNE) != RESET)
    {
        sdk_ringbuffer_put(&UART7_RxBuffer,  USART_ReceiveData(BSP_UARTx));
        if(UART7_RxThreadFlag==OS_TRUE) {
            os_sem_release(&UART7_RxSem);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
////

void BSP_UART7_Init(void){

    bsp_uart7__rx_handler.rx_handler = 0;
    bsp_uart7__rx_handler.userdata = 0;
    os_sem_init(&bsp_uart7__rx_handler.lock, "UART7_WaitSem", 0, OS_QUEUE_FIFO);

    sdk_ringbuffer_init(&UART7_RxBuffer, UART7_RxBlock, sizeof(UART7_RxBlock));
    os_sem_init(&UART7_RxSem, "UART7_RxSem", 0, OS_QUEUE_FIFO);

    hw_usart_dma_enable();
    hw_usart_configuration(BSP_UARTx, BSP_UARTx_BAUDRATE, BSP_UARTx_REMAP);
    hw_usart_enable_irq(BSP_UARTx, UART7_IRQn, 0, HW_USART_IRQ_MODE_RX);

    os_thread_init(&UART7_RxThread, "UART7_RxThread"
            , UART7_RxThreadEntry, 0
            , UART7_RxThreadStack, sizeof(UART7_RxThreadStack)
            , 20, 10);

    os_thread_startup(&UART7_RxThread);

}

void BSP_UART7_SetRxHandler(BSP_UART7_RxHandler rxHandler, void* userdata)
{
    #if defined(BSP_UART7_DEBUG_ENABLE)
    printf("BSP_UART7_SetRxHandler: %x, %x\n", rxHandler, userdata);
    #endif
    bsp_uart7__rx_handler.rx_handler = rxHandler;
    bsp_uart7__rx_handler.userdata = userdata;
}

os_err_t BSP_UART7_Send(uint8_t * data, os_size_t size)
{

#if defined(BSP_UART7_DEBUG_ENABLE)
    UART7_RxBufferUsed = 0;
    sdk_hex_dump("UART7_Send", data, size);
#endif
    
    sdk_ringbuffer_reset(&UART7_RxBuffer);
//    hw_usart_dma_send(BSP_UARTx, data, size);
    hw_usart_send(BSP_UARTx, data, size);

    return OS_EOK;
}

os_err_t BSP_UART7_TimeWait(os_tick_t ticks)
{
    UART7_TimeWaitFlag = OS_TRUE;
    os_err_t err = os_sem_take(&bsp_uart7__rx_handler.lock, ticks);
    UART7_TimeWaitFlag = OS_FALSE;
    #if defined(BSP_UART7_DEBUG_ENABLE)
    UART7_RxBufferUsed = 0;
    printf("BSP_UART7_TimeWait: %d ticks, handler:%x return: %d\n", ticks, bsp_uart7__rx_handler.rx_handler, err);
    #endif
    return err;
}

void BSP_UART7_Notify(void){
    if(UART7_TimeWaitFlag){
        #if defined(BSP_UART7_DEBUG_ENABLE)
        printf("BSP_UART7_Notify\n");
        #endif
        os_sem_release(&bsp_uart7__rx_handler.lock);
    }
}
