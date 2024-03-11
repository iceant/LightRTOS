#ifndef INCLUDED_HW_USART_H
#define INCLUDED_HW_USART_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32G45X_H
#define INCLUDED_N32G45X_H
#include <n32g45x.h>
#endif /*INCLUDED_N32G45X_H*/

#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

#ifndef INCLUDED_SDK_RINGBUFFER_H
#include <sdk_ringbuffer.h>
#endif /*INCLUDED_SDK_RINGBUFFER_H*/

////////////////////////////////////////////////////////////////////////////////
////
#define HW_USART_IRQ_MODE_TX (1<<0)
#define HW_USART_IRQ_MODE_RX (1<<1)


////////////////////////////////////////////////////////////////////////////////
////

void hw_usart_configuration(USART_Module* USARTx, uint32_t BaudRate, uint32_t Remap);

void hw_usart_enable_irq(USART_Module * USARTx, uint8_t IRQn, uint8_t Priority, int mode /* 1-Tx, 2-Rx, 3-TxRx*/);

void hw_usart_send(USART_Module * USARTx, uint8_t* TxBuffer, size_t TxBufferSize);

void hw_usart_dma_send(USART_Module * USARTx, uint8_t* TxBuffer, size_t TxBufferSize);

//void hw_usart_recv(USART_Module * USARTx, uint8_t * RxBuffer, size_t RxBufferSize);

void hw_usart_dma_enable(void);

#endif /*INCLUDED_HW_USART_H*/
