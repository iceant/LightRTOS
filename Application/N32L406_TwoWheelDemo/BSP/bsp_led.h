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
#define BSP_Led_Work_Init() BSP_Led_Init(GPIOC, GPIO_PIN_15);
#define BSP_Led_Work_On() BSP_Led_On(GPIOC, GPIO_PIN_15);
#define BSP_Led_Work_Off() BSP_Led_Off(GPIOC, GPIO_PIN_15);
#define BSP_Led_Work_OnOff() BSP_Led_OnOff(GPIOC, GPIO_PIN_15);
#define BSP_Led_Work_Blink() BSP_Led_Blink(GPIOC, GPIO_PIN_15);


#define BSP_Led_Network_Init() BSP_Led_Init(GPIOC, GPIO_PIN_14);
#define BSP_Led_Network_On() BSP_Led_On(GPIOC, GPIO_PIN_14);
#define BSP_Led_Network_Off() BSP_Led_Off(GPIOC, GPIO_PIN_14);
#define BSP_Led_Network_OnOff() BSP_Led_OnOff(GPIOC, GPIO_PIN_14);
#define BSP_Led_Network_Blink() BSP_Led_Blink(GPIOC, GPIO_PIN_14);



#endif /*INCLUDED_BSP_LED_H*/
