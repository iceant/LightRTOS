#include <EnergyMeter.h>
#include <bsp_can2.h>
#include <bsp_tim2.h>
#include <sdk_hex.h>
#include <stdio.h>
#include <IM1253E.h>
#include <sdk_ring.h>
#include <sdk_fmt.h>
#include <cpu_spinlock.h>
////////////////////////////////////////////////////////////////////////////////
////

#define HOUR_IN_MS 1000*60*60
#define VOLTAGE_UNIT 10000
#define CURRENT_UNIT 1000

#define VOLTAGE_THREAD_STACK 1024
#define VOLTAGE_THREAD_PRIORITY 20
#define VOLTAGE_THREAD_TIMESLICE 10
#define VOLTAGE_THREAD_TIME_MS 130


#define OBJECT_SIZE sizeof(EnergyMeter_Data_T)
#define OBJECT_COUNT 256
#define RING_BLOCK_SIZE (OBJECT_COUNT * OBJECT_SIZE)

#define CALCULATE_THREAD_STACK 1024
#define CALCULATE_THREAD_PRIORITY 22
#define CALCULATE_THREAD_TIMESLICE 10

////////////////////////////////////////////////////////////////////////////////
////

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t Voltage_ThreadStack[VOLTAGE_THREAD_STACK];
static os_thread_t Voltage_Thread;
static os_bool_t Voltage_ThreadFlag = OS_FALSE;

static uint8_t EnergyMeter__DataBlock[RING_BLOCK_SIZE];

static volatile uint32_t EnergyMeter__Voltage=0;
static sdk_ring_t EnergyMeter__DataRing;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t Calculate_ThreadStack[CALCULATE_THREAD_STACK];
static os_thread_t Calculate_Thread;

static sdk_ap_t Calculate__PowerMs=0;
static sdk_ap_t Calculate__EnergyWMs=0;
//static os_sem_t Calculate__Sem;

////////////////////////////////////////////////////////////////////////////////
////
/*
 1078 ms - 110条数据，平均 10ms/1条 数据
 */
static void CurrentSensor__DataHandler(BSP_CAN2_Message * Message, void* userdata){
    EnergyMeter_Data_T Data;
    uint32_t TimeMS = BSP_TIM2_TickCount;
    uint32_t Current = SDK_HEX_GET_UINT32_BE(Message->Data, 0) - 0x80000000 ;

    Data.Current = Current;
    Data.Voltage = EnergyMeter__Voltage;
    Data.TimeMS = TimeMS;
    sdk_ring_put(&EnergyMeter__DataRing, &Data);
}

uint32_t Voltage__Latest=0;
static void Voltage_ThreadEntry(void* p){
    Voltage_ThreadFlag = OS_TRUE;
    while(1){
        IM1253E_GetVoltage((uint32_t*)&EnergyMeter__Voltage, VOLTAGE_THREAD_TIME_MS);
        if(Voltage__Latest!=EnergyMeter__Voltage){
            Voltage__Latest = EnergyMeter__Voltage;
            printf("Update Voltage: %d, Tick:%d\n", Voltage__Latest, BSP_TIM2_TickCount);
        }
    }
}

static void Calculate_ThreadEntry(void* p){
    while(Voltage_ThreadFlag==OS_FALSE); /* Wait for Voltage Thread Ready */
    
    sdk_ap_t PowerMS = sdk_ap_new(0);
    EnergyMeter_Data_T* LatestData = 0;
    
    sdk_ap_t C_Unit = sdk_ap_new(CURRENT_UNIT);/* Current: 0.001 */
    sdk_ap_t V_Unit = sdk_ap_muli(C_Unit,  VOLTAGE_UNIT/10); /*Voltage: 0.0001*/
    sdk_ap_t T_Unit = sdk_ap_muli(V_Unit, HOUR_IN_MS);
//    sdk_ap_t K_Unit = sdk_ap_muli(V_Unit, 1000);
    int nCount = 0;
    
    BSP_CAN2_SetRxHandler(CurrentSensor__DataHandler, 0);
    printf("Calculate Thread Startup...\n");
    while(1){
//        os_sem_take(&Calculate__Sem, OS_WAIT_INFINITY);
        os_size_t used = sdk_ring_used(&EnergyMeter__DataRing);
        for(os_size_t i=0; i<used; i++){
            EnergyMeter_Data_T* Data = sdk_ring_pop(&EnergyMeter__DataRing);

            if(Data->Current==0 || Data->Voltage==0) {
                continue;
            }

            if(LatestData==0){
                LatestData = Data;
            }else{
                sdk_ap_mul2i(PowerMS, Data->Current, Data->Voltage);
                uint32_t DeltaMS = BSP_TIM2_TickDiff(LatestData->TimeMS, Data->TimeMS);
                sdk_ap_t EnergyMS_p = sdk_ap_muli(PowerMS, DeltaMS);
                sdk_ap_t EnergyWMs = sdk_ap_add(Calculate__EnergyWMs, EnergyMS_p);
                sdk_ap_t EnergyWh = sdk_ap_div(EnergyWMs, T_Unit);

                if(nCount++==1000){
                    sdk_fmt_print("Tick: %d, DeltaMS: %d, Current:%d, Voltage:%d, EnergyWMs: %E, EnergyWh: %E (0.0001) \n"
                            , LatestData->TimeMS
                            , DeltaMS
                            , Data->Current
                            , Data->Voltage
                            , EnergyWMs
                            , EnergyWh
                    );
                    nCount=0;
                }


                sdk_ap_free(&EnergyWh);
                sdk_ap_free(&EnergyMS_p);
                sdk_ap_free(&Calculate__EnergyWMs);
                
                Calculate__EnergyWMs = EnergyWMs;
                LatestData = Data;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////


void EnergyMeter_Init(void)
{
    sdk_fmt_register('E', sdk_ap_fmt);
    
    Calculate__PowerMs = sdk_ap_new(0);
    Calculate__EnergyWMs = sdk_ap_new(0);

//    os_sem_init(&Calculate__Sem, "Cal_Sem", 0, OS_QUEUE_FIFO);
    
    // TODO: 加载总电能值
    
    sdk_ring_init(&EnergyMeter__DataRing, EnergyMeter__DataBlock, OBJECT_COUNT, OBJECT_SIZE);
    
    os_thread_init(&Voltage_Thread, "Vol_Thread"
                   , Voltage_ThreadEntry, 0
                   , Voltage_ThreadStack, sizeof(Voltage_ThreadStack)
                   , VOLTAGE_THREAD_PRIORITY
                   , VOLTAGE_THREAD_TIMESLICE);
    os_thread_startup(&Voltage_Thread);
    
    os_thread_init(&Calculate_Thread, "EngCal"
            , Calculate_ThreadEntry, 0
            , Calculate_ThreadStack, sizeof(Calculate_ThreadStack)
            , CALCULATE_THREAD_PRIORITY
            , CALCULATE_THREAD_TIMESLICE);
    os_thread_startup(&Calculate_Thread);
    
}

