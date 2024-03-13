#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <stdio.h>
#include <bmp.h>

#if 1
////////////////////////////////////////////////////////////////////////////////
////
#define THREAD1_STACK_SIZE 1024
#define THREAD2_STACK_SIZE 1024
#define THREAD3_STACK_SIZE 1024
#define OLED_THREAD_STACK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t thread1_stack[THREAD1_STACK_SIZE];
static os_thread_t thread1;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t thread2_stack[THREAD2_STACK_SIZE];
static os_thread_t thread2;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t thread3_stack[THREAD3_STACK_SIZE];
static os_thread_t thread3;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t oled_thread_stack[OLED_THREAD_STACK_SIZE];
static os_thread_t oled_thread;

////////////////////////////////////////////////////////////////////////////////
////

static void ShowDateTime(void){
    printf("%04d-%02d-%02d %02d:%02d:%02d\n"
    , DS1302_GetYear()
    , DS1302_GetMonth()
    , DS1302_GetDate()
    , DS1302_GetHour()
    , DS1302_GetMinute()
    , DS1302_GetSecond()
    );
}

static void SetDateTime(uint16_t year, uint8_t month, uint8_t  date, uint8_t hour, uint8_t min, uint8_t  sec, uint8_t dayOfWeek){
    DS1302_Set(year, month, date, hour, min, sec, dayOfWeek);
}

static void NTP_Sync(void){
    struct tm datetime={0};
    
    ESP01S_CIPSNTPTIME(&ESP01S_Device, &datetime, 3000);
    
    if(datetime.tm_mday!=0 && datetime.tm_year!=1970){
        SetDateTime(datetime.tm_year
                , datetime.tm_mon+1
                , datetime.tm_mday
                , datetime.tm_hour
                , datetime.tm_min
                , datetime.tm_sec
                , datetime.tm_wday);
    }
    
}

static void thread_worker_entry(void* p){
    uint32_t nCount = 0;
    while(1){
        printf("Thread:%s, nCount=%ld\n", os_thread_self()->name, nCount++);
        ShowDateTime();
        os_thread_mdelay(1000);
    }
}

static void OLED_ShowDateTime(void){
    char TimeBuf[20]={0};
    int size = snprintf(TimeBuf, sizeof(TimeBuf), "%04d-%02d-%02d %02d:%02d:%02d"
            , DS1302_GetYear()
            , DS1302_GetMonth()
            , DS1302_GetDate()
            , DS1302_GetHour()
            , DS1302_GetMinute()
            , DS1302_GetSecond()
    );
    TimeBuf[size]='\0';
    OLED_ShowString(0, 0, TimeBuf, 12);
}

static void OLED_ThreadEntry(void* P){

    int sleepMs = 500;
    int x = 0;
    int y = 1;
    int x2 = 118;
    int y2 = 8;
    
    while(1){
        OLED_DrawBMP(x, y, x2, y2, IMG1);
        os_thread_mdelay(sleepMs);
        
        OLED_DrawBMP(x, y, x2, y2, IMG2);
        os_thread_mdelay(sleepMs);
        
        OLED_DrawBMP(x, y, x2, y2, IMG3);
        os_thread_mdelay(sleepMs);
        
        OLED_DrawBMP(x, y, x2, y2, IMG4);
        os_thread_mdelay(sleepMs);
        
        OLED_DrawBMP(x, y, x2, y2, IMG5);
        os_thread_mdelay(sleepMs);
    }
}

static void thread1_entry(void* p){
    os_size_t timeout_ms = (os_size_t)p;
    os_size_t nCount = 0;
    
    ESP01S_Connect(&ESP01S_Device, "PIZER_WLS", "1234567890", 12000);
    ESP01S_CWMODE_Set(&ESP01S_Device, 1, 12000);
    ESP01S_CIPSNTPCFG(&ESP01S_Device, 8, 12000);
    
    NTP_Sync();

    
//    os_thread_init(&thread2, "Thread2", thread_worker_entry, 200, thread2_stack, sizeof(thread2_stack), 10, 10);
//    os_thread_startup(&thread2);
//
//    os_thread_init(&thread3, "Thread3", thread_worker_entry, 200, thread3_stack, sizeof(thread3_stack), 10, 10);
//    os_thread_startup(&thread3);
//


    while(1){

        printf("Thread:%s, nCount=%d\n", os_thread_self()->name, nCount++);

        if(nCount%30==0){
            NTP_Sync();
        }
        ShowDateTime();
        OLED_ShowDateTime();

        os_thread_mdelay(1000);
//        os_thread_sleep(1); /*调用 sleep 会让出 CPU，其它任务会获得执行机会*/
//        os_thread_yield();
    }
    
//    while(1){
//        OLED_ShowDateTime();
//        os_thread_mdelay(1000);
//    }
}

#endif

////////////////////////////////////////////////////////////////////////////////
////
static void USART1__RxHandler(sdk_ringbuffer_t * buffer, void* userdata){
    if(sdk_ringbuffer_find_str(buffer, 0, "\r\n")!=-1){
        printf("USART1: %s\n", buffer->buffer,sdk_ringbuffer_used(buffer));
        sdk_ringbuffer_reset(buffer);
    }
}

////////////////////////////////////////////////////////////////////////////////
////

int main(void){
    board_init();
    
    BSP_USART1_SetRxHandler(USART1__RxHandler, 0);
    
    os_kernel_init();

    /*
     1. 如果 20,5,10 的优先级设置，永远按照 5, 10, 20 的优先级顺序进行调度
     2. 如果 20,10,10 的优先级设置, Thread2 和 Thread3 会交替占用第一优先级，然后才是 Thread1 运行, Thread1 永远在最后
     3. 如果 10,10,10 的优先级设置, 顺序将是 Thread1, Thread2, Thread3 或者 Thread3, Thread2, Thread1
     4. 如果优先级一样，但是 tick 不一样时，tick多的获取的运行时间就多
     5. 如果线程使用 yield让出 cpu，当前线程会被加入就绪表，在下次调度时会安排执行，如果优先级是 10,20,5 那么Thread2将永远无法获得运行机会，如果是 10,10,5 那么 将编程 Thread3, Thread1, Thread3, Thread2 这样的循环运行
     * */
    os_thread_init(&thread1, "Thread1", thread1_entry, (void*)1000, thread1_stack, sizeof(thread1_stack), 10, 10);
    os_thread_startup(&thread1);
    
    os_thread_init(&oled_thread, "OLED", OLED_ThreadEntry, 0, oled_thread_stack, sizeof(oled_thread_stack), 10, 10);
    os_thread_startup(&oled_thread);
    
//    os_thread_init(&thread2, "Thread2", thread1_entry, 200, thread2_stack, sizeof(thread2_stack), 10, 10);
//    os_thread_startup(&thread2);
//
//    os_thread_init(&thread3, "Thread3", thread1_entry, 200, thread3_stack, sizeof(thread3_stack), 5, 20);
//    os_thread_startup(&thread3);
    
    os_kernel_startup();
    
    while(1){}
}

