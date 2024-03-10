#include <hw_usart.h>
#include <hw_gpio.h>

////////////////////////////////////////////////////////////////////////////////
////
#define HW_USART_GPIO_TX(GPIOx, GPIO_Pin, AF) \
    hw_gpio_init(GPIOx, GPIO_Pin, GPIO_Mode_AF_PP, GPIO_No_Pull, GPIO_DC_2mA, GPIO_Slew_Rate_High, AF)
    
#define HW_USART_GPIO_RX(GPIOx, GPIO_Pin, AF) \
    hw_gpio_init(GPIOx, GPIO_Pin, GPIO_Mode_AF_PP, GPIO_Pull_Up, GPIO_DC_2mA, GPIO_Slew_Rate_High, AF)

////////////////////////////////////////////////////////////////////////////////
////
os_err_t hw_usart_init(hw_usart_t* usart, void* USARTx
        , GPIO_Module* Tx_GPIO, uint16_t Tx_Pin
        , GPIO_Module* Rx_GPIO, uint16_t Rx_Pin
        , uint32_t BaudRate)
{

    USART_InitType USART_InitStructure;
    
    usart->USARTx = USARTx;
    usart->Tx_GPIO = Tx_GPIO;
    usart->Tx_Pin = Tx_Pin;
    usart->Rx_GPIO = Rx_GPIO;
    usart->Rx_Pin = Rx_Pin;
    
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    
    if(USARTx==USART1){
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);
        if(Tx_GPIO==GPIOA && Tx_Pin==GPIO_PIN_4){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF1_USART1);
        }else if(Tx_GPIO==GPIOA && Tx_Pin==GPIO_PIN_9){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF4_USART1);
        }else if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_6){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF0_USART1);
        }else if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_8){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF0_USART1);
        }
        
        if(Rx_GPIO==GPIOA && Rx_Pin==GPIO_PIN_5){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF4_USART1);
        }else if(Rx_GPIO==GPIOA && Rx_Pin==GPIO_PIN_10){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF4_USART1);
        }else if(Rx_GPIO==GPIOB && Rx_Pin==GPIO_PIN_7){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF0_USART1);
        }
    }else if(USARTx==USART2){
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART2, ENABLE);
        if(Tx_GPIO==GPIOA && Tx_Pin==GPIO_PIN_2){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF4_USART2);
        }else if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_4){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF4_USART2);
        }
        
        if(Rx_GPIO==GPIOA && Rx_Pin==GPIO_PIN_3){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF4_USART2);
        }else if(Rx_GPIO==GPIOB && Rx_Pin==GPIO_PIN_5){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_USART2);
        }else if(Rx_GPIO==GPIOD && Rx_Pin==GPIO_PIN_15){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF4_USART2);
        }
    }else if(USARTx==USART3){
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_USART3, ENABLE);
        
        if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_10){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF0_USART3);
        }else if(Tx_GPIO==GPIOC && Tx_Pin==GPIO_PIN_10){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF5_USART3);
        }
        
        if(Rx_GPIO==GPIOB && Rx_Pin==GPIO_PIN_11){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF5_USART3);
        }else if(Rx_GPIO==GPIOC && Rx_Pin==GPIO_PIN_11){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF5_USART3);
        }
    }else if(USARTx==UART4){
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_UART4, ENABLE);
        if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_0){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_UART4);
        }else if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_14){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_UART4);
        }else if(Tx_GPIO==GPIOC && Tx_Pin==GPIO_PIN_10){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_UART4);
        }else if(Tx_GPIO==GPIOD && Tx_Pin==GPIO_PIN_13){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_UART4);
        }
        
        if(Rx_GPIO==GPIOB && Rx_Pin==GPIO_PIN_1){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_UART4);
        }else if(Rx_GPIO==GPIOB && Rx_Pin==GPIO_PIN_15){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_UART4);
        }else if(Rx_GPIO==GPIOC && Rx_Pin==GPIO_PIN_11){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_UART4);
        }else if(Rx_GPIO==GPIOD && Rx_Pin==GPIO_PIN_12){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_UART4);
        }
    }else if(USARTx==UART5){
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_UART5, ENABLE);
        
        if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_4){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_UART5);
        }else if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_8){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_UART5);
        }else if(Tx_GPIO==GPIOC && Tx_Pin==GPIO_PIN_12){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_UART5);
        }
        
        if(Rx_GPIO==GPIOB && Rx_Pin==GPIO_PIN_5){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF7_UART5);
        }else if(Rx_GPIO==GPIOB && Rx_Pin==GPIO_PIN_9){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_UART5);
        }else if(Rx_GPIO==GPIOD && Rx_Pin==GPIO_PIN_2){
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_UART5);
        }
    }else if(USARTx==LPUART){
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
        
        if(Tx_GPIO==GPIOA && Tx_Pin==GPIO_PIN_1){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_LPUART);
        }else if(Tx_GPIO==GPIOA && Tx_Pin==GPIO_PIN_4){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_LPUART);
        }else if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_6){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF6_LPUART);
        }else if(Tx_GPIO==GPIOB && Tx_Pin==GPIO_PIN_10){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF4_LPUART);
        }else if(Tx_GPIO==GPIOC && Tx_Pin==GPIO_PIN_4){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF2_LPUART);
        }else if(Tx_GPIO==GPIOC && Tx_Pin==GPIO_PIN_10){
            HW_USART_GPIO_TX(Tx_GPIO, Tx_Pin, GPIO_AF0_LPUART);
        }
        
        if(Rx_GPIO==GPIOA && Rx_Pin==GPIO_PIN_0){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_LPUART);
        }else if(Rx_GPIO==GPIOA && Rx_Pin==GPIO_PIN_3){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_LPUART);
        }else if(Rx_GPIO==GPIOB && Rx_Pin==GPIO_PIN_7){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF6_LPUART);
        }else if(Rx_GPIO==GPIOB && Rx_Pin==GPIO_PIN_11){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF4_LPUART);
        }else if(Rx_GPIO==GPIOC && Rx_Pin==GPIO_PIN_5){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF2_LPUART);
        }else if(Rx_GPIO==GPIOC && Rx_Pin==GPIO_PIN_11){
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
            HW_USART_GPIO_RX(Rx_GPIO, Rx_Pin, GPIO_AF0_LPUART);
        }
    }

    if(USARTx!=LPUART){
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
    
}

