#include <hw_usart.h>
#include <hw_gpio.h>
////////////////////////////////////////////////////////////////////////////////
////
void hw_usart_configuration(USART_Module* USARTx, uint32_t BaudRate, uint32_t Remap)
{

    USART_InitType USART_InitStructure;

    /* Enable GPIO clock */
    if(Remap!=0){
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
        GPIO_ConfigPinRemap(Remap, ENABLE);
    }

    /* Enable USARTy and USARTz Clock */
    if(USARTx==USART1){
        if(Remap==0){
            hw_gpio_init(GPIOA, GPIO_PIN_9, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOA, GPIO_PIN_10, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP_USART1){
            hw_gpio_init(GPIOB, GPIO_PIN_6, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOB, GPIO_PIN_7, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }

        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);
    }else if(USARTx==USART2){
        if(Remap==0){
            hw_gpio_init(GPIOA, GPIO_PIN_2, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOA, GPIO_PIN_3, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP1_USART2){
            hw_gpio_init(GPIOD, GPIO_PIN_5, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOD, GPIO_PIN_6, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP2_USART2){
            hw_gpio_init(GPIOC, GPIO_PIN_8, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOC, GPIO_PIN_9, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP3_USART2){
            hw_gpio_init(GPIOB, GPIO_PIN_4, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOB, GPIO_PIN_5, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }

        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART2, ENABLE);
    }else if(USARTx==USART3){
        if(Remap==0){
            hw_gpio_init(GPIOB, GPIO_PIN_10, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOB, GPIO_PIN_11, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_PART_RMP_USART3){
            hw_gpio_init(GPIOC, GPIO_PIN_10, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOC, GPIO_PIN_11, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_ALL_RMP_USART3){
            hw_gpio_init(GPIOD, GPIO_PIN_8, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOD, GPIO_PIN_9, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }

        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART3, ENABLE);
    }else if(USARTx==UART4){
        if(Remap==0){
            hw_gpio_init(GPIOC, GPIO_PIN_10, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOC, GPIO_PIN_11, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP1_UART4){
            hw_gpio_init(GPIOB, GPIO_PIN_2, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOE, GPIO_PIN_7, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP2_UART4){
            hw_gpio_init(GPIOA, GPIO_PIN_13, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOA, GPIO_PIN_14, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP3_UART4){
            hw_gpio_init(GPIOD, GPIO_PIN_0, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOD, GPIO_PIN_1, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }

        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART4, ENABLE);
    }else if(USARTx==UART5){
        if(Remap==0){
            hw_gpio_init(GPIOC, GPIO_PIN_12, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOD, GPIO_PIN_2, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP1_UART5){
            hw_gpio_init(GPIOB, GPIO_PIN_13, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOB, GPIO_PIN_14, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP2_UART5){
            hw_gpio_init(GPIOE, GPIO_PIN_8, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOE, GPIO_PIN_9, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP3_UART5){
            hw_gpio_init(GPIOB, GPIO_PIN_8, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOB, GPIO_PIN_9, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }

        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_UART5, ENABLE);
    }else if(USARTx==UART6){
        if(Remap==0){
            hw_gpio_init(GPIOE, GPIO_PIN_2, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOE, GPIO_PIN_3, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP2_UART6){
            hw_gpio_init(GPIOC, GPIO_PIN_0, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOC, GPIO_PIN_1, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP3_UART6){
            hw_gpio_init(GPIOB, GPIO_PIN_0, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOB, GPIO_PIN_1, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }

        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_UART6, ENABLE);
    }else if(USARTx==UART7){
        if(Remap==0){
            hw_gpio_init(GPIOC, GPIO_PIN_4, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOC, GPIO_PIN_5, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP1_UART7){
            hw_gpio_init(GPIOC, GPIO_PIN_2, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOC, GPIO_PIN_3, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }else if(Remap==GPIO_RMP3_UART7){
            hw_gpio_init(GPIOG, GPIO_PIN_0, GPIO_Mode_AF_PP, GPIO_Speed_50MHz); /* Tx */
            hw_gpio_init(GPIOG, GPIO_PIN_1, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* Rx */
        }

        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_UART7, ENABLE);
    }

    USART_InitStructure.BaudRate            = BaudRate;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;

    /* Configure USARTx */
    USART_Init(USARTx, &USART_InitStructure);

    /* Enable the USARTx */
    USART_Enable(USARTx, ENABLE);
}


void hw_usart_enable_irq(USART_Module * USARTx, uint8_t IRQn, uint8_t Priority, int mode /* 1-Tx, 2-Rx, 3-TxRx*/){
    NVIC_InitType NVIC_InitStructure;

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel            = IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = Priority; /* 0 */
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    if((mode & HW_USART_IRQ_MODE_TX)){
        USART_ConfigInt(USARTx, USART_INT_TXDE, ENABLE);
    }

    if((mode & HW_USART_IRQ_MODE_RX)){
        USART_ConfigInt(USARTx, USART_INT_RXDNE, ENABLE);
    }
}

void hw_usart_send(USART_Module * USARTx, uint8_t* TxBuffer, size_t TxBufferSize)
{
    size_t i;
    for(i = 0;i<TxBufferSize; i++){
        USART_SendData(USARTx, (uint8_t)*TxBuffer++);
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET)
            ;
    }
}

void hw_usart_recv(USART_Module * USARTx, uint8_t * RxBuffer, size_t RxBufferSize)
{
    size_t i = 0;
    size_t nCount = 0;

    while(1){
        nCount = 0x3FFFFF;
        while (USART_GetIntStatus(USARTx, USART_INT_RXDNE) == RESET){
            if(nCount-- ==0 ){
                return;
            }
        }
        RxBuffer[i++] = USART_ReceiveData(USARTx);
        if(i>=RxBufferSize){
            return;
        }
    }
}

void hw_usart_dma_send(USART_Module * USARTx, uint8_t* TxBuffer, size_t TxBufferSize)
{
    DMA_InitType DMA_InitStructure;
    uint32_t DMA_CH_TxFlag=0;
    DMA_ChannelType* DMA_CHx=0;
    DMA_Module* DMAx=0;

    if(USARTx==USART1){
        DMAx = DMA1;
        DMA_CHx = DMA1_CH4;
        DMA_CH_TxFlag = DMA1_FLAG_TC4;
    }else if(USARTx==USART2){
        DMAx = DMA1;
        DMA_CHx = DMA1_CH7;
        DMA_CH_TxFlag = DMA1_FLAG_TC7;
    }else if(USARTx==USART3){
        DMAx = DMA1;
        DMA_CHx = DMA1_CH2;
        DMA_CH_TxFlag = DMA1_FLAG_TC2;
    }else if(USARTx==UART4){
        DMAx = DMA2;
        DMA_CHx = DMA2_CH5;
        DMA_CH_TxFlag = DMA2_FLAG_TC5;
    }else if(USARTx==UART5){
        DMAx = DMA1;
        DMA_CHx = DMA1_CH1;
        DMA_CH_TxFlag = DMA1_FLAG_TC1;
    }else if(USARTx==UART6){
        DMAx = DMA2;
        DMA_CHx = DMA2_CH2;
        DMA_CH_TxFlag = DMA2_FLAG_TC2;
    }else if(USARTx==UART7){
        DMAx = DMA2;
        DMA_CHx = DMA2_CH7;
        DMA_CH_TxFlag = DMA2_FLAG_TC7;
    }

    /* USARTy TX DMA1 Channel (triggered by USARTy Tx event) Config */
    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.PeriphAddr     = (((uint32_t)(USARTx))+0x04);
    DMA_InitStructure.MemAddr        = (uint32_t)TxBuffer;
    DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
    DMA_InitStructure.BufSize        = TxBufferSize;
    DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
    DMA_InitStructure.DMA_MemoryInc  = DMA_MEM_INC_ENABLE;
    DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_SIZE_BYTE;
    DMA_InitStructure.MemDataSize    = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
    DMA_InitStructure.Priority       = DMA_PRIORITY_VERY_HIGH;
    DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
    DMA_Init(DMA_CHx, &DMA_InitStructure);

    USART_EnableDMA(USARTx, USART_DMAREQ_TX, ENABLE);
    DMA_EnableChannel(DMA_CHx, ENABLE);
//
//    /* wait for DMA Tx DONE! */
//    while (DMA_GetFlagStatus(DMA_CH_TxFlag, DMAx) == RESET)
//    {}
}



