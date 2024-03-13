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
extern volatile uint32_t BSP_TIM2_TickCount;

////////////////////////////////////////////////////////////////////////////////
////


void BSP_TIM2_Init(void);

void BSP_TIM2_SetTimeUpHandler(BSP_TIM2_TimeUpHandler TimeUpHandler, void* userdata);

volatile uint32_t BSP_TIM2_GetTickCount(void);

__STATIC_FORCEINLINE uint32_t BSP_TIM2_TickDiff(uint32_t start, uint32_t end){
    if(end < start){
        return end + (UINT32_MAX-start);
    }else{
        return end-start;
    }
}

#endif /* INCLUDED_BSP_TIM2_H */
