#include "board.h"
#include <string.h>
#include <os_kernel.h>
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

static void USART1_Configuration(void){
    /* Enable GPIO clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA | RCC_APB2_PERIPH_AFIO, ENABLE);
    /* Enable USARTy and USARTz Clock */
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);
    
    GPIO_InitType GPIO_InitStructure;
    
    /* Configure USARTx Tx as alternate function push-pull */
    GPIO_InitStructure.Pin        = GPIO_PIN_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    
    /* Configure USARTx Rx as input floating */
    GPIO_InitStructure.Pin       = GPIO_PIN_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
    
    NVIC_InitType NVIC_InitStructure;
   
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel            = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    
    /* USARTy and USARTz configuration ------------------------------------------------------*/
    USART_InitType USART_InitStructure;
    USART_InitStructure.BaudRate            = 115200;
    USART_InitStructure.WordLength          = USART_WL_8B;
    USART_InitStructure.StopBits            = USART_STPB_1;
    USART_InitStructure.Parity              = USART_PE_NO;
    USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
    USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
    
    /* Configure USARTx */
    USART_Init(USART1, &USART_InitStructure);
    
    USART_ConfigInt(USART1, USART_INT_RXDNE, ENABLE);
    
    /* Enable the USARTx */
    USART_Enable(USART1, ENABLE);
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
    USART1_Configuration();
}

////////////////////////////////////////////////////////////////////////////////
////

