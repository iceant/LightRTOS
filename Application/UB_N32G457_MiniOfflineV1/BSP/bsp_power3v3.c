#include <bsp_power3v3.h>
#include <hw_gpio.h>

////////////////////////////////////////////////////////////////////////////////
////

void BSP_Power3V3_Init(void)
{
    hw_gpio_init(GPIOE, GPIO_PIN_1, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);

}

void BSP_Power3V3_On(void){
    hw_gpio_high(GPIOE, GPIO_PIN_1);
}

void BSP_Power3V3_Off(void){
    hw_gpio_low(GPIOE, GPIO_PIN_1);
}

