#include <DS1302.h>
#include <n32l40x.h>
#include <hw_gpio.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////
#define RST_GPIO            GPIOA
#define RST_GPIO_PIN        GPIO_PIN_4
#define CLK_GPIO            GPIOA
#define CLK_GPIO_PIN        GPIO_PIN_5
#define DATA_GPIO           GPIOA
#define DATA_GPIO_PIN       GPIO_PIN_6

////////////////////////////////////////////////////////////////////////////////
////

#define RST_L() GPIO_ResetBits(RST_GPIO, RST_GPIO_PIN)
#define RST_H() GPIO_SetBits(RST_GPIO, RST_GPIO_PIN)

#define CLK_L() GPIO_ResetBits(CLK_GPIO, CLK_GPIO_PIN)
#define CLK_H() GPIO_SetBits(CLK_GPIO, CLK_GPIO_PIN)

#define DATA_L() GPIO_ResetBits(DATA_GPIO, DATA_GPIO_PIN)
#define DATA_H() GPIO_SetBits(DATA_GPIO, DATA_GPIO_PIN)
#define DATA_IS_HIGH() (GPIO_ReadInputDataBit(DATA_GPIO, DATA_GPIO_PIN)==Bit_SET)
////////////////////////////////////////////////////////////////////////////////
////
static void DS1302_GPIO_Init(void)
{
//    GPIO_InitStruct->Pin        = GPIO_PIN_ALL;
//    GPIO_InitStruct->GPIO_Slew_Rate = GPIO_Slew_Rate_High;
//    GPIO_InitStruct->GPIO_Mode  = GPIO_Mode_Input;
//    GPIO_InitStruct->GPIO_Alternate = GPIO_NO_AF;
//    GPIO_InitStruct->GPIO_Pull = GPIO_No_Pull;
//    GPIO_InitStruct->GPIO_Current = GPIO_DC_2mA;
    hw_gpio_init(RST_GPIO, RST_GPIO_PIN, GPIO_Mode_Out_PP, GPIO_No_Pull, GPIO_DC_2mA, GPIO_Slew_Rate_High, GPIO_NO_AF);
    RST_L();
    hw_gpio_init(CLK_GPIO, CLK_GPIO_PIN, GPIO_Mode_Out_PP, GPIO_No_Pull, GPIO_DC_2mA, GPIO_Slew_Rate_High, GPIO_NO_AF);
    CLK_L();
}

static void DS1302_DataOut_Init(void){
    hw_gpio_init(DATA_GPIO, DATA_GPIO_PIN, GPIO_Mode_Out_PP, GPIO_No_Pull, GPIO_DC_2mA, GPIO_Slew_Rate_Low, GPIO_NO_AF);
    DATA_L();
}
static void DS1302_DataInput_Init(void){
    hw_gpio_init(DATA_GPIO, DATA_GPIO_PIN, GPIO_Mode_Input, GPIO_Pull_Up, GPIO_DC_2mA, GPIO_Slew_Rate_High, GPIO_NO_AF);
}

static void DS1302_WriteByte(uint8_t data){
    DS1302_DataOut_Init();
    uint8_t i;
    CLK_L();
    for(i = 0; i < 8; i++){
        CLK_L();
        if(data & 0x01){
            DATA_H();
        }else{
            DATA_L();
        }
        data>>=1;
        CLK_H();
    }
}

static void Delay_Us(uint32_t us){
    for(; us>0; us--){
        for(int i=68; i>0; i--);
    }
}

/**
 * @brief Decodes the raw binary value stored in registers to decimal format.
 * @param bin Binary-coded decimal value retrieved from register, 0 to 255.
 * @return Decoded decimal value.
 */
static uint8_t DS1302_DecodeBCD(uint8_t bin) {
    return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}

/**
 * @brief Encodes a decimal number to binaty-coded decimal for storage in registers.
 * @param dec Decimal number to encode.
 * @return Encoded binary-coded decimal value.
 */
static uint8_t DS1302_EncodeBCD(uint8_t dec) {
    return (dec % 10 + ((dec / 10) << 4));
}

////////////////////////////////////////////////////////////////////////////////
////


void DS1302_Init(void)
{
    DS1302_GPIO_Init();
}

void DS1302_Set(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t dayOfWeek)
{
    DS1302_SetRegByte(0x8E, 0x00); /* 关闭写保护 */
    DS1302_SetRegByte(0x80, DS1302_EncodeBCD(sec));  /* 秒 */
    DS1302_SetRegByte(0x82, DS1302_EncodeBCD(minute));  /* 分 */
    DS1302_SetRegByte(0x84, DS1302_EncodeBCD(hour));  /* 时 */
    DS1302_SetRegByte(0x86, DS1302_EncodeBCD(date));  /* 日 */
    DS1302_SetRegByte(0x88, DS1302_EncodeBCD(month));  /* 月 */
    DS1302_SetRegByte(0x8A, DS1302_EncodeBCD(dayOfWeek));  /* 周日 */
    DS1302_SetRegByte(0x8C, DS1302_EncodeBCD(year-1970));  /* 1970 + 年 */
    DS1302_SetRegByte(0x8E, 0x80); /* 开启写保护 */
    
    printf("Set Second: %02X\n", DS1302_EncodeBCD(sec));
    printf("Set Minute: %02X\n", DS1302_EncodeBCD(minute));
    printf("Set Hour: %02X\n", DS1302_EncodeBCD(hour));
    printf("Set Date: %02X\n", DS1302_EncodeBCD(date));
    printf("Set Month: %02X\n", DS1302_EncodeBCD(month));
    printf("Set Day: %02X\n", DS1302_EncodeBCD(dayOfWeek));
    printf("Set Year: %02X\n", DS1302_EncodeBCD(year-2000));
}

