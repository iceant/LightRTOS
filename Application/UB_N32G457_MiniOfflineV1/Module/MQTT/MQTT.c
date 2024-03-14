#include <MQTT.h>
#include <A7670C.h>
#include <A7670C_MQTT.h>
#include <os_kernel.h>
#include <sdk_ring.h>
#include <stdio.h>
#include <bsp_cpuid.h>
#include <config.h>
#include <sdk_fmt.h>
#include <sdk_hex.h>
////////////////////////////////////////////////////////////////////////////////
////
#define TX_BUFFER_SZ (1024*4)
#define TX_TASK_COUNT 6
#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY 20
#define THREAD_TIMESLICE 10


////////////////////////////////////////////////////////////////////////////////
////
typedef struct MQTT_TxTask_S{
    uint8_t buffer[TX_BUFFER_SZ];
    int size;
}MQTT_TxTask_T;

////////////////////////////////////////////////////////////////////////////////
////
#define OBJECT_SIZE sizeof(MQTT_TxTask_T)
#define OBJECT_COUNT TX_TASK_COUNT
#define OBJECT_BLOCK_SIZE (OBJECT_COUNT * OBJECT_SIZE)

static uint8_t MQTT__TxTaskBlock[OBJECT_BLOCK_SIZE];
static sdk_ring_t MQTT__TxTaskRing;
////////////////////////////////////////////////////////////////////////////////
////

static A7670C_MQTT_Session session;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t MQTT__TxThreadStack[THREAD_STACK_SIZE];
static os_thread_t MQTT__TxThread;
static char MQTT__Topic_Upstream[256]={0};
static char MQTT__Topic_Downstream[256]={0};
static os_sem_t MQTT_TxSem;
static os_mutex_t MQTT_Lock;
////////////////////////////////////////////////////////////////////////////////
////
static void MQTT__RxDataHandler(void* data, int data_size, void* userdata)
{
    sdk_hex_dump("MQTT_RxDataHandler", data, data_size);
    if(strstr((char*)data, "config reboot=1")){
        __svc(1);
    }
}

static void MQTT__OnConnectLost(void){
    A7670C_Result result;
    int nRetry = 3;
    while(1){
        result = A7670C_MQTT_Connect(&session
                , BSP_CPUID_GetUIDStr()
                , CONFIG_MQTT_SERVER
                ,  64800
                , true
                , CONFIG_MQTT_USERNAME
                , CONFIG_MQTT_PASSWORD);
        if(result==kA7670C_Result_OK){
            break;
        }
        printf("MQTT Connect Failed! Retry...\n");
        A7670C_NopDelay(0x7FFFFF);
        if(nRetry--==0){
            __svc(1); // reboot
        }
    }

    nRetry = 3;
    while(1){
        result = A7670C_MQTT_SubscribeOneTopic(&session, MQTT__Topic_Downstream, kA7670C_Qos_0, kA7670C_Bool_No);
        if(result==kA7670C_Result_OK){
            break;
        }
        printf("MQTT Subscribe Failed! Retry...\n");
        A7670C_NopDelay(0x7FFFFF);
        if(nRetry--==0){
            __svc(1); // reboot
        }
    }
}



static void MQTT__TxThreadEntry(void* p){
    MQTT_TxTask_T * task = 0;
    MQTT__OnConnectLost();
    while(1){
        os_sem_take(&MQTT_TxSem, OS_WAIT_INFINITY);
        int used = sdk_ring_used(&MQTT__TxTaskRing);
        for(int i=0; i<used; i++){
            task = sdk_ring_pop(&MQTT__TxTaskRing);
            if(task!=0){
//                printf("MQTT_TxThread Exec Publish...\n");
//                sdk_hex_dump("MQTT_TxThreadPub", task->buffer, task->size);
                A7670C_Result result = A7670C_MQTT_Publish(&session, MQTT__Topic_Upstream
                                    , task->buffer, task->size
                                    , kA7670C_Qos_1
                                    , 60
                                    , kA7670C_Bool_No
                                    , kA7670C_Bool_No);
                if(result!=kA7670C_Result_OK){
                    /* 尝试重连后再发送，如果还是失败，丢弃 */
                    MQTT__OnConnectLost();
                    result = A7670C_MQTT_Publish(&session, MQTT__Topic_Upstream
                            , task->buffer, task->size
                            , kA7670C_Qos_1
                            , 60
                            , kA7670C_Bool_No
                            , kA7670C_Bool_No);
                }

//                if(result==kA7670C_Result_OK){
//                    sdk_hex_dump("MQTT_Send", task->buffer, 32);
//                }
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
////

void MQTT_Init(void)
{
    sdk_fmt_sfmt(MQTT__Topic_Upstream, sizeof(MQTT__Topic_Upstream), "/DEVICES/%s/UPSTREAM", BSP_CPUID_GetUIDStr());
    sdk_fmt_sfmt(MQTT__Topic_Downstream, sizeof(MQTT__Topic_Downstream), "/DEVICES/%s/DOWNSTREAM", BSP_CPUID_GetUIDStr());

    A7670C_MQTT_Init(&session, MQTT__RxDataHandler, 0, MQTT__OnConnectLost);

    sdk_ring_init(&MQTT__TxTaskRing, MQTT__TxTaskBlock, OBJECT_COUNT, OBJECT_SIZE);
    os_sem_init(&MQTT_TxSem, "MQTT_TxSem", 0, OS_QUEUE_FIFO);
    os_mutex_init(&MQTT_Lock);
    os_thread_init(&MQTT__TxThread, "MQTT_TxThd"
                   , MQTT__TxThreadEntry, 0
                   , MQTT__TxThreadStack, sizeof(MQTT__TxThreadStack)
                   , THREAD_PRIORITY, THREAD_TIMESLICE);
    os_thread_startup(&MQTT__TxThread);
}

int MQTT_Publish(void* data, int size){
    printf("MQTT Publish\n");
    MQTT_TxTask_T * task = 0;

    os_mutex_lock(&MQTT_Lock);
    task = sdk_ring_get_write_slot(&MQTT__TxTaskRing);
    os_mutex_unlock(&MQTT_Lock);

    if(task){
        memcpy(task->buffer, data, size);
        task->size = size;
        os_sem_release(&MQTT_TxSem);
    }

    return 0;
}


