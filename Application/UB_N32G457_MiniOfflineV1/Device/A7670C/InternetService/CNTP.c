#include <CNTP.h>
#include <stdio.h>
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
////
static A7670C_RxHandler_Result Read_Handler(sdk_ringbuffer_t *buffer, void* ud){
    A7670C_CNTP_Read_Response* response = (A7670C_CNTP_Read_Response*)ud;
    sdk_ringbuffer_text_t find_result;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        response->code = kA7670C_Response_Code_OK;
        int find = sdk_ringbuffer_cut(&find_result, buffer, 0, sdk_ringbuffer_used(buffer), "+CNTP: ", "\r\n");
        if(find==0){
            int host_end = sdk_ringbuffer_find_str(buffer, find_result.start, ",");
            int host_length = host_end-find_result.end;
            sdk_ringbuffer_memcpy(response->host, buffer, find_result.start, host_length);
            response->host[host_length]='\0';

            response->timezone = sdk_ringbuffer_strtoul(buffer, host_end+1, 0, 0);
            return kA7670C_RxHandler_Result_DONE;
        }else{
            return kA7670C_RxHandler_Result_RESET;
        }
    }
    return kA7670C_RxHandler_Result_CONTINUE;
}


A7670C_Result A7670C_CNTP_Read(A7670C_CNTP_Read_Response *response, os_size_t timeout_ms)
{
    return A7670C_RequestWithCmd(Read_Handler, response, os_tick_from_ms(timeout_ms), "AT+CNTP?\r\n");
}

////////////////////////////////////////////////////////////////////////////////
////
static A7670C_RxHandler_Result Write_Handler(sdk_ringbuffer_t *buffer, void* ud){
    A7670C_CNTP_Write_Response* response = (A7670C_CNTP_Write_Response*)ud;

    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
//        printf("CNTP_Write: %s\n", buffer->buffer);
//        assert(response);
        response->code = kA7670C_Response_Code_OK;
        return kA7670C_RxHandler_Result_DONE;
    }

    if(sdk_ringbuffer_find_str(buffer, 0, "ERROR\r\n")!=-1){
        assert(response);
        response->code = kA7670C_Response_Code_ERROR;
        return kA7670C_RxHandler_Result_DONE;
    }

    return kA7670C_RxHandler_Result_CONTINUE;
}

A7670C_Result A7670C_CNTP_Write(A7670C_CNTP_Write_Response *response, const char* host, int8_t timezone, os_size_t timeout_ms)
{
    response->code = kA7670C_Response_Code_ERROR;
    return A7670C_RequestWithArgs(Write_Handler, response, os_tick_from_ms(timeout_ms), "AT+CNTP=\"%s\",%d\r\n", host, timezone);
}

////////////////////////////////////////////////////////////////////////////////
////

static A7670C_RxHandler_Result Exec_Handler(sdk_ringbuffer_t * buffer, void* ud)
{
    A7670C_CNTP_Exec_Response* response = (A7670C_CNTP_Exec_Response*)ud;

    if(sdk_ringbuffer_find_str(buffer, 0, "+CNTP: ")!=-1 /*接收结束: 正确*/){
//        printf("CNTP EXEC: %s\n", buffer->buffer);
        sdk_ringbuffer_text_t err_code_text;
        int res = sdk_ringbuffer_cut(&err_code_text, buffer, 0, sdk_ringbuffer_used(buffer), "+CNTP: ", "\r\n");

        if(res==0){
            response->code = kA7670C_Response_Code_OK;
            response->err_code = sdk_ringbuffer_strtoul(buffer, err_code_text.start, 0, 0);
        }

        return kA7670C_RxHandler_Result_DONE;
    }

    if(sdk_ringbuffer_find_str(buffer, 0, "ERROR\r\n")!=-1 /*接收结束: 错误*/){
        response->code = kA7670C_Response_Code_ERROR;
        response->err_code = 0;
        return kA7670C_RxHandler_Result_DONE;
    }

    return kA7670C_RxHandler_Result_CONTINUE;
}

A7670C_Result A7670C_CNTP_Exec(A7670C_CNTP_Exec_Response* result, uint32_t timeout_ms)
{
    result->err_code=-1;
    return A7670C_RequestWithCmd(Exec_Handler, result, os_tick_from_ms(timeout_ms), "AT+CNTP\r\n");
}


