#include <ADS1256.h>
#include <os_kernel.h>
//-----------------------------------------------------------------
// 引脚声明
//-----------------------------------------------------------------
#define ADS1256_RST_L			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET)
#define ADS1256_RST_H			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET)

#define ADS1256_CS_L			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)
#define ADS1256_CS_H			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)

#define ADS1256_DRDY			HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)

////////////////////////////////////////////////////////////////////////////////
////

#define FAC_US (SystemCoreClock/1000000)	// us延时倍乘数

////////////////////////////////////////////////////////////////////////////////
////
void ADS1256__StartScan(void);
void ADS1256__StopScan(void);



//-----------------------------------------------------------------
// void delay_us(u32 nus)
//-----------------------------------------------------------------
//
// 函数功能: SysTick延时us
// 入口参数: u32 nus：要延时的us数.
// 返 回 值: 无
// 注意事项: nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)
//
//-----------------------------------------------------------------
static void delay_us(uint32_t nus)
{

    uint32_t ticks;
    uint32_t told,tnow,tcnt=0;
    uint32_t reload=SysTick->LOAD;			// LOAD的值
    ticks=nus*FAC_US; 						// 需要的节拍数
    told=SysTick->VAL;        		// 刚进入时的计数器值
    while(1)
    {
        tnow=SysTick->VAL;
        if(tnow!=told)
        {
            if(tnow<told)				// 这里注意一下SYSTICK是一个递减的计数器就可以了.
                tcnt+=told-tnow;
            else
                tcnt+=reload-tnow+told;
            told=tnow;
            if(tcnt>=ticks)
                break;						// 时间超过/等于要延迟的时间,则退出.
        }
    }
}

//-----------------------------------------------------------------
// void delay_ms(u16 nms)
//-----------------------------------------------------------------
//
// 函数功能: SysTick延时ms
// 入口参数: u32 nms：要延时的ms数
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
static void delay_ms(uint16_t nms)
{
    uint32_t i;
    for(i=0;i<nms;i++)
        delay_us(1000);
}

////////////////////////////////////////////////////////////////////////////////
////

static int32_t ADS1256__AdcNow[8];		// 8路ADC采集结果（实时）有符号数
static uint8_t ADS1256__Channel;			// 当前通道
static uint8_t ADS1256__ScanMode;			// 扫描模式，0表示单端8路， 1表示差分4路

static SPI_HandleTypeDef ADS1256__SPI_Handler;  	// SPI句柄


////////////////////////////////////////////////////////////////////////////////
////
static void ADS1256__SPI_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN SPI1_MspInit 0 */

    /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**SPI1 GPIO Configuration
    PA4     ------> SPI1_NSS
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin= GPIO_PIN_4;					// CS
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;  // 推挽输出
    GPIO_InitStruct.Pull=GPIO_PULLUP;          // 上拉
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;// 高速
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);			// 初始化

    /* -------------------------------------------------------------------------------------------------------------- */
    GPIO_InitStruct.Pin= GPIO_PIN_3;					// DRDY
    GPIO_InitStruct.Mode=GPIO_MODE_INPUT;  		// 输入
    GPIO_InitStruct.Pull=GPIO_PULLDOWN;        // 上拉
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;// 高速
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);			// 初始化

    GPIO_InitStruct.Pin= GPIO_PIN_0;					// RST
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;  // 推挽输出
    GPIO_InitStruct.Pull=GPIO_PULLUP;          // 上拉
    GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_HIGH;// 高速
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);			// 初始化

    /* -------------------------------------------------------------------------------------------------------------- */
    ADS1256__SPI_Handler.Instance = SPI1;
    ADS1256__SPI_Handler.Init.Mode = SPI_MODE_MASTER;
    ADS1256__SPI_Handler.Init.Direction = SPI_DIRECTION_2LINES;
    ADS1256__SPI_Handler.Init.DataSize = SPI_DATASIZE_8BIT;
    ADS1256__SPI_Handler.Init.CLKPolarity = SPI_POLARITY_LOW;
    ADS1256__SPI_Handler.Init.CLKPhase = SPI_PHASE_2EDGE;
    ADS1256__SPI_Handler.Init.NSS = SPI_NSS_SOFT;
    ADS1256__SPI_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    ADS1256__SPI_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;
    ADS1256__SPI_Handler.Init.TIMode = SPI_TIMODE_DISABLE;
    ADS1256__SPI_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    ADS1256__SPI_Handler.Init.CRCPolynomial = 7;

    HAL_SPI_Init(&ADS1256__SPI_Handler);
    __HAL_SPI_ENABLE(&ADS1256__SPI_Handler);                    // 使能SPI2

}

