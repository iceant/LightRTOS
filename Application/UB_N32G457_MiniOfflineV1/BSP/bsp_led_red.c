#include <bsp_led_red.h>

////////////////////////////////////////////////////////////////////////////////
////

void BSP_LED_Red_Init(void){
//    hw_gpio_init(GPIOD, GPIO_PIN_14, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    hw_gpio_init(GPIOD, GPIO_PIN_15, GPIO_Mode_Out_PP, GPIO_Speed_50MHz); /* RED */

}

void BSP_LED_Red_On(void){
    hw_gpio_high(GPIOD, GPIO_PIN_15);
}

void BSP_LED_Red_Off(void)
{
    hw_gpio_low(GPIOD, GPIO_PIN_15);
}

