#ifndef INCLUDED_BSP_TIMDELAY_H
#define INCLUDED_BSP_TIMDELAY_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32L40X_H
#define INCLUDED_N32L40X_H
#include <n32l40x.h>
#endif /*INCLUDED_N32L40X_H*/


////////////////////////////////////////////////////////////////////////////////
////
void BSP_TIMDelay_Init(void);

void BSP_TIMDelay_us(uint16_t us);

void BSP_TIMDelay_ms(uint16_t ms);


#endif /*INCLUDED_BSP_TIMDELAY_H*/
