#include <SystemDateTime.h>
#include <DS1307.h>
#include <os_mutex.h>
#include <stdio.h>
#include <bsp_tim2.h>
////////////////////////////////////////////////////////////////////////////////
////

#define SDT_THREAD_STACK_SIZE 512
#define SDT_THREAD_PRIORITY 20
#define SDT_THREAD_TIMESLICE 10

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t SDT__Thread_Stack[SDT_THREAD_STACK_SIZE];
static os_thread_t SDT__Thread;
static SystemDateTime_T SDT__CurrentDateTime;
static os_mutex_t SDT__Mutex;
////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE os_bool_t SDT__IsLeapYear(uint16_t year){
    if(((year%4)==0) && ((year%100)!=0) || (year%400==0)){
        return OS_TRUE;
    }
    return OS_FALSE;
}

__STATIC_FORCEINLINE void SDT__AddOneSecond(void){
    SDT__CurrentDateTime.second+=1;
    if(SDT__CurrentDateTime.second==60){
        SDT__CurrentDateTime.second = 0;
        SDT__CurrentDateTime.minute+=1;
        if(SDT__CurrentDateTime.minute==60){
            SDT__CurrentDateTime.minute=0;
            SDT__CurrentDateTime.hour+=1;
            if(SDT__CurrentDateTime.hour==24){
                SDT__CurrentDateTime.hour=0;
                SDT__CurrentDateTime.date+=1;
                if(SDT__CurrentDateTime.month==kSystemDateTime_Month_FEB){
                    if(SDT__IsLeapYear(SDT__CurrentDateTime.year)){
                        if(SDT__CurrentDateTime.date==30/* max 29 */){
                            SDT__CurrentDateTime.date=1;
                            SDT__CurrentDateTime.month+=1;
                            if(SDT__CurrentDateTime.month==13){
                                SDT__CurrentDateTime.year+=1;
                                SDT__CurrentDateTime.month=kSystemDateTime_Month_JAN;
                            }
                        }
                    }else{
                        if(SDT__CurrentDateTime.date==29/* max 28 */){
                            SDT__CurrentDateTime.date=1;
                            SDT__CurrentDateTime.month+=1;
                            if(SDT__CurrentDateTime.month==13){
                                SDT__CurrentDateTime.year+=1;
                                SDT__CurrentDateTime.month=kSystemDateTime_Month_JAN;
                            }
                        }
                    }
                }else{
                    if(SDT__CurrentDateTime.month==kSystemDateTime_Month_JAN
                        || SDT__CurrentDateTime.month==kSystemDateTime_Month_MAR
                        || SDT__CurrentDateTime.month==kSystemDateTime_Month_MAY
                        || SDT__CurrentDateTime.month==kSystemDateTime_Month_JUL
                        || SDT__CurrentDateTime.month==kSystemDateTime_Month_AUG
                        || SDT__CurrentDateTime.month==kSystemDateTime_Month_OCT
                        || SDT__CurrentDateTime.month==kSystemDateTime_Month_DEC)
                    {
                        if(SDT__CurrentDateTime.date==32){
                            SDT__CurrentDateTime.date = 1;
                            SDT__CurrentDateTime.month+=1;
                            if(SDT__CurrentDateTime.month==13){
                                SDT__CurrentDateTime.year+=1;
                                SDT__CurrentDateTime.month=kSystemDateTime_Month_JAN;
                            }
                        }
                    }else{
                        if(SDT__CurrentDateTime.date==31){
                            SDT__CurrentDateTime.date = 1;
                            SDT__CurrentDateTime.month+=1;
                            if(SDT__CurrentDateTime.month==13){
                                SDT__CurrentDateTime.year+=1;
                                SDT__CurrentDateTime.month=kSystemDateTime_Month_JAN;
                            }
                        }
                    }
                }
            }
        }
    }
}

__STATIC_FORCEINLINE void SDT__AddMilliseconds(uint16_t ms){
    SDT__CurrentDateTime.millisecond += ms;
    while(SDT__CurrentDateTime.millisecond>999){
        SDT__CurrentDateTime.millisecond-=1000;
        SDT__AddOneSecond();
    }
}

static void SystemDateTime__TIM2_TimeUpHandler(void* userdata){
    SDT__CurrentDateTime.millisecond++;
}

static void SDT__Thread_Entry(void* p){
    while(1){
        
        uint16_t year = DS1307_GetYear();
        uint8_t month = DS1307_GetMonth();
        uint8_t date = DS1307_GetDate();
        uint8_t hour = DS1307_GetHour();
        uint8_t minute = DS1307_GetMinute();
        uint8_t second = DS1307_GetSecond();
        
        if(year!=1970 && (month>=0 && month<11) && (date>=1 && date<=31)
            && (hour>=0 && hour<=23)
            && (minute>=0 && minute<=59)
            && (second>=0 && second<=59))
        {
            os_mutex_lock(&SDT__Mutex);
            SDT__CurrentDateTime.year = year;
            SDT__CurrentDateTime.month = month;
            SDT__CurrentDateTime.date = date;
            SDT__CurrentDateTime.hour = hour;
            SDT__CurrentDateTime.minute = minute;
            if(SDT__CurrentDateTime.second==second){
                SDT__AddOneSecond();
            }else{
                SDT__CurrentDateTime.second = second;
            }
            SDT__CurrentDateTime.millisecond = 0;
            BSP_TIM2_SetTimeUpHandler(SystemDateTime__TIM2_TimeUpHandler, 0);
            os_mutex_unlock(&SDT__Mutex);
        }else{
            os_mutex_lock(&SDT__Mutex);
            SDT__AddOneSecond();
            SDT__CurrentDateTime.millisecond = 0;
            BSP_TIM2_SetTimeUpHandler(SystemDateTime__TIM2_TimeUpHandler, 0);
            os_mutex_unlock(&SDT__Mutex);
        }
        os_thread_mdelay(SDT_UPDATE_PERIOD_MS);
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void SystemDateTime_Init(void){
    SDT__CurrentDateTime.year = 0;
    SDT__CurrentDateTime.month = 0;
    SDT__CurrentDateTime.date = 0;
    SDT__CurrentDateTime.hour = 0;
    SDT__CurrentDateTime.minute = 0;
    SDT__CurrentDateTime.second = 0;
    SDT__CurrentDateTime.millisecond = 0;
    
    os_mutex_init(&SDT__Mutex);
    
    os_thread_init(&SDT__Thread, "SystemDateTime"
                   , SDT__Thread_Entry, 0
                   , SDT__Thread_Stack, sizeof(SDT__Thread_Stack)
                   , SDT_THREAD_PRIORITY, SDT_THREAD_TIMESLICE);
    os_thread_startup(&SDT__Thread);
}

os_err_t SystemDateTime_Get(SystemDateTime_T* datetime)
{
    os_mutex_lock(&SDT__Mutex);
    memcpy(datetime, &SDT__CurrentDateTime, sizeof(SDT__CurrentDateTime));
    os_mutex_unlock(&SDT__Mutex);
    return OS_EOK;
}
