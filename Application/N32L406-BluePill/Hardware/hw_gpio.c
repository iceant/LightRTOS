#include <hw_gpio.h>

////////////////////////////////////////////////////////////////////////////////
////

void hw_gpio_init(GPIO_Module* GPIOx, uint16_t Pin
                  , GPIO_ModeType ModeType
                  , GPIO_PuPdType PuPdType
                  , GPIO_CurrentType CurrentType
                  , GPIO_SpeedType SpeedType
                  , uint32_t Alternate
                  )
{
    GPIO_InitType GPIO_InitStructure;
    
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
    else
    {
        if (GPIOx == GPIOD)
        {
            RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD, ENABLE);
        }
    }
    
    /* Configure the GPIO pin */
    if (Pin <= GPIO_PIN_ALL)
    {
        GPIO_InitStruct(&GPIO_InitStructure);
        GPIO_InitStructure.Pin        = Pin;
        GPIO_InitStructure.GPIO_Current = CurrentType;
        GPIO_InitStructure.GPIO_Pull    = PuPdType;
        GPIO_InitStructure.GPIO_Mode  = ModeType;
        GPIO_InitStructure.GPIO_Slew_Rate = SpeedType;
        GPIO_InitStructure.GPIO_Alternate = Alternate;
        GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
    }
}


void hw_gpio_high(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIOx->PBSC = Pin;
}

void hw_gpio_low(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIOx->PBC = Pin;
}

void hw_gpio_OnOff(GPIO_Module* GPIOx, uint32_t Pin)
{
    GPIOx->PBSC = Pin;
}

void hw_gpio_toggle(GPIO_Module* GPIOx, uint16_t Pin)
{
    GPIOx->POD ^= Pin;
}

