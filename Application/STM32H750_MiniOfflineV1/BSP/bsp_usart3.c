#include <bsp_usart3.h>
#include <string.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////
UART_HandleTypeDef                  BSP_USART3__Handle;
static DMA_HandleTypeDef            BSP_USART3__DMAHandle;
static BSP_USART3_ReceiveCallback   BSP_USART3__RxCallback=0;
static void *                       BSP_USART3__RxCallbackParameter=0;
////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART3_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

    __HAL_RCC_GPIOB_CLK_ENABLE(); /* Tx GPIO Clock */
    __HAL_RCC_GPIOB_CLK_ENABLE(); /* Rx GPIO Clock */

    /* 配置串口1时钟源*/
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    RCC_PeriphClkInit.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

    /* 使能串口3时钟 */
    __HAL_RCC_USART3_CLK_ENABLE();

    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 配置Rx引脚为复用功能 */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 配置串DEBUG_USART 模式 */
    BSP_USART3__Handle.Instance = USART3;
    BSP_USART3__Handle.Init.BaudRate = 115200;
    BSP_USART3__Handle.Init.WordLength = UART_WORDLENGTH_8B;
    BSP_USART3__Handle.Init.StopBits = UART_STOPBITS_1;
    BSP_USART3__Handle.Init.Parity = UART_PARITY_NONE;
    BSP_USART3__Handle.Init.Mode = UART_MODE_TX_RX;
    BSP_USART3__Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    BSP_USART3__Handle.Init.OverSampling = UART_OVERSAMPLING_16;
    BSP_USART3__Handle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    BSP_USART3__Handle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&BSP_USART3__Handle);

    /*串口1中断初始化 */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);

    /*配置串口接收中断 */
    __HAL_UART_ENABLE_IT(&BSP_USART3__Handle,UART_IT_RXNE);
    __HAL_UART_ENABLE_IT(&BSP_USART3__Handle,UART_IT_TC);
}

void BSP_USART3_SendBytes(uint8_t * data, int size){
    HAL_UART_Transmit(&BSP_USART3__Handle, data, size, 1000);
}

void BSP_USART3_SendString(const char* string){
    BSP_USART3_SendBytes((uint8_t*)string, strlen(string));
}

void BSP_USART3_SetReceiveCallback(BSP_USART3_ReceiveCallback rxCallback, void* userdata){
    BSP_USART3__RxCallback = rxCallback;
    BSP_USART3__RxCallbackParameter = userdata;
}

void BSP_USART3_EnableDMA(void){
    /*开启DMA时钟*/
    __HAL_RCC_DMA1_CLK_ENABLE();

    BSP_USART3__DMAHandle.Instance = DMA1_Stream1;
    /* Deinitialize the stream for new transfer */
    HAL_DMA_DeInit(&BSP_USART3__DMAHandle);

    /*usart1 tx对应dma2，通道4，数据流7*/
    BSP_USART3__DMAHandle.Init.Request = DMA_REQUEST_USART3_TX;
    /*方向：从内存到外设*/
    BSP_USART3__DMAHandle.Init.Direction= DMA_MEMORY_TO_PERIPH;
    /*外设地址不增*/
    BSP_USART3__DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;
    /*内存地址自增*/
    BSP_USART3__DMAHandle.Init.MemInc = DMA_MINC_ENABLE;
    /*外设数据单位*/
    BSP_USART3__DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    /*内存数据单位 8bit*/
    BSP_USART3__DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    /*DMA模式：不断循环*/
//    BSP_USART3__DMAHandle.Init.Mode = DMA_CIRCULAR;
    BSP_USART3__DMAHandle.Init.Mode = DMA_NORMAL;
    /*优先级：中*/
    BSP_USART3__DMAHandle.Init.Priority = DMA_PRIORITY_MEDIUM;
    /*禁用FIFO*/
    BSP_USART3__DMAHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    BSP_USART3__DMAHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    /*存储器突发传输 1个节拍*/
    BSP_USART3__DMAHandle.Init.MemBurst = DMA_MBURST_SINGLE;
    /*外设突发传输 1个节拍*/
    BSP_USART3__DMAHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    /*配置DMA2的数据流7*/
    /* Configure the DMA stream */
    HAL_DMA_Init(&BSP_USART3__DMAHandle);

    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);


    /* Associate the DMA handle */
    __HAL_LINKDMA(&BSP_USART3__Handle, hdmatx, BSP_USART3__DMAHandle);
    __HAL_DMA_ENABLE_IT(&BSP_USART3__DMAHandle, DMA_IT_TC);
}

////////////////////////////////////////////////////////////////////////////////
////
void BSP_USART3_DMA_Send(uint8_t * data, int size)
{
    HAL_UART_Transmit_DMA(&BSP_USART3__Handle, data, size);
    while (HAL_UART_GetState(&BSP_USART3__Handle) != HAL_UART_STATE_READY);
}



////////////////////////////////////////////////////////////////////////////////
////

void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&BSP_USART3__Handle);
}

void DMA1_Stream1_IRQHandler(void){
    HAL_DMA_IRQHandler(&BSP_USART3__DMAHandle);
}
