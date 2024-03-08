#ifndef INCLUDED_HW_I2C_H
#define INCLUDED_HW_I2C_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32G45X_H
#define INCLUDED_N32G45X_H
#include <n32g45x.h>
#endif /*INCLUDED_N32G45X_H*/

////////////////////////////////////////////////////////////////////////////////
////
typedef void (*hw_i2c_master_timeout)(I2C_Module* I2Cx, void* userdata);


////////////////////////////////////////////////////////////////////////////////
////

void hw_i2c_master_config(I2C_Module* I2Cx, uint32_t Remap, uint16_t MasterAddr, uint32_t ClockSpeed /*100000 | 400000*/);

int hw_i2c_master_send(I2C_Module * I2Cx, uint8_t address, uint8_t * data, int size, hw_i2c_master_timeout, void* userdata);

int hw_i2c_master_recv(I2C_Module * I2Cx, uint8_t address, uint8_t * data, int size, hw_i2c_master_timeout, void* userdata);

void hw_i2c_master_reset(I2C_Module *I2Cx, uint32_t Remap);


#endif /*INCLUDED_HW_I2C_H*/
