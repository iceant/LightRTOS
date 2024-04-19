#include <bsp_spi1.h>

SPI_HandleTypeDef BSP_SPI1__Handle;
DMA_HandleTypeDef BSP_SPI1__DMAHandle;

void BSP_SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN SPI1_MspInit 0 */

    /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA4     ------> SPI1_NSS
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* -------------------------------------------------------------------------------------------------------------- */
    BSP_SPI1__Handle.Instance = SPI1;
    BSP_SPI1__Handle.Init.Mode = SPI_MODE_MASTER;
    BSP_SPI1__Handle.Init.Direction = SPI_DIRECTION_2LINES;
    BSP_SPI1__Handle.Init.DataSize = SPI_DATASIZE_8BIT;
    BSP_SPI1__Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    BSP_SPI1__Handle.Init.CLKPhase = SPI_PHASE_2EDGE;
    BSP_SPI1__Handle.Init.NSS = SPI_NSS_SOFT;
    BSP_SPI1__Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    BSP_SPI1__Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    BSP_SPI1__Handle.Init.TIMode = SPI_TIMODE_DISABLE;
    BSP_SPI1__Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    BSP_SPI1__Handle.Init.CRCPolynomial = 7;

    HAL_SPI_Init(&BSP_SPI1__Handle);

}

void BSP_SPI1_DMAEnable(void){
    __HAL_RCC_DMA2_CLK_ENABLE();
    /* -------------------------------------------------------------------------------------------------------------- */
    /* SPI1 DMA Init */
    /* SPI1_TX Init */
    BSP_SPI1__DMAHandle.Instance = DMA2_Stream2;
    BSP_SPI1__DMAHandle.Init.Channel = DMA_CHANNEL_2;
    BSP_SPI1__DMAHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    BSP_SPI1__DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;
    BSP_SPI1__DMAHandle.Init.MemInc = DMA_MINC_ENABLE;
    BSP_SPI1__DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    BSP_SPI1__DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    BSP_SPI1__DMAHandle.Init.Mode = DMA_NORMAL;
    BSP_SPI1__DMAHandle.Init.Priority = DMA_PRIORITY_LOW;
    BSP_SPI1__DMAHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&BSP_SPI1__DMAHandle);

    __HAL_LINKDMA(&BSP_SPI1__Handle,hdmatx,BSP_SPI1__DMAHandle);


    /* -------------------------------------------------------------------------------------------------------------- */
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}

void DMA2_Stream2_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&BSP_SPI1__DMAHandle);
}

