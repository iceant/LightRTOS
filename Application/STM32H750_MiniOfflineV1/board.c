#include "board.h"
#include <string.h>
#include <os_kernel.h>



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
    HAL_UART_Transmit(&BSP_USART1__Handle, (uint8_t *)&ch, 1, 1000);
    return (ch);
}

GETCHAR_PROTOTYPE
{
    int ch;
    /* Loop until the USARTy Receive Data Register is not empty */
   //while(__HAL_UART_GET_FLAG(&BSP_USART1__Handle, UART_FLAG_RXNE)==RESET);
    /* Store the received byte in RxBuffer */
    HAL_UART_Receive(&BSP_USART1__Handle, (uint8_t*)&ch, 1, 1000);
    return ch;
}

////////////////////////////////////////////////////////////////////////////////
////

/**
  * @brief  System Clock 配置
  *         system Clock 配置如下:
	*            System Clock source  = PLL (HSE)
	*            SYSCLK(Hz)           = 400000000 (CPU Clock)
	*            HCLK(Hz)             = 200000000 (AXI and AHBs Clock)
	*            AHB Prescaler        = 2
	*            D1 APB3 Prescaler    = 2 (APB3 Clock  100MHz)
	*            D2 APB1 Prescaler    = 2 (APB1 Clock  100MHz)
	*            D2 APB2 Prescaler    = 2 (APB2 Clock  100MHz)
	*            D3 APB4 Prescaler    = 2 (APB4 Clock  100MHz)
	*            HSE Frequency(Hz)    = 25000000
	*            PLL_M                = 5
	*            PLL_N                = 160
	*            PLL_P                = 2
	*            PLL_Q                = 4
	*            PLL_R                = 2
	*            VDD(V)               = 3.3
	*            Flash Latency(WS)    = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** 启用电源配置更新
    */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    __HAL_RCC_SYSCFG_CLK_ENABLE();
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
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK
                                  |RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1
                                  |RCC_CLOCKTYPE_PCLK2
                                  |RCC_CLOCKTYPE_D3PCLK1
                                  |RCC_CLOCKTYPE_D1PCLK1;
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

void board_init(void)
{
//    SystemClock_Config();
    SCB_EnableICache();
    SCB->CCR|=SCB_CCR_STKALIGN_Msk; // 栈对齐

    /* Configure the NVIC Preemption Priority Bits */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);

    HAL_Init();
    SystemClock_Config();

    BSP_USART1_Init();
    BSP_USART1_EnableDMA();

    BSP_USART3_Init();
    BSP_USART3_EnableDMA();

    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    SysTick_Config(SystemCoreClock/CPU_TICKS_PER_SECOND); /* 1ms = tick */


}
