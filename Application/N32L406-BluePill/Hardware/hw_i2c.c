//#include <hw_i2c.h>
//#include <hw_gpio.h>
//#include <stdio.h>
//
//////////////////////////////////////////////////////////////////////////////////
//////
//#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
//#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
//
//////////////////////////////////////////////////////////////////////////////////
//////
//__STATIC_FORCEINLINE void Delay_us(uint32_t delay){
//    do{
//        __NOP();
//    } while (delay--);
//}
//
//////////////////////////////////////////////////////////////////////////////////
//////
//
//void hw_i2c_master_config(I2C_Module* I2Cx, uint32_t Remap, uint16_t MasterAddr, uint32_t ClockSpeed /*100000 | 400000*/)
//{
//    I2C_InitType i2c1_master;
//
//    GPIO_Module * SCL_GPIO=0;
//    uint16_t SCL_Pin=0;
//    GPIO_Module * SDA_GPIO=0;
//    uint16_t SDA_Pin=0;
//
//
//    if(I2Cx==I2C1){
//        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C1, ENABLE);
//        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C1, DISABLE);
//
//        if(Remap==0){
//            SCL_GPIO = GPIOB;
//            SCL_Pin = GPIO_PIN_6;
//            SDA_GPIO = GPIOB;
//            SDA_Pin = GPIO_PIN_6;
//        }else if(Remap==GPIO_RMP_I2C1){
//
//            SCL_GPIO = GPIOB;
//            SDA_GPIO = GPIOB;
//            SCL_Pin = GPIO_PIN_8;
//            SDA_Pin = GPIO_PIN_9;
//        }
//
//        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C1, ENABLE);
//    }else if(I2Cx==I2C2){
//        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, ENABLE);
//        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, DISABLE);
//        if(Remap==0){
//            SCL_GPIO = GPIOB;
//            SDA_GPIO = GPIOB;
//            SCL_Pin = GPIO_PIN_10;
//            SDA_Pin = GPIO_PIN_11;
//        }else if(Remap==GPIO_RMP1_I2C2){
//            SCL_GPIO = GPIOG;
//            SDA_GPIO = GPIOG;
//            SCL_Pin = GPIO_PIN_2;
//            SDA_Pin = GPIO_PIN_3;
//        }else if(Remap==GPIO_RMP3_I2C2){
//            SCL_GPIO = GPIOA;
//            SDA_GPIO = GPIOA;
//            SCL_Pin = GPIO_PIN_4;
//            SDA_Pin = GPIO_PIN_5;
//        }
//
//        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C2, ENABLE);
//    }else if(I2Cx==I2C3){
//        RCC_EnableAPB2PeriphReset(RCC_APB2_PERIPH_I2C3, ENABLE);
//        RCC_EnableAPB2PeriphReset(RCC_APB2_PERIPH_I2C3, DISABLE);
//
//        if(Remap==0){
//            SCL_GPIO = GPIOC;
//            SDA_GPIO = GPIOC;
//            SCL_Pin = GPIO_PIN_0;
//            SDA_Pin = GPIO_PIN_1;
//        }else if(Remap==GPIO_RMP2_I2C3){
//            SCL_GPIO = GPIOF;
//            SDA_GPIO = GPIOF;
//            SCL_Pin = GPIO_PIN_4;
//            SDA_Pin = GPIO_PIN_5;
//        }else if(Remap==GPIO_RMP3_I2C3){
//            SCL_GPIO = GPIOC;
//            SDA_GPIO = GPIOC;
//            SCL_Pin = GPIO_PIN_4;
//            SDA_Pin = GPIO_PIN_5;
//        }
//
//        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_I2C3, ENABLE);
//    }else if(I2Cx==I2C4){
//        RCC_EnableAPB2PeriphReset(RCC_APB2_PERIPH_I2C4, ENABLE);
//        RCC_EnableAPB2PeriphReset(RCC_APB2_PERIPH_I2C4, DISABLE);
//
//        if(Remap==0){
//            SCL_GPIO = GPIOC;
//            SDA_GPIO = GPIOC;
//            SCL_Pin = GPIO_PIN_6;
//            SDA_Pin = GPIO_PIN_7;
//        }else if(Remap==GPIO_RMP1_I2C4){
//            SCL_GPIO = GPIOD;
//            SDA_GPIO = GPIOD;
//            SCL_Pin = GPIO_PIN_14;
//            SDA_Pin = GPIO_PIN_15;
//
//        }else if(Remap==GPIO_RMP3_I2C4){
//            SCL_GPIO = GPIOA;
//            SDA_GPIO = GPIOA;
//            SCL_Pin = GPIO_PIN_9;
//            SDA_Pin = GPIO_PIN_10;
//        }
//
//        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_I2C4, ENABLE);
//    }
//
//
//    if(Remap!=0){
//        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
//        GPIO_ConfigPinRemap(Remap, ENABLE);
//    }
//
//    hw_gpio_init(SCL_GPIO, SCL_Pin, GPIO_Mode_AF_OD, GPIO_Speed_2MHz); /*SCL*/
//    hw_gpio_init(SDA_GPIO, SDA_Pin, GPIO_Mode_AF_OD, GPIO_Speed_2MHz); /*SDA*/
//
//    hw_gpio_high(SCL_GPIO, SCL_Pin);
//    hw_gpio_high(SDA_GPIO, SDA_Pin);
//
//    Delay_us(5);
//
//    int i;
//    for (i = 0; i < 9; i++)
//    {
//        hw_gpio_high(SCL_GPIO, SCL_Pin);
//        Delay_us(5);
//        hw_gpio_low(SCL_GPIO, SCL_Pin);
//        Delay_us(5);
//    }
//
//    I2Cx->CTRL1|=I2C_CTRL1_SWRESET;   /*复位控制器*/
//    Delay_us(5);
//    I2Cx->CTRL1&= ~I2C_CTRL1_SWRESET;                   /*解除复位*/
//
//    I2C_DeInit(I2Cx);
//    i2c1_master.BusMode     = I2C_BUSMODE_I2C;
//    i2c1_master.FmDutyCycle = I2C_FMDUTYCYCLE_2;
//    i2c1_master.OwnAddr1    = MasterAddr;
//    i2c1_master.AckEnable   = I2C_ACKEN;
//    i2c1_master.AddrMode    = I2C_ADDR_MODE_7BIT;
//    i2c1_master.ClkSpeed    = ClockSpeed; // 100K
//
//    I2C_Init(I2Cx, &i2c1_master);
//    I2C_Enable(I2Cx, ENABLE);
//}
//
//
//int hw_i2c_master_send(I2C_Module * I2Cx, uint8_t address, uint8_t * data, int size, hw_i2c_master_timeout timeout, void* userdata)
//{
//    int send_size = size;
//    uint32_t I2CTimeout             = I2CT_LONG_TIMEOUT;
//    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            timeout(I2Cx, userdata);
//            return -1;
//        }
//    }
//
//    I2C_GenerateStart(I2Cx, ENABLE);
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_MODE_FLAG)) // EV5
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            timeout(I2Cx, userdata);
//            return -2;
//        }
//    }
//
//    I2C_SendAddr7bit(I2Cx, address, I2C_DIRECTION_SEND);
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_TXMODE_FLAG)) // EV6
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            timeout(I2Cx, userdata);
//            return -3;
//        }
//    }
//
//    while (send_size-- > 0)
//    {
//        I2C_SendData(I2Cx, *data++);
//        I2CTimeout = I2CT_LONG_TIMEOUT;
//        while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING)) // EV8
//        {
//            if ((I2CTimeout--) == 0)
//            {
//                timeout(I2Cx, userdata);
//                return -4;
//            }
//        }
//    }
//
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDED)) // EV8-2
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            timeout(I2Cx, userdata);
//            return -5;
//        }
//    }
//
//    I2C_GenerateStop(I2Cx, ENABLE);
//
//    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            timeout(I2Cx, userdata);
//            return -6;
//        }
//    }
//
//    return size;
//}
//
//
//int hw_i2c_master_recv(I2C_Module * I2Cx, uint8_t address, uint8_t * data, int size, hw_i2c_master_timeout timeout, void* userdata)
//{
//    int received_size = 0;
//    uint32_t I2CTimeout             = I2CT_LONG_TIMEOUT;
//    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            timeout(I2Cx, userdata);
//            return -1;
//        }
//    }
//    I2C_ConfigAck(I2Cx, ENABLE);
//
//    I2C_GenerateStart(I2Cx, ENABLE);
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_MODE_FLAG)) // EV5
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            timeout(I2Cx, userdata);
//            return -2;
//        }
//    }
//
//    // send addr
//    I2C_SendAddr7bit(I2Cx, address, I2C_DIRECTION_RECV);
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_RXMODE_FLAG)) // EV6
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            timeout(I2Cx, userdata);
//            return -3;
//        }
//    }
//
//    if (size == 1)
//    {
//        I2C_ConfigAck(I2Cx, DISABLE);
//        (void)(I2Cx->STS1); /// clear ADDR
//        (void)(I2Cx->STS2);
//
//        I2C_GenerateStop(I2Cx, ENABLE);
//        I2CTimeout = I2CT_LONG_TIMEOUT;
//        while (!I2C_GetFlag(I2Cx, I2C_FLAG_RXDATNE))
//        {
//            if ((I2CTimeout--) == 0)
//            {
//                timeout(I2Cx, userdata);
//                return -4;
//            }
//        }
//        *data++ = I2C_RecvData(I2Cx); size--; received_size++;
//    }
//    else if (size == 2)
//    {
//        I2Cx->CTRL1 |= 0x0800; /// set ACKPOS
//        (void)(I2Cx->STS1);
//        (void)(I2Cx->STS2);
//        I2C_ConfigAck(I2Cx, DISABLE);
//
//        I2CTimeout = I2CT_LONG_TIMEOUT;
//        while (!I2C_GetFlag(I2Cx, I2C_FLAG_BYTEF))
//        {
//            if ((I2CTimeout--) == 0)
//            {
//                timeout(I2Cx, userdata);
//                return -5;
//            }
//        }
//        I2C_GenerateStop(I2Cx, ENABLE);
//
//        *data++ = I2C_RecvData(I2Cx); size--; received_size++;
//        *data++ = I2C_RecvData(I2Cx); size--; received_size++;
//    }
//    else
//    {
//        I2C_ConfigAck(I2Cx, ENABLE);
//        (void)(I2Cx->STS1);
//        (void)(I2Cx->STS2);
//
//        while (size)
//        {
//            if (size == 3)
//            {
//                I2CTimeout = I2CT_LONG_TIMEOUT;
//                while (!I2C_GetFlag(I2Cx, I2C_FLAG_BYTEF))
//                {
//                    if ((I2CTimeout--) == 0)
//                    {
//                        timeout(I2Cx, userdata);
//                        return -6;
//                    }
//                }
//                I2C_ConfigAck(I2Cx, DISABLE);
//                *data++ = I2C_RecvData(I2Cx); size--; received_size++;
//
//                I2CTimeout = I2CT_LONG_TIMEOUT;
//                while (!I2C_GetFlag(I2Cx, I2C_FLAG_BYTEF))
//                {
//                    if ((I2CTimeout--) == 0)
//                    {
//                        timeout(I2Cx, userdata);
//                        return -7;
//                    }
//                }
//
//                I2C_GenerateStop(I2Cx, ENABLE);
//
//                *data++ = I2C_RecvData(I2Cx); size--; received_size++;
//                *data++ = I2C_RecvData(I2Cx); size--; received_size++;
//
//                break;
//            }
//
//            I2CTimeout = I2CT_LONG_TIMEOUT;
//            while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_RECVD_FLAG)) // EV7
//            {
//                if ((I2CTimeout--) == 0)
//                {
//                    timeout(I2Cx, userdata);
//                    return -8;
//                }
//            }
//            *data++ = I2C_RecvData(I2Cx); size--; received_size++;
//        }
//    }
//
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
//    {
//        if ((I2CTimeout--) == 0)
//        {
//            timeout(I2Cx, userdata);
//            return -9;
//        }
//    }
//
//    return received_size;
//}
//
//void hw_i2c_master_reset(I2C_Module *I2Cx, uint32_t Remap)
//{
//    uint32_t I2CTimeout=0;
//
//    GPIO_Module * SCL_GPIO=0;
//    uint16_t SCL_Pin=0;
//    GPIO_Module * SDA_GPIO=0;
//    uint16_t SDA_Pin=0;
//
//    if(I2Cx==I2C1){
//        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C1, ENABLE);
//        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C1, DISABLE);
//
//        if(Remap==0){
//            SCL_GPIO = GPIOB;
//            SCL_Pin = GPIO_PIN_6;
//            SDA_GPIO = GPIOB;
//            SDA_Pin = GPIO_PIN_6;
//        }else if(Remap==GPIO_RMP_I2C1){
//            SCL_GPIO = GPIOB;
//            SDA_GPIO = GPIOB;
//            SCL_Pin = GPIO_PIN_8;
//            SDA_Pin = GPIO_PIN_9;
//        }
//
//        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C1, ENABLE);
//    }else if(I2Cx==I2C2){
//        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, ENABLE);
//        RCC_EnableAPB1PeriphReset(RCC_APB1_PERIPH_I2C2, DISABLE);
//        if(Remap==0){
//            SCL_GPIO = GPIOB;
//            SDA_GPIO = GPIOB;
//            SCL_Pin = GPIO_PIN_10;
//            SDA_Pin = GPIO_PIN_11;
//        }else if(Remap==GPIO_RMP1_I2C2){
//            SCL_GPIO = GPIOG;
//            SDA_GPIO = GPIOG;
//            SCL_Pin = GPIO_PIN_2;
//            SDA_Pin = GPIO_PIN_3;
//        }else if(Remap==GPIO_RMP3_I2C2){
//            SCL_GPIO = GPIOA;
//            SDA_GPIO = GPIOA;
//            SCL_Pin = GPIO_PIN_4;
//            SDA_Pin = GPIO_PIN_5;
//        }
//
//        RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_I2C2, ENABLE);
//    }else if(I2Cx==I2C3){
//        RCC_EnableAPB2PeriphReset(RCC_APB2_PERIPH_I2C3, ENABLE);
//        RCC_EnableAPB2PeriphReset(RCC_APB2_PERIPH_I2C3, DISABLE);
//
//        if(Remap==0){
//            SCL_GPIO = GPIOC;
//            SDA_GPIO = GPIOC;
//            SCL_Pin = GPIO_PIN_0;
//            SDA_Pin = GPIO_PIN_1;
//        }else if(Remap==GPIO_RMP2_I2C3){
//            SCL_GPIO = GPIOF;
//            SDA_GPIO = GPIOF;
//            SCL_Pin = GPIO_PIN_4;
//            SDA_Pin = GPIO_PIN_5;
//        }else if(Remap==GPIO_RMP3_I2C3){
//            SCL_GPIO = GPIOC;
//            SDA_GPIO = GPIOC;
//            SCL_Pin = GPIO_PIN_4;
//            SDA_Pin = GPIO_PIN_5;
//        }
//
//        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_I2C3, ENABLE);
//    }else if(I2Cx==I2C4){
//        RCC_EnableAPB2PeriphReset(RCC_APB2_PERIPH_I2C4, ENABLE);
//        RCC_EnableAPB2PeriphReset(RCC_APB2_PERIPH_I2C4, DISABLE);
//
//        if(Remap==0){
//            SCL_GPIO = GPIOC;
//            SDA_GPIO = GPIOC;
//            SCL_Pin = GPIO_PIN_6;
//            SDA_Pin = GPIO_PIN_7;
//        }else if(Remap==GPIO_RMP1_I2C4){
//            SCL_GPIO = GPIOD;
//            SDA_GPIO = GPIOD;
//            SCL_Pin = GPIO_PIN_14;
//            SDA_Pin = GPIO_PIN_15;
//
//        }else if(Remap==GPIO_RMP3_I2C4){
//            SCL_GPIO = GPIOA;
//            SDA_GPIO = GPIOA;
//            SCL_Pin = GPIO_PIN_9;
//            SDA_Pin = GPIO_PIN_10;
//        }
//
//        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_I2C4, ENABLE);
//    }
//
//    hw_gpio_init(SCL_GPIO, SCL_Pin, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /*SCL*/
//    hw_gpio_init(SDA_GPIO, SDA_Pin, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /*SDA*/
//
//
//    I2CTimeout = I2CT_LONG_TIMEOUT;
//    for (;;)
//    {
//        if ((SCL_Pin | SDA_Pin) == ((SCL_GPIO->PID & SCL_Pin) | (SDA_GPIO->PID & SDA_Pin)))
//        {
//            I2Cx->CTRL1 |= 0x8000;
//            __NOP();
//            __NOP();
//            __NOP();
//            __NOP();
//            __NOP();
//            I2Cx->CTRL1 &= ~0x8000;
//            break;
//        }
//        else
//        {
//            if ((I2CTimeout--) == 0)
//            {
//                uint8_t i;
//                hw_gpio_init(SCL_GPIO, SCL_Pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz); /*SCL*/
//                hw_gpio_init(SDA_GPIO, SDA_Pin, GPIO_Mode_Out_PP, GPIO_Speed_50MHz); /*SDA*/
//
//                for (i = 0; i < 9; i++)
//                {
//                    hw_gpio_high(SCL_GPIO, SCL_Pin);
//                    Delay_us(5);
//                    hw_gpio_low(SCL_GPIO, SCL_Pin);
//                    Delay_us(5);
//                }
//            }
//        }
//    }
//}
//
