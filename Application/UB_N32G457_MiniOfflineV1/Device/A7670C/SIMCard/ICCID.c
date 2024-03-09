#include <ICCID.h>

////////////////////////////////////////////////////////////////////////////////
////
static  A7670C_RxHandler_Result Read_Handler(sdk_ringbuffer_t * buffer, void* ud){
    A7670C_ICCID_Read_Response* result = (A7670C_ICCID_Read_Response*) ud;

    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_text_t text;
        int err = sdk_ringbuffer_cut(&text, buffer, 0, sdk_ringbuffer_used(buffer), "+ICCID: ", "\r\n");
        if(err==0){
            os_size_t size = text.end-text.start;
            sdk_ringbuffer_memcpy(result->ICCID, buffer, text.start, size);
            result->ICCID[size]='\0';
        }

        return kA7670C_RxHandler_Result_DONE;
    }
    return kA7670C_RxHandler_Result_CONTINUE;
}


A7670C_Result A7670C_ICCID_Read(A7670C_ICCID_Read_Response* response, uint32_t timeout_ms)
{
    return A7670C_RequestWithCmd(Read_Handler, response, os_tick_from_ms(timeout_ms), "AT+CICCID\r\n");
}