////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE uint8_t ADS1256_SPI_ReadWrite_Data(uint8_t dat)
{
    uint8_t RxDat;
    HAL_SPI_TransmitReceive(&ADS1256__SPI_Handler,&dat,&RxDat,1,1000);
    return RxDat;
}



////////////////////////////////////////////////////////////////////////////////
////
void ADS1256_SetScanMode(uint8_t ScanMode)
{
    ADS1256__ScanMode = ScanMode;
}

//-----------------------------------------------------------------
// void ADS1256_Send8Bit(uint8_t data)
//-----------------------------------------------------------------
//
// 函数功能: 向SPI总线发送8个bit数据。 不带CS控制。
// 入口参数: uint8_t data： 待发送的数据
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256_Send8Bit(uint8_t data)
{
    ADS1256_SPI_ReadWrite_Data(data);
}

//-----------------------------------------------------------------
// uint8_t ADS1256_Recive8Bit(void)
//-----------------------------------------------------------------
//
// 函数功能: 从SPI总线接收8个bit数据。 不带CS控制。
// 入口参数: 无
// 返 回 值: 读取的数据
// 注意事项: 无
//
//-----------------------------------------------------------------
uint8_t ADS1256_Recive8Bit(void)
{
    uint8_t RxDat;
    RxDat = ADS1256_SPI_ReadWrite_Data(0xFF);
    return RxDat;
}

//-----------------------------------------------------------------
// void ADS1256_WriteReg(uint8_t RegID, uint8_t RegValue)
//-----------------------------------------------------------------
//
// 函数功能: 写指定的寄存器
// 入口参数: uint8_t RegID：寄存器ID
//					 uint8_t RegValue：寄存器值
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256_WriteReg(uint8_t RegID, uint8_t RegValue)
{
    ADS1256_CS_L;												// SPI片选 = 0
    ADS1256_Send8Bit(ADS125X_CMD_WREG | RegID);	// 写寄存器的命令, 并发送寄存器地址
    ADS1256_Send8Bit(0x00);							// 寄存器个数 - 1, 此处写1个寄存器
    ADS1256_Send8Bit(RegValue);					// 发送寄存器值
    ADS1256_CS_H;												// SPI片选 = 1
}

//-----------------------------------------------------------------
// uint8_t ADS1256_ReadReg(uint8_t _RegID)
//-----------------------------------------------------------------
//
// 函数功能: 读指定的寄存器
// 入口参数: uint8_t RegID：寄存器ID
// 返 回 值: uint8_t read：寄存器值
// 注意事项: 无
//
//-----------------------------------------------------------------
uint8_t ADS1256_ReadReg(uint8_t _RegID)
{
    uint8_t RegValue;

    ADS1256_CS_L;													// SPI片选 = 0
    ADS1256_Send8Bit(ADS125X_CMD_WREG | _RegID);	// 写寄存器的命令, 并发送寄存器地址
    ADS1256_Send8Bit(0x00);								// 寄存器个数 - 1, 此处读1个寄存器

    delay_us(1);													// 必须延迟才能读取芯片返回数据

    RegValue = ADS1256_Recive8Bit();			// 读寄存器值
    ADS1256_CS_H;													// SPI片选 = 1

    return RegValue;
}

//-----------------------------------------------------------------
// void ADS1256_WriteCmd(uint8_t cmd)
//-----------------------------------------------------------------
//
// 函数功能: 发送单字节命令
// 入口参数: uint8_t cmd：命令
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256_WriteCmd(uint8_t cmd)
{
    ADS1256_CS_L;	// SPI片选 = 0
    ADS1256_Send8Bit(cmd);
    ADS1256_CS_H;	// SPI片选 = 1
}

//-----------------------------------------------------------------
// uint8_t ADS1256_ReadChipID(void)
//-----------------------------------------------------------------
//
// 函数功能: 读芯片ID, 读状态寄存器中的高4bit
// 入口参数: uint8_t cmd：命令
// 返 回 值: 8bit状态寄存器值的高4位
// 注意事项: 无
//
//-----------------------------------------------------------------
uint8_t ADS1256_ReadChipID(void)
{
    uint8_t id;

    ADS1256_WaitDRDY();
    id = ADS1256_ReadReg(ADS125X_REG_STATUS);
    return (id >> 4);
}

