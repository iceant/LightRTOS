#ifndef INCLUDED_BSP_USART1_H
#define INCLUDED_BSP_USART1_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32H7XX_H
#define INCLUDED_STM32H7XX_H
#include <stm32h7xx.h>
#endif /*INCLUDED_STM32H7XX_H*/

////////////////////////////////////////////////////////////////////////////////
////
extern UART_HandleTypeDef   BSP_USART1__Handle;


////////////////////////////////////////////////////////////////////////////////
////
typedef void (*BSP_USART1_ReceiveCallback)(uint8_t data, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////
void BSP_USART1_Init(void);

void BSP_USART1_EnableDMA(void);

void BSP_USART1_SendBytes(uint8_t * data, int size);

void BSP_USART1_SendString(const char* string);

void BSP_USART1_SetReceiveCallback(BSP_USART1_ReceiveCallback rxCallback, void* userdata);

void BSP_USART1_DMA_Send(uint8_t * data, int size);

#endif /*INCLUDED_BSP_USART1_H*/
