#include <bsp_uart5.h>

#include <stdio.h>
#include <sdk_hex.h>
#include <assert.h>
#include <os_mutex.h>
////////////////////////////////////////////////////////////////////////////////
////
#define HW_USARTx UART5
#define HW_USARTx_TxGPIO GPIOB
#define HW_USARTx_TxPIN  GPIO_PIN_4
#define HW_USARTx_RxGPIO GPIOB
#define HW_USARTx_RxPIN  GPIO_PIN_5
#define HW_USARTx_BAUDRATE  115200

#define UART5_RX_BLOCK_SIZE 512
#define UART5_RX_THREAD_STACK_SIZE 1024

#define UART5_RX_THREAD_PRIORITY   20
#define UART5_RX_THREAD_TIMESLICE  10

////////////////////////////////////////////////////////////////////////////////
////
typedef struct BPS_UART5_RxHandlerRecord{
    BSP_UART5_RxHandler handler;
    void* userdata;
}BPS_UART5_RxHandlerRecord;

////////////////////////////////////////////////////////////////////////////////
////
static os_mutex_t BSP_UART5__Mutex;
static hw_usart_t HW_USART_OBJ;

static uint8_t UART5_RxBlock[UART5_RX_BLOCK_SIZE];
static sdk_ringbuffer_t UART5_RxBuffer;
static os_sem_t UART5_RxSem;
static os_sem_t UART5_NotifySem;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t UART5_RxThread_Stack[UART5_RX_THREAD_STACK_SIZE];
static os_thread_t UART5_RxThread;

static BPS_UART5_RxHandlerRecord BSP_UART5__RxHandler={0, 0};

static volatile os_bool_t UART5_RxThreadFlag = OS_FALSE;

static void Delay(uint32_t time){
    for(;time>0; time--){
        for(int i=68; i>0; i--);
    }
}
////////////////////////////////////////////////////////////////////////////////
////

static void UART5_RxThreadEntry(void* parameter){
    UART5_RxThreadFlag = OS_TRUE;

    while(1){
        os_sem_take(&UART5_RxSem, OS_WAIT_INFINITY);

        if(BSP_UART5__RxHandler.handler){
            BSP_UART5__RxHandler.handler(&UART5_RxBuffer, BSP_UART5__RxHandler.userdata);
        }

        if(sdk_ringbuffer_is_full(&UART5_RxBuffer)){
            sdk_ringbuffer_reset(&UART5_RxBuffer);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void UART5_IRQHandler(void){
    uint8_t ch;
    if (USART_GetIntStatus(HW_USARTx, USART_INT_RXDNE) != RESET)
    {
        ch = USART_ReceiveData(HW_USARTx);
        if(UART5_RxThreadFlag==OS_TRUE){
            sdk_ringbuffer_put(&UART5_RxBuffer, ch);
            os_sem_release(&UART5_RxSem);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void BSP_UART5_Init(void)
{
    sdk_ringbuffer_init(&UART5_RxBuffer, UART5_RxBlock, sizeof(UART5_RxBlock));
    os_sem_init(&UART5_RxSem, "UART5_RxSem", 0, OS_QUEUE_FIFO);
    os_sem_init(&UART5_NotifySem, "UART5_NfySem", 0, OS_QUEUE_FIFO);
    os_mutex_init(&BSP_UART5__Mutex);
//    hw_usart_enable_dma();
    hw_usart_init(&HW_USART_OBJ, HW_USARTx, HW_USARTx_TxGPIO, HW_USARTx_TxPIN, HW_USARTx_RxGPIO, HW_USARTx_RxPIN, HW_USARTx_BAUDRATE);
    hw_usart_enable_irq(HW_USARTx, UART5_IRQn, 0, HW_USART_IRQ_MODE_RX);

    os_thread_init(&UART5_RxThread, "UART5_RxThd", UART5_RxThreadEntry, 0
            , UART5_RxThread_Stack, sizeof(UART5_RxThread_Stack)
            , UART5_RX_THREAD_PRIORITY
            , UART5_RX_THREAD_TIMESLICE
    );
    os_thread_startup(&UART5_RxThread);
}

void BSP_UART5_SetRxHandler(BSP_UART5_RxHandler rxHandler, void* userdata){
//    printf("UART5_SetRxHandler BEFORE: %p, %p\n", BSP_UART5__RxHandler.handler, BSP_UART5__RxHandler.userdata);
    os_mutex_lock(&BSP_UART5__Mutex);
    BSP_UART5__RxHandler.handler = rxHandler;
    BSP_UART5__RxHandler.userdata = userdata;
    os_mutex_unlock(&BSP_UART5__Mutex);
//    printf("UART5_SetRxHandler END: %p, %p\n", rxHandler, userdata);
}

int BSP_UART5_Send(uint8_t* data, int size){
//    printf("UART5_Send: %s\n", data);
    os_mutex_lock(&BSP_UART5__Mutex);
    sdk_ringbuffer_reset(&UART5_RxBuffer);
    hw_usart_send(HW_USARTx, data, size);
//    hw_usart_dma_send(HW_USARTx, data, size);
    os_mutex_unlock(&BSP_UART5__Mutex);
    return size;
}

int BSP_UART5_TimeWait(uint32_t timeout_ms){
    os_mutex_lock(&BSP_UART5__Mutex);
//    printf("UART5_TimeWait Sem.Count:%d\n", UART5_NotifySem.value);
//    assert(UART5_NotifySem.value==0);
    int err =  os_sem_take(&UART5_NotifySem, os_tick_from_millisecond(timeout_ms));
    os_mutex_unlock(&BSP_UART5__Mutex);
//    printf("UART5_TimeWait Result:%d, Sem.Count:%d\n", err, UART5_NotifySem.value);
    return err;
}

void BSP_UART5_Notify(void)
{
//    printf("UART5_Notify... %d\n", UART5_NotifySem.value);
    os_sem_release(&UART5_NotifySem);
}
