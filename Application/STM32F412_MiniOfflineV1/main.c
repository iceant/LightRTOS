#include <main.h>
#include "board.h"
#include <os_kernel.h>
#include <stdio.h>
#include <sdk_ringbuffer.h>
#include <sdk_fmt.h>


////////////////////////////////////////////////////////////////////////////////
////
#define USART1_RX_BLOCK_SZ 256
#define USART1_RX_THREAD_STACK_SIZE 1024

static uint8_t USART1_RxBlock[USART1_RX_BLOCK_SZ];
static sdk_ringbuffer_t USART1_RxBuffer;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t USART1_RxThreadStack[USART1_RX_THREAD_STACK_SIZE];
static os_thread_t USART1_RxThread;
static os_sem_t USART1_RxSem;

static void USART1_RxCallback(uint8_t data, void* userdata)
{
    sdk_ringbuffer_put(&USART1_RxBuffer, data);
    os_sem_release(&USART1_RxSem);
}

static void USART1_RxThreadEntry(void* parameter){
    printf("USART1_RxThreadEntry Init...\r\n");
    while(1){
        os_sem_take(&USART1_RxSem, OS_WAIT_INFINITY);

        if(sdk_ringbuffer_find_str(&USART1_RxBuffer, 0, "reboot")!=-1){
            cpu_reboot();
        }else if(sdk_ringbuffer_find_str(&USART1_RxBuffer, 0, "\r\n")!=-1){
            printf("[USART1] %s\r\n", USART1_RxBuffer.buffer);
            sdk_ringbuffer_reset(&USART1_RxBuffer);
        }
        if(sdk_ringbuffer_is_full(&USART1_RxBuffer)){
            sdk_ringbuffer_reset(&USART1_RxBuffer);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////
#define USART3_RX_BLOCK_SZ 256
#define USART3_RX_THREAD_STACK_SIZE 1024

static uint8_t USART3_RxBlock[USART3_RX_BLOCK_SZ];
static sdk_ringbuffer_t USART3_RxBuffer;

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t USART3_RxThreadStack[USART3_RX_THREAD_STACK_SIZE];
static os_thread_t USART3_RxThread;
static os_sem_t USART3_RxSem;

static void USART3_RxCallback(uint8_t data, void* userdata)
{
    sdk_ringbuffer_put(&USART3_RxBuffer, data);
    os_sem_release(&USART3_RxSem);
}

static void USART3_RxThreadEntry(void* parameter){
    printf("USART3_RxThreadEntry Init...\r\n");
    while(1){
        os_sem_take(&USART3_RxSem, OS_WAIT_INFINITY);
        if(sdk_ringbuffer_find_str(&USART3_RxBuffer, 0, "\r\n")!=-1){
//            printf("[USART3] %s\r\n", USART3_RxBuffer.buffer);
            sdk_ringbuffer_reset(&USART3_RxBuffer);
        }
        if(sdk_ringbuffer_is_full(&USART3_RxBuffer)){
            sdk_ringbuffer_reset(&USART3_RxBuffer);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////
#define THREAD1_STACK_SIZE 1024

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t Boot_Thread_stack[THREAD1_STACK_SIZE];
static os_thread_t Boot_Thread;

static int32_t ADS1256_ADC[8]={0};
static int32_t ADS1256_ADC_V[8]={0};
static double ADS1256_Vol[8] = {0};
////////////////////////////////////////////////////////////////////////////////
////


static void Boot_Thread_Entry(void* p){
//    BSP_USART1_SendString("Boot Thread Entry Enter...\n");
    const char* message = "Boot Thread Entry Enter...\n";
    BSP_USART1_SendString(message);
//    BSP_USART1_DMA_Send((uint8_t *)message, strlen(message));

    /* -------------------------------------------------------------------------------------------------------------- */
    /* 初始化接收线程 */
    /* -------------------------------------------------------------------------------------------------------------- */
    sdk_ringbuffer_init(&USART1_RxBuffer, USART1_RxBlock, USART1_RX_BLOCK_SZ);
    os_sem_init(&USART1_RxSem, "USART1_RxSem", 0, OS_QUEUE_FIFO);
    BSP_USART1_SetReceiveCallback(USART1_RxCallback, 0);

    os_thread_init(&USART1_RxThread, "USART1_RxThd", USART1_RxThreadEntry, 0
                   , USART1_RxThreadStack, sizeof(USART1_RxThreadStack), 20, 10);
    os_thread_startup(&USART1_RxThread);

    /* -------------------------------------------------------------------------------------------------------------- */
    /* 初始化 USART3 线程 */
    /* -------------------------------------------------------------------------------------------------------------- */
    sdk_ringbuffer_init(&USART3_RxBuffer, USART3_RxBlock, USART3_RX_BLOCK_SZ);
    os_sem_init(&USART3_RxSem, "USART3_RxSem", 0, OS_QUEUE_FIFO);
    BSP_USART3_SetReceiveCallback(USART3_RxCallback, 0);

    os_thread_init(&USART3_RxThread, "USART3_RxThd", USART3_RxThreadEntry, 0
                   , USART3_RxThreadStack, sizeof(USART3_RxThreadStack), 20, 10);
    os_thread_startup(&USART3_RxThread);

    /* -------------------------------------------------------------------------------------------------------------- */
    /* 主线程 */
    /* -------------------------------------------------------------------------------------------------------------- */

    uint32_t nCount = 0;

    ADS1256_SetScanMode(ADS135X_SCAN_MODE_SIG);
    int ch_num = 1;

    char dis_buf[32];					// 显示缓冲区
    char dis_buf2[32];					// 显示缓冲区
    char dis_buf3[32];					// 显示缓冲区
    int32_t adc[8];					// 采样结果
    int32_t adc_1[8];				//
    unsigned long ulResult;
    long double volt[8];					// 实际电压值

#define V_RATIO 0.50310559006211200000
#define LSB 0.00597852457311875000


    while(1){
        printf("nCount:%d \r\n", nCount++);

#if 1
        for (int i = 0; i < ch_num; i++)
        {

            adc[i] = (int32_t)ADS1256_GetAdc(i);
//            adc_1[i] = (adc[i] ^ 0x800000);				// 4194303 = 2.5V , 这是理论值，实际可以根据2.5V基准的实际值进行公式矫正
//            volt[i] = (((0.596047*adc_1[i])-5000000)/1000000);  // 测量出来是以v作单位

//            printf("CH%1d: %d(0x%08x)", i, adc[i], adc[i]); //发送数据
//            printDouble(adc[i] * 0.000596046518808, 15);
//            printf("\r\n");
//            ftoa(adc[i] *  0.000596046518808 , fbuf, sizeof(fbuf));
//            printf("CH%1d: %f\r\n", i, adc[i] *  0.000596046518808);

//            sdk_fmt_print("CH:%d 0x%08x(%d) %f\r\n", i, adc[i], adc[i], (float)(adc[i] * 0.596046518808));
//            if(adc[i]==0xffffffff || adc[i]==0x00001000 ||adc[i]==0x003fffff){
//                printf("CH%d INVALID!!!\r\n", i);
//                break;
//            }
//            volt[i] = (float)(adc[i]* 0.005960465188081880f);
            volt[i] = (float)(adc[i]* LSB

            );

            float Vstd = 24988.0f;
            float Vo = volt[i] / V_RATIO;
            float Pv = (Vstd-Vo)/Vstd;
            printf("CH:%d 0x%08x(%d) %s %s %s\r\n", i, adc[i], adc[i]
                   , ftoa(volt[i], dis_buf, sizeof(dis_buf))
                   , ftoa(Vo, dis_buf2, sizeof(dis_buf2))
                   , ftoa(Pv, dis_buf3, sizeof(dis_buf3))
                   );
        }
//        printf("\r\n");

        os_thread_mdelay(1000);
#endif

    }
}

////////////////////////////////////////////////////////////////////////////////
////

int main(void){
    board_init();

    const char* message = "Board Init Finished!\r\n";
    BSP_USART1_SendString(message);
//    BSP_USART1_DMA_Send((uint8_t *)message, strlen(message));

#if 0
    printf("ADS1256_StartScan 1\r\n");
    ADS1256_StartScan();
    printf("ADS1256_StartScan 2\r\n");
#endif

    os_kernel_init();

    os_thread_init(&Boot_Thread, "BootThd", Boot_Thread_Entry, 0, Boot_Thread_stack, sizeof(Boot_Thread_stack), 10, 10);
    os_thread_startup(&Boot_Thread);

    os_kernel_startup();

}

