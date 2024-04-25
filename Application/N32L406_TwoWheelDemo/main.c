#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <sdk_hex.h>
#include <sdk_ring.h>
#include <sdk_crc16.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
////

typedef struct BMS_Data_S{
    uint16_t TotalVoltage;
    uint16_t Current;
    uint16_t RemainAH; /*剩余容量*/
    uint16_t StdAH; /*标称容量*/
    uint16_t CycleTimes; /*循环次数*/
    uint16_t MfgDate; /*生产日期*/
    uint16_t BalanceState; /*均衡状态*/
    uint16_t BalanceStateH; /*均衡状态_高*/
    uint16_t ProtectStatus; /*保护状态*/
    uint8_t SoftVersion;
    uint8_t RSOC;
    uint8_t FET_MOS;
    uint8_t CellCount; /*电池串数*/
    uint8_t NTC_Count;
    uint16_t NTC_Values[6];
    uint16_t CellVoltages[30];
}BMS_Data_T;


#define THREAD1_STACK_SIZE 1024
#define CANRX_STACK_SIZE 1024

#define OBJECT_SIZE sizeof(CanRxMessage)
#define OBJECT_COUNT 10
#define BLOCK_SIZE (OBJECT_COUNT * OBJECT_SIZE)

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t BootThread_Stack[THREAD1_STACK_SIZE];
static os_thread_t BootThread;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t CANRxThread_Stack[CANRX_STACK_SIZE];
static os_thread_t CANRxThread;
static os_sem_t CANRxSem;
static os_sem_t CANRxWait;

static uint8_t CANRx_Block[BLOCK_SIZE];
static sdk_ring_t CANRxRing;
static bool CANRxThreadFlag = false;

static BMS_Data_T BMS_Data;
////////////////////////////////////////////////////////////////////////////////
////

typedef void (*BatteryDataHandler)(uint8_t * data, uint8_t size, void* userdata);

static BatteryDataHandler BatteryData__Handler=0;
static void* BatteryData__Handler_Userdata = 0;

static void CANRxThread_Entry(void* p){
    printf("CANRxThread_Entry Startup...\r\n");
    os_size_t used = 0;
    CANRxThreadFlag = true;
    while(1){
        os_sem_take(&CANRxSem, OS_WAIT_INFINITY);
        CanRxMessage * RxMsg = sdk_ring_pop(&CANRxRing);
        if(BatteryData__Handler){
            BatteryData__Handler(RxMsg->Data, RxMsg->DLC, BatteryData__Handler_Userdata);
        }
    }
}

static int CAN_TimedRequest(CanTxMessage * TxMsg, BatteryDataHandler handler, void* userdata, uint32_t timeout_ms){
    BatteryData__Handler = handler;
    BatteryData__Handler_Userdata = userdata;

    sdk_ring_reset(&CANRxRing);
    int ret = BSP_CAN1_Send(TxMsg);
    os_err_t err = os_sem_take(&CANRxWait, os_tick_from_millisecond(timeout_ms));

    BatteryData__Handler = 0;
    BatteryData__Handler_Userdata = 0;

    return err;
}


