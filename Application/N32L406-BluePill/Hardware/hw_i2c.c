#include <hw_i2c.h>
#include <hw_gpio.h>
#include <stdio.h>
#include "hw_delay.h"

////////////////////////////////////////////////////////////////////////////////
////
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

////////////////////////////////////////////////////////////////////////////////
////
#define HW_I2C_GPIO_SCL(GPIOx, GPIO_Pin, AF) \
    hw_gpio_init(GPIOx, GPIO_Pin, GPIO_Mode_AF_OD, GPIO_Pull_Up, GPIO_DC_2mA, GPIO_Slew_Rate_High, AF)

#define HW_I2C_GPIO_SDA(GPIOx, GPIO_Pin, AF) \
    hw_gpio_init(GPIOx, GPIO_Pin, GPIO_Mode_AF_OD, GPIO_Pull_Up, GPIO_DC_2mA, GPIO_Slew_Rate_High, AF)

////////////////////////////////////////////////////////////////////////////////
////

static void Delay(uint32_t nCount)
{
    uint32_t tcnt;
    while(nCount--)
    {
        tcnt = 64000 / 5;
        while (tcnt--){;}
    }
}

static void Delay_us(uint32_t nCount)
{
    uint32_t tcnt;
    while (nCount--)
    {
        tcnt = 64 / 5;
        while (tcnt--){;}
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void hw_i2c_master_config(I2C_Module* I2Cx
        , GPIO_Module* SCL_GPIO, uint16_t SCL_Pin
        , GPIO_Module* SDA_GPIO, uint16_t SDA_Pin
        , uint16_t MasterAddr, uint32_t ClockSpeed /*100000 | 400000*/)
{
    I2C_InitType i2c1_master;
    
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    
    if(I2Cx==I2C1){
        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C1, ENABLE);
        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C1, DISABLE);

        if(SCL_GPIO==GPIOA && SCL_Pin==GPIO_PIN_4){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF7_I2C1);
        }else if(SCL_GPIO==GPIOA && SCL_Pin==GPIO_PIN_15){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF7_I2C1);
        }else if(SCL_GPIO==GPIOB && SCL_Pin==GPIO_PIN_6){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF1_I2C1);
        }else if(SCL_GPIO==GPIOB && SCL_Pin==GPIO_PIN_8){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF4_I2C1);
        }else if(SCL_GPIO==GPIOC && SCL_Pin==GPIO_PIN_0){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF7_I2C1);
        }else if(SCL_GPIO==GPIOC && SCL_Pin==GPIO_PIN_4){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF7_I2C1);
        }else if(SCL_GPIO==GPIOD && SCL_Pin==GPIO_PIN_13){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF7_I2C1);
        }
        
        if(SDA_GPIO==GPIOA && SDA_Pin==GPIO_PIN_5){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF7_I2C1);
        }else if(SDA_GPIO==GPIOA && SDA_Pin==GPIO_PIN_14){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF7_I2C1);
        }else if(SDA_GPIO==GPIOB && SDA_Pin==GPIO_PIN_7){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF1_I2C1);
        }else if(SDA_GPIO==GPIOB && SDA_Pin==GPIO_PIN_9){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF4_I2C1);
        }else if(SDA_GPIO==GPIOC && SDA_Pin==GPIO_PIN_1){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF7_I2C1);
        }else if(SDA_GPIO==GPIOC && SDA_Pin==GPIO_PIN_5){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF7_I2C1);
        }else if(SDA_GPIO==GPIOD && SDA_Pin==GPIO_PIN_12){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF7_I2C1);
        }
        
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C1, ENABLE);
    }else if(I2Cx==I2C2){
        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, ENABLE);
        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, DISABLE);
        
        if(SCL_GPIO==GPIOA && SCL_Pin==GPIO_PIN_3){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF5_I2C2);
        }else if(SCL_GPIO==GPIOA && SCL_Pin==GPIO_PIN_9){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF6_I2C2);
        }else if(SCL_GPIO==GPIOB && SCL_Pin==GPIO_PIN_10){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF6_I2C2);
        }else if(SCL_GPIO==GPIOB && SCL_Pin==GPIO_PIN_13){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF5_I2C2);
        }else if(SCL_GPIO==GPIOD && SCL_Pin==GPIO_PIN_15){
            HW_I2C_GPIO_SCL(SCL_GPIO, SCL_Pin, GPIO_AF6_I2C2);
        }
        
        if(SDA_GPIO==GPIOA && SDA_Pin==GPIO_PIN_2){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF5_I2C2);
        }else if(SDA_GPIO==GPIOA && SDA_Pin==GPIO_PIN_8){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF6_I2C2);
        }else if(SDA_GPIO==GPIOA && SDA_Pin==GPIO_PIN_10){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF6_I2C2);
        }else if(SDA_GPIO==GPIOB && SDA_Pin==GPIO_PIN_11){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF6_I2C2);
        }else if(SDA_GPIO==GPIOB && SDA_Pin==GPIO_PIN_14){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF5_I2C2);
        }else if(SDA_GPIO==GPIOD && SDA_Pin==GPIO_PIN_14){
            HW_I2C_GPIO_SDA(SDA_GPIO, SDA_Pin, GPIO_AF6_I2C2);
        }
        
        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C2, ENABLE);
    }

    hw_gpio_high(SCL_GPIO, SCL_Pin);
    hw_gpio_high(SDA_GPIO, SDA_Pin);
    
    Delay_us(5);

    int i;
    for (i = 0; i < 9; i++)
    {
        hw_gpio_high(SCL_GPIO, SCL_Pin);
        Delay_us(5);
        hw_gpio_low(SCL_GPIO, SCL_Pin);
        Delay_us(5);
    }

    I2Cx->CTRL1|=I2C_CTRL1_SWRESET;   /*复位控制器*/
    Delay_us(5);
    I2Cx->CTRL1&= ~I2C_CTRL1_SWRESET;                   /*解除复位*/

    I2C_DeInit(I2Cx);
    i2c1_master.BusMode     = I2C_BUSMODE_I2C;
    i2c1_master.FmDutyCycle = I2C_FMDUTYCYCLE_2;
    i2c1_master.OwnAddr1    = MasterAddr;
    i2c1_master.AckEnable   = I2C_ACKEN;
    i2c1_master.AddrMode    = I2C_ADDR_MODE_7BIT;
    i2c1_master.ClkSpeed    = ClockSpeed; // 100K

    I2C_Init(I2Cx, &i2c1_master);
    I2C_Enable(I2Cx, ENABLE);
}


