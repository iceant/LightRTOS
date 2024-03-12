#include <ESP01S.h>
#include <string.h>
#include <stdio.h>
#include <os_memory.h>
#include <os_mutex.h>
#include <sdk_hex.h>
////////////////////////////////////////////////////////////////////////////////
////

typedef struct ESP01S_Request_S{
    ESP01S_Device_T * device;
    void* userdata;
}ESP01S_Request_T;

static os_mutex_t ESP01S__Mutex;
////////////////////////////////////////////////////////////////////////////////
////

void ESP01S_Init(ESP01S_Device_T * device, ESP01S_IO_T* IO)
{
    device->device_io = IO;
    os_mutex_init(&ESP01S__Mutex);
}

int ESP01S_RequestWithCmd(ESP01S_Device_T* device
                          , ESP01S_RxHandler handler, void* userdata
                          , uint32_t timeout_ms, const char* cmd)
{
    int err = 0;
    os_mutex_lock(&ESP01S__Mutex);
    device->device_io->set_rx_handler(handler, userdata);
    device->device_io->send((uint8_t*)cmd, (int)strlen(cmd));
    err = device->device_io->wait(timeout_ms);
    device->device_io->set_rx_handler(0, 0);
    os_mutex_unlock(&ESP01S__Mutex);
    return err;
}

static uint8_t ESP01S__RequestBuffer[256]={0};
int ESP01S_RequestWithArgs(ESP01S_Device_T* device, ESP01S_RxHandler handler, void* userdata, uint32_t timeout_ms, const char* fmt, ...)
{
    int buffer_size = sizeof(ESP01S__RequestBuffer);
    va_list args;
    
    os_mutex_lock(&ESP01S__Mutex);
    device->device_io->set_rx_handler(handler, userdata);
    
    va_start(args, fmt);
    int need_size = vsnprintf((char*)ESP01S__RequestBuffer, buffer_size, fmt, args);
    va_end(args);
    
    if(need_size>buffer_size){
        char* buffer = OS_ALLOC(need_size+1);
        if(!buffer) return -1;
        
        va_start(args, fmt);
        vsnprintf(buffer, need_size, fmt, args);
        va_end(args);
        buffer[need_size]='\0';
        
        device->device_io->send((uint8_t*)buffer, need_size);
        OS_FREE(buffer);
    }else{
        device->device_io->send(ESP01S__RequestBuffer, need_size);
    }
    int err = 0;
    err = device->device_io->wait(timeout_ms);
    device->device_io->set_rx_handler(0, 0);
    os_mutex_unlock(&ESP01S__Mutex);
    return err;
}


////////////////////////////////////////////////////////////////////////////////
////
static void ESP01S_CWMODE_Get_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    int* Mode = Request->userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_text_t text;
        int cut_result = sdk_ringbuffer_cut(&text, buffer, 0, sdk_ringbuffer_used(buffer), "+CWMODE:", "\r\n");
        if(cut_result==0){
            *Mode = (int)sdk_ringbuffer_strtoul(buffer, text.start, 0, 10);
        }
        sdk_ringbuffer_reset(buffer);
        
    }
}

int ESP01S_CWMODE_Get(ESP01S_Device_T * device, int* Mode, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = Mode};
    return ESP01S_RequestWithCmd(device, ESP01S_CWMODE_Get_Handler, &request, timeout_ms, "AT+CWMODE?\r\n");
}

////////////////////////////////////////////////////////////////////////////////
////
static void ESP01S_CWMODE_Set_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T * Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_CWMODE_Set(ESP01S_Device_T* device, int Mode, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithArgs(device, ESP01S_CWMODE_Set_Handler, &request, timeout_ms, "AT+CWMODE=%d\r\n", Mode);
}

////////////////////////////////////////////////////////////////////////////////
////
static void ESP01S_Connect_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T * Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}


int ESP01S_Connect(ESP01S_Device_T* device, const char* SSID, const char* Password, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithArgs(device, ESP01S_Connect_Handler, &request, timeout_ms, "AT+CWJAP=\"%s\",\"%s\"\r\n"
                                  , SSID
                                  , Password);
}

