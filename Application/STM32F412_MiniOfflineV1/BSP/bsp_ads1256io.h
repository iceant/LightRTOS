#ifndef INCLUDED_BSP_ADS1256IO_H
#define INCLUDED_BSP_ADS1256IO_H


////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32F4XX_H
#define INCLUDED_STM32F4XX_H
#include <stm32f4xx.h>
#endif /*INCLUDED_STM32F4XX_H*/

////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE void BSP_ADS1256IO_RST_Low(void){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
}

__STATIC_FORCEINLINE void BSP_ADS1256IO_RST_High(void){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
}

__STATIC_FORCEINLINE int BSP_ADS1256IO_DRDY_Read(void){
    return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3);
}

__STATIC_FORCEINLINE void BSP_ADS1256IO_CS_Low(void){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
}

__STATIC_FORCEINLINE void BSP_ADS1256IO_CS_High(void){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
}

__STATIC_FORCEINLINE void BSP_ADS1256IO_SCLK_Low(void){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
}

__STATIC_FORCEINLINE void BSP_ADS1256IO_SCLK_High(void){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
}

__STATIC_FORCEINLINE int BSP_ADS1256IO_DOUT_Read(void){
    return HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2);
}


__STATIC_FORCEINLINE void BSP_ADS1256IO_DIN_Low(void){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
}

__STATIC_FORCEINLINE void BSP_ADS1256IO_DIN_High(void){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
}

////////////////////////////////////////////////////////////////////////////////
////
typedef void (*BSP_ADS1256IO_ScanFunction)(void);

////////////////////////////////////////////////////////////////////////////////
////
void BSP_ADS1256IO_Init(void);

void BSP_ADS1256IO_ResetHard(void);

void BSP_ADS1256IO_Send8Bit(uint8_t data);

uint8_t BSP_ADS1256IO_Recive8Bit(void);

void BSP_ADS1256IO_StartScan(void);

void BSP_ADS1256IO_StopScan(void);

void BSP_ADS1256IO_SetScanFunction(BSP_ADS1256IO_ScanFunction fn);

#endif /*INCLUDED_BSP_ADS1256IO_H*/
