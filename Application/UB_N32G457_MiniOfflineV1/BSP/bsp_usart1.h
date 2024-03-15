#ifndef INCLUDED_BSP_USART1_H
#define INCLUDED_BSP_USART1_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32G45X_H
#define INCLUDED_N32G45X_H
#include <n32g45x.h>
#endif /*INCLUDED_N32G45X_H*/

#ifndef INCLUDED_SDK_RINGBUFFER_H
#include <sdk_ringbuffer.h>
#endif /*INCLUDED_SDK_RINGBUFFER_H*/

#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

////////////////////////////////////////////////////////////////////////////////
////
#define BSP_USART1_RX_STATE_CONTINUE    1
#define BSP_USART1_RX_STATE_DONE        2
#define BSP_USART1_RX_STATE_RESET       3

////////////////////////////////////////////////////////////////////////////////
////
typedef int (*BSP_USART1_RxHandler)(sdk_ringbuffer_t * buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////
void BSP_USART1_Init(void);

void BSP_USART1_SetRxHandler(BSP_USART1_RxHandler rxHandler, void* userdata);

os_err_t BSP_USART1_TimeWait(os_time_t timeout_ms);

os_err_t BSP_USART1_Send(uint8_t * data, os_size_t size);

void BSP_USART1_Notify(void);

#endif /*INCLUDED_BSP_USART1_H*/
