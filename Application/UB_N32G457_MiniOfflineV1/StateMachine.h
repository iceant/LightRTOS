#ifndef INCLUDED_STATEMACHINE_H
#define INCLUDED_STATEMACHINE_H

////////////////////////////////////////////////////////////////////////////////
//// 
#ifndef INCLUDED_SDK_ARRAY_H
#include <sdk_array.h>
#endif /*INCLUDED_SDK_ARRAY_H*/

////////////////////////////////////////////////////////////////////////////////
////
#define DEVICE_STATE_BOOTING            (1<<0)
#define DEVICE_STATE_NETWORK_STARTUP    (1<<1)
#define DEVICE_STATE_NETWORK_READY      (1<<2)
#define DEVICE_STATE_IDLE               (1<<3)
#define DEVICE_STATE_METER_START        (1<<4)
#define DEVICE_STATE_METER_STOP         (1<<5)
#define DEVICE_STATE_CAN_START          (1<<6)
#define DEVICE_STATE_CAN_STOP           (1<<7)
#define DEVICE_STATE_NETWORK_SENDING    (1<<8)
#define DEVICE_STATE_NETWORK_SENDED     (1<<9)

////////////////////////////////////////////////////////////////////////////////
////

typedef void (*StateMachine_Transition_Function)(void* parameter);

typedef struct StateMachine_TransitionFunctionClosure_S{
    StateMachine_Transition_Function function;
    void* parameter;
}StateMachine_TransitionFunctionClosure_T;

typedef struct StateMachine_Transition_S{
    int target_state;
    StateMachine_TransitionFunctionClosure_T enter;
    StateMachine_TransitionFunctionClosure_T inside;
    StateMachine_TransitionFunctionClosure_T exit;
}StateMachine_Transition_T;

typedef struct StateMachine_State_S{
    int state;
    sdk_array_t transitions;
}StateMachine_State_T;

typedef struct StateMachine_S{
    StateMachine_State_T* current_state;
    sdk_array_t states;
}StateMachine_T;

////////////////////////////////////////////////////////////////////////////////
////

int StateMachine_Init(StateMachine_T * StateMachine);

int StateMachine_SetState(StateMachine_T * StateMachine, StateMachine_State_T* State);

int StateMachine_GetState(StateMachine_T* StateMachine, int StateCode, StateMachine_State_T** State);

int StateMachine_StateInit(StateMachine_State_T* State, int StateCode);

int StateMachine_AddState(StateMachine_T * StateMachine, StateMachine_State_T* State);

int StateMachine_AddStateTransition(StateMachine_State_T* State, StateMachine_Transition_T* transition);

#endif /*INCLUDED_STATEMACHINE_H*/
