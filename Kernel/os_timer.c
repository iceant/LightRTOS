#include <os_timer.h>
#include <cpu_spinlock.h>

////////////////////////////////////////////////////////////////////////////////
////

#define OS_TIMER_WHEEL0_BITS 8
#define OS_TIMER_WHEELx_BITS 6

/* 256 */
#define OS_TIMER_WHEEL0_SIZE (1<<OS_TIMER_WHEEL0_BITS)
/* 64 */
#define OS_TIMER_WHEELx_SIZE (1<<OS_TIMER_WHEELx_BITS)
/* 0xFF */
#define OS_TIMER_WHEEL0_MASK (OS_TIMER_WHEEL0_SIZE-1)
/* 0x3F */
#define OS_TIMER_WHEELx_MASK (OS_TIMER_WHEELx_SIZE-1)

#define OS_TIMER_WHEELx_INDEX(E, N) (((E) >> ((OS_TIMER_WHEEL0_BITS) + (N) * OS_TIMER_WHEELx_BITS)) & OS_TIMER_WHEELx_MASK)

#define WHEEL0_MAX (1<<8)
#define WHEEL1_MAX (1<<14)
#define WHEEL2_MAX (1<<20)
#define WHEEL3_MAX (1<<26)
#define WHEEL4_MAX (0xFFFFFFFFU)
////////////////////////////////////////////////////////////////////////////////
////
static cpu_spinlock_t os_timer__spinlock={.atomic.counter=0};
static volatile os_time_t os_timer__current_time=0;
static os_list_t os_timer__wheel0[OS_TIMER_WHEEL0_SIZE]={0};    /* 0x00000000 ~ 0x000000FF */
static os_list_t os_timer__wheel1[OS_TIMER_WHEELx_SIZE]={0};    /* 0x00000100 ~ 0x00003FFF */
static os_list_t os_timer__wheel2[OS_TIMER_WHEELx_SIZE]={0};    /* 0x00004000 ~ 0x000FFFFF */
static os_list_t os_timer__wheel3[OS_TIMER_WHEELx_SIZE]={0};    /* 0x00100000 ~ 0x03FFFFFF */
static os_list_t os_timer__wheel4[OS_TIMER_WHEELx_SIZE]={0};    /* 0x04000000 ~ 0xFFFFFFFF */

////////////////////////////////////////////////////////////////////////////////
////

__STATIC_FORCEINLINE void os_timer__find_wheel(os_time_t expires, os_list_t** wheel){
    if(expires<WHEEL0_MAX){ /* 1111 1111*/
        *wheel = &os_timer__wheel0[expires];
    }else if(expires < WHEEL1_MAX){ /* 11 1111 1111 1111 */
        *wheel = &os_timer__wheel1[((expires >> 8)&0x3F)];
    }else if(expires < WHEEL2_MAX){ /* 1111 1111 1111 1111 1111 */
        *wheel = &os_timer__wheel2[((expires >> 14)&0x3F)];
    }else if(expires < WHEEL3_MAX){ /* 11 1111 1111 1111 1111 1111 1111 */
        *wheel = &os_timer__wheel3[((expires >> 20)&0x3F)];
    }else if(expires <= WHEEL4_MAX){ /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        *wheel = &os_timer__wheel4[((expires >> 26)&0x3F)];
    }
}

__STATIC_FORCEINLINE void os_timer__insert_by_expires(os_timer_node_t* timer, os_list_t * wheel)
{
    os_list_node_t * node;
    os_timer_node_t* p;
    
    if(OS_LIST_IS_EMPTY(wheel)){
        OS_LIST_INSERT_AFTER(wheel, &timer->node);
        return;
    }
    
    for(node = OS_LIST_NEXT(wheel); node!=wheel; node = OS_LIST_NEXT(node)){
        p = OS_CONTAINER_OF(node, os_timer_node_t, node);
        if(p->expires > timer->expires){
            OS_LIST_INSERT_BEFORE(node, &timer->node);
            return;
        }
    }
    
    OS_LIST_INSERT_AFTER(wheel, &timer->node);
}

static os_err_t os_timer__insert_no_lock(os_timer_node_t* node){
    os_list_t * wheel;
    
    OS_LIST_INIT(&node->node);
    node->expires = os_timer__current_time + node->time;
    os_timer__find_wheel(node->expires, &wheel);
    os_timer__insert_by_expires(node, wheel);

    return OS_EOK;
}
////////////////////////////////////////////////////////////////////////////////
////

os_err_t os_timer_init(void){
    os_size_t i;
    cpu_spinlock_init(&os_timer__spinlock);
    
    os_timer__current_time = 0;
    
    for(i=0; i<OS_TIMER_WHEEL0_SIZE; i++){
        OS_LIST_INIT(&os_timer__wheel0[i]);
    }
    for(i=0; i<OS_TIMER_WHEELx_SIZE; i++){
        OS_LIST_INIT(&os_timer__wheel1[i]);
    }
    for(i=0; i<OS_TIMER_WHEELx_SIZE; i++){
        OS_LIST_INIT(&os_timer__wheel2[i]);
    }
    for(i=0; i<OS_TIMER_WHEELx_SIZE; i++){
        OS_LIST_INIT(&os_timer__wheel3[i]);
    }
    for(i=0; i<OS_TIMER_WHEELx_SIZE; i++){
        OS_LIST_INIT(&os_timer__wheel4[i]);
    }
    
    return OS_EOK;
}

os_bool_t os_timer_tick(void){
    os_list_t *wheel;
    os_list_node_t * node;
    os_time_t time;
    os_timer_node_t * timer_node;
    os_bool_t need_schedule_flag = OS_FALSE;
    
    cpu_spinlock_lock(&os_timer__spinlock);
    {
        os_timer__current_time++;
        time = os_timer__current_time;
        os_timer__find_wheel(time, &wheel);
        
        for(node = OS_LIST_NEXT(wheel); node!=wheel; ){
            timer_node = OS_CONTAINER_OF(node, os_timer_node_t, node);
            node = OS_LIST_NEXT(node);
            if(time >= timer_node->expires){
                OS_LIST_REMOVE(&timer_node->node);
                
                timer_node->timeout(time, timer_node->userdata);
                if(timer_node->flag & OS_TIMER_TYPE_REPEAT){
                    os_timer__insert_no_lock(timer_node);
                }
                
                need_schedule_flag = OS_TRUE;
            }
        }
    }
    cpu_spinlock_unlock(&os_timer__spinlock);
    
    return need_schedule_flag;
}

os_err_t os_timer_insert(os_timer_node_t* node)
{
    os_list_t * wheel;
    
    OS_LIST_INIT(&node->node);
    
    cpu_spinlock_lock(&os_timer__spinlock);
    {
        node->expires = os_timer__current_time + node->time;
        os_timer__find_wheel(node->expires, &wheel);
        os_timer__insert_by_expires(node, wheel);
    }
    cpu_spinlock_unlock(&os_timer__spinlock);
    
    return OS_EOK;
}


os_err_t os_timer_add(os_timer_node_t* node, os_timer_timeout timeout, void* userdata, os_time_t time, int flag)
{
    os_list_t * wheel;
    
    cpu_spinlock_lock(&os_timer__spinlock);
    {
        OS_LIST_REMOVE(&node->node);
        node->timeout = timeout;
        node->userdata = userdata;
        node->flag = flag;
        node->time = time;

        node->expires = os_timer__current_time + time;
        os_timer__find_wheel(node->expires, &wheel);
        os_timer__insert_by_expires(node, wheel);
    }
    cpu_spinlock_unlock(&os_timer__spinlock);

    return OS_EOK;
}

