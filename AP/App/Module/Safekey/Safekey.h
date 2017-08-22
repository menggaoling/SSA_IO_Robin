#ifndef __SAFEKEY_H__
#define __SAFEKEY_H__

#include "Safekey_Hw.h"


void Safekey_Initial(void);
UCHAR Safekey_IsRemove(void);
void Safekey_Set_LCB_Remove(void);
void Safekey_Clear_LCB_Remove(void);
void Safekey_Set_IR_Remove(void);
void Safekey_Clear_IR_Remove(void);
void Safekey_1ms_Int(void);
void Safekey_Erp(void);


#endif

