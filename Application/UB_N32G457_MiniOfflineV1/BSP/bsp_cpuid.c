#include <bsp_cpuid.h>
#include <string.h>
#include <sdk_hex.h>
////////////////////////////////////////////////////////////////////////////////
////


#define BSP_CPUID_UID_ADDRESS   0x1FFFF7F0
#define BSP_CPUID_UCID_ADDRESS  0x1FFFF7C0

static uint8_t BSP_CPUID__UID_ReadFlag = 0;
static uint8_t BSP_CPUID__UCID_ReadFlag = 0;

static char BSP_CPUID__UID_STR[25]={0};
static char BSP_CPUID__UCID_STR[33]={0};
////////////////////////////////////////////////////////////////////////////////
////


void BSP_CPUID_GetUID(BSP_CPUID_UID_T* UID){
    memcpy(UID->value, (void*)BSP_CPUID_UID_ADDRESS, 12);
}

void BSP_CPUID_GetUCID(BSP_CPUID_UCID_T* UCID){
    memcpy(UCID->value, (void*)BSP_CPUID_UCID_ADDRESS, 16);
}

const char* BSP_CPUID_GetUIDStr(void){
    if(BSP_CPUID__UID_ReadFlag==0){
        BSP_CPUID__UID_ReadFlag=1;
        BSP_CPUID_UID_T UID;
        BSP_CPUID_GetUID(&UID);
        SDK_HEX_ENCODE_BE((char*)BSP_CPUID__UID_STR, sizeof(BSP_CPUID__UID_STR), UID.value, sizeof(UID.value));
    }
    return BSP_CPUID__UID_STR;
}
const char* BSP_CPUID_GetUCIDStr(void){
    if(BSP_CPUID__UCID_ReadFlag==0){
        BSP_CPUID__UCID_ReadFlag=1;
        BSP_CPUID_UCID_T UCID;
        BSP_CPUID_GetUCID(&UCID);
        SDK_HEX_ENCODE_BE(BSP_CPUID__UCID_STR, sizeof(BSP_CPUID__UCID_STR), UCID.value, sizeof(UCID.value));
    }
    return BSP_CPUID__UCID_STR;
}