//-----------------------------------------------------------------
// void ADS1256_WaitDRDY(void)
//-----------------------------------------------------------------
//
// 函数功能: 等待内部操作完成。 自校准时间较长，需要等待。
// 入口参数: 无
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256_WaitDRDY(void)
{
    uint32_t i;

    for (i = 0; i < 40000000; i++)
    {
        if(ADS1256_DRDY == GPIO_PIN_RESET)
        {
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void ADS1256_Init(void)
{
    uint8_t i;

    ADS1256__SPI_Init();

    ADS1256__Channel = 0;
    for (i = 0; i < 8; i++)
    {
        ADS1256__AdcNow[i] = 0;
    }

    os_kernel_register_service(10, ADS1256__StartScan);
    os_kernel_register_service(11, ADS1256__StopScan);
}


//-----------------------------------------------------------------
// void ADS1256_SetChannal(uint8_t ch)
//-----------------------------------------------------------------
//
// 函数功能: 配置通道号。多路复用。AIN- 固定接地（ACOM).
// 入口参数: uint8_t ch：通道号， 0-7
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256_SetChannal(uint8_t ch)
{
    /*
    位7-4 PSEL3, PSEL2, PSEL1, PSEL0: 正向输入通道 (AINP) 选择
        0000 = AIN0 (default)
        0001 = AIN1
        0010 = AIN2 (ADS1256 only)
        0011 = AIN3 (ADS1256 only)
        0100 = AIN4 (ADS1256 only)
        0101 = AIN5 (ADS1256 only)
        0110 = AIN6 (ADS1256 only)
        0111 = AIN7 (ADS1256 only)
        1xxx = AINCOM (when PSEL3 = 1, PSEL2, PSEL1, PSEL0 are “don’t care”)

    位 3-0 NSEL3, NSEL2, NSEL1, NSEL0:反向输入通道 (AINN)选择
        0000 = AIN0
        0001 = AIN1 (default)
        0010 = AIN2 (ADS1256 only)
        0011 = AIN3 (ADS1256 only)
        0100 = AIN4 (ADS1256 only)
        0101 = AIN5 (ADS1256 only)
        0110 = AIN6 (ADS1256 only)
        0111 = AIN7 (ADS1256 only)
        1xxx = AINCOM (when NSEL3 = 1, NSEL2, NSEL1, NSEL0 are “don’t care”)
    */
    if (ch > 7)
    {
        return;
    }
    ADS1256_WriteReg(ADS125X_REG_MUX, (ch << 4) | (1 << 3));	// Bit3 = 1, AINN 固定接 AINCOM
}

//-----------------------------------------------------------------
// void ADS1256_SetDiffChannal(uint8_t ch)
//-----------------------------------------------------------------
//
// 函数功能: 配置差分通道号。多路复用。
// 入口参数: uint8_t ch：通道号,0-3；共4对
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256_SetDiffChannal(uint8_t ch)
{
    /*
    位7-4 PSEL3, PSEL2, PSEL1, PSEL0: 正向输入通道 (AINP) 选择
        0000 = AIN0 (default)
        0001 = AIN1
        0010 = AIN2 (ADS1256 only)
        0011 = AIN3 (ADS1256 only)
        0100 = AIN4 (ADS1256 only)
        0101 = AIN5 (ADS1256 only)
        0110 = AIN6 (ADS1256 only)
        0111 = AIN7 (ADS1256 only)
        1xxx = AINCOM (when PSEL3 = 1, PSEL2, PSEL1, PSEL0 are “don’t care”)

    位 3-0 NSEL3, NSEL2, NSEL1, NSEL0:反向输入通道 (AINN)选择
        0000 = AIN0
        0001 = AIN1 (default)
        0010 = AIN2 (ADS1256 only)
        0011 = AIN3 (ADS1256 only)
        0100 = AIN4 (ADS1256 only)
        0101 = AIN5 (ADS1256 only)
        0110 = AIN6 (ADS1256 only)
        0111 = AIN7 (ADS1256 only)
        1xxx = AINCOM (when NSEL3 = 1, NSEL2, NSEL1, NSEL0 are “don’t care”)
    */
    if (ch == 0)
    {
        ADS1256_WriteReg(ADS125X_REG_MUX, (0 << 4) | 1);	/* 差分输入 AIN0， AIN1 */
    }
    else if (ch == 1)
    {
        ADS1256_WriteReg(ADS125X_REG_MUX, (2 << 4) | 3);	/* 差分输入 AIN2， AIN3 */
    }
    else if (ch == 2)
    {
        ADS1256_WriteReg(ADS125X_REG_MUX, (4 << 4) | 5);	/* 差分输入 AIN4， AIN5 */
    }
    else if (ch == 3)
    {
        ADS1256_WriteReg(ADS125X_REG_MUX, (6 << 4) | 7);	/* 差分输入 AIN6， AIN7 */
    }
}

//-----------------------------------------------------------------
// int32_t ADS1256_ReadData(void)
//-----------------------------------------------------------------
//
// 函数功能: 读ADC数据
// 入口参数: 无
// 返 回 值: 采样结果
// 注意事项: 无
//
//-----------------------------------------------------------------
int32_t ADS1256_ReadData(void)
{
    os_scheduler_disable();

    uint32_t read = 0;

    ADS1256_CS_L;									// SPI片选 = 0
    ADS1256_Send8Bit(ADS125X_CMD_RDATA);	// 读数据的命令
    delay_us(1);									// 必须延迟才能读取芯片返回数据
    // 读采样结果，3个字节，高字节在前
    read = ADS1256_Recive8Bit() << 16;
    read += (ADS1256_Recive8Bit() << 8);
    read += ADS1256_Recive8Bit();
    ADS1256_CS_H;									// SPI片选 = 1
    // 负数进行扩展。24位有符号数扩展为32位有符号数
    if (read & 0x800000)
    {
        /*
        read -= 0x800000;
        read += 0x80000000;
        */
        read +=0xFF000000;
    }

    os_scheduler_enable();

    return (int32_t)read;
}

//-----------------------------------------------------------------
// int32_t ADS1256_ReadAdc(uint8_t ch)
//-----------------------------------------------------------------
//
// 函数功能: 读指定通道的ADC数据
// 入口参数: uint8_t ch：通道号， 0-7
// 返 回 值: 采样结果
// 注意事项: 无
//
//-----------------------------------------------------------------
int32_t ADS1256_ReadAdc(uint8_t ch)
{
    // ADS1256 数据手册第21页
    int32_t read;

    while (ADS1256_DRDY == GPIO_PIN_RESET);			// 等待 DRDY 高
    while (ADS1256_DRDY == GPIO_PIN_SET);				// 等待 DRDY 低

    ADS1256_SetChannal(ch);				// 切换模拟通道
    delay_us(5);

    ADS1256_WriteCmd(ADS125X_CMD_SYNC);		// 同步A/D转换
    delay_us(5);

    ADS1256_WriteCmd(ADS125X_CMD_WAKEUP); // 正常情况下，这个时候 DRDY 已经为高
    delay_us(25);

    read =  (int32_t)ADS1256_ReadData();

    while (ADS1256_DRDY == GPIO_PIN_RESET);	// 等待 DRDY 高
    while (ADS1256_DRDY == GPIO_PIN_SET);		// 等待 DRDY 低
    read =  (int32_t)ADS1256_ReadData();
    return read;
}

//-----------------------------------------------------------------
// void ADS1256_StartScan(void)
//-----------------------------------------------------------------
//
// 函数功能: 将 DRDY引脚 （PA3）配置成外部中断触发方式， 中断服务程序中扫描8个通道的数据。
// 入口参数: ucDiffMode : 0 表示单端模式（扫描8路）； 1表示差分模式，扫描4路
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256__StartScan(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();   // 开启GPIOB时钟

    GPIO_Initure.Pin  =	GPIO_PIN_3;
    GPIO_Initure.Mode = GPIO_MODE_IT_FALLING;   // 下升沿触发
    GPIO_Initure.Pull = GPIO_PULLUP;						// 上拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    // 中断线11-PA3
    HAL_NVIC_SetPriority(EXTI3_IRQn,0,2);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);         // 使能中断线
}


//-----------------------------------------------------------------
// void ADS1256_StopScan(void)
//-----------------------------------------------------------------
//
// 函数功能: 停止 DRDY 中断
// 入口参数: 无
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256__StopScan(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();       		// 使能GPIOB时钟

    HAL_NVIC_DisableIRQ(EXTI3_IRQn);    // 使能中断线11

    GPIO_Initure.Pin= GPIO_PIN_3;					// DRDY
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  		// 输入
    GPIO_Initure.Pull=GPIO_PULLUP;          // 上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;// 高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);			// 初始化
}

//-----------------------------------------------------------------
// int32_t ADS1256_GetAdc(uint8_t ch)
//-----------------------------------------------------------------
//
// 函数功能: 从缓冲区读取ADC采样结果。采样结构是由中断服务程序填充的。
// 入口参数: uint8_t ch：通道号 (0 - 7)
// 返 回 值: ADC采集结果（有符号数）
// 注意事项: 无
//
//-----------------------------------------------------------------
int32_t ADS1256_GetAdc(uint8_t ch)
{
    int32_t iTemp;

    if (ch > 7)
    {
        return 0;
    }

    ADS1256_StopScan();		// 禁止中断

    iTemp = ADS1256__AdcNow[ch];

    ADS1256_StartScan();	// 使能中断

    return iTemp;
}


void ADS1256_ISR(void)
{
    os_scheduler_disable();

    if (ADS1256__ScanMode == 0)	// 0 表示单端8路扫描，1表示差分4路扫描
    {
        // 读取采集结构，保存在全局变量
        ADS1256_SetChannal(ADS1256__Channel);			// 切换模拟通道
//        ADS1256_Delay(250);

        ADS1256_WriteCmd(ADS125X_CMD_SYNC);
//        ADS1256_Delay(250);

        ADS1256_WriteCmd(ADS125X_CMD_WAKEUP);
//        ADS1256_Delay(250);

        if (ADS1256__Channel == 0)
        {
            ADS1256__AdcNow[7] = ADS1256_ReadData();	// 注意保存的是上一个通道的数据
        }
        else
        {
            ADS1256__AdcNow[ADS1256__Channel-1] = ADS1256_ReadData();	// 注意保存的是上一个通道的数据
        }

        if (++ADS1256__Channel >= 8)
        {
            ADS1256__Channel = 0;
        }
    }
    else	// 差分4路扫描
    {
        // 读取采集结构，保存在全局变量
        ADS1256_SetDiffChannal(ADS1256__Channel);	// 切换模拟通道
        delay_us(5);

        ADS1256_WriteCmd(ADS125X_CMD_SYNC);
        delay_us(5);

        ADS1256_WriteCmd(ADS125X_CMD_WAKEUP);
        delay_us(25);

        if (ADS1256__Channel == 0)
        {
            ADS1256__AdcNow[3] = ADS1256_ReadData();	// 注意保存的是上一个通道的数据
        }
        else
        {
            ADS1256__AdcNow[ADS1256__Channel-1] = ADS1256_ReadData();	// 注意保存的是上一个通道的数据
        }

        if (++ADS1256__Channel >= 4)
        {
            ADS1256__Channel = 0;
        }
    }

    os_scheduler_enable();
}

//-----------------------------------------------------------------
// int32_t ADS1256_GetAdc(uint8_t ch)
//-----------------------------------------------------------------
//
// 函数功能: 配置ADC参数，增益和数据输出速率
// 入口参数: uint8_t gain：增益
//					 uint8_t drate：数据输出速率
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256_CfgADC(uint8_t gain, uint8_t drate)
{
    uint8_t buf[4];		// 暂存ADS1256 寄存器配置参数，之后连续写4个寄存器

    ADS1256_StopScan();
    ADS1256_ResetHard();
//	ADS1256_WriteCmd(CMD_RESET);
    ADS1256_WriteReg(ADS125X_REG_STATUS,0xf4);
    ADS1256_WriteCmd(ADS125X_CMD_SELFCAL);
    ADS1256_WaitDRDY();

    /* 状态寄存器定义
        Bits 7-4 ID3, ID2, ID1, ID0  Factory Programmed Identification Bits (Read Only)

        Bit 3 ORDER: Data Output Bit Order
            0 = Most Significant Bit First (default)
            1 = Least Significant Bit First
        Input data  is always shifted in most significant byte and bit first. Output data is always shifted out most significant
        byte first. The ORDER bit only controls the bit order of the output data within the byte.

        Bit 2 ACAL : Auto-Calibration
            0 = Auto-Calibration Disabled (default)
            1 = Auto-Calibration Enabled
        When Auto-Calibration is enabled, self-calibration begins at the completion of the WREG command that changes
        the PGA (bits 0-2 of ADCON register), DR (bits 7-0 in the DRATE register) or BUFEN (bit 1 in the STATUS register)
        values.

        Bit 1 BUFEN: Analog Input Buffer Enable
            0 = Buffer Disabled (default)
            1 = Buffer Enabled

        Bit 0 DRDY :  Data Ready (Read Only)
            This bit duplicates the state of the DRDY pin.

        ACAL=1使能自校准功能。当 PGA，BUFEEN, DRATE改变时会启动自校准
    */
    buf[0] = (0 << 3) | (1 << 2) | (1 << 1);
    // ADS1256_WriteReg(REG_STATUS, (0 << 3) | (1 << 2) | (1 << 1));

    buf[1] = 0x08;	// 高四位0表示AINP接 AIN0,  低四位8表示 AINN 固定接 AINCOM

    /*	ADCON: A/D Control Register (Address 02h)
        Bit 7 Reserved, always 0 (Read Only)
        Bits 6-5 CLK1, CLK0 : D0/CLKOUT Clock Out Rate Setting
            00 = Clock Out OFF
            01 = Clock Out Frequency = fCLKIN (default)
            10 = Clock Out Frequency = fCLKIN/2
            11 = Clock Out Frequency = fCLKIN/4
            When not using CLKOUT, it is recommended that it be turned off. These bits can only be reset using the RESET pin.

        Bits 4-2 SDCS1, SCDS0: Sensor Detect Current Sources
            00 = Sensor Detect OFF (default)
            01 = Sensor Detect Current = 0.5 μ A
            10 = Sensor Detect Current = 2 μ A
            11 = Sensor Detect Current = 10μ A
            The Sensor Detect Current Sources can be activated to verify  the integrity of an external sensor supplying a signal to the
            ADS1255/6. A shorted sensor produces a very small signal while an open-circuit sensor produces a very large signal.

        Bits 2-0 PGA2, PGA1, PGA0: Programmable Gain Amplifier Setting
            000 = 1 (default)
            001 = 2
            010 = 4
            011 = 8
            100 = 16
            101 = 32
            110 = 64
            111 = 64
    */
    buf[2] = (0 << 5) | (0 << 2) | (gain << 1);
    // ADS1256_WriteReg(REG_ADCON, (0 << 5) | (0 << 2) | (GAIN_1 << 0));	/* 选择1;1增益, 输入正负5V

    // 因为切换通道和读数据耗时 123uS, 因此扫描中断模式工作时，最大速率 = DRATE_1000SPS
    buf[3] = drate;	// DRATE_10SPS;	/* 选择数据输出速率

    ADS1256_CS_L;										// SPI片选 = 0
    ADS1256_Send8Bit(ADS125X_CMD_WREG | 0);	// 写寄存器的命令, 并发送寄存器地址
    ADS1256_Send8Bit(0x03);					// 寄存器个数 - 1, 此处3表示写4个寄存器
    ADS1256_Send8Bit(buf[0]);				// 设置状态寄存器
    ADS1256_Send8Bit(buf[1]);				// 设置输入通道参数
    ADS1256_Send8Bit(buf[2]);				// 设置ADCON控制寄存器，增益
    ADS1256_Send8Bit(buf[3]);				// 设置输出数据速率
    ADS1256_CS_H;										// SPI片选 = 1

    delay_us(50);
}

//-----------------------------------------------------------------
// int32_t ADS1256_GetAdc(uint8_t ch)
//-----------------------------------------------------------------
//
// 函数功能: 硬件复位 ADS1256芯片.低电平复位。最快4个时钟，也就是 4x0.13uS = 0.52uS
// 入口参数: 无
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256_ResetHard(void)
{
    ADS1256_RST_L;
    delay_us(5);
    ADS1256_RST_H;
    delay_us(5);
}

//-----------------------------------------------------------------
// void ADS1256_Delay(uint16_t t)
//-----------------------------------------------------------------
//
// 函数功能: ADS1256延时
// 入口参数: uint16_t t：延时数
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void ADS1256_Delay(uint16_t t)
{
    do {
        ;
    } while (--t);
}

////////////////////////////////////////////////////////////////////////////////
////


//-----------------------------------------------------------------
// void EXTI3_IRQHandler(void)
//-----------------------------------------------------------------
//
// 函数功能: 中断线15-10中断服务函数，调用中断处理公用函数
// 入口参数: 无
// 返回参数: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);// 调用中断处理公用函数
}

//-----------------------------------------------------------------
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//-----------------------------------------------------------------
//
// 函数功能: 中断服务程序中需要做的事情
// 入口参数: uint16_t GPIO_Pin：中断引脚
// 返回参数: 无
// 注意事项: 在HAL库中所有的外部中断服务函数都会调用此函数
//
//-----------------------------------------------------------------
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_3)
        ADS1256_ISR();
}


