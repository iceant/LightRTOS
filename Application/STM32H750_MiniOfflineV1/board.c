#include "board.h"
#include <string.h>
#include <os_kernel.h>

////////////////////////////////////////////////////////////////////////////////
////
static UART_HandleTypeDef IO_USART1;


////////////////////////////////////////////////////////////////////////////////
////

#ifdef __GNUC__
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
    #define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
    #define GETCHAR_PROTOTYPE int fget(FILE* f)
#endif


PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&IO_USART1, (uint8_t *)&ch, 1, 1000);
    return (ch);
}

GETCHAR_PROTOTYPE
{
    /* Loop until the USARTy Receive Data Register is not empty */
    //while(__HAL_UART_GET_FLAG(&IO_USART1, UART_FLAG_RXFNE)==RESET);
    /* Store the received byte in RxBuffer */
    uint8_t ch=0;
    HAL_UART_Receive(&IO_USART1, &ch, 1, 1000);
    return ch;
}

////////////////////////////////////////////////////////////////////////////////
////
static void IO_USART1_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

    __HAL_RCC_GPIOA_CLK_ENABLE(); /* TX_GPIO_CLK_ENALBE */
    __HAL_RCC_GPIOA_CLK_ENABLE(); /* RX_GPIO_CLK_ENALBE */

    /* 配置串口1时钟源*/
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
    /* 使能串口1时钟 */
    __USART1_CLK_ENABLE();

    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 配置Rx引脚为复用功能 */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* 配置串DEBUG_USART 模式 */
    IO_USART1.Instance = USART1;
    IO_USART1.Init.BaudRate = 115200;
    IO_USART1.Init.WordLength = UART_WORDLENGTH_8B;
    IO_USART1.Init.StopBits = UART_STOPBITS_1;
    IO_USART1.Init.Parity = UART_PARITY_NONE;
    IO_USART1.Init.Mode = UART_MODE_TX_RX;
    IO_USART1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    IO_USART1.Init.OverSampling = UART_OVERSAMPLING_16;
    IO_USART1.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    IO_USART1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    HAL_UART_Init(&IO_USART1);

    /*串口1中断初始化 */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    /*配置串口接收中断 */
    __HAL_UART_ENABLE_IT(&IO_USART1,UART_IT_RXNE);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** 启用电源配置更新
    */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    /** 配置主内稳压器输出电压
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
    /** 初始化CPU、AHB和APB总线时钟
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while(1);
    }
    /** 初始化CPU、AHB和APB总线时钟
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                  |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        while(1);
    }
}
////////////////////////////////////////////////////////////////////////////////
////

void IO_USART1_SendString(const char* str){
    unsigned int k=0;
    do
    {
//        while(HAL_UART_GetState(&IO_USART1)!=HAL_USART_STATE_READY);
        HAL_UART_Transmit( &IO_USART1,(uint8_t *)(str + k) ,1,1000);
//        HAL_USART_Transmit( &IO_USART1,(uint8_t *)(str + k) ,1,1000);
        k++;
    } while(*(str + k)!='\0');
}

void board_init(void)
{
//    SystemClock_Config();
    SCB_EnableICache();
    SCB->CCR|=SCB_CCR_STKALIGN_Msk; // 栈对齐

    /* Configure the NVIC Preemption Priority Bits */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);

    HAL_Init();
    SystemClock_Config();

    IO_USART1_Init();

    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    SysTick_Config(SystemCoreClock/CPU_TICKS_PER_SECOND); /* 1ms = tick */


}
