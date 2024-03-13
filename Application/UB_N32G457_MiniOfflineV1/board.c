#include "board.h"
#include <string.h>
#include <os_kernel.h>
#include <bsp_usart1.h>
#include <bsp_power3v3.h>
#include <bsp_i2c1.h>
#include <DS1307.h>
////////////////////////////////////////////////////////////////////////////////
////

#ifdef __GNUC__
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
    #define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
    #define GETCHAR_PROTOTYPE int fget(FILE* f)
#endif


PUTCHAR_PROTOTYPE
{
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET)
        ;
    
    return (ch);
}

GETCHAR_PROTOTYPE
{
    /* Loop until the USARTy Receive Data Register is not empty */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXDNE) == RESET)
    {
    }
    /* Store the received byte in RxBuffer */
    return USART_ReceiveData(USART1);
}

////////////////////////////////////////////////////////////////////////////////
////
static void A7670C_ResetPin_On(void){
    hw_gpio_high(GPIOE, GPIO_PIN_4);
}
static void A7670C_ResetPin_Off(void){
    hw_gpio_low(GPIOE, GPIO_PIN_4);
}
static uint8_t A7670C_ResetPin_Read(void){
    return GPIO_ReadOutputDataBit(GPIOE, GPIO_PIN_4);
}
static void A7670C_PwrKeyPin_On(void){
    hw_gpio_high(GPIOE, GPIO_PIN_5);
}
static void A7670C_PwrKeyPin_Off(void){
    hw_gpio_low(GPIOE, GPIO_PIN_5);
}
static uint8_t A7670C_PwrKeyPin_Read(void){
    return GPIO_ReadOutputDataBit(GPIOE, GPIO_PIN_5);
}
static void A7670C_PwrEnPin_On(void){
    hw_gpio_high(GPIOE, GPIO_PIN_6);
}
static void A7670C_PwrEnPin_Off(void){
    hw_gpio_low(GPIOE, GPIO_PIN_6);
}
static uint8_t A7670C_PwrEnPin_Read(void){
    return GPIO_ReadOutputDataBit(GPIOE, GPIO_PIN_6);
}
static void A7670C_StatusPin_On(void){
    hw_gpio_high(GPIOB, GPIO_PIN_10);
}
static void A7670C_StatusPin_Off(void){
    hw_gpio_low(GPIOB, GPIO_PIN_10);
}
static uint8_t A7670C_StatusPin_Read(void){
    return GPIO_ReadInputDataBit(GPIOB, GPIO_PIN_10);
}

////////////////////////////////////////////////////////////////////////////////
////


static io_i2c_t io_I2C1 = {.send=BSP_I2C1_Send, .recv=BSP_I2C1_Recv, .reset = BSP_I2C1_Reset};

static A7670C_Pin_T A7670C_ResetPin = {.on = A7670C_ResetPin_On, .off = A7670C_ResetPin_Off, .read = A7670C_ResetPin_Read };
static A7670C_Pin_T A7670C_PwrKeyPin = {.on = A7670C_PwrKeyPin_On, .off = A7670C_PwrKeyPin_Off, .read = A7670C_PwrKeyPin_Read };
static A7670C_Pin_T A7670C_PwrEnPin = {.on = A7670C_PwrEnPin_On, .off = A7670C_PwrEnPin_Off, .read = A7670C_PwrEnPin_Read };
static A7670C_Pin_T A7670C_StatusPin = {.on = A7670C_StatusPin_On, .off = A7670C_StatusPin_Off, .read = A7670C_StatusPin_Read };
static A7670C_IO_T A7670C_IO = {.setRxHandler=(void (*)(void *, void *)) BSP_UART5_SetRxHandler,
                                .wait=BSP_UART5_TimeWait,
                                .send=BSP_UART5_Send,
                                .notify = BSP_UART5_Notify};
static IM1253E_IO_T IM1253E_IO = {.setRxHandler=(void (*)(void *, void *)) BSP_UART7_SetRxHandler,
                                  .wait = BSP_UART7_TimeWait,
                                  .send = BSP_UART7_Send,
                                  .notify = BSP_UART7_Notify
                                  };
////////////////////////////////////////////////////////////////////////////////
////

void board_init(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
    SCB->CCR|=SCB_CCR_STKALIGN_Msk; // 栈对齐
    
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    
    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    SysTick_Config(SystemCoreClock/CPU_TICKS_PER_SECOND); /* 1ms = tick */

    /* BSP Configurations */
    BSP_Power3V3_Init();
    BSP_Power3V3_On();

    hw_gpio_init(GPIOE, GPIO_PIN_4, GPIO_Mode_Out_PP, GPIO_Speed_50MHz); /* 4G Reset */
    hw_gpio_init(GPIOE, GPIO_PIN_5, GPIO_Mode_Out_PP, GPIO_Speed_50MHz); /* 4G PWR_KEY */
    hw_gpio_init(GPIOE, GPIO_PIN_6, GPIO_Mode_Out_PP, GPIO_Speed_50MHz); /* 4G PWR_EN */
    hw_gpio_init(GPIOB, GPIO_PIN_10, GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz); /* 4G Status */
    hw_gpio_high(GPIOE, GPIO_PIN_6); /*Turn On 4G Module*/
    
    /* BSP Device On */
    
    BSP_USART1_Init();  /* CONSOLE IO */
    sFLASH_Init();
    
#if defined(MS_TIME_ENABLE)
    BSP_TIM2_Init();
#endif

#if defined(RTC_ENABLE)
    BSP_I2C1_Init();    /* RTC IO */
#endif

#if defined(NETWORK_ENABLE)
    BSP_UART5_Init();   /* NETWORK IO */
#endif

#if defined(VOLTAGE_SENSOR_ENABLE)
    BSP_UART7_Init();   /* IM1253E IO */
#endif

#if defined(CURRENT_SENSOR_ENABLE)
    BSP_CAN2_Init();    /* CURRENT IO */
#endif

#if defined(RTC_ENABLE)
    /* Device Configuration */
    DS1307_Init(&io_I2C1);
#endif

#if defined(NETWORK_ENABLE)
    A7670C_Init(&A7670C_PwrEnPin, &A7670C_PwrKeyPin, &A7670C_StatusPin, &A7670C_ResetPin, &A7670C_IO);
#endif
    
#if defined(VOLTAGE_SENSOR_ENABLE)
    IM1253E_Init(&IM1253E_IO);
#endif

}
