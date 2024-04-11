#ifndef INCLUDED_BSP_USART2_H
#define INCLUDED_BSP_USART2_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32H7XX_H
#define INCLUDED_STM32H7XX_H
#include <stm32h7xx.h>
#endif /*INCLUDED_STM32H7XX_H*/

#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

#ifndef INCLUDED_SDK_RINGBUFFER_H
#include <sdk_ringbuffer.h>
#endif /*INCLUDED_SDK_RINGBUFFER_H*/


////////////////////////////////////////////////////////////////////////////////
////
extern UART_HandleTypeDef   BSP_USART2__Handle;


////////////////////////////////////////////////////////////////////////////////
////
//typedef void (*BSP_USART2_ReceiveCallback)(uint8_t data, void* userdata);

#define BSP_USART2_RXHANDLER_RESULT_CONTINUE    1
#define BSP_USART2_RXHANDLER_RESULT_DONE        2
#define BSP_USART2_RXHANDLER_RESULT_RESET       3
#define BSP_USART2_RXHANDLER_RESULT_SKIP        4

typedef int (*BSP_USART2_RxHandler_T)(sdk_ringbuffer_t * buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////
void BSP_USART2_Init(void);

void BSP_USART2_EnableDMA(void);

int BSP_USART2_SendBytes(uint8_t * data, int size);

void BSP_USART2_SendString(const char* string);

//void BSP_USART2_SetReceiveCallback(BSP_USART2_ReceiveCallback rxCallback, void* userdata);

void BSP_USART2_DMA_Send(uint8_t * data, int size);

os_err_t BSP_USART2_TimeWait(os_time_t timeout_ms);
os_err_t BSP_USART2_Notify(void);
void BSP_USART2_SetDefaultRxHandler(BSP_USART2_RxHandler_T defaultRxHandler, void* userdata);
void BSP_USART2_SetRxHandler(BSP_USART2_RxHandler_T rxHandler, void* userdata);

#endif /*INCLUDED_BSP_USART2_H*/
