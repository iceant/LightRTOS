#ifndef INCLUDED_BSP_DWTDELAY_H
#define INCLUDED_BSP_DWTDELAY_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32F4XX_H
#define INCLUDED_STM32F4XX_H
#include <stm32f4xx.h>
#endif /*INCLUDED_STM32F4XX_H*/

////////////////////////////////////////////////////////////////////////////////
////
void BSP_DWTDelay_Init(void);

void BSP_DWTDelay(uint32_t us);

__STATIC_INLINE void BSP_DWTDelay_us(volatile uint32_t au32_microseconds)
{
    uint32_t au32_initial_ticks = DWT->CYCCNT;
//    uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
    uint32_t au32_ticks = (SystemCoreClock / 1000000);
    au32_microseconds *= au32_ticks;
    while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds-au32_ticks);
}

__STATIC_INLINE void BSP_DWTDelay_ms(volatile uint32_t au32_milliseconds)
{
    uint32_t au32_initial_ticks = DWT->CYCCNT;
//    uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000);
    uint32_t au32_ticks = (SystemCoreClock / 1000);
    au32_milliseconds *= au32_ticks;
    while ((DWT->CYCCNT - au32_initial_ticks) < au32_milliseconds);
}

#endif /*INCLUDED_BSP_DWTDELAY_H*/
