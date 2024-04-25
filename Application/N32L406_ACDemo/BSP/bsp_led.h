#ifndef INCLUDED_BSP_LED_H
#define INCLUDED_BSP_LED_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32L40X_H
#define INCLUDED_N32L40X_H
#include <n32l40x.h>
#endif /*INCLUDED_N32L40X_H*/

////////////////////////////////////////////////////////////////////////////////
////
void BSP_Led_Init(GPIO_Module* GPIOx, uint16_t Pin);

void BSP_Led_On(GPIO_Module* GPIOx, uint16_t Pin);

void BSP_Led_Off(GPIO_Module* GPIOx, uint16_t Pin);

void BSP_Led_OnOff(GPIO_Module* GPIOx, uint32_t Pin);

void BSP_Led_Blink(GPIO_Module* GPIOx, uint16_t Pin);

////////////////////////////////////////////////////////////////////////////////
////
#define BSP_Led_Red_Init() BSP_Led_Init(GPIOB, GPIO_PIN_12);
#define BSP_Led_Red_On() BSP_Led_On(GPIOB, GPIO_PIN_12);
#define BSP_Led_Red_Off() BSP_Led_Off(GPIOB, GPIO_PIN_12);
#define BSP_Led_Red_OnOff() BSP_Led_OnOff(GPIOB, GPIO_PIN_12);
#define BSP_Led_Red_Blink() BSP_Led_Blink(GPIOB, GPIO_PIN_12);


#define BSP_Led_Blue_Init() BSP_Led_Init(GPIOB, GPIO_PIN_13);
#define BSP_Led_Blue_On() BSP_Led_On(GPIOB, GPIO_PIN_13);
#define BSP_Led_Blue_Off() BSP_Led_Off(GPIOB, GPIO_PIN_13);
#define BSP_Led_Blue_OnOff() BSP_Led_OnOff(GPIOB, GPIO_PIN_13);
#define BSP_Led_Blue_Blink() BSP_Led_Blink(GPIOB, GPIO_PIN_13);



#endif /*INCLUDED_BSP_LED_H*/
