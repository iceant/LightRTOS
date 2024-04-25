#ifndef INCLUDED_DS1302_H
#define INCLUDED_DS1302_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif /*INCLUDED_STDINT_H*/

////////////////////////////////////////////////////////////////////////////////
////
void DS1302_Init(void);

void DS1302_SetRegByte(uint8_t address, uint8_t data);

uint8_t DS1302_GetRegByte(uint8_t address);

void DS1302_Set(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t dayOfWeek);

void DS1302_SetSecond(uint8_t second);
void DS1302_SetMinute(uint8_t minute);
void DS1302_SetHour(uint8_t hour);
void DS1302_SetDate(uint8_t date);
void DS1302_SetMonth(uint8_t month);
void DS1302_SetYear(uint16_t year);
void DS1302_SetDayOfWeek(uint8_t dayOfWeek /* 1- 7 */);

uint16_t DS1302_GetYear(void);
uint8_t DS1302_GetMonth(void);
uint8_t DS1302_GetDate(void);
uint8_t DS1302_GetHour(void);
uint8_t DS1302_GetMinute(void);
uint8_t DS1302_GetSecond(void);
uint8_t DS1302_GetDayOfWeek(void);


#endif /* INCLUDED_DS1302_H */
