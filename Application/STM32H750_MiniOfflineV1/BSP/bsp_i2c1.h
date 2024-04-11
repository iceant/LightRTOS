#ifndef INCLUDED_BSP_I2C1_H
#define INCLUDED_BSP_I2C1_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32H7XX_H
#define INCLUDED_STM32H7XX_H
#include <stm32h7xx.h>
#endif /*INCLUDED_STM32H7XX_H*/


////////////////////////////////////////////////////////////////////////////////
////
void BSP_I2C1_Init(void);

void BSP_I2C1_DeInit(void);

void BSP_I2C1_EnableDMA(void);

void BSP_I2C1_DisableDMA(void);

int BSP_I2C1_MasterSend(uint8_t slaveAddress, uint8_t * data, int data_size);

int BSP_I2C1_MasterReceive(uint8_t slaveAddress, uint8_t * data, int data_size);


#endif /*INCLUDED_BSP_I2C1_H*/
