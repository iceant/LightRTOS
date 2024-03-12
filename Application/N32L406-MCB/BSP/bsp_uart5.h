#ifndef INCLUDED_BSP_UART5_H
#define INCLUDED_BSP_UART5_H

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

#define BSP_UART5_RX_DONE      0
#define BSP_UART5_RX_CONTINUE  1
#define BSP_UART5_RX_RESET     2

#define BSP_UART5_EOK      0
#define BSP_UART5_ETIMEOUT (-1)

////////////////////////////////////////////////////////////////////////////////
////

typedef void (*BSP_UART5_RxHandler)(sdk_ringbuffer_t* buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////

void BSP_UART5_Init(void);

void BSP_UART5_SetRxHandler(BSP_UART5_RxHandler rxHandler, void* userdata);

int BSP_UART5_Send(uint8_t* data, int size);

int BSP_UART5_TimeWait(uint32_t timeout_ms);

void BSP_UART5_Notify(void);

#endif /*INCLUDED_BSP_UART5_H*/
