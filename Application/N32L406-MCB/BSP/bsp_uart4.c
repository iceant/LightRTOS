#include <bsp_uart4.h>

#include <stdio.h>
#include <sdk_hex.h>
#include <assert.h>
#include <os_mutex.h>
////////////////////////////////////////////////////////////////////////////////
////
#define HW_USARTx UART4
#define HW_USARTx_TxGPIO GPIOB
#define HW_USARTx_TxPIN  GPIO_PIN_0
#define HW_USARTx_RxGPIO GPIOB
#define HW_USARTx_RxPIN  GPIO_PIN_1
#define HW_USARTx_BAUDRATE  115200

#define UART4_RX_BLOCK_SIZE 512
#define UART4_RX_THREAD_STACK_SIZE 1024

#define UART4_RX_THREAD_PRIORITY   20
#define UART4_RX_THREAD_TIMESLICE  10

////////////////////////////////////////////////////////////////////////////////
////
typedef struct BPS_UART4_RxHandlerRecord{
    BSP_UART4_RxHandler handler;
    void* userdata;
}BPS_UART4_RxHandlerRecord;

////////////////////////////////////////////////////////////////////////////////
////
static os_mutex_t BSP_UART4__Mutex;
static hw_usart_t HW_USART_OBJ;

static uint8_t UART4_RxBlock[UART4_RX_BLOCK_SIZE];
static sdk_ringbuffer_t UART4_RxBuffer;
static os_sem_t UART4_RxSem;
static os_sem_t UART4_NotifySem;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t UART4_RxThread_Stack[UART4_RX_THREAD_STACK_SIZE];
static os_thread_t UART4_RxThread;

static BPS_UART4_RxHandlerRecord BSP_UART4__RxHandler={0, 0};

static volatile os_bool_t UART4_RxThreadFlag = OS_FALSE;

static void Delay(uint32_t time){
    for(;time>0; time--){
        for(int i=68; i>0; i--);
    }
}
////////////////////////////////////////////////////////////////////////////////
////

static void UART4_RxThreadEntry(void* parameter){
    UART4_RxThreadFlag = OS_TRUE;

    while(1){
        os_sem_take(&UART4_RxSem, OS_WAIT_INFINITY);

        if(BSP_UART4__RxHandler.handler){
            BSP_UART4__RxHandler.handler(&UART4_RxBuffer, BSP_UART4__RxHandler.userdata);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void UART4_IRQHandler(void){
    uint8_t ch;
    if (USART_GetIntStatus(HW_USARTx, USART_INT_RXDNE) != RESET)
    {
        ch = USART_ReceiveData(HW_USARTx);
        if(UART4_RxThreadFlag==OS_TRUE){
            sdk_ringbuffer_put(&UART4_RxBuffer, ch);
            os_sem_release(&UART4_RxSem);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void BSP_UART4_Init(void)
{
    sdk_ringbuffer_init(&UART4_RxBuffer, UART4_RxBlock, sizeof(UART4_RxBlock));
    os_sem_init(&UART4_RxSem, "UART4_RxSem", 0, OS_QUEUE_FIFO);
    os_sem_init(&UART4_NotifySem, "UART4_NfySem", 0, OS_QUEUE_FIFO);
    os_mutex_init(&BSP_UART4__Mutex);
    hw_usart_enable_dma();
    hw_usart_init(&HW_USART_OBJ, HW_USARTx, HW_USARTx_TxGPIO, HW_USARTx_TxPIN, HW_USARTx_RxGPIO, HW_USARTx_RxPIN, HW_USARTx_BAUDRATE);
    hw_usart_enable_irq(HW_USARTx, UART4_IRQn, 0, HW_USART_IRQ_MODE_RX);

    os_thread_init(&UART4_RxThread, "UART4_RxThd", UART4_RxThreadEntry, 0
            , UART4_RxThread_Stack, sizeof(UART4_RxThread_Stack)
            , UART4_RX_THREAD_PRIORITY
            , UART4_RX_THREAD_TIMESLICE
    );
    os_thread_startup(&UART4_RxThread);
}

void BSP_UART4_SetRxHandler(BSP_UART4_RxHandler rxHandler, void* userdata){
//    printf("UART4_SetRxHandler BEFORE: %p, %p\n", BSP_UART4__RxHandler.handler, BSP_UART4__RxHandler.userdata);
    os_mutex_lock(&BSP_UART4__Mutex);
    BSP_UART4__RxHandler.handler = rxHandler;
    BSP_UART4__RxHandler.userdata = userdata;
    os_mutex_unlock(&BSP_UART4__Mutex);
//    printf("UART4_SetRxHandler END: %p, %p\n", rxHandler, userdata);
}

int BSP_UART4_Send(uint8_t* data, int size){
    printf("BSP_UART4_Send: size=%d\n", size);
    sdk_hex_dump("BSP_UART4_Send", data, size);
    os_mutex_lock(&BSP_UART4__Mutex);
    sdk_ringbuffer_reset(&UART4_RxBuffer);

//    hw_usart_send(HW_USARTx, data, size);
    hw_usart_dma_send(HW_USARTx, data, size);
    os_mutex_unlock(&BSP_UART4__Mutex);
    return size;
}

int BSP_UART4_TimeWait(uint32_t timeout_ms){
    os_mutex_lock(&BSP_UART4__Mutex);
//    printf("UART4_TimeWait Sem.Count:%d\n", UART4_NotifySem.value);
//    assert(UART4_NotifySem.value==0);
    int err =  os_sem_take(&UART4_NotifySem, os_tick_from_millisecond(timeout_ms));
    os_mutex_unlock(&BSP_UART4__Mutex);
//    printf("UART4_TimeWait Result:%d, Sem.Count:%d\n", err, UART4_NotifySem.value);
    return err;
}

void BSP_UART4_Notify(void)
{
//    printf("UART4_Notify... %d\n", UART4_NotifySem.value);
    os_sem_release(&UART4_NotifySem);
}
