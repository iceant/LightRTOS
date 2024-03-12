#include <bsp_usart3.h>

#include <stdio.h>
#include <sdk_hex.h>
#include <assert.h>
#include <os_mutex.h>
////////////////////////////////////////////////////////////////////////////////
////
#define HW_USARTx USART3
#define HW_USARTx_TxGPIO GPIOB
#define HW_USARTx_TxPIN  GPIO_PIN_10
#define HW_USARTx_RxGPIO GPIOB
#define HW_USARTx_RxPIN  GPIO_PIN_11
#define HW_USARTx_BAUDRATE  115200

#define USART3_RX_BLOCK_SIZE 512
#define USART3_RX_THREAD_STACK_SIZE 1024

#define USART3_RX_THREAD_PRIORITY   20
#define USART3_RX_THREAD_TIMESLICE  10

////////////////////////////////////////////////////////////////////////////////
////
typedef struct BPS_USART3_RxHandlerRecord{
    BSP_USART3_RxHandler handler;
    void* userdata;
}BPS_USART3_RxHandlerRecord;

////////////////////////////////////////////////////////////////////////////////
////
static os_mutex_t BSP_USART3__Mutex;
static hw_usart_t HW_USART_OBJ;

static uint8_t USART3_RxBlock[USART3_RX_BLOCK_SIZE];
static sdk_ringbuffer_t USART3_RxBuffer;
static os_sem_t USART3_RxSem;
static os_sem_t USART3_NotifySem;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t USART3_RxThread_Stack[USART3_RX_THREAD_STACK_SIZE];
static os_thread_t USART3_RxThread;

static BPS_USART3_RxHandlerRecord BSP_USART3__RxHandler={0, 0};

static volatile os_bool_t USART3_RxThreadFlag = OS_FALSE;

static void Delay(uint32_t time){
    for(;time>0; time--){
        for(int i=68; i>0; i--);
    }
}
////////////////////////////////////////////////////////////////////////////////
////

static void USART3_RxThreadEntry(void* parameter){
    USART3_RxThreadFlag = OS_TRUE;

    while(1){
        os_sem_take(&USART3_RxSem, OS_WAIT_INFINITY);

        if(BSP_USART3__RxHandler.handler){
            BSP_USART3__RxHandler.handler(&USART3_RxBuffer, BSP_USART3__RxHandler.userdata);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void USART3_IRQHandler(void){
    uint8_t ch;
    if (USART_GetIntStatus(HW_USARTx, USART_INT_RXDNE) != RESET)
    {
        ch = USART_ReceiveData(HW_USARTx);
        if(USART3_RxThreadFlag==OS_TRUE){
            sdk_ringbuffer_put(&USART3_RxBuffer, ch);
            os_sem_release(&USART3_RxSem);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART3_Init(void)
{
    sdk_ringbuffer_init(&USART3_RxBuffer, USART3_RxBlock, sizeof(USART3_RxBlock));
    os_sem_init(&USART3_RxSem, "USART3_RxSem", 0, OS_QUEUE_FIFO);
    os_sem_init(&USART3_NotifySem, "USART3_NfySem", 0, OS_QUEUE_FIFO);
    os_mutex_init(&BSP_USART3__Mutex);
//    hw_usart_enable_dma();
    hw_usart_init(&HW_USART_OBJ, HW_USARTx, HW_USARTx_TxGPIO, HW_USARTx_TxPIN, HW_USARTx_RxGPIO, HW_USARTx_RxPIN, HW_USARTx_BAUDRATE);
    hw_usart_enable_irq(HW_USARTx, USART3_IRQn, 0, HW_USART_IRQ_MODE_RX);

    os_thread_init(&USART3_RxThread, "USART3_RxThd", USART3_RxThreadEntry, 0
            , USART3_RxThread_Stack, sizeof(USART3_RxThread_Stack)
            , USART3_RX_THREAD_PRIORITY
            , USART3_RX_THREAD_TIMESLICE
    );
    os_thread_startup(&USART3_RxThread);
}

void BSP_USART3_SetRxHandler(BSP_USART3_RxHandler rxHandler, void* userdata){
//    printf("USART3_SetRxHandler BEFORE: %p, %p\n", BSP_USART3__RxHandler.handler, BSP_USART3__RxHandler.userdata);
    os_mutex_lock(&BSP_USART3__Mutex);
    BSP_USART3__RxHandler.handler = rxHandler;
    BSP_USART3__RxHandler.userdata = userdata;
    os_mutex_unlock(&BSP_USART3__Mutex);
//    printf("USART3_SetRxHandler END: %p, %p\n", rxHandler, userdata);
}

int BSP_USART3_Send(uint8_t* data, int size){
//    printf("USART3_Send: %s\n", data);
    os_mutex_lock(&BSP_USART3__Mutex);
    sdk_ringbuffer_reset(&USART3_RxBuffer);
    hw_usart_send(HW_USARTx, data, size);
//    hw_usart_dma_send(HW_USARTx, data, size);
    os_mutex_unlock(&BSP_USART3__Mutex);
    return size;
}

int BSP_USART3_TimeWait(uint32_t timeout_ms){
    os_mutex_lock(&BSP_USART3__Mutex);
//    printf("USART3_TimeWait Sem.Count:%d\n", USART3_NotifySem.value);
//    assert(USART3_NotifySem.value==0);
    int err =  os_sem_take(&USART3_NotifySem, os_tick_from_millisecond(timeout_ms));
    os_mutex_unlock(&BSP_USART3__Mutex);
//    printf("USART3_TimeWait Result:%d, Sem.Count:%d\n", err, USART3_NotifySem.value);
    return err;
}

void BSP_USART3_Notify(void)
{
//    printf("USART3_Notify... %d\n", USART3_NotifySem.value);
    os_sem_release(&USART3_NotifySem);
}
