#include <bsp_i2c1.h>
#include <hw_i2c.h>
#include <cpu_spinlock.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////
static cpu_spinlock_t lock;

static void BSP_I2C1__timeout(I2C_Module* I2Cx, void* userdata)
{
    BSP_I2C1_Reset();
}


////////////////////////////////////////////////////////////////////////////////
////

void BSP_I2C1_Init(void){
    cpu_spinlock_init(&lock);
    hw_i2c_master_config(I2C1, GPIO_RMP_I2C1, 0x00, 400000);
}

void BSP_I2C1_Reset(void){
    hw_i2c_master_reset(I2C1, GPIO_RMP_I2C1);
    hw_i2c_master_config(I2C1, GPIO_RMP_I2C1, 0x00, 400000);
}

int BSP_I2C1_Send(uint8_t address, uint8_t* data, int size){
    int err = 0;
    cpu_spinlock_lock(&lock);
    err = hw_i2c_master_send(I2C1, address, data, size, BSP_I2C1__timeout, 0);
    cpu_spinlock_unlock(&lock);
    if(err<0){
        printf("BSP_I2C1_Send Error: %d\n", err);
    }
    return err;
}

int BSP_I2C1_Recv(uint8_t address, uint8_t* data, int size){
    int err = 0;
    cpu_spinlock_lock(&lock);
    err = hw_i2c_master_recv(I2C1, address, data, size, BSP_I2C1__timeout, 0);
    cpu_spinlock_unlock(&lock);
    if(err<0){
        printf("BSP_I2C1_Recv Error: %d\n", err);
    }
    return err;
}

