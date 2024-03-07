#include <os_tick.h>

os_tick_t os_tick_from_millisecond(uint32_t ms)
{
//    return OS_PAGE(ms, 1000/CPU_TICKS_PER_SECOND);
    os_tick_t tick;
    tick = (CPU_TICKS_PER_SECOND * ms + 999)/1000;
    return tick;
}
