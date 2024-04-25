#ifndef INCLUDED_BSP_CAN1_H
#define INCLUDED_BSP_CAN1_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32L40X_H
#define INCLUDED_N32L40X_H
#include <n32l40x.h>
#endif /*INCLUDED_N32L40X_H*/


////////////////////////////////////////////////////////////////////////////////
////
typedef void (*BSP_CAN1_RxHandler)(CanRxMessage* rxMessage, void* userdata);



////////////////////////////////////////////////////////////////////////////////
////
void BSP_CAN1_Init(void);

int BSP_CAN1_Send(CanTxMessage* TxMessage);

void BSP_CAN1_SetRxHandler(BSP_CAN1_RxHandler handler, void* userdata);

#endif /*INCLUDED_BSP_CAN1_H*/
