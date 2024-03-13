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
typedef void (*BSP_CAN2_RxHandler)(sdk_ring_t * ring, void* userdata);

////////////////////////////////////////////////////////////////////////////////
////

void BSP_CAN2_Init(void);

void BSP_CAN2_SetRxHandler(BSP_CAN2_RxHandler handler, void* userdata);


#endif /* INCLUDED_BSP_CAN2_H */
