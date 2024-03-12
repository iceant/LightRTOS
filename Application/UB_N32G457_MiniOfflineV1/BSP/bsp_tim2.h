#ifndef INCLUDED_BSP_TIM2_H
#define INCLUDED_BSP_TIM2_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_N32G45X_H
#define INCLUDED_N32G45X_H
#include <n32g45x.h>
#endif /*INCLUDED_N32G45X_H*/

////////////////////////////////////////////////////////////////////////////////
////

typedef void (*BSP_TIM2_TimeUpHandler)(void* userdata);
typedef void (*BSP_TIM2_OnSecondHandler)(void);
////////////////////////////////////////////////////////////////////////////////
////

void BSP_TIM2_Init(void);

void BSP_TIM2_SetTimeUpHandler(BSP_TIM2_TimeUpHandler TimeUpHandler, void* userdata);

volatile uint32_t BSP_TIM2_GetTickCount(void);

#endif /* INCLUDED_BSP_TIM2_H */
