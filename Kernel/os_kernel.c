#include <os_kernel.h>
#include "os_idle.h"

////////////////////////////////////////////////////////////////////////////////
////

#define FLAG_OFF    (0)
#define FLAG_INIT   (1<<1)
#define FLAG_START  (1<<2)
#define FLAG_DONE   (1<<3)

static int8_t os_kernel__init_flag = FLAG_OFF;
static int8_t os_kernel__startup_flag = FLAG_OFF;

////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_kernel_init(void){
    os_err_t err;
    
    if(os_kernel__init_flag>FLAG_OFF) return OS_ERROR;
    
    os_kernel__init_flag = FLAG_INIT;
    
    err = os_memory_init();
    if(err!=OS_EOK){
        return  OS_KERNEL_EMODULE_MEMORY;
    }
    
    err = os_scheduler_init();
    if(err!=OS_EOK){
        return OS_KERNEL_EMODULE_SCHEDULER;
    }
    
    os_idle_init();
    
    os_kernel__init_flag = FLAG_DONE;
    
    return OS_EOK;
}

os_err_t  os_kernel_startup(void)
{
    if(os_kernel__init_flag!=FLAG_DONE) return OS_ERROR;
    os_kernel__startup_flag = FLAG_DONE;
    return os_scheduler_schedule();
}

os_bool_t os_kernel_is_startup(void){
    return (os_kernel__startup_flag==FLAG_DONE && os_scheduler_current_thread()!=0)?OS_TRUE:OS_FALSE;
}