int hw_i2c_master_send(I2C_Module * I2Cx, uint8_t address, uint8_t * data, int size, hw_i2c_master_timeout timeout, void* userdata)
{
    int send_size = size;
    uint32_t I2CTimeout             = I2CT_LONG_TIMEOUT;
    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
        {
            timeout(I2Cx, userdata);
            return -1;
        }
    }

    I2C_GenerateStart(I2Cx, ENABLE);
    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_MODE_FLAG)) // EV5
    {
        if ((I2CTimeout--) == 0)
        {
            timeout(I2Cx, userdata);
            return -2;
        }
    }

    I2C_SendAddr7bit(I2Cx, address, I2C_DIRECTION_SEND);
    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_TXMODE_FLAG)) // EV6
    {
        if ((I2CTimeout--) == 0)
        {
            timeout(I2Cx, userdata);
            return -3;
        }
    }

    while (send_size-- > 0)
    {
        I2C_SendData(I2Cx, *data++);
        I2CTimeout = I2CT_LONG_TIMEOUT;
        while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING)) // EV8
        {
            if ((I2CTimeout--) == 0)
            {
                timeout(I2Cx, userdata);
                return -4;
            }
        }
    }

    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDED)) // EV8-2
    {
        if ((I2CTimeout--) == 0)
        {
            timeout(I2Cx, userdata);
            return -5;
        }
    }

    I2C_GenerateStop(I2Cx, ENABLE);

    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
        {
            timeout(I2Cx, userdata);
            return -6;
        }
    }

    return size;
}


