#ifndef INCLUDED_BOARD_H
#define INCLUDED_BOARD_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_N32L40X_H
#define INCLUDED_N32L40X_H
#include <n32l40x.h>
#endif /*INCLUDED_N32L40X_H*/

#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif /* INCLUDED_STDINT_H */

#ifndef INCLUDED_STDDEF_H
#define INCLUDED_STDDEF_H
#include <stddef.h>
#endif /* INCLUDED_STDDEF_H */

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_BSP_USART1_H
#include <bsp_usart1.h>
#endif /*INCLUDED_BSP_USART1_H*/

#ifndef INCLUDED_BSP_USART2_H
#include <bsp_usart2.h>
#endif /*INCLUDED_BSP_USART2_H*/

#ifndef INCLUDED_BSP_USART3_H
#include <bsp_usart3.h>
#endif /*INCLUDED_BSP_USART3_H*/

#ifndef INCLUDED_BSP_UART4_H
#include <bsp_uart4.h>
#endif /*INCLUDED_BSP_UART4_H*/

#ifndef INCLUDED_BSP_UART5_H
#include <bsp_uart5.h>
#endif /*INCLUDED_BSP_UART5_H*/

////////////////////////////////////////////////////////////////////////////////
////
void board_init(void);

#endif /* INCLUDED_BOARD_H */
