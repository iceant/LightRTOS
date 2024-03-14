#ifndef INCLUDED_BSP_CAN2_H
#define INCLUDED_BSP_CAN2_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_N32G45X_H
#define INCLUDED_N32G45X_H
#include <n32g45x.h>
#endif /*INCLUDED_N32G45X_H*/

#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

#ifndef INCLUDED_SDK_RING_H
#include <sdk_ring.h>
#endif /*INCLUDED_SDK_RING_H*/


////////////////////////////////////////////////////////////////////////////////
////
typedef struct
{
    uint32_t StdId; /*!< Specifies the standard identifier.
                         This parameter can be a value between 0 to 0x7FF. */

    uint32_t ExtId; /*!< Specifies the extended identifier.
                         This parameter can be a value between 0 to 0x1FFFFFFF. */

    uint8_t IDE; /*!< Specifies the type of identifier for the message that
                      will be received. This parameter can be a value of
                      @ref CAN_identifier_type */

    uint8_t RTR; /*!< Specifies the type of frame for the received message.
                      This parameter can be a value of
                      @ref CAN_remote_transmission_request */

    uint8_t DLC; /*!< Specifies the length of the frame that will be received.
                      This parameter can be a value between 0 to 8 */

    uint8_t Data[8]; /*!< Contains the data to be received. It ranges from 0 to
                          0xFF. */

    uint8_t FMI; /*!< Specifies the index of the filter the message stored in
                      the mailbox passes through. This parameter can be a
                      value between 0 to 0xFF */
    uint32_t TimeMS;
} BSP_CAN2_Message;

typedef void (*BSP_CAN2_RxHandler)(BSP_CAN2_Message * Message, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////

void BSP_CAN2_Init(void);

void BSP_CAN2_SetRxHandler(BSP_CAN2_RxHandler handler, void* userdata);

#endif /* INCLUDED_BSP_CAN2_H */
