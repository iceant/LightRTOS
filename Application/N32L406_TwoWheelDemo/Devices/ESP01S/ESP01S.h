#ifndef INCLUDED_ESP01S_H
#define INCLUDED_ESP01S_H

////////////////////////////////////////////////////////////////////////////////
////
#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif /*INCLUDED_STDINT_H*/

#ifndef INCLUDED_STDARG_H
#define INCLUDED_STDARG_H
#include <stdarg.h>
#endif /*INCLUDED_STDARG_H*/

#ifndef INCLUDED_SDK_RINGBUFFER_H
#include <sdk_ringbuffer.h>
#endif /* INCLUDED_SDK_RINGBUFFER_H */

#ifndef INCLUDED_TIME_H
#define INCLUDED_TIME_H
#include <time.h>
#endif /*INCLUDED_TIME_H*/


////////////////////////////////////////////////////////////////////////////////
////

#define ESP01S_RX_DONE      0
#define ESP01S_RX_CONTINUE  1
#define ESP01S_RX_RESET     2

////////////////////////////////////////////////////////////////////////////////
////
typedef int (*ESP01S_RxHandler)(sdk_ringbuffer_t* buffer, void* userdata);

typedef struct ESP01S_IO_S{
    int (*send)(uint8_t* data, int size);
    void (*set_rx_handler)(void* handler, void* userdata);
    int (*wait)(uint32_t timeout_ms);
    void (*notify)(void);
}ESP01S_IO_T;

typedef struct ESP01S_Device_S{
    ESP01S_IO_T * device_io;
}ESP01S_Device_T;

typedef struct ESP01S_IP_S{
    char IPv4[16];
    char Gateway[16];
    char Netmask[16];
}ESP01S_IP_T;

typedef struct ESP01S_CIFSR_S{
    char IPv4[16];
    char MAC[18];
}ESP01S_CIFSR_T;
////////////////////////////////////////////////////////////////////////////////
////

void ESP01S_Init(ESP01S_Device_T * device, ESP01S_IO_T* IO);

int ESP01S_RequestWithCmd(ESP01S_Device_T* device, ESP01S_RxHandler handler, void* userdata, uint32_t timeout_ms, const char* cmd);
int ESP01S_RequestWithArgs(ESP01S_Device_T* device, ESP01S_RxHandler handler, void* userdata, uint32_t timeout_ms, const char* fmt, ...);

////////////////////////////////////////////////////////////////////////////////
////
int ESP01S_CWMODE_Get(ESP01S_Device_T * device, int* Mode, uint32_t timeout_ms);
int ESP01S_CWMODE_Set(ESP01S_Device_T* device, int Mode, uint32_t timeout_ms);
int ESP01S_Connect(ESP01S_Device_T* device, const char* SSID, const char* Password, uint32_t timeout_ms);
int ESP01S_Get_IP(ESP01S_Device_T* device, ESP01S_IP_T * IP, uint32_t timeout_ms);

int ESP01S_CIPSTATUS(ESP01S_Device_T* device, int* Status, uint32_t timeout_ms);
int ESP01S_TCP_Connect(ESP01S_Device_T* device, const char* Host, int Port, uint32_t timeout_ms);
int ESP01S_TCPv6_Connect(ESP01S_Device_T* device, const char* Host, int Port, uint32_t timeout_ms);
int ESP01S_UDP_Connect(ESP01S_Device_T* device
        , const char* Host, int Port, int LocalPort, int Mode
        , uint32_t timeout_ms);
int ESP01S_UDPv6_Connect(ESP01S_Device_T* device
        , const char* Host, int Port, int LocalPort, int Mode
        , uint32_t timeout_ms);
int ESP01S_SSL_Connect(ESP01S_Device_T* device
        , const char* Host, int Port
        , uint32_t timeout_ms);
int ESP01S_CIPSEND(ESP01S_Device_T* device, uint32_t timeout_ms);
int ESP01S_CIPCLOSE(ESP01S_Device_T* device, uint32_t timeout_ms);
int ESP01S_CIFSR(ESP01S_Device_T* device, ESP01S_CIFSR_T* Result, uint32_t timeout_ms);
int ESP01S_CIPSNTPTIME(ESP01S_Device_T* device, struct tm* Result, uint32_t timeout_ms);
int ESP01S_CIPSNTPCFG(ESP01S_Device_T* device, int TimeZone, uint32_t timeout_ms);
#endif /* INCLUDED_ESP01S_H */
