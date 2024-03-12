#ifndef INCLUDED_SYSTEMDATETIME_H
#define INCLUDED_SYSTEMDATETIME_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

////////////////////////////////////////////////////////////////////////////////
////

#define SDT_UPDATE_PERIOD_MS 1000

////////////////////////////////////////////////////////////////////////////////
////

typedef enum SystemDateTime_Month{
    kSystemDateTime_Month_JAN = 1,
    kSystemDateTime_Month_FEB = 2,
    kSystemDateTime_Month_MAR = 3,
    kSystemDateTime_Month_APR = 4,
    kSystemDateTime_Month_MAY = 5,
    kSystemDateTime_Month_JUN = 6,
    kSystemDateTime_Month_JUL = 7,
    kSystemDateTime_Month_AUG = 8,
    kSystemDateTime_Month_SEP = 9,
    kSystemDateTime_Month_NOV = 10,
    kSystemDateTime_Month_OCT = 11,
    kSystemDateTime_Month_DEC = 12,
}SystemDateTime_Month;

typedef struct SystemDateTime_S{
    uint16_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    os_size_t millisecond;
}SystemDateTime_T;
////////////////////////////////////////////////////////////////////////////////
////

void SystemDateTime_Init(void);

os_err_t SystemDateTime_Get(SystemDateTime_T* datetime);

#endif /* INCLUDED_SYSTEMDATETIME_H */
