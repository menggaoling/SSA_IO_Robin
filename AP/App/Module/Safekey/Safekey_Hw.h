#ifndef __SAFEKEY_HW_H__
#define __SAFEKEY_HW_H__


#include "includes.h"
#include "hw.h"


void Safekey_Hw_Initial(void);
UCHAR Safekey_Hw_Get_Safekey(void);
UCHAR Safekey_Hw_Get_Shut(void);
void Safekey_Hw_ERP(void);


#endif


