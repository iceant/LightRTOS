#include <bsp_led_green.h>

void BSP_LED_Green_Init(void){
    hw_gpio_init(GPIOD, GPIO_PIN_14, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
}

void BSP_LED_Green_On(void)
{
    hw_gpio_high(GPIOD, GPIO_PIN_15);
}

void BSP_LED_Green_Off(void){
    hw_gpio_low(GPIOD, GPIO_PIN_15);
}
