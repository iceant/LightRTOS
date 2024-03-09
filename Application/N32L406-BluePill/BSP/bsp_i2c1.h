#ifndef INCLUDED_BSP_I2C1_H
#define INCLUDED_BSP_I2C1_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_HW_I2C_H
#include <hw_i2c.h>
#endif /*INCLUDED_HW_I2C_H*/

////////////////////////////////////////////////////////////////////////////////
////

void BSP_I2C1_Init(void);

int BSP_I2C1_Send(uint8_t address, uint8_t* data, int size);

int BSP_I2C1_Recv(uint8_t address, uint8_t* buffer, int size);


#endif /* INCLUDED_BSP_I2C1_H */
