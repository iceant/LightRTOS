#ifndef INCLUDED_BSP_CAN1_H
#define INCLUDED_BSP_CAN1_H


////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32H7XX_H
#define INCLUDED_STM32H7XX_H
#include <stm32h7xx.h>
#endif /*INCLUDED_STM32H7XX_H*/

////////////////////////////////////////////////////////////////////////////////
////
void BSP_CAN1_Init(void);

void BSP_CAN1_Send(uint32_t Identifier, uint32_t IdType /*FDCAN_STANDARD_ID | FDCAN_EXTENDED_ID*/
                   , uint32_t frameType /*FDCAN_DATA_FRAME | FDCAN_REMOTE_FRAME*/
                   , uint8_t *TxData, int DataLength);



#endif /*INCLUDED_BSP_CAN1_H*/
