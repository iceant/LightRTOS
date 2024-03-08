#ifndef INCLUDED_HW_GPIO_H
#define INCLUDED_HW_GPIO_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32G45X_H
#define INCLUDED_N32G45X_H
#include <n32g45x.h>
#endif /*INCLUDED_N32G45X_H*/

////////////////////////////////////////////////////////////////////////////////
////

void hw_gpio_init(GPIO_Module* GPIOx, uint16_t Pin, GPIO_ModeType ModeType, GPIO_SpeedType SpeedType);

void hw_gpio_high(GPIO_Module* GPIOx, uint16_t Pin);

void hw_gpio_low(GPIO_Module* GPIOx, uint16_t Pin);

void hw_gpio_OnOff(GPIO_Module* GPIOx, uint32_t Pin);

void hw_gpio_toggle(GPIO_Module* GPIOx, uint16_t Pin);

#endif /*INCLUDED_HW_GPIO_H*/
