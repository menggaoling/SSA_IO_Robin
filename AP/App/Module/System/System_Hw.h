#ifndef  __SYSTEM_HW_H
#define  __SYSTEM_HW_H

#include "includes.h"


void System_Hw_Initial(UCHAR bFirst);
void RCC_Configuration(void);
void System_Hw_ResetArm(void);
void System_Hw_1ms_Int(void);
void System_Hw_Wakeup(void);
void System_Hw_OSEnterRecoveryMode(void);
void System_Hw_Erp(void);


#endif



    
    
    
    