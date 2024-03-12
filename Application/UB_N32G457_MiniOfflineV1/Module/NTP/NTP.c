#include <NTP.h>
#include <os_kernel.h>
#include <A7670C.h>
#include <DS1307.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////
#define NTP_THREAD_STACK_SIZE 512
#define NTP_THREAD_PRIORITY 20
#define NTP_THREAD_TIMESLICE 10

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t NTP__Thread_Stack[NTP_THREAD_STACK_SIZE];
static os_thread_t NTP__Thread;

static void NTP__Thread_Entry(void* p){
    
    A7670C_CNTP_Write_Response CNTP_Write_Response;
    do {
        A7670C_CNTP_Write(&CNTP_Write_Response, "ntp.aliyun.com", 32, 12000);
    }while(CNTP_Write_Response.code!=kA7670C_Response_Code_OK);
    
    while(1){
        NTP_Sync();
        os_thread_mdelay(NTP_SYNC_PERIOD_MS);
    }
}

////////////////////////////////////////////////////////////////////////////////
////

int NTP_Init(void){
    os_thread_init(&NTP__Thread, "NTP", NTP__Thread_Entry, 0
                   , NTP__Thread_Stack, sizeof(NTP__Thread_Stack)
                   , NTP_THREAD_PRIORITY, NTP_THREAD_TIMESLICE
                   );
    
    os_thread_startup(&NTP__Thread);
    
    return 0;
}

os_err_t NTP_Sync(void){
    A7670C_CNTP_Exec_Response CNTP_Exec_Response;
    A7670C_CCLK_Read_Response CCLK_Read_Response;
    A7670C_CCLK_DateTime CCLK_DateTime;
    
    while(1){
        
        do{
            A7670C_CNTP_Exec(&CNTP_Exec_Response, 12000);
        }while(CNTP_Exec_Response.code!=kA7670C_Response_Code_OK);
        
        do {
            A7670C_CCLK_Read(&CCLK_Read_Response, 12000);
        }while(CCLK_Read_Response.code!=kA7670C_Response_Code_OK);
        
        A7670C_CCLK_ToDateTime(&CCLK_DateTime, &CCLK_Read_Response);
        
        if(CCLK_DateTime.year==2070
           && CCLK_DateTime.month==1
           && CCLK_DateTime.day==1
                ){
            return OS_ERROR;
        }
        
        DS1307_SetYear(CCLK_DateTime.year);
        DS1307_SetMonth(CCLK_DateTime.month);
        DS1307_SetDate(CCLK_DateTime.day);
        DS1307_SetHour(CCLK_DateTime.hour);
        DS1307_SetMinute(CCLK_DateTime.min);
        DS1307_SetSecond(CCLK_DateTime.sec+2);
        
        return OS_EOK;
    }
}

