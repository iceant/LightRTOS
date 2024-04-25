#ifndef INCLUDED_BSP_USART1_H
#define INCLUDED_BSP_USART1_H

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

#define BSP_USART1_RX_DONE      0
#define BSP_USART1_RX_CONTINUE  1
#define BSP_USART1_RX_RESET     2

#define BSP_USART1_EOK      0
#define BSP_USART1_ETIMEOUT (-1)

////////////////////////////////////////////////////////////////////////////////
////

typedef void (*BSP_USART1_RxHandler)(sdk_ringbuffer_t* buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART1_Init(void);

void BSP_USART1_SetRxHandler(BSP_USART1_RxHandler rxHandler, void* userdata);

int BSP_USART1_Send(uint8_t* data, int size);

int BSP_USART1_TimeWait(uint32_t timeout_ms);



#endif /* INCLUDED_BSP_USART1_H */
