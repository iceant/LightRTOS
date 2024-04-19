#include "board.h"
#include <string.h>
#include <os_kernel.h>
#include <stdio.h>



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
//    BSP_USART1_DMA_Send(&ch, 1);
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
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
static void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 100000000
  *            HCLK(Hz)                       = 100000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 12000000
  *            PLL_M                          = 6
  *            PLL_N                          = 100
  *            PLL_P                          = 2
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 6;
    RCC_OscInitStruct.PLL.PLLN = 100;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLP_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

////////////////////////////////////////////////////////////////////////////////
////


//static ADS1256_IO_T ADS1256__IO = {.RST_High=BSP_ADS1256IO_RST_High,
//                                   .RST_Low = BSP_ADS1256IO_RST_Low,
//                                   .CS_High = BSP_ADS1256IO_CS_High,
//                                   .CS_Low = BSP_ADS1256IO_CS_Low,
//                                   .SetScanFunction = BSP_ADS1256IO_SetScanFunction,
//                                   .StartScan = 0,
//                                   .StopScan = 0,
//                                   .DRDY_Read = BSP_ADS1256IO_DRDY_Read,
//                                   .DOUT_Read = BSP_ADS1256IO_DOUT_Read,
//                                   .DIN_Low = BSP_ADS1256IO_DIN_Low,
//                                   .DIN_High = BSP_ADS1256IO_DIN_High,
//                                   .SCLK_High = BSP_ADS1256IO_SCLK_High,
//                                   .SCLK_Low = BSP_ADS1256IO_SCLK_Low
//};


////////////////////////////////////////////////////////////////////////////////
////

void board_init(void)
{
    SCB->CCR|=SCB_CCR_STKALIGN_Msk; // 栈对齐
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_Init();
    SystemClock_Config();

    /* Configure the NVIC Preemption Priority Bits */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);

    BSP_USART1_Init();
    BSP_USART1_EnableDMA();

    BSP_USART3_Init();
    BSP_USART3_EnableDMA();

//    BSP_SPI1_Init();

#if 0
    printf("BSP_DWTDelay Init...\r\n");
    BSP_DWTDelay_Init();
    printf("BSP_DWTDelay Init Done!\r\n");
#endif

    printf("BSP_TimerDelay_Init ...\r\n");
    BSP_TimerDelay_Init();
    printf("BSP_TimerDelay_Init Done!\r\n");

#if 0
    printf("BSP_ADS1256IO_Init...\r\n");
    BSP_ADS1256IO_Init();
    printf("BSP_ADS1256IO_Init Done!\r\n");
#endif

    printf("ADS1256_Init...\r\n");
//    ADS1256_Init(&ADS1256__IO);
//    ADS125X_Init(&ADS1256, &BSP_SPI1__Handle, ADS125X_DRATE_2_5SPS, ADS125X_PGA1, 0);
    ADS1256_Init();
    ADS1256_CfgADC(ADS125X_PGA1, ADS125X_DRATE_2_5SPS);	//配置ADC参数： 增益1:1, 数据输出速率 15Hz
    printf("ADS1256_Init Done!\r\n");


    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    SysTick_Config(SystemCoreClock/CPU_TICKS_PER_SECOND); /* 1ms = tick */

}


