#include <bsp_can1.h>
#include <n32l40x.h>

////////////////////////////////////////////////////////////////////////////////
////

#define  CAN_MODE_TYPE_NORMAL            1
#define  CAN_MODE_TYPE_LOOPBACK          2
#define  CAN_MODE_TYPE_SILENT            3
#define  CAN_MODE_TYPE_SILENT_LOOPBACK   4
#define  CAN_MODE_TYPE                   CAN_MODE_TYPE_NORMAL

#if(CAN_MODE_TYPE==CAN_MODE_TYPE_NORMAL)
#define  OPERATINGMODE      CAN_Normal_Mode
#elif(CAN_MODE_TYPE==CAN_MODE_TYPE_LOOPBACK)
#define  OPERATINGMODE      CAN_LoopBack_Mode
#elif(CAN_MODE_TYPE==CAN_MODE_TYPE_SILENT)
#define  OPERATINGMODE      CAN_Silent_Mode
#elif(CAN_MODE_TYPE==CAN_MODE_TYPE_SILENT_LOOPBACK)
#define  OPERATINGMODE      CAN_Silent_LoopBack_Mode
#endif

#define  CAN_BAUDRATE_1M            1
#define  CAN_BAUDRATE_500K          2
#define  CAN_BAUDRATE_250K          3
#define  CAN_BAUDRATE_125K          4
#define  CAN_BAUDRATE               CAN_BAUDRATE_250K

#if(CAN_BAUDRATE==CAN_BAUDRATE_1M)
#define   CAN_BIT_RSJW              CAN_RSJW_1tq
#define   CAN_BIT_BS1               CAN_TBS1_5tq
#define   CAN_BIT_BS2               CAN_TBS2_2tq
#define   CAN_BAUDRATEPRESCALER     2
#elif(CAN_BAUDRATE==CAN_BAUDRATE_500K)
#define   CAN_BIT_RSJW              CAN_RSJW_1tq
#define   CAN_BIT_BS1               CAN_TBS1_10tq
#define   CAN_BIT_BS2               CAN_TBS2_5tq
#define   CAN_BAUDRATEPRESCALER     2
#elif(CAN_BAUDRATE==CAN_BAUDRATE_250K)
#define   CAN_BIT_RSJW              CAN_RSJW_1tq
#define   CAN_BIT_BS1               CAN_TBS1_10tq
#define   CAN_BIT_BS2               CAN_TBS2_5tq
#define   CAN_BAUDRATEPRESCALER     4
#elif(CAN_BAUDRATE==CAN_BAUDRATE_125K)
#define   CAN_BIT_RSJW              CAN_RSJW_1tq
#define   CAN_BIT_BS1               CAN_TBS1_10tq
#define   CAN_BIT_BS2               CAN_TBS2_5tq
#define   CAN_BAUDRATEPRESCALER     8
#endif


#define  CAN_TXDLC_8                     ((uint8_t)8)
#define  CAN_FILTERNUM0                  ((uint8_t)0)
#define  CAN_TX1_SID                     0x0400
#define  CAN_TX2_EID                     0x12345678
#define  CAN_TX_MESSAGE_BUFF_SIZE        2
#define  CAN_RX_MESSAGE_BUFF_SIZE        64

#define  CAN_FILTER_STDID(STDID)         ((STDID&0x7FF)<<5)

#define  CAN_FILTER_EXTID_H(EXTID)       ((uint16_t)  (((EXTID<<3)|0x04)>>16) )
#define  CAN_FILTER_EXTID_L(EXTID)       ((uint16_t)  (( EXTID<<3)|0x04) )

#define  CAN_STD_ID_H_MASK_CARE          0xFFE0
#define  CAN_STD_ID_L_MASK_CARE          0x0000
#define  CAN_STD_ID_H_MASK_DONT_CARE     0x0000
#define  CAN_STD_ID_L_MASK_DONT_CARE     0x0000

#define  CAN_EXT_ID_H_MASK_CARE          0xFFFF
#define  CAN_EXT_ID_L_MASK_CARE          0xFFF8
#define  CAN_EXT_ID_H_MASK_DONT_CARE     0x0000
#define  CAN_EXT_ID_L_MASK_DONT_CARE     0x0000


