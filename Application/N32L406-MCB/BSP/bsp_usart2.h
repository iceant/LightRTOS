#ifndef INCLUDED_BSP_USART2_H
#define INCLUDED_BSP_USART2_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_HW_USART_H
#include <hw_usart.h>
#endif /* INCLUDED_HW_USART_H */

#ifndef INCLUDED_SDK_RINGBUFFER_H
#include <sdk_ringbuffer.h>
#endif /* INCLUDED_SDK_RINGBUFFER_H */

////////////////////////////////////////////////////////////////////////////////
////

#define BSP_USART2_RX_DONE      0
#define BSP_USART2_RX_CONTINUE  1
#define BSP_USART2_RX_RESET     2

#define BSP_USART2_EOK      0
#define BSP_USART2_ETIMEOUT (-1)

////////////////////////////////////////////////////////////////////////////////
////

typedef void (*BSP_USART2_RxHandler)(sdk_ringbuffer_t* buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART2_Init(void);

void BSP_USART2_SetRxHandler(BSP_USART2_RxHandler rxHandler, void* userdata);

int BSP_USART2_Send(uint8_t* data, int size);

int BSP_USART2_TimeWait(uint32_t timeout_ms);

void BSP_USART2_Notify(void);

#endif /* INCLUDED_BSP_USART2_H */