void hw_usart_enable_dma(void){
    /* DMA clock enable */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPH_DMA, ENABLE);
}

void hw_usart_enable_irq(USART_Module * USARTx, uint8_t IRQn, uint8_t Priority, int mode /* 1-Tx, 2-Rx, 3-TxRx*/)
{
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
    uint32_t DMA_Remap=0;

    if(USARTx==USART1){
        DMAx = DMA;
        DMA_CHx = DMA_CH4;
        DMA_CH_TxFlag = DMA_FLAG_TC4;
        DMA_Remap = DMA_REMAP_USART1_TX;
    }else if(USARTx==USART2){
        DMAx = DMA;
        DMA_CHx = DMA_CH7;
        DMA_CH_TxFlag = DMA_FLAG_TC7;
        DMA_Remap = DMA_REMAP_USART2_TX;
    }else if(USARTx==USART3){
        DMAx = DMA;
        DMA_CHx = DMA_CH1;
        DMA_CH_TxFlag = DMA_FLAG_TC1;
        DMA_Remap = DMA_REMAP_USART3_TX;
    }else if(USARTx==UART4){
        DMAx = DMA;
        DMA_CHx = DMA_CH5;
        DMA_CH_TxFlag = DMA_FLAG_TC5;
        DMA_Remap = DMA_REMAP_UART4_TX;
    }else if(USARTx==UART5){
        DMAx = DMA;
        DMA_CHx = DMA_CH7;
        DMA_CH_TxFlag = DMA_FLAG_TC7;
        DMA_Remap = DMA_REMAP_UART5_TX;
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
    DMA_RequestRemap(DMA_Remap, DMAx, DMA_CHx, ENABLE);
    USART_EnableDMA(USARTx, USART_DMAREQ_TX, ENABLE);
    DMA_EnableChannel(DMA_CHx, ENABLE);

    /* wait for DMA Tx DONE! */
    while (DMA_GetFlagStatus(DMA_CH_TxFlag, DMAx) == RESET)
    {}
}



