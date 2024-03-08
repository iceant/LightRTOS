#ifndef INCLUDED_BSP_I2C1_H
#define INCLUDED_BSP_I2C1_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32G45X_H
#define INCLUDED_N32G45X_H
#include <n32g45x.h>
#endif /*INCLUDED_N32G45X_H*/


////////////////////////////////////////////////////////////////////////////////
////

void BSP_I2C1_Init(void);

void BSP_I2C1_Reset(void);

int BSP_I2C1_Send(uint8_t address, uint8_t* data, int size);

int BSP_I2C1_Recv(uint8_t address, uint8_t* data, int size);




#endif /*INCLUDED_BSP_I2C1_H*/
