#include <bsp_i2c1.h>

////////////////////////////////////////////////////////////////////////////////
////
I2C_HandleTypeDef BSP_I2C1__Handle;
DMA_HandleTypeDef BSP_I2C1__DMATxHandle;

////////////////////////////////////////////////////////////////////////////////
////

void BSP_I2C1_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /* Initializes the peripherals clock */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Enable GPIO TX/RX clock */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();


    BSP_I2C1__Handle.Instance = I2C1;
    BSP_I2C1__Handle.Init.Timing = 0x00B03FDB;
    BSP_I2C1__Handle.Init.OwnAddress1 = 0;
    BSP_I2C1__Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    BSP_I2C1__Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    BSP_I2C1__Handle.Init.OwnAddress2 = 0;
    BSP_I2C1__Handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    BSP_I2C1__Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    BSP_I2C1__Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&BSP_I2C1__Handle);

    HAL_I2CEx_ConfigAnalogFilter(&BSP_I2C1__Handle, I2C_ANALOGFILTER_ENABLE);
    HAL_I2CEx_ConfigDigitalFilter(&BSP_I2C1__Handle, 0);
}

void BSP_I2C1_EnableDMA(void)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* I2C1 DMA Init */
    /* I2C1_TX Init */
    BSP_I2C1__DMATxHandle.Instance = DMA1_Stream2;
    BSP_I2C1__DMATxHandle.Init.Request = DMA_REQUEST_I2C1_TX;
    BSP_I2C1__DMATxHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    BSP_I2C1__DMATxHandle.Init.PeriphInc = DMA_PINC_DISABLE;
    BSP_I2C1__DMATxHandle.Init.MemInc = DMA_MINC_ENABLE;
    BSP_I2C1__DMATxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    BSP_I2C1__DMATxHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    BSP_I2C1__DMATxHandle.Init.Mode = DMA_NORMAL;
    BSP_I2C1__DMATxHandle.Init.Priority = DMA_PRIORITY_MEDIUM;
    BSP_I2C1__DMATxHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&BSP_I2C1__DMATxHandle);

    __HAL_LINKDMA(&BSP_I2C1__Handle,hdmatx,BSP_I2C1__DMATxHandle);

    /* DMA1_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
}

void BSP_I2C1_DeInit(void)
{
    __HAL_RCC_I2C1_CLK_DISABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
}

void BSP_I2C1_DisableDMA(void)
{
    /* I2C1 DMA DeInit */
    HAL_DMA_DeInit(BSP_I2C1__Handle.hdmatx);
}

int BSP_I2C1_MasterSend(uint8_t slaveAddress, uint8_t * data, int data_size)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&BSP_I2C1__Handle, slaveAddress, data[0], I2C_MEMADD_SIZE_8BIT, &data[1], data_size-1, 100);
    if(status==HAL_OK){
        return 0;
    }else{
        return status;
    }
//    while(HAL_I2C_GetState(&BSP_I2C1__Handle)!=HAL_I2C_STATE_READY);
}

int BSP_I2C1_MasterReceive(uint8_t slaveAddress, uint8_t * data, int data_size)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&BSP_I2C1__Handle, slaveAddress,
                                                data[0], I2C_MEMADD_SIZE_8BIT
                                                , &data[1], data_size-1, 100);
    return status;

}

int BSP_I2C1_MasterDMASend(uint8_t slaveAddress, uint8_t * data, int data_size)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write_DMA(&BSP_I2C1__Handle, slaveAddress
                                                     , data[0], I2C_MEMADD_SIZE_8BIT
                                                     ,&data[1], data_size-1);
    while(HAL_I2C_GetState(&BSP_I2C1__Handle)!=HAL_I2C_STATE_READY);
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
////
void DMA1_Stream2_IRQHandler(void){
    HAL_DMA_IRQHandler(&BSP_I2C1__DMATxHandle);
}

