#ifndef INCLUDED_BOARD_H
#define INCLUDED_BOARD_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32F4XX_H
#define INCLUDED_STM32F4XX_H
#include <stm32f4xx.h>
#endif /*INCLUDED_STM32F4XX_H*/

#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif /* INCLUDED_STDINT_H */

#ifndef INCLUDED_STDDEF_H
#define INCLUDED_STDDEF_H
#include <stddef.h>
#endif /* INCLUDED_STDDEF_H */

#ifndef INCLUDED_BSP_USART1_H
#include <bsp_usart1.h>
#endif /*INCLUDED_BSP_USART1_H*/



////////////////////////////////////////////////////////////////////////////////
////
void board_init(void);

#endif /* INCLUDED_BOARD_H */
