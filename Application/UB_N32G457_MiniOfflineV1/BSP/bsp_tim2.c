#include <bsp_tim2.h>
#include <os_kernel.h>

////////////////////////////////////////////////////////////////////////////////
////

#define TM2_TICK_PER_MS 1000000
#define TIM2_CLK (SYSCLK_FREQ/2)
#define TIM2_PSC (TIM2_CLK/TM2_TICK_PER_MS)
#define TIM2_TICK_PERIOD (1000-1)

#define BSP_TIM2_SECOND_THREAD_STACK_SIZE 512
#define BSP_TIM2_SECOND_THREAD_PRIORITY 20
#define BSP_TIM2_SECOND_THREAD_TIMESLICE 10
////////////////////////////////////////////////////////////////////////////////
////

static BSP_TIM2_TimeUpHandler BSP_TIM2__TimeUpHandler=0;
static void* BSP_TIM2__TimeUpHandler_Parameter = 0;
static volatile uint32_t BSP_TIM2__TickCount=0;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t BSP_TIM2__SecondThread_Stack[BSP_TIM2_SECOND_THREAD_STACK_SIZE];
static os_thread_t BSP_TIM2__SecondThread;
static os_sem_t BSP_TIM2__Sem;
static os_bool_t BSP_TIM2__ThreadReadyFlag = OS_FALSE;
////////////////////////////////////////////////////////////////////////////////
////
static void BSP_TIM2_SecondThread_Entry(void* p){
    os_sem_init(&BSP_TIM2__Sem, "TIM2_Sem", 0, OS_QUEUE_FIFO);
    BSP_TIM2__ThreadReadyFlag = OS_TRUE;
    while(1){
        os_sem_take(&BSP_TIM2__Sem, OS_WAIT_INFINITY);
        if(BSP_TIM2__TimeUpHandler){
            BSP_TIM2__TimeUpHandler(BSP_TIM2__TimeUpHandler_Parameter);
        }
    }
}

static void RCC_Configuration(void){
    /* PCLK1 = HCLK/4 */
    RCC_ConfigPclk1(RCC_HCLK_DIV4);
    /* TIM2 clock enable */
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_TIM2, ENABLE);
    
}

static void NVIC_Configuration(void)
{
    NVIC_InitType NVIC_InitStructure;
    
    /* Enable the TIM2 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    
    NVIC_Init(&NVIC_InitStructure);
}


static void TIM_Configuration(void)
{
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
//    OCInitType TIM_OCInitStructure;
    
    /* Compute the prescaler value */
    //                                                                                       100000000
//    uint16_t PrescalerValue = (SystemCoreClock/1000000)-1; //(uint16_t) (SystemCoreClock / 12000000) - 1;
    uint16_t PrescalerValue = TIM2_PSC; //(uint16_t) (SystemCoreClock / (4 * 1000000)) - 1;
    
    /* Time base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period    = TIM2_TICK_PERIOD;
    TIM_TimeBaseStructure.Prescaler = 0;
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;
    
    TIM_InitTimeBase(TIM2, &TIM_TimeBaseStructure);
    
    /* Prescaler configuration */
    TIM_ConfigPrescaler(TIM2, PrescalerValue, TIM_PSC_RELOAD_MODE_IMMEDIATE);
    
    /* TIM2 enable counter */
    TIM_Enable(TIM2, ENABLE);
}
////////////////////////////////////////////////////////////////////////////////
////

void BSP_TIM2_Init(void)
{
    RCC_Configuration();
    NVIC_Configuration();
    TIM_Configuration();
    

    os_thread_init(&BSP_TIM2__SecondThread, "TIM2_SecThd"
                   , BSP_TIM2_SecondThread_Entry, 0
                   , BSP_TIM2__SecondThread_Stack, sizeof(BSP_TIM2__SecondThread_Stack)
                   , BSP_TIM2_SECOND_THREAD_PRIORITY
                   , BSP_TIM2_SECOND_THREAD_TIMESLICE);
    os_thread_startup(&BSP_TIM2__SecondThread);
}

void BSP_TIM2_SetTimeUpHandler(BSP_TIM2_TimeUpHandler TimeUpHandler, void* userdata)
{
    TIM_ConfigInt(TIM2, TIM_INT_UPDATE, DISABLE);
    
    BSP_TIM2__TimeUpHandler = TimeUpHandler;
    BSP_TIM2__TimeUpHandler_Parameter = userdata;
    
    /* TIM2 enable update irq */
    TIM_ConfigInt(TIM2, TIM_INT_UPDATE, ENABLE);
}

volatile uint32_t BSP_TIM2_GetTickCount(void){
    return BSP_TIM2__TickCount;
}




////////////////////////////////////////////////////////////////////////////////
////
void TIM2_IRQHandler(void)
{
    if (TIM_GetIntStatus(TIM2, TIM_INT_UPDATE) != RESET)
    {
        BSP_TIM2__TickCount++;
        TIM_ClrIntPendingBit(TIM2, TIM_INT_UPDATE);
        
//        if(BSP_TIM2__ThreadReadyFlag==OS_TRUE){
//            os_sem_release(&BSP_TIM2__Sem);
//        }
        if(BSP_TIM2__TimeUpHandler){
            BSP_TIM2__TimeUpHandler(BSP_TIM2__TimeUpHandler_Parameter);
        }
        
        
    }
}


