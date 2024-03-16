#include <Console.h>
#include <bsp_usart1.h>

////////////////////////////////////////////////////////////////////////////////
////
static int Console__RxHandler(sdk_ringbuffer_t* buffer, void* userdata){
    if(sdk_ringbuffer_find_str(buffer, 0, "\r\n")!=-1){
        
        if(sdk_ringbuffer_find_str(buffer, 0, "reboot")!=-1){
            __svc(1);
        }
        
        sdk_ringbuffer_reset(buffer);
        BSP_USART1_Notify();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
////


int Console_Init(void){
    BSP_USART1_SetRxHandler(Console__RxHandler, 0);
}
