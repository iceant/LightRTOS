#include <bsp_timerdelay.h>
#include <stdio.h>
#include <os_kernel.h>

#define TIMER  TIM4

static uint32_t gu32_ticks;
static TIM_HandleTypeDef HTIMx;

void BSP_TimerDelay_Init(void)
{
    __HAL_RCC_TIM4_CLK_ENABLE();

//    gu32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
    gu32_ticks = (SystemCoreClock / 1000000);

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    HTIMx.Instance = TIMER;
    HTIMx.Init.Prescaler = gu32_ticks-1;
    HTIMx.Init.CounterMode = TIM_COUNTERMODE_UP;
    HTIMx.Init.Period = 65535;
    HTIMx.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HTIMx.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&HTIMx);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&HTIMx, &sClockSourceConfig);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&HTIMx, &sMasterConfig);

    HAL_TIM_Base_Start(&HTIMx);
}

uint32_t BSP_TimerDelay_Tick(void){
    return HTIMx.Instance->CNT;
}

void BSP_TimerDelay_us(uint16_t us)
{
#if 0
    HTIMx.Instance->CNT = 0;
    while (HTIMx.Instance->CNT < us);
#endif
    os_scheduler_disable();
    HAL_TIM_Base_Start(&HTIMx);
    HTIMx.Instance->CNT = 0;
    while (HTIMx.Instance->CNT < us);
    HAL_TIM_Base_Stop(&HTIMx);
    os_scheduler_enable();
#if 0
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload = 0;

    reload = HTIMx.Init.Period;
    ticks = us * (SystemCoreClock / 1000000);
    tcnt = 0;
    told = HTIMx.Instance->CNT;

    while(1){
        tnow = HTIMx.Instance->CNT;
        if(tnow!=told){
            if(tnow < told){
                tcnt += told - tnow;
            }else{
                tcnt += reload - tnow + told;
            }
            told = tnow;
        }

        if(tcnt >= ticks){
            break;
        }
    }
#endif
}

void BSP_TimerDelay_ms(uint16_t au16_ms)
{
    os_scheduler_disable();
    HAL_TIM_Base_Start(&HTIMx);
    do{
        HTIMx.Instance->CNT = 0;
        while (HTIMx.Instance->CNT < 1000);
    }while(au16_ms--);
    HAL_TIM_Base_Stop(&HTIMx);
    os_scheduler_enable();
}


