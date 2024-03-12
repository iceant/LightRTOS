#ifndef INCLUDED_BSP_UART4_H
#define INCLUDED_BSP_UART4_H

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

#define BSP_UART4_RX_DONE      0
#define BSP_UART4_RX_CONTINUE  1
#define BSP_UART4_RX_RESET     2

#define BSP_UART4_EOK      0
#define BSP_UART4_ETIMEOUT (-1)

////////////////////////////////////////////////////////////////////////////////
////

typedef void (*BSP_UART4_RxHandler)(sdk_ringbuffer_t* buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////

void BSP_UART4_Init(void);

void BSP_UART4_SetRxHandler(BSP_UART4_RxHandler rxHandler, void* userdata);

int BSP_UART4_Send(uint8_t* data, int size);

int BSP_UART4_TimeWait(uint32_t timeout_ms);

void BSP_UART4_Notify(void);

#endif /*INCLUDED_BSP_UART4_H*/
