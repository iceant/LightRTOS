#include <bsp_can2.h>
#include <os_kernel.h>
#include <sdk_ring.h>
////////////////////////////////////////////////////////////////////////////////
////
#define BSP_CAN2_RX_THREAD_STACK_SIZE   1024
#define BSP_CAN2_RX_THREAD_PRIORITY     20
#define BSP_CAN2_RX_THREAD_TIMESLICE    10


#define  CAN_BAUDRATE_1M            1
#define  CAN_BAUDRATE_500K          2
#define  CAN_BAUDRATE_250K          3
#define  CAN_BAUDRATE_125K          4
#define  CAN_BAUDRATE               CAN_BAUDRATE_250K

#if(CAN_BAUDRATE==CAN_BAUDRATE_1M)
#define   CAN_BIT_RSJW              CAN_RSJW_1tq
#define   CAN_BIT_BS1               CAN_TBS1_3tq
#define   CAN_BIT_BS2               CAN_TBS2_1tq
#define   CAN_BAUDRATEPRESCALER     5
#elif(CAN_BAUDRATE==CAN_BAUDRATE_500K)
#define   CAN_BIT_RSJW              CAN_RSJW_1tq
#define   CAN_BIT_BS1               CAN_TBS1_8tq
#define   CAN_BIT_BS2               CAN_TBS2_1tq
#define   CAN_BAUDRATEPRESCALER     5
#elif(CAN_BAUDRATE==CAN_BAUDRATE_250K)
#define   CAN_BIT_RSJW              CAN_RSJW_1tq
#define   CAN_BIT_BS1               CAN_TBS1_8tq
#define   CAN_BIT_BS2               CAN_TBS2_1tq
#define   CAN_BAUDRATEPRESCALER     10
#elif(CAN_BAUDRATE==CAN_BAUDRATE_125K)
#define   CAN_BIT_RSJW              CAN_RSJW_1tq
#define   CAN_BIT_BS1               CAN_TBS1_8tq
#define   CAN_BIT_BS2               CAN_TBS2_1tq
#define   CAN_BAUDRATEPRESCALER     20
#endif

#define RING_OBJECT_SIZE sizeof(CanRxMessage)
#define COUNT_PER_SECOND 110
#define RING_OBJECT_COUNT (COUNT_PER_SECOND * 10)
#define RING_BLOCK_SIZE (RING_OBJECT_SIZE * RING_OBJECT_COUNT)
////////////////////////////////////////////////////////////////////////////////
////

static BSP_CAN2_RxHandler BSP_CAN2__RxHandler=0;
static void* BSP_CAN2__RxHandlerParameter=0;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t BSP_CAN2__RxThreadStack[BSP_CAN2_RX_THREAD_STACK_SIZE];
static os_thread_t BSP_CAN2__RxThread;
static os_sem_t BSP_CAN2__RxSem;
static os_bool_t BSP_CAN2__RxThreadFlag = OS_FALSE;

static uint8_t BSP_CAN2__RingBlock[RING_BLOCK_SIZE];
static sdk_ring_t BSP_CAN2__RxRing;
////////////////////////////////////////////////////////////////////////////////
////

