#ifndef INCLUDED_BSP_USART3_H
#define INCLUDED_BSP_USART3_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32F4XX_H
#define INCLUDED_STM32F4XX_H
#include <stm32f4xx.h>
#endif /*INCLUDED_STM32F4XX_H*/



////////////////////////////////////////////////////////////////////////////////
////
extern UART_HandleTypeDef   BSP_USART3__Handle;


////////////////////////////////////////////////////////////////////////////////
////
typedef void (*BSP_USART3_ReceiveCallback)(uint8_t data, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////
void BSP_USART3_Init(void);

void BSP_USART3_EnableDMA(void);

void BSP_USART3_SendBytes(uint8_t * data, int size);

void BSP_USART3_SendString(const char* string);

void BSP_USART3_SetReceiveCallback(BSP_USART3_ReceiveCallback rxCallback, void* userdata);

void BSP_USART3_DMA_Send(uint8_t * data, int size);

#endif /*INCLUDED_BSP_USART3_H*/
