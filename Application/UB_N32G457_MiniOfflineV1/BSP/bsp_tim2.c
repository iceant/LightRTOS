#include <bsp_tim2.h>

////////////////////////////////////////////////////////////////////////////////
////

#define TM2_TICK_PER_MS 1000000
#define TIM2_CLK (SYSCLK_FREQ/2)
#define TIM2_PSC (TIM2_CLK/TM2_TICK_PER_MS)
#define TIM2_TICK_PERIOD (1000-1)

////////////////////////////////////////////////////////////////////////////////
////

static BSP_TIM2_TimeUpHandler BSP_TIM2__TimeUpHandler=0;
static void* BSP_TIM2__TimeUpHandler_Parameter = 0;
////////////////////////////////////////////////////////////////////////////////
////

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
}

void BSP_TIM2_SetTimeUpHandler(BSP_TIM2_TimeUpHandler TimeUpHandler, void* userdata)
{
    TIM_ConfigInt(TIM2, TIM_INT_UPDATE, DISABLE);
    
    BSP_TIM2__TimeUpHandler = TimeUpHandler;
    BSP_TIM2__TimeUpHandler_Parameter = userdata;
    
    /* TIM2 enable update irq */
    TIM_ConfigInt(TIM2, TIM_INT_UPDATE, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
////
void TIM2_IRQHandler(void)
{
    if (TIM_GetIntStatus(TIM2, TIM_INT_UPDATE) != RESET)
    {
        if(BSP_TIM2__TimeUpHandler){
            BSP_TIM2__TimeUpHandler(BSP_TIM2__TimeUpHandler_Parameter);
        }
        TIM_ClrIntPendingBit(TIM2, TIM_INT_UPDATE);
    }
}

