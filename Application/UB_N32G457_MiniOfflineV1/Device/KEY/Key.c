#include <Key.h>
#include <bsp_key.h>
#include <n32g45x.h>
#include <stdio.h>

#include <bsp_led_red.h>
#include <hw_gpio.h>

////////////////////////////////////////////////////////////////////////////////
////

#define IN1_HIGH() GPIO_SetBits(GPIOE,GPIO_PIN_11)
#define IN1_LOW() GPIO_ResetBits(GPIOE,GPIO_PIN_11)

#define IN2_HIGH() GPIO_SetBits(GPIOE,GPIO_PIN_12)
#define IN2_LOW() GPIO_ResetBits(GPIOE,GPIO_PIN_12)

////////////////////////////////////////////////////////////////////////////////
////

void Key_DelayUs(uint32_t time) {
    uint32_t i = 0;
    while (time--) {
        i = 10;
        while (i--);
    }
}

void Key_DelayMs(uint32_t time) {
    uint32_t i = 0;
    while (time--) {
        i = 12000;
        while (i--);
    }
}

////////////////////////////////////////////////////////////////////////////////
////
static void Key__Down(void){
    printf("[KEY] Key Down...\n ");
    hw_gpio_high(GPIOB, GPIO_PIN_11);
    BSP_LED_Red_On();
    Key_DelayMs(1000);
    IN1_HIGH();
    IN2_LOW();
}

static void Key__Up(void){
    printf("[KEY] Key Up...\n ");
    IN1_LOW();
    IN2_LOW();
    hw_gpio_low(GPIOB, GPIO_PIN_11);
    BSP_LED_Red_Off();
}

////////////////////////////////////////////////////////////////////////////////
////

void Key_Init(void){
    BSP_Key_Set(Key__Down, Key__Up);
    if(BSP_Key_GetStatus()==0){
        Key__Down();
    }else{
        Key__Up();
    }
}

