#include <CHCS_CAB_500A.h>
#include <os_kernel.h>
#include <sdk_hex.h>
////////////////////////////////////////////////////////////////////////////////
////

typedef struct
{
    uint32_t StdId; /*!< Specifies the standard identifier.
                         This parameter can be a value between 0 to 0x7FF. */
    
    uint32_t ExtId; /*!< Specifies the extended identifier.
                         This parameter can be a value between 0 to 0x1FFFFFFF. */
    
    uint8_t IDE; /*!< Specifies the type of identifier for the message that
                      will be received. This parameter can be a value of
                      @ref CAN_identifier_type */
    
    uint8_t RTR; /*!< Specifies the type of frame for the received message.
                      This parameter can be a value of
                      @ref CAN_remote_transmission_request */
    
    uint8_t DLC; /*!< Specifies the length of the frame that will be received.
                      This parameter can be a value between 0 to 8 */
    
    uint8_t Data[8]; /*!< Contains the data to be received. It ranges from 0 to
                          0xFF. */
    
    uint8_t FMI; /*!< Specifies the index of the filter the message stored in
                      the mailbox passes through. This parameter can be a
                      value between 0 to 0xFF */
} CHCS_CAB_500A_CAN_Message;

////////////////////////////////////////////////////////////////////////////////
////
static CHCS_CAB_500A_IO_T* CHCS_CAB_500A__IO=0;
static long CHCS_CAB_500A__Current=0;

////////////////////////////////////////////////////////////////////////////////
////
static void CHCS_CAB_500A__Handler(void* Message, void* userdata){
    CHCS_CAB_500A_CAN_Message* RxMsg = Message;
    CHCS_CAB_500A__Current = SDK_HEX_GET_UINT32_BE(RxMsg->Data, 0) - 0x80000000;
    if(CHCS_CAB_500A__Current==-1 || CHCS_CAB_500A__Current>500000 || CHCS_CAB_500A__Current<4){
        CHCS_CAB_500A__Current = 0;
    }
}
////////////////////////////////////////////////////////////////////////////////
////

void CHCS_CAB_500A_Init(CHCS_CAB_500A_IO_T* IO){
    CHCS_CAB_500A__IO = IO;
    IO->setRxHandler(CHCS_CAB_500A__Handler, 0);
}

long CHCS_CAB_500A_GetCurrent(void){
    return CHCS_CAB_500A__Current;
}

