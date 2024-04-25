#ifndef INCLUDED_HW_USART_H
#define INCLUDED_HW_USART_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32L40X_H
#define INCLUDED_N32L40X_H
#include <n32l40x.h>
#endif /*INCLUDED_N32L40X_H*/

#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

////////////////////////////////////////////////////////////////////////////////
////

typedef struct hw_usart_s{
    USART_Module * USARTx;
    GPIO_Module * Tx_GPIO;
    uint16_t Tx_Pin;
    GPIO_Module * Rx_GPIO;
    uint16_t Rx_Pin;
}hw_usart_t;

////////////////////////////////////////////////////////////////////////////////
////

#define HW_USART_IRQ_MODE_TX (1<<0)
#define HW_USART_IRQ_MODE_RX (1<<1)

////////////////////////////////////////////////////////////////////////////////
////

os_err_t hw_usart_init(hw_usart_t* usart, void* USARTx
        , GPIO_Module* Tx_GPIO, uint16_t Tx_Pin
        , GPIO_Module* Rx_GPIO, uint16_t Rx_Pin
        , uint32_t BaudRate);

void hw_usart_enable_dma(void);

void hw_usart_enable_irq(USART_Module * USARTx, uint8_t IRQn, uint8_t Priority, int mode /* 1-Tx, 2-Rx, 3-TxRx*/);

void hw_usart_send(USART_Module * USARTx, uint8_t* TxBuffer, size_t TxBufferSize);

void hw_usart_recv(USART_Module * USARTx, uint8_t * RxBuffer, size_t RxBufferSize);

void hw_usart_dma_send(USART_Module * USARTx, uint8_t* TxBuffer, size_t TxBufferSize);

#endif /*INCLUDED_HW_USART_H*/
