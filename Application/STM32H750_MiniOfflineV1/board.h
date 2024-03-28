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

////////////////////////////////////////////////////////////////////////////////
////
void board_init(void);

void IO_USART1_SendString(const char* string);

#endif /* INCLUDED_BOARD_H */