////////////////////////////////////////////////////////////////////////////////
////
static void BatteryHandler(uint8_t* data, uint8_t size, void* userdata){
    uint32_t id = (uint32_t)userdata;
//    printf("ID: %08x\r\n", id);
//    sdk_hex_dump("BAT", data, size);
    int idx = 0;
    if(size<=0){
        return;
    }
    switch (id) {
        case 0x100:
            BMS_Data.TotalVoltage = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            BMS_Data.Current = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            BMS_Data.RemainAH = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            break;
        case 0x101:
            BMS_Data.StdAH = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            BMS_Data.CycleTimes = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            BMS_Data.RSOC = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            break;
        case 0x102:
            BMS_Data.BalanceState = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            BMS_Data.BalanceStateH = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            BMS_Data.ProtectStatus = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            break;
        case 0x103:
            BMS_Data.FET_MOS = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            BMS_Data.MfgDate = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            BMS_Data.SoftVersion = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;
            break;
        case 0x104:
            BMS_Data.CellCount = data[0];
            BMS_Data.NTC_Count = data[1];
//            printf("BCOUN:%d, NTC_CNT:%d\r\n", BMS_Data.CellCount, BMS_Data.NTC_Count);
            break;
        case 0x105:
            BMS_Data.NTC_Values[0] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size==4) break;
            BMS_Data.NTC_Values[1] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size==6) break;
            BMS_Data.NTC_Values[2] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size==8) break;
            break;
        case 0x106:
            BMS_Data.NTC_Values[3] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size==4) break;
            BMS_Data.NTC_Values[4] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size==6) break;
            BMS_Data.NTC_Values[5] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size==8) break;
            break;
        case 0x107:
            BMS_Data.CellVoltages[0] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 4) break;
            BMS_Data.CellVoltages[1] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 6) break;
            BMS_Data.CellVoltages[2] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 8) break;
            break;
        case 0x108:
            BMS_Data.CellVoltages[3] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 4) break;
            BMS_Data.CellVoltages[4] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 6) break;
            BMS_Data.CellVoltages[5] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 7) break;
            break;
        case 0x109:
            BMS_Data.CellVoltages[6] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 4) break;
            BMS_Data.CellVoltages[7] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 6) break;
            BMS_Data.CellVoltages[8] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 8) break;
            break;
        case 0x10A:
            BMS_Data.CellVoltages[9] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 4) break;
            BMS_Data.CellVoltages[10] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 6) break;
            BMS_Data.CellVoltages[11] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 8) break;
            break;
        case 0x10B:
            BMS_Data.CellVoltages[12] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2; if(size == 4) break;
            BMS_Data.CellVoltages[13] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2; if(size == 6) break;
            BMS_Data.CellVoltages[14] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2; if(size == 8) break;
            break;
        case 0x10C:
            BMS_Data.CellVoltages[15] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 4) break;
            BMS_Data.CellVoltages[16] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 6) break;
            BMS_Data.CellVoltages[17] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 8) break;
            break;
        case 0x10D:
            BMS_Data.CellVoltages[18] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 4) break;
            BMS_Data.CellVoltages[19] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 6) break;
            BMS_Data.CellVoltages[20] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 8) break;
            break;
        case 0x10E:
            BMS_Data.CellVoltages[21] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 4) break;
            BMS_Data.CellVoltages[22] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 6) break;
            BMS_Data.CellVoltages[23] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 8) break;
            break;
        case 0x10F:
            BMS_Data.CellVoltages[24] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 4) break;
            BMS_Data.CellVoltages[25] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 6) break;
            BMS_Data.CellVoltages[26] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 8) break;
            break;
        case 0x110:
            BMS_Data.CellVoltages[27] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 4) break;
            BMS_Data.CellVoltages[28] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 6) break;
            BMS_Data.CellVoltages[29] = SDK_HEX_GET_UINT16_BE(data, idx); idx+=2;if(size == 8) break;
            break;
        default:
            break;
    }

    os_sem_release(&CANRxWait);
}

////////////////////////////////////////////////////////////////////////////////
////

static void BootThread_Entry(void* p){
    printf("BootThread Startup...\r\n");

    sdk_ring_init(&CANRxRing, CANRx_Block, OBJECT_COUNT, OBJECT_SIZE);
    os_sem_init(&CANRxSem, "CANRXSem", 0, OS_QUEUE_FIFO);
    os_sem_init(&CANRxWait, "CANRxWait", 0, OS_QUEUE_FIFO);
    os_thread_init(&CANRxThread, "CANRxThd", CANRxThread_Entry, 0, CANRxThread_Stack, sizeof(CANRxThread_Stack), 20, 10);
    os_thread_startup(&CANRxThread);

    CanTxMessage TxMsg;
    uint8_t RxData;
    uint32_t i = 0;
    int err = 0;

    while(1){
        for(i=0x100; i<=0x110; i++) {
            TxMsg.StdId = i;
            TxMsg.RTR = CAN_RTRQ_REMOTE;
            TxMsg.IDE = CAN_ID_STD;
            TxMsg.DLC = 0;
            err = CAN_TimedRequest(&TxMsg, BatteryHandler, (void*)i, 300);
        }
        os_thread_mdelay(1000);
    }
}

////////////////////////////////////////////////////////////////////////////////
////

