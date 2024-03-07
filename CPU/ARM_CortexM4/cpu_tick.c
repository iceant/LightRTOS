#include <cpu_tick.h>

////////////////////////////////////////////////////////////////////////////////
////
static void cpu_tick__default_function(void){}

static cpu_tick_function_t cpu_tick__function = cpu_tick__default_function;

////////////////////////////////////////////////////////////////////////////////
////

void cpu_tick_set(cpu_tick_function_t function)
{
    cpu_tick__function = function;
}

////////////////////////////////////////////////////////////////////////////////
////

void SysTick_Handler(void)
{
    cpu_tick__function();
}

