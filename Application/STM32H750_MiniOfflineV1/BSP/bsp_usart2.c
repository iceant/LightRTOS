#include <bsp_usart2.h>
#include <string.h>
#include <stdio.h>

#include <os_kernel.h>
#include <sdk_ringbuffer.h>
#include <A7670C.h>
////////////////////////////////////////////////////////////////////////////////
////
UART_HandleTypeDef                  BSP_USART2__Handle;
static DMA_HandleTypeDef            BSP_USART2__DMAHandle;
//static BSP_USART2_ReceiveCallback   BSP_USART2__RxCallback=0;
//static void *                       BSP_USART2__RxCallbackParameter=0;
static os_sem_t                     BSP_USART2__WaitSem;

static BSP_USART2_RxHandler_T BSP_USART2__RxHandler=0;
static void* BSP_USART2__RxHandlerUserdata=0;

static BSP_USART2_RxHandler_T BSP_USART2__DefaultRxHandler = 0;
static void* BSP_USART2__DefaultRxHandlerUserdata = 0;

/* ------------------------------------------------------------------------------------------------------------------ */
#define BSP_USART2_RXTHREAD_STACK_SIZE 1024
#define BSP_USART2_RXBLOCK_SIZE         10240 + 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t                      BSP_USART2_RxThreadStack[BSP_USART2_RXTHREAD_STACK_SIZE];
static os_thread_t                  BSP_USART2_RxThread;
static os_sem_t                     BSP_USART2_RxSem;
static uint8_t                      BSP_USART2_RxBlock[BSP_USART2_RXBLOCK_SIZE];
static sdk_ringbuffer_t             BSP_USART2_RxBuffer;
static bool                         BSP_USART2_RxThreadFlag = false;

static void BSP_USART2_RxThreadEntry(void* parameter){
    BSP_USART2_RxThreadFlag = true;
    int err = 0;
    while(1){
        os_sem_take(&BSP_USART2_RxSem, OS_WAIT_INFINITY);
        if(BSP_USART2__DefaultRxHandler){
            err = BSP_USART2__DefaultRxHandler(&BSP_USART2_RxBuffer, BSP_USART2__DefaultRxHandlerUserdata);
            if(err==BSP_USART2_RXHANDLER_RESULT_DONE || err==BSP_USART2_RXHANDLER_RESULT_RESET){
                continue;
            }
        }

        if(BSP_USART2__RxHandler){
            err = BSP_USART2__RxHandler(&BSP_USART2_RxBuffer, BSP_USART2__RxHandlerUserdata);
            if(err==BSP_USART2_RXHANDLER_RESULT_DONE || err==BSP_USART2_RXHANDLER_RESULT_RESET){
                continue;
            }
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART2_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

    __HAL_RCC_GPIOD_CLK_ENABLE(); /* Tx GPIO Clock */
//    __HAL_RCC_GPIOD_CLK_ENABLE(); /* Rx GPIO Clock */

    /* 配置串口1时钟源*/
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    RCC_PeriphClkInit.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

    /* 使能串口1时钟 */
    __HAL_RCC_USART2_CLK_ENABLE();

    /* 配置Tx引脚为复用功能  */
    /**USART2 GPIO Configuration
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* 配置串DEBUG_USART 模式 */
    BSP_USART2__Handle.Instance = USART2;
    BSP_USART2__Handle.Init.BaudRate = 115200;
    BSP_USART2__Handle.Init.WordLength = UART_WORDLENGTH_8B;
    BSP_USART2__Handle.Init.StopBits = UART_STOPBITS_1;
    BSP_USART2__Handle.Init.Parity = UART_PARITY_NONE;
    BSP_USART2__Handle.Init.Mode = UART_MODE_TX_RX;
    BSP_USART2__Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    BSP_USART2__Handle.Init.OverSampling = UART_OVERSAMPLING_16;
    BSP_USART2__Handle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    BSP_USART2__Handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&BSP_USART2__Handle);

    /*串口1中断初始化 */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    /*配置串口接收中断 */
    __HAL_UART_ENABLE_IT(&BSP_USART2__Handle,UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&BSP_USART2__Handle,UART_IT_TC);

    ////////////////////////////////////////////////////////////////////////////////
    ////
    os_sem_init(&BSP_USART2__WaitSem, "USART2_Sem", 0, OS_QUEUE_FIFO);

    BSP_USART2_RxThreadFlag = false;
    sdk_ringbuffer_init(&BSP_USART2_RxBuffer, BSP_USART2_RxBlock, BSP_USART2_RXBLOCK_SIZE);
    os_sem_init(&BSP_USART2_RxSem, "USART2_RxSem", 0, OS_QUEUE_FIFO);
    os_thread_init(&BSP_USART2_RxThread, "USART2_RxThd", BSP_USART2_RxThreadEntry, 0
                   , BSP_USART2_RxThreadStack, BSP_USART2_RXTHREAD_STACK_SIZE
                   , 20 , 10);
    os_thread_startup(&BSP_USART2_RxThread);
}

int BSP_USART2_SendBytes(uint8_t * data, int size){
    HAL_UART_Transmit(&BSP_USART2__Handle, data, size, 1000);
//    while (HAL_UART_GetState(&BSP_USART2__Handle) != HAL_UART_STATE_READY);
    return 0;
}

void BSP_USART2_SendString(const char* string){
    BSP_USART2_SendBytes((uint8_t*)string, strlen(string));
}

//void BSP_USART2_SetReceiveCallback(BSP_USART2_ReceiveCallback rxCallback, void* userdata){
//    BSP_USART2__RxCallback = rxCallback;
//    BSP_USART2__RxCallbackParameter = userdata;
//}

void BSP_USART2_EnableDMA(void){
    /*开启DMA时钟*/
    __HAL_RCC_DMA1_CLK_ENABLE();

    BSP_USART2__DMAHandle.Instance = DMA1_Stream3;
    /* Deinitialize the stream for new transfer */
    HAL_DMA_DeInit(&BSP_USART2__DMAHandle);

    /*usart1 tx对应dma2，通道4，数据流7*/
    BSP_USART2__DMAHandle.Init.Request = DMA_REQUEST_USART2_TX;
    /*方向：从内存到外设*/
    BSP_USART2__DMAHandle.Init.Direction= DMA_MEMORY_TO_PERIPH;
    /*外设地址不增*/
    BSP_USART2__DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;
    /*内存地址自增*/
    BSP_USART2__DMAHandle.Init.MemInc = DMA_MINC_ENABLE;
    /*外设数据单位*/
    BSP_USART2__DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    /*内存数据单位 8bit*/
    BSP_USART2__DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    /*DMA模式：不断循环*/
//    BSP_USART2__DMAHandle.Init.Mode = DMA_CIRCULAR;
    BSP_USART2__DMAHandle.Init.Mode = DMA_NORMAL;
    /*优先级：中*/
    BSP_USART2__DMAHandle.Init.Priority = DMA_PRIORITY_MEDIUM;
    /*禁用FIFO*/
    BSP_USART2__DMAHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    BSP_USART2__DMAHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    /*存储器突发传输 1个节拍*/
    BSP_USART2__DMAHandle.Init.MemBurst = DMA_MBURST_SINGLE;
    /*外设突发传输 1个节拍*/
    BSP_USART2__DMAHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    /*配置DMA2的数据流7*/
    /* Configure the DMA stream */
    HAL_DMA_Init(&BSP_USART2__DMAHandle);

    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);


    /* Associate the DMA handle */
    __HAL_LINKDMA(&BSP_USART2__Handle, hdmatx, BSP_USART2__DMAHandle);
    __HAL_DMA_ENABLE_IT(&BSP_USART2__DMAHandle, DMA_IT_TC);
}

