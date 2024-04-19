#ifndef INCLUDED_BSP_TIMERDELAY_H
#define INCLUDED_BSP_TIMERDELAY_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32F4XX_H
#define INCLUDED_STM32F4XX_H
#include <stm32f4xx.h>
#endif /*INCLUDED_STM32F4XX_H*/

////////////////////////////////////////////////////////////////////////////////
////
void BSP_TimerDelay_Init(void);

void BSP_TimerDelay_us(uint16_t au16_us);

void BSP_TimerDelay_ms(uint16_t au16_ms);

uint32_t BSP_TimerDelay_Tick(void);

#endif /*INCLUDED_BSP_TIMERDELAY_H*/
