#include <A7670C.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <sdk_hex.h>


////////////////////////////////////////////////////////////////////////////////
////
static A7670C_RxHandler_Result A7670C_AT__Handler(sdk_ringbuffer_t * buffer, void* ud){
    if(sdk_ringbuffer_find_str(buffer, 0, "AT\r\r\nOK\r\n")!=-1
        || sdk_ringbuffer_find_str(buffer, 0, "*ATREADY: 1")!=-1){
        return kA7670C_RxHandler_Result_DONE;
    }
    return kA7670C_RxHandler_Result_CONTINUE;
}

A7670C_Result A7670C_AT(os_time_t timeout_ms){
    A7670C_Result result;
    result = A7670C_RequestWithCmd(A7670C_AT__Handler, 0, os_tick_from_millisecond(timeout_ms), "AT\r\n");
    return result;
}

////////////////////////////////////////////////////////////////////////////////
////
static A7670C_RxHandler_Result WaitPBDone_Handler(sdk_ringbuffer_t* buffer, void*ud){
    if(sdk_ringbuffer_find_str(buffer, 0, "PB DONE\r\n")!=-1){
        return kA7670C_RxHandler_Result_DONE;
    }
    return kA7670C_RxHandler_Result_CONTINUE;
}

A7670C_Result A7670C_WaitPBDone(uint32_t timeout_ms)
{
    return A7670C_RequestWithHandler(WaitPBDone_Handler, 0, os_tick_from_millisecond(timeout_ms));
}

////////////////////////////////////////////////////////////////////////////////
////
A7670C_Result A7670C_Startup(void){
    A7670C_Result result;
    int nRetry = 0;

__A7670C__Boot:
    A7670C_PowerOff();
    A7670C_PowerOn();
    A7670C_NopDelay(0x7FFFFF);
    while(!A7670C_IsPowerOn()){
        A7670C_PowerOff();
        A7670C_PowerOn();
        A7670C_NopDelay(10);
    }

    while(1){
        printf("A7670C wait for AT ready...\n");
        result = A7670C_AT(12000);
        if(result==kA7670C_Result_OK){
            break;
        }
        if(nRetry++==10){
            goto __A7670C__Boot;
        }
        os_thread_mdelay(1000);
    }

    nRetry = 0;
    while(1){
        printf("A7670C wait for CPIN ready...\n");
        A7670C_CPIN_Read_Response CPIN_Read_Response;
        result = A7670C_CPIN_Read(&CPIN_Read_Response, 12000);
        if(CPIN_Read_Response.code==kA7670C_Response_Code_OK){
            break;
        }
        if(nRetry++==10){
            return kA7670C_Result_TIMEOUT;
        }
        os_thread_mdelay(1000);
    }
    
    nRetry = 0;
    while(1){
        printf("A7670C checking CSQ quality...");
        A7670C_CSQ_Exec_Response CSQ_Exec_Response;
        result = A7670C_CSQ_Exec(&CSQ_Exec_Response, 12000);
        if(CSQ_Exec_Response.code == kA7670C_Response_Code_OK){
            printf(" rssi: %d, ber: %d\n", CSQ_Exec_Response.rssi, CSQ_Exec_Response.ber);
            if(CSQ_Exec_Response.rssi>0 && CSQ_Exec_Response.rssi<31){
                break;
            }
        }
        if(nRetry++==10){
            return kA7670C_Result_TIMEOUT;
        }
        os_thread_mdelay(1000);
    }
    
    nRetry = 0;
    while(1){
        printf("A7670C CS service...");
        A7670C_CREG_Read_Response CREG_Read_Response;
        result = A7670C_CREG_Read(&CREG_Read_Response, 12000);
        if(CREG_Read_Response.code == kA7670C_Response_Code_OK){
            printf("n:%d, stat: %d\n",CREG_Read_Response.n ,CREG_Read_Response.stat);
            if(CREG_Read_Response.stat==1){
                break;
            }
        }
        if(nRetry++==10){
            return kA7670C_Result_TIMEOUT;
        }
        os_thread_mdelay(1000);
    }
    
    int  A7670C_PS_Flag = 0;
    nRetry = 0;
    while(1){
        printf("A7670C PS service(CGREG)...");
        A7670C_CGREG_Read_Response CGREG_Read_Response;
        result = A7670C_CGREG_Read(&CGREG_Read_Response, 12000);
        if(CGREG_Read_Response.code == kA7670C_Response_Code_OK){
            printf("n:%d, stat: %d\n",CGREG_Read_Response.n ,CGREG_Read_Response.stat);
            if(CGREG_Read_Response.stat==1){
                A7670C_PS_Flag = 1;
                break;
            }
        }
        if(nRetry++==10){
            break;
        }
        os_thread_mdelay(1000);
    }
    
    if(A7670C_PS_Flag==0){
        nRetry = 0;
        while(1){
            printf("A7670C PS service(CEREG)...");
            A7670C_CEREG_Read_Response CEREG_Read_Response;
            result = A7670C_CGREG_Read(&CEREG_Read_Response, 12000);
            if(CEREG_Read_Response.code == kA7670C_Response_Code_OK){
                printf("n:%d, stat: %d\n",CEREG_Read_Response.n ,CEREG_Read_Response.stat);
                if(CEREG_Read_Response.stat==1){
                    A7670C_PS_Flag = 1;
                    break;
                }
            }
            if(nRetry++==10){
                return kA7670C_Result_TIMEOUT;
            }
            os_thread_mdelay(1000);
        }
    }
    
    nRetry = 0;
    while(1){
        printf("A7670C UE system info...");
        A7670C_CPSI_Read_Response CPSI_Read_Response;
        result = A7670C_CPSI_Read(&CPSI_Read_Response, 12000);
        if(CPSI_Read_Response.code == kA7670C_Response_Code_OK){
            printf("SystemMode: %d\n", CPSI_Read_Response.SystemMode);
            if(CPSI_Read_Response.SystemMode!=kA7670C_CPSI_Read_SystemMode_NOSERVICE){
                break;
            }
        }
        if(nRetry++==10){
            return kA7670C_Result_TIMEOUT;
        }
        os_thread_mdelay(1000);
    }

    while(1){
        if(A7670C_WaitPBDone(120000)==kA7670C_Result_OK){
            printf("PB DONE!!!\n");
            break;
        }
    }

    return kA7670C_Result_OK;
    
}