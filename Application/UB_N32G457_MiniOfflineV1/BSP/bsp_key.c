#include <bsp_key.h>
#include <n32g45x.h>
#include <os_kernel.h>
#include <stdio.h>
#include <hw_gpio.h>
////////////////////////////////////////////////////////////////////////////////
////
#define KEY_INPUT_PORT        GPIOB
#define KEY_INPUT_PIN         GPIO_PIN_15
#define KEY_INPUT_EXTI_LINE   EXTI_LINE15
#define KEY_INPUT_PORT_SOURCE GPIOB_PORT_SOURCE
#define KEY_INPUT_PIN_SOURCE  GPIO_PIN_SOURCE15
#define KEY_INPUT_IRQn        EXTI15_10_IRQn
#define KEY_INPUT_IRQHandler  EXTI15_10_IRQHandler

////////////////////////////////////////////////////////////////////////////////
////
void KeyInputExtiInit(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIO_InitType GPIO_InitStructure;
    EXTI_InitType EXTI_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

    /* Enable the GPIO Clock */
    if (GPIOx == GPIOA)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOB, ENABLE);
    }
    else if (GPIOx == GPIOC)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
    }
    else if (GPIOx == GPIOD)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD, ENABLE);
    }
    else if (GPIOx == GPIOE)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOE, ENABLE);
    }
    else if (GPIOx == GPIOF)
    {
        RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOF, ENABLE);
    }
    else
    {
        if (GPIOx == GPIOG)
        {
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOG, ENABLE);
        }
    }
    RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
    /*Configure the GPIO pin as input floating*/
    if (Pin <= GPIO_PIN_ALL)
    {
        GPIO_InitStructure.Pin        = Pin;
//        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
    }
    /*Configure key EXTI Line to key input Pin*/
    GPIO_ConfigEXTILine(KEY_INPUT_PORT_SOURCE, KEY_INPUT_PIN_SOURCE);

    /*Configure key EXTI line*/
    EXTI_InitStructure.EXTI_Line    = KEY_INPUT_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // EXTI_Trigger_Rising;EXTI_Trigger_Falling EXTI_Trigger_Rising_Falling
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&EXTI_InitStructure);

    /*Set key input interrupt priority*/
    NVIC_InitStructure.NVIC_IRQChannel                   = KEY_INPUT_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


////////////////////////////////////////////////////////////////////////////////
////

#define BSP_KEY_THREAD_STACK_SIZE   1024
#define BSP_KEY_THREAD_PRIORITY     20
#define BSP_KEY_THREAD_TIMESLICE    10

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t BSP_Key__ThreadStack[BSP_KEY_THREAD_STACK_SIZE];
static os_sem_t BSP_Key__Sem;
static os_thread_t BSP_Key__Thread;
static BSP_Key_Click BSP_Key__DownClick;
static BSP_Key_Click BSP_Key__UpClick;
static os_bool_t BSP_Key_ThreadFlag = OS_FALSE;
////////////////////////////////////////////////////////////////////////////////
////

static void BSP_Key__ThreadEntry(void* p){
    BSP_Key_ThreadFlag = OS_TRUE;
    while(1){
        os_sem_take(&BSP_Key__Sem, OS_WAIT_INFINITY);
        uint8_t state = GPIO_ReadInputDataBit(GPIOB, GPIO_PIN_15); /*0-down, 1-up*/
        printf("[KEY] State = %d\r\n", state);
        if(state==0 && BSP_Key__DownClick){
            BSP_Key__DownClick();
        }else if(state==1 && BSP_Key__UpClick){
            BSP_Key__UpClick();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
////

void BSP_Key_Init(void){
    KeyInputExtiInit(KEY_INPUT_PORT, KEY_INPUT_PIN);
    hw_gpio_init(GPIOE, GPIO_PIN_10, GPIO_Mode_Out_OD, GPIO_Speed_2MHz);
    hw_gpio_init(GPIOE, GPIO_PIN_11, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    hw_gpio_init(GPIOE, GPIO_PIN_12, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
    hw_gpio_init(GPIOE, GPIO_PIN_13, GPIO_Mode_Out_OD, GPIO_Speed_2MHz);

}

void BSP_Key_Set(BSP_Key_Click onDown, BSP_Key_Click onUp)
{
    BSP_Key__DownClick = onDown;
    BSP_Key__UpClick = onUp;

    os_sem_init(&BSP_Key__Sem, "BSP_Key_Sem", 0, OS_QUEUE_FIFO);

    os_thread_init(&BSP_Key__Thread, "BSP_Key_Thd", BSP_Key__ThreadEntry, 0
            , BSP_Key__ThreadStack, sizeof(BSP_Key__ThreadStack)
            , BSP_KEY_THREAD_PRIORITY, BSP_KEY_THREAD_TIMESLICE
    );

    os_thread_startup(&BSP_Key__Thread);
}

int BSP_Key_GetStatus(void){
    return GPIO_ReadInputDataBit(GPIOB, GPIO_PIN_15);
}

////////////////////////////////////////////////////////////////////////////////
////

void KEY_INPUT_IRQHandler(void)
{
    if (RESET != EXTI_GetITStatus(KEY_INPUT_EXTI_LINE))
    {
        EXTI_ClrITPendBit(KEY_INPUT_EXTI_LINE);
        
        if(BSP_Key_ThreadFlag){
            os_sem_release(&BSP_Key__Sem);
        }
    }
}


