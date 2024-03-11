#include <bsp_i2c1.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////

#define BSP_I2Cx I2C1
#define BSP_I2Cx_SPEED 400000
#define BSP_I2Cx_MASTER_ADDRESS 0x00

////////////////////////////////////////////////////////////////////////////////
////
void BSP_I2C1__TimeOut(I2C_Module* I2Cx, void* userdata)
{
    printf("BSP_I2C1_Timeout...\n");
    BSP_I2C1_Init();
}

////////////////////////////////////////////////////////////////////////////////
////

void BSP_I2C1_Init(void){
    hw_i2c_master_config(BSP_I2Cx
            , GPIOB, GPIO_PIN_8
            , GPIOB, GPIO_PIN_7
            , BSP_I2Cx_MASTER_ADDRESS
            , BSP_I2Cx_SPEED);
}

int BSP_I2C1_Send(uint8_t address, uint8_t* data, int size)
{
    int err;
    
    err = hw_i2c_master_send(BSP_I2Cx, address, data, size, BSP_I2C1__TimeOut, 0);
    if(err<0){
        printf("BSP_I2C_Send To %02X, Size: %d, Err: %d\n", address, size, err);
    }
    
    return err;
}

int BSP_I2C1_Recv(uint8_t address, uint8_t* buffer, int size)
{
    int err =  hw_i2c_master_recv(BSP_I2Cx, address, buffer, size, BSP_I2C1__TimeOut, 0);
    if(err<0){
        printf("BSP_I2C1_Recv From %02X, Err: %d\n", address, err);
    }
    return err;
}

