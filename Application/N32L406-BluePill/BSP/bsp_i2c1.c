#include <bsp_i2c1.h>
#include <stdio.h>
#include <os_mutex.h>
////////////////////////////////////////////////////////////////////////////////
////

#define BSP_I2Cx I2C1
#define BSP_I2Cx_SPEED 400000
#define BSP_I2Cx_MASTER_ADDRESS 0x00

////////////////////////////////////////////////////////////////////////////////
////
static os_mutex_t BSP_I2C1__Lock;
static BSP_I2C1_TimeoutHandler BSP_I2C1__TimeoutHandler=0;
static void* BSP_I2C1__TimeoutHandler_Parameter=0;

////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE void BSP_I2C1__Delay(unsigned long time){
    for(int i=0; i<time; i++){
        for(int j=0; j<72; j++);
    }
}

static void BSP_I2C1__TimeOut(I2C_Module* I2Cx, void* userdata)
{
    printf("BSP_I2C1_Timeout...\n");
    if(BSP_I2C1__TimeoutHandler){
        printf("Call BSP_I2C1__TimeoutHandler\n");
        BSP_I2C1__TimeoutHandler(BSP_I2C1__TimeoutHandler_Parameter);
    }else{
        BSP_I2C1_Reset();
    }
    printf("BSP_I2C1_Timeout Done!!!\n");
}

////////////////////////////////////////////////////////////////////////////////
////

void BSP_I2C1_Init(void){
    os_mutex_init(&BSP_I2C1__Lock);
    hw_i2c_master_config(BSP_I2Cx
            , GPIOB, GPIO_PIN_8
            , GPIOB, GPIO_PIN_7
            , BSP_I2Cx_MASTER_ADDRESS
            , BSP_I2Cx_SPEED);
}

int BSP_I2C1_Send(uint8_t address, uint8_t* data, int size)
{
    int err;
    os_mutex_lock(&BSP_I2C1__Lock);
    err = hw_i2c_master_send(BSP_I2Cx, address, data, size, BSP_I2C1__TimeOut, 0);
    os_mutex_unlock(&BSP_I2C1__Lock);
    if(err<0){
        printf("BSP_I2C_Send To %02X, Size: %d, Err: %d\n", address, size, err);
    }
    
    return err;
}

int BSP_I2C1_Recv(uint8_t address, uint8_t* buffer, int size)
{
    int err;
    os_mutex_lock(&BSP_I2C1__Lock);
    err =  hw_i2c_master_recv(BSP_I2Cx, address, buffer, size, BSP_I2C1__TimeOut, 0);
    os_mutex_unlock(&BSP_I2C1__Lock);
    
    if(err<0){
        printf("BSP_I2C1_Recv From %02X, Err: %d\n", address, err);
    }
    return err;
}

void BSP_I2C1_Reset(void){
    hw_i2c_master_config(BSP_I2Cx
            , GPIOB, GPIO_PIN_8
            , GPIOB, GPIO_PIN_7
            , BSP_I2Cx_MASTER_ADDRESS
            , BSP_I2Cx_SPEED);
}

void BSP_I2C1_SetTimeoutHandler(BSP_I2C1_TimeoutHandler handler, void* userdata){
    BSP_I2C1__TimeoutHandler = handler;
    BSP_I2C1__TimeoutHandler_Parameter = userdata;
}



