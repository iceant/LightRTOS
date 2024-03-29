#ifndef INCLUDED_HW_I2C_H
#define INCLUDED_HW_I2C_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32L40X_H
#define INCLUDED_N32L40X_H
#include <n32l40x.h>
#endif /*INCLUDED_N32L40X_H*/



////////////////////////////////////////////////////////////////////////////////
////
typedef void (*hw_i2c_master_timeout)(I2C_Module* I2Cx, void* userdata);


////////////////////////////////////////////////////////////////////////////////
////

void hw_i2c_master_config(I2C_Module* I2Cx
                          , GPIO_Module* SCL_GPIO, uint16_t SCL_Pin
                          , GPIO_Module* SDA_GPIO, uint16_t SDA_Pin
                          , uint16_t MasterAddr, uint32_t ClockSpeed /*100000 | 400000*/);

int hw_i2c_master_send(I2C_Module * I2Cx, uint8_t address, uint8_t * data, int size, hw_i2c_master_timeout, void* userdata);

int hw_i2c_master_recv(I2C_Module * I2Cx, uint8_t address, uint8_t * data, int size, hw_i2c_master_timeout, void* userdata);


#endif /*INCLUDED_HW_I2C_H*/