////////////////////////////////////////////////////////////////////////////////
////
void BSP_USART2_DMA_Send(uint8_t * data, int size)
{
    HAL_UART_Transmit_DMA(&BSP_USART2__Handle, data, size);
    while (HAL_UART_GetState(&BSP_USART2__Handle) != HAL_UART_STATE_READY);
}

////////////////////////////////////////////////////////////////////////////////
////
os_err_t BSP_USART2_TimeWait(os_time_t timeout_ms)
{
    return os_sem_take(&BSP_USART2__WaitSem, os_tick_from_millisecond(timeout_ms));
}

os_err_t BSP_USART2_Notify(void)
{
    return os_sem_release(&BSP_USART2__WaitSem);
}


void BSP_USART2_SetRxHandler(BSP_USART2_RxHandler_T rxHandler, void* userdata)
{
    BSP_USART2__RxHandler = rxHandler;
    BSP_USART2__RxHandlerUserdata = userdata;
}


void BSP_USART2_SetDefaultRxHandler(BSP_USART2_RxHandler_T defaultRxHandler, void* userdata)
{
    BSP_USART2__DefaultRxHandler = defaultRxHandler;
    BSP_USART2__DefaultRxHandlerUserdata = userdata;
}

////////////////////////////////////////////////////////////////////////////////
////

void USART2_IRQHandler(void)
{

    uint8_t ch=0;
    if(__HAL_UART_GET_FLAG( &BSP_USART2__Handle, UART_FLAG_RXNE ) != RESET)
    {
        ch=( uint16_t)READ_REG(BSP_USART2__Handle.Instance->RDR);
//        if(BSP_USART2__RxCallback){
//            BSP_USART2__RxCallback(ch, BSP_USART2__RxCallbackParameter);
//        }
        if(BSP_USART2_RxThreadFlag){
            sdk_ringbuffer_put(&BSP_USART2_RxBuffer, ch);
            os_sem_release(&BSP_USART2_RxSem);
        }
    }else{
        HAL_UART_IRQHandler(&BSP_USART2__Handle);
    }
}

void DMA1_Stream3_IRQHandler(void){
    HAL_DMA_IRQHandler(&BSP_USART2__DMAHandle);
}
