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

#ifndef INCLUDED_OS_MEMORY_H
#include <os_memory.h>
#endif /*INCLUDED_OS_MEMORY_H*/


#ifndef INCLUDED_BSP_USART1_H
#include <bsp_usart1.h>
#endif /*INCLUDED_BSP_USART1_H*/

#ifndef INCLUDED_BSP_USART3_H
#include <bsp_usart3.h>
#endif /*INCLUDED_BSP_USART3_H*/

#ifndef INCLUDED_BSP_DWTDELAY_H
#include <bsp_dwtdelay.h>
#endif /*INCLUDED_BSP_DWTDELAY_H*/

#ifndef INCLUDED_BSP_ADS1256IO_H
#include <bsp_ads1256io.h>
#endif /*INCLUDED_BSP_ADS1256IO_H*/

#ifndef INCLUDED_BSP_TIMERDELAY_H
#include <bsp_timerdelay.h>
#endif /*INCLUDED_BSP_TIMERDELAY_H*/

#ifndef INCLUDED_BSP_SPI1_H
#include <bsp_spi1.h>
#endif /*INCLUDED_BSP_SPI1_H*/

#ifndef INCLUDED_ADS1256_H
#include <ADS1256.h>
#endif /*INCLUDED_ADS1256_H*/

////////////////////////////////////////////////////////////////////////////////
////
void board_init(void);

#endif /* INCLUDED_BOARD_H */
