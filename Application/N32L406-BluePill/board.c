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
//static DS1307_IO_T DS1307_DeviceIO={.send=BSP_I2C1_Send, .recv=BSP_I2C1_Recv};
static ESP01S_IO_T ESP01S_DeviceIO={.send=BSP_USART2_Send, .set_rx_handler=(void*)BSP_USART2_SetRxHandler
                                    , .wait=BSP_USART2_TimeWait, .notify=BSP_USART2_Notify};
ESP01S_Device_T ESP01S_Device={.device_io=&ESP01S_DeviceIO};

static OLED_IO_T OLED_IO={.recv=BSP_I2C1_Recv, .send=BSP_I2C1_Send, .reset=BSP_I2C1_Reset};

////////////////////////////////////////////////////////////////////////////////
////
static void OLED_IO_TimeoutHandler(void* userdata){
    BSP_I2C1_Reset();
//    for(int i=0; i<0x3FFFFF; i++);
    printf("Call OLED_TurnOn()...\n");
//    OLED_Display_On();
}

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
    
    /* Hardware Configurations */
    BSP_USART1_Init();
    BSP_I2C1_Init();
    BSP_USART2_Init();
    
    /* Device Configuration */
//    DS1307_Init(&DS1307_DeviceIO);
    DS1302_Init();
    ESP01S_Init(&ESP01S_Device, &ESP01S_DeviceIO);
    OLED_Init(&OLED_IO);
    BSP_I2C1_SetTimeoutHandler(OLED_IO_TimeoutHandler, 0);
    OLED_TurnOn();
}

////////////////////////////////////////////////////////////////////////////////
////

