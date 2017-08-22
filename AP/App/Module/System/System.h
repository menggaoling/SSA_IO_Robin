#ifndef  __SYSTEM_H
#define  __SYSTEM_H

#include "includes.h"


void System_Initial(UCHAR bFirst);
void System_1ms(void);
void System_Reset(void);
void System_ResetArm(void);
void System_Wakeup(void);
void System_OSEnterRecoveryMode(void);
void System_Erp(void);


#endif

