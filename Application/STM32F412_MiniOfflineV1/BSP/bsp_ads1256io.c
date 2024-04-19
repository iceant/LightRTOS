#include <bsp_ads1256io.h>
#include <bsp_timerdelay.h>

////////////////////////////////////////////////////////////////////////////////
////
__STATIC_FORCEINLINE void delay_us(uint16_t us){
    BSP_TimerDelay_us(us);
}

__STATIC_FORCEINLINE void ADS1256IO_Delay(uint32_t t){
    do {
        ;
    } while (--t);
}
////////////////////////////////////////////////////////////////////////////////
////


void BSP_ADS1256IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_5 /* RST */ | GPIO_PIN_4 /*CS*/ | GPIO_PIN_0 /*SCLK*/ | GPIO_PIN_1 /*DIN*/;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pull = GPIO_PIN_2; /*DOUT*/
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pull = GPIO_PIN_3; /*DRDY*/
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    BSP_ADS1256IO_RST_Low();
    BSP_TimerDelay_ms(1);
    BSP_ADS1256IO_RST_High();
    BSP_TimerDelay_ms(100);

    BSP_ADS1256IO_CS_High();
    BSP_ADS1256IO_SCLK_Low();
    BSP_ADS1256IO_DIN_High();

}

void BSP_ADS1256IO_ResetHard(void)
{
    BSP_ADS1256IO_RST_Low();
    delay_us(5);
    BSP_ADS1256IO_RST_High();
    delay_us(5);
}

void BSP_ADS1256IO_Send8Bit(uint8_t data)
{
    uint8_t i;

    // 连续发送多个字节时，需要延迟一下
    ADS1256IO_Delay(250);
    ADS1256IO_Delay(250);
    //　ADS1256 要求 SCL高电平和低电平持续时间最小 200ns
    for(i = 0; i < 8; i++)
    {
        if (data & 0x80)
        {
            BSP_ADS1256IO_DIN_High();
        }
        else
        {
            BSP_ADS1256IO_DIN_Low();
        }
        BSP_ADS1256IO_SCLK_High();
        ADS1256IO_Delay(250);
        data <<= 1;
        BSP_ADS1256IO_SCLK_Low();			// ADS1256 是在SCK下降沿采样DIN数据, 数据必须维持 50nS
        ADS1256IO_Delay(250);
    }
}

uint8_t BSP_ADS1256IO_Recive8Bit(void)
{
    uint8_t i;
    uint8_t read = 0;

    ADS1256IO_Delay(250);
    ADS1256IO_Delay(250);
    //　ADS1256 要求 SCL高电平和低电平持续时间最小 200ns
    for (i = 0; i < 8; i++)
    {
        BSP_ADS1256IO_SCLK_High();
        ADS1256IO_Delay(250);
        read = read<<1;
        BSP_ADS1256IO_SCLK_Low();
        if (BSP_ADS1256IO_DOUT_Read()  == GPIO_PIN_SET)
        {
            read++;
        }
        ADS1256IO_Delay(250);
    }
    return read;
}

//-----------------------------------------------------------------
// void BSP_ADS1256IO_StartScan(void)
//-----------------------------------------------------------------
//
// 函数功能: 将 DRDY引脚 （PA3）配置成外部中断触发方式， 中断服务程序中扫描8个通道的数据。
// 入口参数: ucDiffMode : 0 表示单端模式（扫描8路）； 1表示差分模式，扫描4路
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void BSP_ADS1256IO_StartScan(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();   // 开启GPIOA时钟

    GPIO_Initure.Pin  =	GPIO_PIN_3;
    GPIO_Initure.Mode = GPIO_MODE_IT_FALLING;               // 下升沿触发
    GPIO_Initure.Pull = GPIO_PULLUP;						// 上拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    // 中断线3-PA3
    HAL_NVIC_SetPriority(EXTI3_IRQn,0,1);   // 抢占优先级为2，子优先级为1
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);         // 使能中断线3
}



//-----------------------------------------------------------------
// void BSP_ADS1256IO_StopScan(void)
//-----------------------------------------------------------------
//
// 函数功能: 停止 DRDY 中断
// 入口参数: 无
// 返 回 值: 无
// 注意事项: 无
//
//-----------------------------------------------------------------
void BSP_ADS1256IO_StopScan(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();       		// 使能GPIOA时钟

    HAL_NVIC_DisableIRQ(EXTI3_IRQn);    // 使能中断线11

    GPIO_Initure.Pin= GPIO_PIN_3;					    // DRDY
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  		        // 推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                      // 上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;            // 高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);	// 初始化
}


////////////////////////////////////////////////////////////////////////////////
////

static BSP_ADS1256IO_ScanFunction BSP_ADS1256__ScanFunction=0;
void BSP_ADS1256IO_SetScanFunction(BSP_ADS1256IO_ScanFunction fn)
{
    BSP_ADS1256__ScanFunction = fn;
}


////////////////////////////////////////////////////////////////////////////////
////
//void EXTI3_IRQHandler(void){
//    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);// 调用中断处理公用函数
//}

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
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//    if(GPIO_Pin == GPIO_PIN_3)
//    {
//        if(BSP_ADS1256__ScanFunction){
//            BSP_ADS1256__ScanFunction();
//        }
//    }
//}



