#ifndef INCLUDED_BSP_UART5_H
#define INCLUDED_BSP_UART5_H

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
#define BSP_UART5_RX_STATE_CONTINUE     1
#define BSP_UART5_RX_STATE_DONE         2
#define BSP_UART5_RX_STATE_RESET        3
#define BSP_UART5_RX_STATE_SKIP         4

////////////////////////////////////////////////////////////////////////////////
////
typedef int (*BSP_UART5_RxHandler)(sdk_ringbuffer_t * buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////
void BSP_UART5_Init(void);

void BSP_UART5_SetRxHandler(BSP_UART5_RxHandler rxHandler, void* userdata);

os_err_t BSP_UART5_TimeWait(os_tick_t ticks);

os_err_t BSP_UART5_Send(uint8_t * data, os_size_t size);

#endif /*INCLUDED_BSP_UART5_H*/