////////////////////////////////////////////////////////////////////////////////
////
static void CAN_GPIO_Config(void)
{
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    /* Configures CAN IOs */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO | RCC_APB2_PERIPH_GPIOA, ENABLE);
    /* Configure CAN RX PA11 */
    GPIO_InitStructure.Pin       = GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Input;
    GPIO_InitStructure.GPIO_Pull = GPIO_Pull_Up;
    GPIO_InitStructure.GPIO_Alternate = GPIO_AF1_CAN;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    /* Configure CAN TX PA12 */
    GPIO_InitStructure.Pin        = GPIO_PIN_12;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
}

static void CAN_Filter_Init(void)
{
    CAN_FilterInitType CAN_FilterInitStructure;
    /* CAN filter init */
    CAN_FilterInitStructure.Filter_Num            = CAN_FILTERNUM0;
    CAN_FilterInitStructure.Filter_Mode           = CAN_Filter_IdMaskMode;
    CAN_FilterInitStructure.Filter_Scale          = CAN_Filter_32bitScale;
//    CAN_FilterInitStructure.Filter_HighId         = CAN_FILTER_EXTID_H(0x00005678);
//    CAN_FilterInitStructure.Filter_LowId          = CAN_FILTER_EXTID_L(0x00005678);
    CAN_FilterInitStructure.Filter_HighId         = CAN_EXT_ID_H_MASK_DONT_CARE;
    CAN_FilterInitStructure.Filter_LowId          = CAN_EXT_ID_L_MASK_DONT_CARE;
    CAN_FilterInitStructure.FilterMask_HighId     = CAN_STD_ID_H_MASK_DONT_CARE;
    CAN_FilterInitStructure.FilterMask_LowId      = CAN_STD_ID_L_MASK_DONT_CARE;
    CAN_FilterInitStructure.Filter_FIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.Filter_Act            = ENABLE;
    CAN_InitFilter(&CAN_FilterInitStructure);
    CAN_INTConfig(CAN, CAN_INT_FMP0, ENABLE);
}

static void CAN_NVIC_Config(void)
{
    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = CAN_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void CAN_Config(void)
{
    CAN_InitType CAN_InitStructure;
    /* Configure CAN */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_CAN, ENABLE);
    /* CAN register init */
    CAN_DeInit(CAN);
    /* Struct init*/
    CAN_InitStruct(&CAN_InitStructure);
    /* CAN cell init */
    CAN_InitStructure.TTCM              = DISABLE;
    CAN_InitStructure.ABOM              = DISABLE;
    CAN_InitStructure.AWKUM             = DISABLE;
    CAN_InitStructure.NART              = DISABLE;
    CAN_InitStructure.RFLM              = DISABLE;
    CAN_InitStructure.TXFP              = ENABLE;
    CAN_InitStructure.OperatingMode     = OPERATINGMODE;
    CAN_InitStructure.RSJW              = CAN_BIT_RSJW;
    CAN_InitStructure.TBS1              = CAN_BIT_BS1;
    CAN_InitStructure.TBS2              = CAN_BIT_BS2;
    CAN_InitStructure.BaudRatePrescaler = CAN_BAUDRATEPRESCALER;
    /*Initializes the CAN */
    CAN_Init(CAN, &CAN_InitStructure);
    CAN_Filter_Init();
    CAN_NVIC_Config();
}

static BSP_CAN1_RxHandler BPS_CAN1__RxHandler=0;
static void* BSP_CAN1__RxHandlerUserdata=0;
////////////////////////////////////////////////////////////////////////////////
////

void BSP_CAN1_Init(void)
{
    CAN_GPIO_Config();
    CAN_Config();
}

uint8_t BSP_CAN1_Send(CanTxMessage* TxMessage)
{
    uint8_t TransmitMailbox = CAN_TransmitMessage(CAN, TxMessage);
    uint16_t Time_out       = 0xFFFF;
    while ((CAN_TransmitSTS(CAN, TransmitMailbox) != CANTXSTSOK) && (Time_out != 0)) Time_out--;
    if(Time_out==0){
        return -1;
    }
    return 0;
}

void BSP_CAN1_SetRxHandler(BSP_CAN1_RxHandler handler, void* userdata)
{
    BPS_CAN1__RxHandler = handler;
    BSP_CAN1__RxHandlerUserdata = userdata;
}

////////////////////////////////////////////////////////////////////////////////
////

void CAN_RX0_IRQHandler(void)
{
    CanRxMessage Can_RxMessage;
    CAN_ReceiveMessage(CAN, CAN_FIFO0, &Can_RxMessage);
    if(BPS_CAN1__RxHandler){
        BPS_CAN1__RxHandler(&Can_RxMessage, BSP_CAN1__RxHandlerUserdata);
    }
}

