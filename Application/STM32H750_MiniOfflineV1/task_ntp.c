#include <task_ntp.h>
#include <A7670C.h>
#include <DS1307.h>
#include <stdio.h>
////////////////////////////////////////////////////////////////////////////////
////
#define TASK_NTP_STACK_SIZE 1024
#define TASK_NTP_PERIOD_MS  (30*1000)

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t TaskNTP_Stack[TASK_NTP_STACK_SIZE];
static os_thread_t TaskNTP_Thread;

static void TaskNTP_ThreadEntry(void* parameter){
    while(1){
        TaskNTP_Update();
        os_thread_mdelay(TASK_NTP_PERIOD_MS);
    }
}
////////////////////////////////////////////////////////////////////////////////
////

void TaskNTP_Update(void)
{
    A7670C_CNTP_Exec_Response CNTP_Exec_Response;
    A7670C_CNTP_Exec(&CNTP_Exec_Response, 12000);
    if(CNTP_Exec_Response.code!=kA7670C_Response_Code_OK){
        printf("[TaskNTP] CNTP Exec Failed! Code=%d, Error=%d\r\n", CNTP_Exec_Response.code, CNTP_Exec_Response.err_code);
        return;
    }
    A7670C_CCLK_Read_Response CCLK_Read_Response;
    A7670C_CCLK_Read(&CCLK_Read_Response, 12000);
    if(CCLK_Read_Response.code!=kA7670C_Response_Code_OK){
        printf("[TaskNTP] CCLK Read Failed! Code=%d\r\n", CCLK_Read_Response.code);
        return;
    }
    A7670C_CCLK_DateTime datetime;
    A7670C_CCLK_ToDateTime(&datetime, &CCLK_Read_Response);
    if((datetime.month>0 && datetime.month<13)
        && (datetime.day>0 && datetime.day<32)
        && (datetime.hour>=0 && datetime.hour<24)
        && (datetime.min>=0 && datetime.min<60)
        && (datetime.sec>=0 && datetime.sec<58)
        ){
        DS1307_SetYear(datetime.year);
        DS1307_SetMonth(datetime.month);
        DS1307_SetDate(datetime.day);
        DS1307_SetHour(datetime.hour);
        DS1307_SetMinute(datetime.min);
        DS1307_SetSecond(datetime.sec+2);
        printf("[TaskNTP] Set Datetime: %04d-%02d-%02d %02d:%02d:%02d\r\n",
               datetime.year, datetime.month, datetime.day, datetime.hour, datetime.min, datetime.sec+2);
    }
}

void TaskNTP_Init(void)
{
    os_thread_init(&TaskNTP_Thread, "TaskNTP", TaskNTP_ThreadEntry, 0
                   , TaskNTP_Stack, TASK_NTP_STACK_SIZE, 20, 10);
    os_thread_startup(&TaskNTP_Thread);
}

