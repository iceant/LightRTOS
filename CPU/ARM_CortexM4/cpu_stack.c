#include <cpu_stack.h>
#include <cpu_atomic.h>
#include <assert.h>
#include <cpu_lock.h>

////////////////////////////////////////////////////////////////////////////////
////
#define CPU_STACK_SWITCH_FLAG_ON    1
#define CPU_STACK_SWITCH_FLAG_OFF   0

////////////////////////////////////////////////////////////////////////////////
////

volatile void* cpu__stack_next_p = 0;
volatile void* cpu__stack_curr_p = 0;
volatile uint8_t cpu__stack_switch_flag = CPU_STACK_SWITCH_FLAG_OFF;

////////////////////////////////////////////////////////////////////////////////
////

static cpu_lock_t cpu_stack__lock=0;

////////////////////////////////////////////////////////////////////////////////
////

/*
17  PSR
16  PC
15  LR
14  R12
13  R3
12  R2
11  R1
10  R0          <<-- PSP
09  R11
08  R10
07  R9
06  R8
05  R7
04  R6
03  R5
02  R4
01  CONTROL
00  LR              <<--- PSP_array[0]
--------------------
 */
int cpu_stack_init(void *thread_entry, void *parameter, void *stack_addr, int stack_size, void **cpu_sp) {
    if (stack_size < 18 * 4) return -1;
    
    cpu_uintptr_t sp = ((cpu_uintptr_t) stack_addr + stack_size - 18 * 4);
    
    CPU_REG(sp + (17 << 2)) = (cpu_uintptr_t) 0x01000000; /*xPSR*/
    CPU_REG(sp + (16 << 2)) = (cpu_uintptr_t) thread_entry; /*PC*/
    CPU_REG(sp + (10 << 2)) = (cpu_uintptr_t) parameter; /*R0*/
    CPU_REG(sp + (1 << 2)) = (cpu_uintptr_t) 0x03; /*Init CONTROL register with NO_PRIVILEGE|NO_FP*/
    CPU_REG(sp) = (cpu_uintptr_t) 0xFFFFFFFDUL; /*EXC_RETURN*/
    
    if (cpu_sp) {
        *cpu_sp = (void *) sp;
    }
    
    return 0;
}

int cpu_stack_switch(void** current_stack_p, void** next_stack_p)
{
    
    assert(next_stack_p);
    
    if(cpu__stack_switch_flag==CPU_STACK_SWITCH_FLAG_ON){
        return -1;
    }
    
    cpu_lock_lock(&cpu_stack__lock);
    {
        cpu__stack_switch_flag = CPU_STACK_SWITCH_FLAG_ON;
        cpu__stack_curr_p = current_stack_p;
        cpu__stack_next_p = next_stack_p;
    }
    cpu_lock_unlock(&cpu_stack__lock);

    if(cpu_in_privilege()==1){
        /*设置中断需要特权，已在特权模式，直接设置*/
        CPU_REG(SCB_ICSR) |= SCB_ICSR_PENDSVSET_Msk;
    }else{
        /*设置中断需要特权，没有特权，通过 SVC 来设置*/
        __svc(0);
    }

    return 0;
}


