#ifndef INCLUDED_DEV_CONSOLE_H
#define INCLUDED_DEV_CONSOLE_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_HW_USART_H
#include <hw_usart.h>
#endif /*INCLUDED_HW_USART_H*/

#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

#ifndef INCLUDED_SDK_RINGBUFFER_H
#include <sdk_ringbuffer.h>
#endif /*INCLUDED_SDK_RINGBUFFER_H*/

#ifndef INCLUDED_OS_ERROR_H
#include <os_error.h>
#endif /*INCLUDED_OS_ERROR_H*/


////////////////////////////////////////////////////////////////////////////////
////

typedef enum dev_console_rx_result_e{
    kDevConsoleRxResult_CONTINUE = 1,
    kDevConsoleRxResult_DONE = 2,
}dev_console_rx_result_t;

typedef dev_console_rx_result_t (*dev_console_rx_handler)(sdk_ringbuffer_t * buffer, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////
void dev_console_init(void* USARTx);

void dev_console_send(uint8_t * data, os_size_t data_size);

os_err_t dev_console_recv(dev_console_rx_handler rx_handler, void* userdata);


#endif /*INCLUDED_DEV_CONSOLE_H*/