////////////////////////////////////////////////////////////////////////////////
////
static void ESP01S_Get_IP_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T * Request = userdata;
    ESP01S_IP_T* IP = Request->userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        int used = sdk_ringbuffer_used(buffer);
        sdk_ringbuffer_text_t text;
        int cut = sdk_ringbuffer_cut(&text, buffer, 0, used, "+CIPSTA:ip:\"", "\"\r\n");
        
        if(cut!=0){
            goto __ESP01S_Get_IP_Handler__EXIT;
        }
        
        memset(IP->IPv4, 0, sizeof(IP->IPv4));
        sdk_ringbuffer_memcpy((uint8_t*)IP->IPv4, buffer, text.start, text.end-text.start);
        
        cut = sdk_ringbuffer_cut(&text, buffer, text.end, used, "+CIPSTA:gateway:\"", "\"\r\n");
        if(cut!=0){
            goto __ESP01S_Get_IP_Handler__EXIT;
        }
        memset(IP->Gateway, 0, sizeof(IP->Gateway));
        sdk_ringbuffer_memcpy((uint8_t*)IP->Gateway, buffer, text.start, text.end-text.start);
        
        cut = sdk_ringbuffer_cut(&text, buffer, text.end, used, "+CIPSTA:netmask:\"", "\"\r\n");
        if(cut!=0){
            goto __ESP01S_Get_IP_Handler__EXIT;
        }
        memset(IP->Netmask, 0, sizeof(IP->Netmask));
        sdk_ringbuffer_memcpy((uint8_t*)IP->Netmask, buffer, text.start, text.end-text.start);
        
__ESP01S_Get_IP_Handler__EXIT:
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}


int ESP01S_Get_IP(ESP01S_Device_T* device, ESP01S_IP_T * IP, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = IP};
    return ESP01S_RequestWithCmd(device, ESP01S_Get_IP_Handler, &request, timeout_ms, "AT+CIPSTA?\r\n");
}

////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_CIPSTATUS_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    int* Status = Request->userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        
        sdk_ringbuffer_text_t text;
        int cut = sdk_ringbuffer_cut(&text, buffer, 0, sdk_ringbuffer_used(buffer), "STATUS:", "\r\n");
        if(cut!=0){
            goto __ESP01S_CIPSTATUS_Handler__Exit;
        }
        
        *Status = (int)sdk_ringbuffer_strtoul(buffer, text.start, 0, 10);
        
__ESP01S_CIPSTATUS_Handler__Exit:
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_CIPSTATUS(ESP01S_Device_T* device, int* Status, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = Status};
    return ESP01S_RequestWithCmd(device, ESP01S_CIPSTATUS_Handler, &request, timeout_ms, "AT+CIPSTATUS\r\n");
}


////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_TCP_Connect_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_TCP_Connect(ESP01S_Device_T* device, const char* Host, int Port, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithArgs(device, ESP01S_TCP_Connect_Handler, &request, timeout_ms
                                  , "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", Host, Port);
}


////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_TCPv6_Connect_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_TCPv6_Connect(ESP01S_Device_T* device, const char* Host, int Port, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithArgs(device, ESP01S_TCPv6_Connect_Handler, &request, timeout_ms
            , "AT+CIPSTART=\"TCPv6\",\"%s\",%d\r\n", Host, Port);
}


////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_UDP_Connect_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_UDP_Connect(ESP01S_Device_T* device
                       , const char* Host, int Port, int LocalPort, int Mode
                       , uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithArgs(device, ESP01S_UDP_Connect_Handler, &request, timeout_ms
            , "AT+CIPSTART=\"UDP\",\"%s\",%d,%d,%d\r\n", Host, Port, LocalPort, Mode);
}

////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_UDPv6_Connect_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_UDPv6_Connect(ESP01S_Device_T* device
                       , const char* Host, int Port, int LocalPort, int Mode
                       , uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithArgs(device, ESP01S_UDPv6_Connect_Handler, &request, timeout_ms
            , "AT+CIPSTART=\"UDPv6\",\"%s\",%d,%d,%d\r\n", Host, Port, LocalPort, Mode);
}



////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_SSL_Connect_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_SSL_Connect(ESP01S_Device_T* device
                       , const char* Host, int Port
                       , uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithArgs(device, ESP01S_SSL_Connect_Handler, &request, timeout_ms
            , "AT+CIPSTART=\"SSL\",\"%s\",%d\r\n", Host, Port);
}




////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_CIPSEND_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_CIPSEND(ESP01S_Device_T* device, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithCmd(device, ESP01S_CIPSEND_Handler, &request, timeout_ms
            , "AT+CIPSEND\r\n");
}

////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_CIPCLOSE_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_CIPCLOSE(ESP01S_Device_T* device, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithCmd(device, ESP01S_CIPSEND_Handler, &request, timeout_ms
            , "AT+CIPCLOSE\r\n");
}



////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_CIFSR_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    ESP01S_CIFSR_T * Result = Request->userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        
        sdk_ringbuffer_text_t text;
        int cut = sdk_ringbuffer_cut(&text, buffer, 0, sdk_ringbuffer_used(buffer), "+CIFSR:STAIP,\"", "\"\r\n");
        if(cut!=0){
            goto __ESP01S_CIFSR_Handler__EXIT;
        }
        
        memset(Result->IPv4, 0, sizeof(Result->IPv4));
        sdk_ringbuffer_memcpy(Result->IPv4, buffer, text.start, text.end-text.start);
        
        cut = sdk_ringbuffer_cut(&text, buffer, 0, sdk_ringbuffer_used(buffer), "+CIFSR:STAMAC,\"", "\"\r\n");
        if(cut!=0){
            goto __ESP01S_CIFSR_Handler__EXIT;
        }
        
        memset(Result->MAC, 0, sizeof(Result->MAC));
        sdk_ringbuffer_memcpy(Result->MAC, buffer, text.start, text.end-text.start);
    
    
