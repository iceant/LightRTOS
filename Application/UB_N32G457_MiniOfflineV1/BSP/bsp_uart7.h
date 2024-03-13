#ifndef INCLUDED_BSP_UART7_H
#define INCLUDED_BSP_UART7_H


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
typedef void (*BSP_UART7_RxHandler)(sdk_ringbuffer_t * buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////
void BSP_UART7_Init(void);

void BSP_UART7_SetRxHandler(BSP_UART7_RxHandler rxHandler, void* userdata);

os_err_t BSP_UART7_TimeWait(os_tick_t ticks);

os_err_t BSP_UART7_Send(uint8_t * data, os_size_t size);

void BSP_UART7_Notify(void);

#endif /*INCLUDED_BSP_UART7_H*/
