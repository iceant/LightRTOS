#include <bsp_can1.h>
#include <sdk_hex.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////
static FDCAN_HandleTypeDef              BSP_CAN1__Handle;
static FDCAN_RxHeaderTypeDef            BSP_CAN1__RxHeader;
static uint8_t                          BSP_CAN1__RxData[16];
static FDCAN_FilterTypeDef              BSP_CAN1__RxFilter;

////////////////////////////////////////////////////////////////////////////////
////

void BSP_CAN1_Init(void)
{
    /* -------------------------------------------------------------------------------------------------------------- */
    /* GPIO Configuration */
    /* -------------------------------------------------------------------------------------------------------------- */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Initializes the peripherals clock
        */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* FDCAN1 clock enable */
    __HAL_RCC_FDCAN_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PA11     ------> FDCAN1_RX
    PA12     ------> FDCAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* -------------------------------------------------------------------------------------------------------------- */
    /* NVIC Configuration */
    /* -------------------------------------------------------------------------------------------------------------- */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
    //    __HAL_FDCAN_ENABLE_IT(&BSP_CAN1__Handle, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);

    /* -------------------------------------------------------------------------------------------------------------- */
    /* CAN Configuration */
    /* -------------------------------------------------------------------------------------------------------------- */
    BSP_CAN1__Handle.Instance = FDCAN1;
    BSP_CAN1__Handle.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    BSP_CAN1__Handle.Init.Mode = FDCAN_MODE_NORMAL;
    BSP_CAN1__Handle.Init.AutoRetransmission = ENABLE;
    BSP_CAN1__Handle.Init.TransmitPause = DISABLE;
    BSP_CAN1__Handle.Init.ProtocolException = ENABLE;
    BSP_CAN1__Handle.Init.NominalPrescaler = 16; /* tq = NominalPrescaler x (1/fdcan_ker_ck) */
    BSP_CAN1__Handle.Init.NominalSyncJumpWidth = 0x01;
    BSP_CAN1__Handle.Init.NominalTimeSeg1 = 16; /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
    BSP_CAN1__Handle.Init.NominalTimeSeg2 = 3;
    BSP_CAN1__Handle.Init.DataPrescaler = 16;
    BSP_CAN1__Handle.Init.DataSyncJumpWidth = 0x01;
    BSP_CAN1__Handle.Init.DataTimeSeg1 = 16; /* DataTimeSeg1 = Propagation_segment + Phase_segment_1 */
    BSP_CAN1__Handle.Init.DataTimeSeg2 = 3;
    BSP_CAN1__Handle.Init.MessageRAMOffset = 0;
    BSP_CAN1__Handle.Init.StdFiltersNbr = 1;
    BSP_CAN1__Handle.Init.ExtFiltersNbr = 1;
    BSP_CAN1__Handle.Init.RxFifo0ElmtsNbr = 32;
    BSP_CAN1__Handle.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
    BSP_CAN1__Handle.Init.RxFifo1ElmtsNbr = 0;
    BSP_CAN1__Handle.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
    BSP_CAN1__Handle.Init.RxBuffersNbr = 16;
    BSP_CAN1__Handle.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
    BSP_CAN1__Handle.Init.TxEventsNbr = 0;
    BSP_CAN1__Handle.Init.TxBuffersNbr = 0;
    BSP_CAN1__Handle.Init.TxFifoQueueElmtsNbr = 16;
    BSP_CAN1__Handle.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    BSP_CAN1__Handle.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
    if (HAL_FDCAN_Init(&BSP_CAN1__Handle) != HAL_OK)
    {
        printf("[BSP_CAN1] FDCAN Init Failed!\r\n");
        while(1);
    }


    /* -------------------------------------------------------------------------------------------------------------- */
    /* FILTER Configuration */
    /* -------------------------------------------------------------------------------------------------------------- */
    BSP_CAN1__RxFilter.IdType=FDCAN_STANDARD_ID;
    BSP_CAN1__RxFilter.FilterIndex=0;
    BSP_CAN1__RxFilter.FilterType=FDCAN_FILTER_MASK;
    BSP_CAN1__RxFilter.FilterConfig=FDCAN_FILTER_TO_RXBUFFER;
    BSP_CAN1__RxFilter.FilterID1=0;
    BSP_CAN1__RxFilter.FilterID2=0;
    BSP_CAN1__RxFilter.RxBufferIndex = 0;
    HAL_FDCAN_ConfigFilter(&BSP_CAN1__Handle, &BSP_CAN1__RxFilter);

    BSP_CAN1__RxFilter.IdType=FDCAN_EXTENDED_ID;
    BSP_CAN1__RxFilter.FilterIndex=0;
    BSP_CAN1__RxFilter.FilterType=FDCAN_FILTER_MASK;
    BSP_CAN1__RxFilter.FilterConfig=FDCAN_FILTER_TO_RXBUFFER;
    BSP_CAN1__RxFilter.FilterID1=0;
    BSP_CAN1__RxFilter.FilterID2=0;
    BSP_CAN1__RxFilter.RxBufferIndex = 0;
    HAL_FDCAN_ConfigFilter(&BSP_CAN1__Handle, &BSP_CAN1__RxFilter);

    /* Configure global filter to reject all non-matching frames */
    HAL_FDCAN_ConfigGlobalFilter(&BSP_CAN1__Handle
                                 , FDCAN_ACCEPT_IN_RX_FIFO0
                                 , FDCAN_ACCEPT_IN_RX_FIFO0
                                 , FDCAN_FILTER_REMOTE
                                 , FDCAN_FILTER_REMOTE);


/* Activate Rx FIFO 0 new message notification on both FDCAN instances */
    HAL_FDCAN_ActivateNotification(&BSP_CAN1__Handle, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(&BSP_CAN1__Handle, FDCAN_IT_BUS_OFF, 0);


#if 0
    /* Configure and enable Tx Delay Compensation, required for BRS mode.
      TdcOffset default recommended value: DataTimeSeg1 * DataPrescaler
      TdcFilter default recommended value: 0 */
    HAL_FDCAN_ConfigTxDelayCompensation(&BSP_CAN1__Handle
                                        , BSP_CAN1__Handle.Init.DataPrescaler * BSP_CAN1__Handle.Init.DataTimeSeg1, 0);
    HAL_FDCAN_EnableTxDelayCompensation(&BSP_CAN1__Handle);
#endif

    HAL_FDCAN_Start(&BSP_CAN1__Handle);
    printf("[CAN1] Startup...\r\n");
}

////////////////////////////////////////////////////////////////////////////////
////


void BSP_CAN1_Send(uint32_t Identifier, uint32_t IdType, uint32_t frameType, uint8_t *TxData, int DataLength)
{
    FDCAN_TxHeaderTypeDef   TxHeader = {0};
    TxHeader.Identifier = Identifier;
    TxHeader.IdType = IdType; // 标准-FDCAN_STANDARD_ID; 扩展-FDCAN_EXTENDED_ID
    TxHeader.TxFrameType = frameType; // 数据帧-FDCAN_DATA_FRAME; 远程帧-FDCAN_REMOTE_FRAME
    TxHeader.DataLength = DataLength; // FDCAN_DLC_BYTES_xx
    // xx = 0 1 2 3 4 5 6 7 8 12 16 20 24 32 48 64
    TxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE; // FDCAN_ESI_ACTIVE FDCAN_ESI_PASSIVE
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF; // 波特率不可变-FDCAN_BRS_OFF; 波特率可变-FDCAN_BRS_ON
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN; // 经典CAN-FDCAN_CLASSIC_CAN; CANFD-FDCAN_FD_CAN
    // TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    // TxHeader.MessageMarker = 0;

    while(HAL_FDCAN_GetTxFifoFreeLevel(&BSP_CAN1__Handle) == 0); // 等待有发送邮箱可用
    HAL_FDCAN_AddMessageToTxFifoQ(&BSP_CAN1__Handle, &TxHeader, TxData); // 发送数据帧
}

////////////////////////////////////////////////////////////////////////////////
////
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
//    __HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_BUS_OFF);
    if(hfdcan->Instance == FDCAN1) {
        BSP_CAN1_Init();
    }
}

