#include <bsp_usart3.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
////
UART_HandleTypeDef                      BSP_USART3__Handle;
static DMA_HandleTypeDef                BSP_USART3__DMAHandle;
static BSP_USART3_ReceiveCallback       BSP_USART3__RxCallback=0;
static void *                           BSP_USART3__RxCallbackParameter=0;

////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 使能串口1时钟 */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE(); /* Tx GPIO Clock */
    __HAL_RCC_GPIOC_CLK_ENABLE(); /* Rx GPIO Clock */


    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* 配置Rx引脚为复用功能 */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* 配置串DEBUG_USART 模式 */
    BSP_USART3__Handle.Instance = USART3;
    BSP_USART3__Handle.Init.BaudRate = 115200;
    BSP_USART3__Handle.Init.WordLength = UART_WORDLENGTH_8B;
    BSP_USART3__Handle.Init.StopBits = UART_STOPBITS_1;
    BSP_USART3__Handle.Init.Parity = UART_PARITY_NONE;
    BSP_USART3__Handle.Init.Mode = UART_MODE_TX_RX;
    BSP_USART3__Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    BSP_USART3__Handle.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&BSP_USART3__Handle);

    /*串口1中断初始化 */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);

    /*配置串口接收中断 */
    __HAL_UART_ENABLE_IT(&BSP_USART3__Handle,UART_IT_RXNE);
}

void BSP_USART3_EnableDMA(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    BSP_USART3__DMAHandle.Instance = DMA1_Stream3;
    HAL_DMA_DeInit(&BSP_USART3__DMAHandle);

    BSP_USART3__DMAHandle.Init.Channel = DMA_CHANNEL_4;
    BSP_USART3__DMAHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    BSP_USART3__DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;
    BSP_USART3__DMAHandle.Init.MemInc = DMA_MINC_ENABLE;
    BSP_USART3__DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    BSP_USART3__DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    BSP_USART3__DMAHandle.Init.Mode = DMA_NORMAL;
    BSP_USART3__DMAHandle.Init.Priority = DMA_PRIORITY_MEDIUM;
    BSP_USART3__DMAHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    BSP_USART3__DMAHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    /*存储器突发传输 1个节拍*/
    BSP_USART3__DMAHandle.Init.MemBurst = DMA_MBURST_SINGLE;
    /*外设突发传输 1个节拍*/
    BSP_USART3__DMAHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&BSP_USART3__DMAHandle);

    __HAL_LINKDMA(&BSP_USART3__Handle,hdmatx,BSP_USART3__DMAHandle);

    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

    __HAL_DMA_ENABLE_IT(&BSP_USART3__DMAHandle, DMA_IT_TC);
}

void BSP_USART3_SendBytes(uint8_t * data, int size){
    HAL_UART_Transmit(&BSP_USART3__Handle, data, size, 1000);
}

void BSP_USART3_SendString(const char* string){
    BSP_USART3_SendBytes((uint8_t*)string, strlen(string));
}

void BSP_USART3_SetReceiveCallback(BSP_USART3_ReceiveCallback rxCallback, void* userdata)
{
    BSP_USART3__RxCallback = rxCallback;
    BSP_USART3__RxCallbackParameter = userdata;
}

void BSP_USART3_DMA_Send(uint8_t * data, int size){
    while (HAL_UART_GetState(&BSP_USART3__Handle) != HAL_UART_STATE_READY);
    HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(&BSP_USART3__Handle, data, size);
}

////////////////////////////////////////////////////////////////////////////////
////

void USART3_IRQHandler(void){
    HAL_UART_IRQHandler(&BSP_USART3__Handle);
}

void DMA1_Stream3_IRQHandler(void){
    HAL_DMA_IRQHandler(&BSP_USART3__DMAHandle);
}



