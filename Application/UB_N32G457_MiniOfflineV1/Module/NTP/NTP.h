#ifndef INCLUDED_NTP_H
#define INCLUDED_NTP_H

////////////////////////////////////////////////////////////////////////////////
////

#ifndef INCLUDED_OS_KERNEL_H
#include <os_kernel.h>
#endif /*INCLUDED_OS_KERNEL_H*/

////////////////////////////////////////////////////////////////////////////////
////

#define NTP_SYNC_PERIOD_MS (1000 * 30)
////////////////////////////////////////////////////////////////////////////////
////

int NTP_Init(void);

os_err_t NTP_Sync(void);

#endif /* INCLUDED_NTP_H */