//void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
//{
//    printf("HAL_FDCAN_RxFifo0Callback\r\n");
//    if(hfdcan==&BSP_CAN1__Handle){
//        if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
//        {
//            /* Retrieve Rx messages from RX FIFO0 */
//            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &BSP_CAN1__RxHeader, BSP_CAN1__RxData);
//            printf("[CAN1] Received\r\n");
//            sdk_hex_dump("CAN1_RX", BSP_CAN1__RxData, BSP_CAN1__RxHeader.DataLength);
//        }
//    }
//}

void FDCAN1_IT0_IRQHandler(void)
{
    if (__HAL_FDCAN_GET_FLAG(&BSP_CAN1__Handle, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE) != 0U){//是新数据接收中断
        /* Clear the Rx Buffer New Message flag */
        __HAL_FDCAN_CLEAR_FLAG(&BSP_CAN1__Handle, FDCAN_FLAG_RX_FIFO0_NEW_MESSAGE);//清除中断标志
        HAL_FDCAN_GetRxMessage(&BSP_CAN1__Handle, FDCAN_RX_FIFO0, &BSP_CAN1__RxHeader, BSP_CAN1__RxData);
        printf("[CAN1_IRQ] Received\r\n");
        sdk_hex_dump("CAN1_IRQ_RX", BSP_CAN1__RxData, BSP_CAN1__RxHeader.DataLength);
    }
    HAL_FDCAN_IRQHandler(&BSP_CAN1__Handle);
}