static uint8_t ResponseData[256]={0};
static uint8_t CMD1[]={0xDD, 0xA5, 0x03, 0x00, 0xFF, 0xFD, 0x77};
static uint8_t CMD2[]={0xDD, 0xA5, 0x04, 0x00, 0xFF, 0xFC, 0x77};
static void USART2__RxHandler(sdk_ringbuffer_t * buffer, void * userdata){

    int start_idx = -1;
    int cmd = 0;
    int used = sdk_ringbuffer_used(buffer);
    if(used<7){
        return;
    }

    for(int i=0; i<used; i++){
        if(sdk_ringbuffer_peek(buffer, i)==0xDD){
            if(sdk_ringbuffer_peek(buffer, i+1)==0xA5
                && sdk_ringbuffer_peek(buffer, i+2)==0x03
                && sdk_ringbuffer_peek(buffer, i+3)==0x00
                && sdk_ringbuffer_peek(buffer, i+4)==0xFF
                && sdk_ringbuffer_peek(buffer, i+5)==0xFD
                && sdk_ringbuffer_peek(buffer, i+6)==0x77
                ){
                start_idx = i;
                cmd = 1;
                break;
            }else if(sdk_ringbuffer_peek(buffer, i+1)==0xA5
                && sdk_ringbuffer_peek(buffer, i+2)==0x04
                && sdk_ringbuffer_peek(buffer, i+3)==0x00
                && sdk_ringbuffer_peek(buffer, i+4)==0xFF
                && sdk_ringbuffer_peek(buffer, i+5)==0xFC
                && sdk_ringbuffer_peek(buffer, i+6)==0x77
                    ){
                start_idx = i;
                cmd = 2;
                break;
            }
        }
    }

//    printf("START_IDX=%d\r\n", start_idx);

    if(start_idx==-1){
        sdk_ringbuffer_reset(buffer);
        return;
    }

    /*指令1:正确*/
    if(cmd==1){
        int idx = 0;
        ResponseData[idx] = 0xDD; idx+=1;
        ResponseData[idx] = 0x03; idx+=1;
        ResponseData[idx] = 0x00; idx+=1; /*正确*/
        ResponseData[idx] = 0x00; idx+=1; /*数据长度*/
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.TotalVoltage); idx+=2;
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.Current); idx+=2;
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.RemainAH); idx+=2;
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.StdAH); idx+=2;
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.CycleTimes); idx+=2;
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.MfgDate); idx+=2;
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.BalanceState); idx+=2;
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.BalanceStateH); idx+=2;
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.ProtectStatus); idx+=2;
        ResponseData[idx] = BMS_Data.SoftVersion; idx+=1;
        ResponseData[idx] = BMS_Data.RSOC; idx+=1;
        ResponseData[idx] = BMS_Data.FET_MOS; idx+=1;
        ResponseData[idx] = BMS_Data.CellCount; idx+=1;
        ResponseData[idx] = BMS_Data.NTC_Count; idx+=1;
        for(int i=0; i<BMS_Data.NTC_Count; i++){
            SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.NTC_Values[i]); idx+=2;
        }
        ResponseData[3] = idx-4;

        uint16_t crc = sdk_crc16(ResponseData, idx);
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, crc); idx+=2;
        ResponseData[idx]=0x77; idx+=1;

        BSP_USART2_Send(ResponseData, idx);

    }else if(cmd==2){
        int idx = 0;
        ResponseData[idx] = 0xDD; idx+=1;
        ResponseData[idx] = 0x04; idx+=1;
        ResponseData[idx] = 0x00; idx+=1; /*正确*/
        ResponseData[idx] = 0x00; idx+=1; /*数据长度, offset=0x03*/
        for(int i=0; i<BMS_Data.CellCount; i++){
            SDK_HEX_SET_UINT16_BE(ResponseData, idx, BMS_Data.CellVoltages[i]); idx+=2;
        }
        ResponseData[3] = idx-4;

        uint16_t crc = sdk_crc16(ResponseData, idx);
        SDK_HEX_SET_UINT16_BE(ResponseData, idx, crc); idx+=2;
        ResponseData[idx]=0x77; idx+=1;

        BSP_USART2_Send(ResponseData, idx);

    }

    sdk_ringbuffer_reset(buffer);

    BSP_Led_Work_On();
    os_thread_mdelay(10);
    BSP_Led_Work_Off();

    BSP_Led_Network_On();
    os_thread_mdelay(10);
    BSP_Led_Network_Off();

}

////////////////////////////////////////////////////////////////////////////////
////
static void USART1__RxHandler(sdk_ringbuffer_t * buffer, void* userdata){

    BSP_Led_Work_Blink();

    if(sdk_ringbuffer_find_str(buffer, 0, "reboot")!=-1){
        cpu_reboot();
        return;
    }
    if(sdk_ringbuffer_find_str(buffer, 0, "\r\n")!=-1){
        printf("USART1: %s\n", buffer->buffer,sdk_ringbuffer_used(buffer));
        sdk_ringbuffer_reset(buffer);
    }
}

static void CAN1__RxHandler(CanRxMessage* rxMessage, void* userdata)
{
    if(CANRxThreadFlag){
        sdk_ring_put(&CANRxRing, rxMessage);
        os_sem_release(&CANRxSem);
    }
}
////////////////////////////////////////////////////////////////////////////////
////

int main(void){
    board_init();

    memset(&BMS_Data, 0, sizeof(BMS_Data));

    BSP_USART1_SetRxHandler(USART1__RxHandler, 0);
    BSP_USART2_SetRxHandler(USART2__RxHandler, 0);
    BSP_CAN1_SetRxHandler(CAN1__RxHandler, 0);

    os_kernel_init();

    os_thread_init(&BootThread, "BootThd", BootThread_Entry, 0
                   , BootThread_Stack, sizeof(BootThread_Stack), 10, 10);
    os_thread_startup(&BootThread);

    os_kernel_startup();
    
    while(1){}
}

