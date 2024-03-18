#ifndef INCLUDED_CHCS_CAB_500A_H
#define INCLUDED_CHCS_CAB_500A_H

////////////////////////////////////////////////////////////////////////////////
////
typedef void (*CHCS_CAB_500A_IO_RxHandler)(void* message, void* userdata);

typedef struct CHCS_CAB_500A_IO_S{
    void (*setRxHandler)(void* rxHandler, void* userdata);
}CHCS_CAB_500A_IO_T;

////////////////////////////////////////////////////////////////////////////////
////

void CHCS_CAB_500A_Init(CHCS_CAB_500A_IO_T* IO);

long CHCS_CAB_500A_GetCurrent(void);

#endif /* INCLUDED_CHCS_CAB_500A_H */