int hw_i2c_master_recv(I2C_Module * I2Cx, uint8_t address, uint8_t * data, int size, hw_i2c_master_timeout timeout, void* userdata)
{
    int received_size = 0;
    uint32_t I2CTimeout             = I2CT_LONG_TIMEOUT;
    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
        {
            timeout(I2Cx, userdata);
            return -1;
        }
    }
    I2C_ConfigAck(I2Cx, ENABLE);

    I2C_GenerateStart(I2Cx, ENABLE);
    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_MODE_FLAG)) // EV5
    {
        if ((I2CTimeout--) == 0)
        {
            timeout(I2Cx, userdata);
            return -2;
        }
    }

    // send addr
    I2C_SendAddr7bit(I2Cx, address, I2C_DIRECTION_RECV);
    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_RXMODE_FLAG)) // EV6
    {
        if ((I2CTimeout--) == 0)
        {
            timeout(I2Cx, userdata);
            return -3;
        }
    }

    if (size == 1)
    {
        I2C_ConfigAck(I2Cx, DISABLE);
        (void)(I2Cx->STS1); /// clear ADDR
        (void)(I2Cx->STS2);

        I2C_GenerateStop(I2Cx, ENABLE);
        I2CTimeout = I2CT_LONG_TIMEOUT;
        while (!I2C_GetFlag(I2Cx, I2C_FLAG_RXDATNE))
        {
            if ((I2CTimeout--) == 0)
            {
                timeout(I2Cx, userdata);
                return -4;
            }
        }
        *data++ = I2C_RecvData(I2Cx); size--; received_size++;
    }
    else if (size == 2)
    {
        I2Cx->CTRL1 |= 0x0800; /// set ACKPOS
        (void)(I2Cx->STS1);
        (void)(I2Cx->STS2);
        I2C_ConfigAck(I2Cx, DISABLE);

        I2CTimeout = I2CT_LONG_TIMEOUT;
        while (!I2C_GetFlag(I2Cx, I2C_FLAG_BYTEF))
        {
            if ((I2CTimeout--) == 0)
            {
                timeout(I2Cx, userdata);
                return -5;
            }
        }
        I2C_GenerateStop(I2Cx, ENABLE);

        *data++ = I2C_RecvData(I2Cx); size--; received_size++;
        *data++ = I2C_RecvData(I2Cx); size--; received_size++;
    }
    else
    {
        I2C_ConfigAck(I2Cx, ENABLE);
        (void)(I2Cx->STS1);
        (void)(I2Cx->STS2);

        while (size)
        {
            if (size == 3)
            {
                I2CTimeout = I2CT_LONG_TIMEOUT;
                while (!I2C_GetFlag(I2Cx, I2C_FLAG_BYTEF))
                {
                    if ((I2CTimeout--) == 0)
                    {
                        timeout(I2Cx, userdata);
                        return -6;
                    }
                }
                I2C_ConfigAck(I2Cx, DISABLE);
                *data++ = I2C_RecvData(I2Cx); size--; received_size++;

                I2CTimeout = I2CT_LONG_TIMEOUT;
                while (!I2C_GetFlag(I2Cx, I2C_FLAG_BYTEF))
                {
                    if ((I2CTimeout--) == 0)
                    {
                        timeout(I2Cx, userdata);
                        return -7;
                    }
                }

                I2C_GenerateStop(I2Cx, ENABLE);

                *data++ = I2C_RecvData(I2Cx); size--; received_size++;
                *data++ = I2C_RecvData(I2Cx); size--; received_size++;

                break;
            }

            I2CTimeout = I2CT_LONG_TIMEOUT;
            while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_RECVD_FLAG)) // EV7
            {
                if ((I2CTimeout--) == 0)
                {
                    timeout(I2Cx, userdata);
                    return -8;
                }
            }
            *data++ = I2C_RecvData(I2Cx); size--; received_size++;
        }
    }

    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
        {
            timeout(I2Cx, userdata);
            return -9;
        }
    }

    return received_size;
}



