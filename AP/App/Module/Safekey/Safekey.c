#include "Safekey.h"


#define SAFEKEY_REMOVE_MAX_TIME     100     //ms


__no_init UCHAR by_SafeKeyIsRemove;
__no_init UCHAR by_SafeKeyIsRemoveLCB;
__no_init UCHAR by_SafeKeyIsRemoveIR;
__no_init UCHAR by_RemoveCounter;


void Safekey_Initial(void)
{
    Safekey_Hw_Initial();
    by_SafeKeyIsRemove = 0;
    by_SafeKeyIsRemoveLCB = 0;
    by_SafeKeyIsRemoveIR = 0;
    by_RemoveCounter = 0;
}

UCHAR Safekey_IsRemove(void)
{
    return by_SafeKeyIsRemove || by_SafeKeyIsRemoveLCB || by_SafeKeyIsRemoveIR;
}

void Safekey_Set_LCB_Remove(void)
{
    by_SafeKeyIsRemoveLCB = 1;
}

void Safekey_Clear_LCB_Remove(void)
{
    by_SafeKeyIsRemoveLCB = 0;
}

void Safekey_Set_IR_Remove(void)
{
    by_SafeKeyIsRemoveIR = 1;
}

void Safekey_Clear_IR_Remove(void)
{
    by_SafeKeyIsRemoveIR = 0;
}

void Safekey_1ms_Int(void)
{
    if(Safekey_Hw_Get_Safekey())
    {
        by_SafeKeyIsRemove = 0;
        by_RemoveCounter = 0;
    }
    else
    {
        if(by_RemoveCounter > SAFEKEY_REMOVE_MAX_TIME)
        {
            by_SafeKeyIsRemove = 1;
        }
        else
        {
            by_RemoveCounter++;
        }
    }
}

void Safekey_Erp(void)
{
    Safekey_Hw_ERP();
}










