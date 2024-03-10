#include <bsp_usart2.h>
#include <stdio.h>
#include <sdk_hex.h>
#include <assert.h>
#include <os_mutex.h>
////////////////////////////////////////////////////////////////////////////////
////
#define HW_USARTx USART2
#define HW_USARTx_TxGPIO GPIOB
#define HW_USARTx_TxPIN  GPIO_PIN_4
#define HW_USARTx_RxGPIO GPIOB
#define HW_USARTx_RxPIN  GPIO_PIN_5
#define HW_USARTx_BAUDRATE  115200

#define USART2_RX_BLOCK_SIZE 1024
#define USART2_RX_THREAD_STACK_SIZE 1024

#define USART2_RX_THREAD_PRIORITY   20
#define USART2_RX_THREAD_TIMESLICE  10

////////////////////////////////////////////////////////////////////////////////
////
typedef struct BPS_USART2_RxHandlerRecord{
    BSP_USART2_RxHandler handler;
    void* userdata;
}BPS_USART2_RxHandlerRecord;

////////////////////////////////////////////////////////////////////////////////
////
static os_mutex_t BSP_USART2__Mutex;
static hw_usart_t HW_USART_OBJ;

static uint8_t USART2_RxBlock[USART2_RX_BLOCK_SIZE];
static sdk_ringbuffer_t USART2_RxBuffer;
static os_sem_t USART2_RxSem;
static os_sem_t USART2_NotifySem;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t USART2_RxThread_Stack[USART2_RX_THREAD_STACK_SIZE];
static os_thread_t USART2_RxThread;

static BPS_USART2_RxHandlerRecord BSP_USART2__RxHandler={0, 0};

static volatile os_bool_t USART2_RxThreadFlag = OS_FALSE;

static void Delay(uint32_t time){
    for(;time>0; time--){
        for(int i=68; i>0; i--);
    }
}
////////////////////////////////////////////////////////////////////////////////
////
        
static void USART2_RxThreadEntry(void* parameter){
    USART2_RxThreadFlag = OS_TRUE;
    
    while(1){
        os_sem_take(&USART2_RxSem, OS_WAIT_INFINITY);
        
        if(BSP_USART2__RxHandler.handler){
            BSP_USART2__RxHandler.handler(&USART2_RxBuffer, BSP_USART2__RxHandler.userdata);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void USART2_IRQHandler(void){
    uint8_t ch;
    if (USART_GetIntStatus(HW_USARTx, USART_INT_RXDNE) != RESET)
    {
        if(USART2_RxThreadFlag==OS_TRUE){
            ch = USART_ReceiveData(HW_USARTx);
            sdk_ringbuffer_put(&USART2_RxBuffer, ch);
            os_sem_release(&USART2_RxSem);
        }
        USART_ClrIntPendingBit(HW_USARTx, USART_INT_RXDNE);
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART2_Init(void)
{
    sdk_ringbuffer_init(&USART2_RxBuffer, USART2_RxBlock, sizeof(USART2_RxBlock));
    os_sem_init(&USART2_RxSem, "USART2_RxSem", 0, OS_QUEUE_FIFO);
    os_sem_init(&USART2_NotifySem, "USART2_NfySem", 0, OS_QUEUE_FIFO);
    os_mutex_init(&BSP_USART2__Mutex);
    hw_usart_enable_dma();
    hw_usart_init(&HW_USART_OBJ, HW_USARTx, HW_USARTx_TxGPIO, HW_USARTx_TxPIN, HW_USARTx_RxGPIO, HW_USARTx_RxPIN, HW_USARTx_BAUDRATE);
    hw_usart_enable_irq(HW_USARTx, USART2_IRQn, 0, HW_USART_IRQ_MODE_RX);

    os_thread_init(&USART2_RxThread, "USART2_RxThd", USART2_RxThreadEntry, 0
            , USART2_RxThread_Stack, sizeof(USART2_RxThread_Stack)
            , USART2_RX_THREAD_PRIORITY
            , USART2_RX_THREAD_TIMESLICE
    );
    os_thread_startup(&USART2_RxThread);
}

void BSP_USART2_SetRxHandler(BSP_USART2_RxHandler rxHandler, void* userdata){
//    printf("USART2_SetRxHandler BEFORE: %p, %p\n", BSP_USART2__RxHandler.handler, BSP_USART2__RxHandler.userdata);
    os_mutex_lock(&BSP_USART2__Mutex);
    BSP_USART2__RxHandler.handler = rxHandler;
    BSP_USART2__RxHandler.userdata = userdata;
    os_mutex_unlock(&BSP_USART2__Mutex);
//    printf("USART2_SetRxHandler END: %p, %p\n", rxHandler, userdata);
}

int BSP_USART2_Send(uint8_t* data, int size){
    printf("USART2_Send: %s\n", data);
    os_mutex_lock(&BSP_USART2__Mutex);
    sdk_ringbuffer_reset(&USART2_RxBuffer);
//    hw_usart_send(HW_USARTx, data, size);
    hw_usart_dma_send(HW_USARTx, data, size);
    os_mutex_unlock(&BSP_USART2__Mutex);
    return size;
}

int BSP_USART2_TimeWait(uint32_t timeout_ms){
    os_mutex_lock(&BSP_USART2__Mutex);
    printf("USART2_TimeWait Sem.Count:%d\n", USART2_NotifySem.value);
    assert(USART2_NotifySem.value==0);
    int err =  os_sem_take(&USART2_NotifySem, os_tick_from_millisecond(timeout_ms));
    sdk_ringbuffer_reset(&USART2_RxBuffer);
    os_mutex_unlock(&BSP_USART2__Mutex);
    printf("USART2_TimeWait Result:%d, Sem.Count:%d\n", err, USART2_NotifySem.value);
    return err;
}

void BSP_USART2_Notify(void)
{
    printf("USART2_Notify... %d\n", USART2_NotifySem.value);
    os_sem_release(&USART2_NotifySem);
}
