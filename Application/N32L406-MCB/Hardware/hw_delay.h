#ifndef INCLUDED_HW_DELAY_H
#define INCLUDED_HW_DELAY_H

////////////////////////////////////////////////////////////////////////////////
////

#define HW_DELAY_CONST 0x28FFFF

////////////////////////////////////////////////////////////////////////////////
////

#define HW_LOOP_DELAY(CNT) \
    do{                    \
        uint32_t count = (CNT);\
        for (; count > 0; count--); \
    }while(0)

    
#endif /* INCLUDED_HW_DELAY_H */