static void BSP_CAN2__RxThreadEntry(void* p){
    CAN_INTConfig(CAN2, CAN_INT_FMP0, ENABLE);
    BSP_CAN2__RxThreadFlag = OS_TRUE;
    while(1){
        os_sem_take(&BSP_CAN2__RxSem, OS_WAIT_INFINITY);
        if(BSP_CAN2__RxHandler){
            BSP_CAN2__RxHandler(&BSP_CAN2__RxRing, BSP_CAN2__RxHandlerParameter);
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
////
static void NVIC_Config(void)
{
    NVIC_InitType NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = CAN2_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    CAN_INTConfig(CAN1, CAN_INT_FMP0, ENABLE);
}

static void CAN_GPIO_Configuration(void){
    GPIO_InitType GPIO_InitStructure;
    GPIO_InitStruct(&GPIO_InitStructure);
    /* Configures IOs */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO | RCC_APB2_PERIPH_GPIOD, ENABLE);
    /* Configure RX pin */
    GPIO_InitStructure.Pin       = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
    /* Configure TX pin */
    GPIO_InitStructure.Pin        = GPIO_PIN_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitPeripheral(GPIOD, &GPIO_InitStructure);
    
    /* Remap GPIOs */
    GPIO_ConfigPinRemap(GPIO_RMP3_CAN2, ENABLE);
}

static void CAN_Filter_Init(void)
{
    CAN_FilterInitType CAN_FilterInitStructure;
    /* CAN filter init */
    CAN_FilterInitStructure.Filter_Num            = 0;
    CAN_FilterInitStructure.Filter_Mode           = CAN_Filter_IdMaskMode;
    CAN_FilterInitStructure.Filter_Scale          = CAN_Filter_32bitScale;
    CAN_FilterInitStructure.Filter_HighId         = 0x0000;
    CAN_FilterInitStructure.Filter_LowId          = 0x0000;
    CAN_FilterInitStructure.FilterMask_HighId     = 0x0000;
    CAN_FilterInitStructure.FilterMask_LowId      = 0x0000;
    CAN_FilterInitStructure.Filter_FIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.Filter_Act            = ENABLE;
    CAN2_InitFilter(&CAN_FilterInitStructure);
}

static void CAN_Config(void)
{
    CAN_InitType CAN_InitStructure;
    /* Configure CAN1 and CAN2 */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_CAN2, ENABLE);
    /* CAN2 register init */
    CAN_DeInit(CAN2);
    /* Struct init*/
    CAN_InitStruct(&CAN_InitStructure);
    /* CAN1 and CAN2  cell init */
    CAN_InitStructure.TTCM          = DISABLE;
    CAN_InitStructure.ABOM          = DISABLE;
    CAN_InitStructure.AWKUM         = DISABLE;
    CAN_InitStructure.NART          = DISABLE;
    CAN_InitStructure.RFLM          = DISABLE;
    CAN_InitStructure.TXFP          = ENABLE;
    CAN_InitStructure.OperatingMode = CAN_Normal_Mode;
    CAN_InitStructure.RSJW          = CAN_BIT_RSJW;
    CAN_InitStructure.TBS1          = CAN_BIT_BS1;
    CAN_InitStructure.TBS2          = CAN_BIT_BS2;
    CAN_InitStructure.BaudRatePrescaler = CAN_BAUDRATEPRESCALER;
    /*Initializes the CAN1  and CAN2 */
    CAN_Init(CAN2, &CAN_InitStructure);
    CAN_Filter_Init();
}


////////////////////////////////////////////////////////////////////////////////
////

void BSP_CAN2_Init(void){
    os_sem_init(&BSP_CAN2__RxSem, "BSP_CAN2_RxSem", 0, OS_QUEUE_FIFO);
    sdk_ring_init(&BSP_CAN2__RxRing, BSP_CAN2__RingBlock, RING_OBJECT_COUNT, RING_OBJECT_SIZE);
    
    CAN_GPIO_Configuration();
    CAN_Config();
    NVIC_Config();
    
    os_thread_init(&BSP_CAN2__RxThread, "CAN2_Rx", BSP_CAN2__RxThreadEntry, 0
        , BSP_CAN2__RxThreadStack, sizeof(BSP_CAN2__RxThreadStack)
        , BSP_CAN2_RX_THREAD_PRIORITY
        , BSP_CAN2_RX_THREAD_TIMESLICE);
    os_thread_startup(&BSP_CAN2__RxThread);
}


void BSP_CAN2_SetRxHandler(BSP_CAN2_RxHandler handler, void* userdata){
    CAN_INTConfig(CAN2, CAN_INT_FMP0, DISABLE);
    
    BSP_CAN2__RxHandler = handler;
    BSP_CAN2__RxHandlerParameter = userdata;
    
    CAN_INTConfig(CAN2, CAN_INT_FMP0, ENABLE);
}


////////////////////////////////////////////////////////////////////////////////
////

void CAN2_RX0_IRQHandler(void)
{
    CanRxMessage RxMessage;
    CAN_ReceiveMessage(CAN2, CAN_FIFO0, &RxMessage);
    sdk_ring_put(&BSP_CAN2__RxRing, &RxMessage);
    if(BSP_CAN2__RxThreadFlag==OS_TRUE){
        os_sem_release(&BSP_CAN2__RxSem);
    }
}
