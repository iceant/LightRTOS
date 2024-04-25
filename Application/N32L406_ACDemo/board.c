#include "board.h"
#include <string.h>
#include <os_kernel.h>
#include <stdio.h>
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
static OLED_IO_T OLED_IO={.send=BSP_I2C1_Send, .recv=BSP_I2C1_Recv, .reset=BSP_I2C1_Reset};


////////////////////////////////////////////////////////////////////////////////
////

void board_init(void)
{
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
    SCB->CCR|=SCB_CCR_STKALIGN_Msk; // 栈对齐
    
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    ////////////////////////////////////////////////////////////////////////////////
    ////


    /* Hardware Configurations */

    BSP_TIMDelay_Init();

    BSP_Led_Red_Init();
    BSP_Led_Blue_Init();

    BSP_USART1_Init();
    BSP_USART2_Init();

    ////////////////////////////////////////////////////////////////////////////////
    ////
    OLED_Init(&OLED_IO);



    ////////////////////////////////////////////////////////////////////////////////
    ////


    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    SysTick_Config(SystemCoreClock/CPU_TICKS_PER_SECOND); /* 1ms = tick */
}

////////////////////////////////////////////////////////////////////////////////
////

