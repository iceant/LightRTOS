#ifndef INCLUDED_BOARD_H
#define INCLUDED_BOARD_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STM32H7XX_H
#define INCLUDED_STM32H7XX_H
#include <stm32h7xx.h>
#endif /*INCLUDED_STM32H7XX_H*/


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

#ifndef INCLUDED_BSP_USART3_H
#include <bsp_usart3.h>
#endif /*INCLUDED_BSP_USART3_H*/

#ifndef INCLUDED_BSP_I2C1_H
#include <bsp_i2c1.h>
#endif /*INCLUDED_BSP_I2C1_H*/

#ifndef INCLUDED_BSP_USART2_H
#include <bsp_usart2.h>
#endif /*INCLUDED_BSP_USART2_H*/

#ifndef INCLUDED_DS1307_H
#include <DS1307.h>
#endif /*INCLUDED_DS1307_H*/

#ifndef INCLUDED_A7670C_H
#include <A7670C.h>
#endif /*INCLUDED_A7670C_H*/


////////////////////////////////////////////////////////////////////////////////
////
void board_init(void);

#endif /* INCLUDED_BOARD_H */
