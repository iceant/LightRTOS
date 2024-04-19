#include <SVCHandler.h>
#include <stdio.h>
#include <cpu_macros.h>
////////////////////////////////////////////////////////////////////////////////
////
volatile cpu_uintptr_t svc_exc_return = 0;

static void SVC_PendSV(void){
    CPU_REG(SCB_ICSR) |= SCB_ICSR_PENDSVSET_Msk;
}

static SVC_Handler_Function SVCTable[256]={
        SVC_PendSV, cpu__reboot, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        , 0, 0, 0, 0, 0
};

////////////////////////////////////////////////////////////////////////////////
////

void SVC_Handler_C(unsigned  int * svc_args)
{
    unsigned char svc_number;
    svc_number = ((char*)svc_args[6])[-2];
    SVC_Handler_Function Function = SVCTable[svc_number];

    if(Function){
        Function();
    }else{
        printf("ERROR: Unknown SVC service number.\n");
        printf(" - SVC number 0x%x\n", svc_number);
    }
//
//    switch (svc_number) {
//        case 0:
//            CPU_REG(SCB_ICSR) |= SCB_ICSR_PENDSVSET_Msk;
////            svc_exc_return = 0xFFFFFFFDU;
//            break;
//        case 1:
//            cpu__reboot();
////            svc_exc_return = 0xFFFFFFFDU;
//            break;
//        default:
//            printf("ERROR: Unknown SVC service number.\n");
//            printf(" - SVC number 0x%x\n", svc_number);
//            break;
//    }

}

void SVC_Handler_Set(uint8_t idx, SVC_Handler_Function fn)
{
    SVCTable[idx] = fn;
}

