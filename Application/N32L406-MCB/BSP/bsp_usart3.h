#ifndef INCLUDED_BSP_USART3_H
#define INCLUDED_BSP_USART3_H

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

#define BSP_USART3_RX_DONE      0
#define BSP_USART3_RX_CONTINUE  1
#define BSP_USART3_RX_RESET     2

#define BSP_USART3_EOK      0
#define BSP_USART3_ETIMEOUT (-1)

////////////////////////////////////////////////////////////////////////////////
////

typedef void (*BSP_USART3_RxHandler)(sdk_ringbuffer_t* buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////

void BSP_USART3_Init(void);

void BSP_USART3_SetRxHandler(BSP_USART3_RxHandler rxHandler, void* userdata);

int BSP_USART3_Send(uint8_t* data, int size);

int BSP_USART3_TimeWait(uint32_t timeout_ms);

void BSP_USART3_Notify(void);

#endif /*INCLUDED_BSP_USART3_H*/
