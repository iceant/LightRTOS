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


#ifndef INCLUDED_BSP_I2C1_H
#include <bsp_i2c1.h>
#endif /*INCLUDED_BSP_I2C1_H*/

#ifndef INCLUDED_BSP_CAN1_H
#include <bsp_can1.h>
#endif /*INCLUDED_BSP_CAN1_H*/


////////////////////////////////////////////////////////////////////////////////
////



#ifndef INCLUDED_DS1302_H
#include <DS1302.h>
#endif /*INCLUDED_DS1302_H*/

#ifndef INCLUDED_ESP01S_H
#include <ESP01S.h>
#endif /*INCLUDED_ESP01S_H*/

#ifndef INCLUDED_OLED_H
#include <OLED.h>
#endif /*INCLUDED_OLED_H*/


////////////////////////////////////////////////////////////////////////////////
////

extern ESP01S_Device_T ESP01S_Device;

////////////////////////////////////////////////////////////////////////////////
////
void board_init(void);

#endif /* INCLUDED_BOARD_H */