void DS1302_SetSecond(uint8_t second){
    DS1302_SetRegByte(0x8E, 0x00); /* 关闭写保护 */
    DS1302_SetRegByte(0x80, DS1302_EncodeBCD(second));  /* 秒 */
    DS1302_SetRegByte(0x8E, 0x80); /* 开启写保护 */
}

void DS1302_SetMinute(uint8_t minute){
    DS1302_SetRegByte(0x8E, 0x00); /* 关闭写保护 */
    DS1302_SetRegByte(0x82, DS1302_EncodeBCD(minute));  /* 分 */
    DS1302_SetRegByte(0x8E, 0x80); /* 开启写保护 */
}

void DS1302_SetHour(uint8_t hour){
    DS1302_SetRegByte(0x8E, 0x00); /* 关闭写保护 */
    DS1302_SetRegByte(0x84, DS1302_EncodeBCD(hour));  /* 时 */
    DS1302_SetRegByte(0x8E, 0x80); /* 开启写保护 */
}

void DS1302_SetDate(uint8_t date){
    DS1302_SetRegByte(0x8E, 0x00); /* 关闭写保护 */
    DS1302_SetRegByte(0x86, DS1302_EncodeBCD(date));  /* 日期 */
    DS1302_SetRegByte(0x8E, 0x80); /* 开启写保护 */
}

void DS1302_SetMonth(uint8_t month){
    DS1302_SetRegByte(0x8E, 0x00); /* 关闭写保护 */
    DS1302_SetRegByte(0x88, DS1302_EncodeBCD(month));  /* 月 */
    DS1302_SetRegByte(0x8E, 0x80); /* 开启写保护 */
}

void DS1302_SetDayOfWeek(uint8_t dayOfWeek /* 1- 7 */){
    DS1302_SetRegByte(0x8E, 0x00); /* 关闭写保护 */
    DS1302_SetRegByte(0x8A, DS1302_EncodeBCD(dayOfWeek));  /* 月 */
    DS1302_SetRegByte(0x8E, 0x80); /* 开启写保护 */
}

void DS1302_SetYear(uint16_t year){
    DS1302_SetRegByte(0x8E, 0x00); /* 关闭写保护 */
    DS1302_SetRegByte(0x8C, DS1302_EncodeBCD(year-1970));  /* 月 */
    DS1302_SetRegByte(0x8E, 0x80); /* 开启写保护 */
}


uint16_t DS1302_GetYear(void){
    return DS1302_DecodeBCD(DS1302_GetRegByte(0x8D)) + 1970;
}

uint8_t DS1302_GetDayOfWeek(void)
{
    return DS1302_DecodeBCD(DS1302_GetRegByte(0x8B));
}

uint8_t DS1302_GetMonth(void){
    return DS1302_DecodeBCD(DS1302_GetRegByte(0x89));
}

uint8_t DS1302_GetDate(void){
    return DS1302_DecodeBCD(DS1302_GetRegByte(0x87));
}

uint8_t DS1302_GetHour(void){
    return DS1302_DecodeBCD(DS1302_GetRegByte(0x85));
}

uint8_t DS1302_GetMinute(void)
{
    return DS1302_DecodeBCD(DS1302_GetRegByte(0x83));
}

uint8_t DS1302_GetSecond(void)
{
    return DS1302_DecodeBCD(DS1302_GetRegByte(0x81));
}


void DS1302_SetRegByte(uint8_t address, uint8_t data){
    RST_L();CLK_L();Delay_Us(1);
    RST_H();Delay_Us(2);
    DS1302_WriteByte(address);
    DS1302_WriteByte(data);
    RST_L();CLK_L();Delay_Us(2);
//    RST_H();
}

uint8_t DS1302_GetRegByte(uint8_t address){
    uint8_t i;
    uint8_t return_data = 0x00;
    
    RST_L();CLK_L();Delay_Us(3);
    RST_H();Delay_Us(3);
    
    DS1302_WriteByte(address);
    DS1302_DataInput_Init();
    Delay_Us(2);
    for(i = 0; i<8; i++){
        Delay_Us(2);
        return_data>>=0x01;
        CLK_H();Delay_Us(3);
        CLK_L();Delay_Us(14);
        if(DATA_IS_HIGH()){
            return_data = return_data|0x80; /* 1000 0000 */
        }
    }
    Delay_Us(2);
    RST_L();DATA_L();

    return (return_data >> 1);
}
