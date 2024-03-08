#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <stdio.h>

#if 1
////////////////////////////////////////////////////////////////////////////////
////
#define THREAD1_STACK_SIZE 1024
#define THREAD2_STACK_SIZE 1024
#define THREAD3_STACK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t thread1_stack[THREAD1_STACK_SIZE];
static os_thread_t thread1;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t thread2_stack[THREAD2_STACK_SIZE];
static os_thread_t thread2;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t thread3_stack[THREAD3_STACK_SIZE];
static os_thread_t thread3;

////////////////////////////////////////////////////////////////////////////////
////

static void thread1_entry(void* p){
    os_size_t timeout_ms = (os_size_t)p;
    os_size_t nCount = 0;
    DS1307_SetYear(2024);
    DS1307_SetMonth(3);
    DS1307_SetDate(8);
    DS1307_SetHour(19);
    DS1307_SetMinute(21);
    DS1307_SetSecond(10);
    while(1){
        printf("Thread:%s, nCount=%d\n", os_thread_self()->name, nCount++);
        printf("%04d-%02d-%02d %02d:%02d:%02d\n"
               , DS1307_GetYear()
               , DS1307_GetMonth()
               , DS1307_GetDate()
               , DS1307_GetHour()
               , DS1307_GetMinute()
               , DS1307_GetSecond()
               );
        os_thread_mdelay(timeout_ms);
//        os_thread_sleep(1); /*调用 sleep 会让出 CPU，其它任务会获得执行机会*/
//        os_thread_yield();
    }
}


#endif

////////////////////////////////////////////////////////////////////////////////
////

int main(void){

    board_init();
    
    os_kernel_init();
    
    /*
     1. 如果 20,5,10 的优先级设置，永远按照 5, 10, 20 的优先级顺序进行调度
     2. 如果 20,10,10 的优先级设置, Thread2 和 Thread3 会交替占用第一优先级，然后才是 Thread1 运行, Thread1 永远在最后
     3. 如果 10,10,10 的优先级设置, 顺序将是 Thread1, Thread2, Thread3 或者 Thread3, Thread2, Thread1
     4. 如果优先级一样，但是 tick 不一样时，tick多的获取的运行时间就多
     5. 如果线程使用 yield让出 cpu，当前线程会被加入就绪表，在下次调度时会安排执行，如果优先级是 10,20,5 那么Thread2将永远无法获得运行机会，如果是 10,10,5 那么 将编程 Thread3, Thread1, Thread3, Thread2 这样的循环运行
     * */
    os_thread_init(&thread1, "Thread1", thread1_entry, 1000, thread1_stack, sizeof(thread1_stack), 10, 10);
    os_thread_startup(&thread1);

    os_thread_init(&thread2, "Thread2", thread1_entry, 1500, thread2_stack, sizeof(thread2_stack), 10, 10);
    os_thread_startup(&thread2);

    os_thread_init(&thread3, "Thread3", thread1_entry, 2000, thread3_stack, sizeof(thread3_stack), 5, 20);
    os_thread_startup(&thread3);

    os_kernel_startup();
    
    while(1){}
}
