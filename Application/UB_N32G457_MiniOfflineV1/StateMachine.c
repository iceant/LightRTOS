#include <StateMachine.h>

int StateMachine_Init(StateMachine_T * StateMachine) {
    StateMachine->current_state = 0;
    os_err_t err = sdk_array_init(&StateMachine->states, sizeof(StateMachine_State_T), 0);
    return err;
}

int StateMachine_SetState(StateMachine_T * StateMachine, StateMachine_State_T* State){
    StateMachine_Transition_T* target_transition = 0;

    if(StateMachine->current_state){

        if(StateMachine->current_state->exit.function){
            StateMachine->current_state->exit.function(StateMachine, StateMachine->current_state->exit.parameter);
        }

        if(StateMachine_FindTransition(StateMachine, State->state, &target_transition)==OS_EOK){
            if(target_transition->enter.function){
                target_transition->enter.function(StateMachine, target_transition->enter.parameter);
            }
        }
    }

    StateMachine->current_state = State;

    if(target_transition){
        if(target_transition->inside.function){
            target_transition->inside.function(StateMachine, target_transition->inside.parameter);
        }
    }

    return OS_EOK;
}

int StateMachine_GetState(StateMachine_T* StateMachine, int StateCode, StateMachine_State_T** State){
    os_size_t i;
    StateMachine_State_T* state_p = 0;
    for(i=0; i< StateMachine->states.size; i++){
        sdk_array_get(&StateMachine->states, i, (void**)&state_p);
        if(state_p && state_p->state == StateCode){
            *State = state_p;
            return OS_EOK;
        }
    }
    return OS_ERROR;
}

int StateMachine_AddState(StateMachine_T * StateMachine, StateMachine_State_T* State)
{
    return sdk_array_push(&StateMachine->states, State);
}

int StateMachine_StateInit(StateMachine_State_T* State, int StateCode)
{
    State->state = StateCode;
    return sdk_array_init(&State->transitions, sizeof(StateMachine_Transition_T), 0);
}

int StateMachine_AddStateTransition(StateMachine_State_T* State, StateMachine_Transition_T* transition)
{
    return sdk_array_push(&State->transitions, transition);
}

int StateMachine_FindTransition(StateMachine_T* StateMachine, int ToStateCode, StateMachine_Transition_T** transition){
    os_size_t i;
    StateMachine_Transition_T* transition_p;

    if(!StateMachine->current_state) return OS_ERROR;

    for(i=0; i<StateMachine->current_state->transitions.size; i++){
        sdk_array_get(&StateMachine->current_state->transitions, i, (void**)&transition_p);
        if(transition_p->target_state==ToStateCode){
            *transition = transition_p;
            return OS_EOK;
        }
    }
    return OS_ERROR;
}