__ESP01S_CIFSR_Handler__EXIT:
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_CIFSR(ESP01S_Device_T* device, ESP01S_CIFSR_T* Result, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = Result};
    return ESP01S_RequestWithCmd(device, ESP01S_CIFSR_Handler, &request, timeout_ms
            , "AT+CIFSR\r\n");
}


////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_CIPSNTPTIME_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    struct tm * Result = Request->userdata;
    sdk_ringbuffer_text_t text;
    int used = sdk_ringbuffer_used(buffer);

    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_hex_dump("ESP01S_CIPSNTPTIME_Handler", buffer->buffer, used);
        int cut = sdk_ringbuffer_cut(&text, buffer, 0, used, "+CIPSNTPTIME:", "\r\n");
        
        if(sdk_ringbuffer_strcmp(buffer, text.start, text.start+3, "Mon")==0){
            Result->tm_wday = 1;
        }else if(sdk_ringbuffer_strcmp(buffer, text.start, text.start+3, "Tue")==0){
            Result->tm_wday = 2;
        }else if(sdk_ringbuffer_strcmp(buffer, text.start, text.start+3, "Wed")==0){
            Result->tm_wday = 3;
        }else if(sdk_ringbuffer_strcmp(buffer, text.start, text.start+3, "Thu")==0){
            Result->tm_wday = 4;
        }else if(sdk_ringbuffer_strcmp(buffer, text.start, text.start+3, "Fri")==0){
            Result->tm_wday = 5;
        }else if(sdk_ringbuffer_strcmp(buffer, text.start, text.start+3, "Sat")==0){
            Result->tm_wday = 6;
        }else if(sdk_ringbuffer_strcmp(buffer, text.start, text.start+3, "Sun")==0){
            Result->tm_wday = 0;
        }
        int idx = text.start + 4;
        
        if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Jan")==0){
            Result->tm_mon = 0;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Feb")==0){
            Result->tm_mon = 1;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Mar")==0){
            Result->tm_mon = 2;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Apr")==0){
            Result->tm_mon = 3;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "May")==0){
            Result->tm_mon = 4;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Jun")==0){
            Result->tm_mon = 5;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Jul")==0){
            Result->tm_mon = 6;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Aug")==0){
            Result->tm_mon = 7;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Sep")==0){
            Result->tm_mon = 8;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Oct")==0){
            Result->tm_mon = 9;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Nov")==0){
            Result->tm_mon = 10;
        }else if(sdk_ringbuffer_strcmp(buffer, idx, idx + 3, "Dec")==0){
            Result->tm_mon = 11;
        }
        
        Result->tm_mday = (int)sdk_ringbuffer_strtoul(buffer, text.start+8, 0, 10);
        Result->tm_hour = (int)sdk_ringbuffer_strtoul(buffer, text.start+11, 0, 10);
        Result->tm_min = (int)sdk_ringbuffer_strtoul(buffer, text.start+14, 0, 10);
        Result->tm_sec = (int)sdk_ringbuffer_strtoul(buffer, text.start+17, 0, 10);
        Result->tm_year = (int)sdk_ringbuffer_strtoul(buffer, text.start+20, 0, 10);
        
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
    
}

int ESP01S_CIPSNTPTIME(ESP01S_Device_T* device, struct tm* Result, uint32_t timeout_ms)
{
    ESP01S_Request_T request = {.device = device, .userdata = Result};
    return ESP01S_RequestWithCmd(device, ESP01S_CIPSNTPTIME_Handler, &request, timeout_ms
            , "AT+CIPSNTPTIME?\r\n");
}


////////////////////////////////////////////////////////////////////////////////
////

static void ESP01S_CIPSNTPCFG_Handler(sdk_ringbuffer_t* buffer, void* userdata){
    ESP01S_Request_T* Request = userdata;
    if(sdk_ringbuffer_find_str(buffer, 0, "OK\r\n")!=-1){
        sdk_ringbuffer_reset(buffer);
        Request->device->device_io->notify();
    }
}

int ESP01S_CIPSNTPCFG(ESP01S_Device_T* device, int TimeZone, uint32_t timeout_ms){
    ESP01S_Request_T request = {.device = device, .userdata = 0};
    return ESP01S_RequestWithArgs(device, ESP01S_CIPSNTPCFG_Handler, &request, timeout_ms
            , "AT+CIPSNTPCFG=1,%d\r\n", TimeZone);
}

