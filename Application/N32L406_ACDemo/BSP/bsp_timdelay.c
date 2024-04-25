#include <bsp_timdelay.h>
#include <os_kernel.h>
////////////////////////////////////////////////////////////////////////////////
////
#define TIMx TIM2
#define TIMx_IRQHandler TIM2_IRQHandler
#define TIMx_RCC RCC_APB1_PERIPH_TIM2


////////////////////////////////////////////////////////////////////////////////
////

void BSP_TIMDelay_Init(void)
{
    IRQn_Type TIMx_IRQn = TIM2_IRQn;

    /*--------------------------------------------------------------------------------------------------------------*/
    /* RCC */
    /*--------------------------------------------------------------------------------------------------------------*/
    /* PCLK1 = HCLK/4 */
    RCC_ConfigPclk1(RCC_HCLK_DIV4);

    /* TIM2 clock enable */
    RCC_EnableAPB1PeriphClk(TIMx_RCC, ENABLE);

    /*--------------------------------------------------------------------------------------------------------------*/
    /* NVIC */
    /*--------------------------------------------------------------------------------------------------------------*/
//    NVIC_InitType NVIC_InitStructure;
//
//    /* Enable the TIM2 global Interrupt */
//    NVIC_InitStructure.NVIC_IRQChannel                   = TIMx_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
//
//    NVIC_Init(&NVIC_InitStructure);

    /*--------------------------------------------------------------------------------------------------------------*/
    /* TIM */
    /*--------------------------------------------------------------------------------------------------------------*/
    TIM_TimeBaseInitType TIM_TimeBaseStructure;
    uint16_t PrescalerValue = 0;

    /* Compute the prescaler value */
    PrescalerValue = (SystemCoreClock/1000000) - 1; //(uint16_t) (SystemCoreClock / 12000000) - 1;

    /* Time base configuration */
    TIM_InitTimBaseStruct(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.Period    = 65535;
    TIM_TimeBaseStructure.Prescaler = 0;
    TIM_TimeBaseStructure.ClkDiv    = TIM_CLK_DIV1;
    TIM_TimeBaseStructure.CntMode   = TIM_CNT_MODE_UP;

    TIM_InitTimeBase(TIMx, &TIM_TimeBaseStructure);

    /* Prescaler configuration */
    TIM_ConfigPrescaler(TIMx, PrescalerValue, TIM_PSC_RELOAD_MODE_IMMEDIATE);

    /* TIM2 enable update irq */
//    TIM_ConfigInt(TIMx, TIM_INT_UPDATE, ENABLE);

    /* TIM2 enable counter */
    //TIM_Enable(TIMx, ENABLE);
}

#if 0
void BSP_TIMDelay_Start(void){
    BSP_TIMDelay_Init();
    TIM_Enable(TIMx, ENABLE);
}

void BSP_TIMDelay_Stop(void){
    TIM_Enable(TIMx, DISABLE);
    RCC_EnableAPB1PeriphClk(TIMx_RCC, DISABLE);
}
#endif

void BSP_TIMDelay_us(uint16_t us)
{
    TIM_Enable(TIMx, ENABLE);
    TIMx->CNT=0;
    while(TIMx->CNT < us);
    TIM_Enable(TIMx, DISABLE);
}

void BSP_TIMDelay_ms(uint16_t ms){
    uint32_t timeout = (ms-4)/2;
    os_scheduler_disable();
    TIM_Enable(TIMx, ENABLE);
    do {
        TIMx->CNT=0;
        while(TIMx->CNT < 1000);
    }while(timeout--);
    TIM_Enable(TIMx, DISABLE);
    os_scheduler_enable();
}

////////////////////////////////////////////////////////////////////////////////
////
//void TIMx_IRQHandler(void)
//{
//    if (TIM_GetIntStatus(TIMx, TIM_INT_UPDATE) != RESET)
//    {
//        TIM_ClrIntPendingBit(TIMx, TIM_INT_UPDATE);
//    }
//}

