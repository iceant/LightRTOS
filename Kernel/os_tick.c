#include <os_tick.h>

os_tick_t os_tick_from_millisecond(uint32_t ms)
{
    os_tick_t tick;
    tick = (CPU_TICKS_PER_SECOND * ms + 999)/1000;
    return tick;
}

/*
 CPU_TICKS_PER_SECOND = TICKS/1000ms
 CPU_TICKS_PER_SECOND * 1000 = TICKS/ms
 1TICK = (CPU_TICKS_PER_SECOND * 1000)ms
 */
os_size_t os_tick_to_millisecond(os_tick_t ticks){
    os_size_t timeout_ms;
    timeout_ms = (ticks*1000 - 999)/CPU_TICKS_PER_SECOND;
    return timeout_ms;
}
