#ifndef __FAN_H
#define __FAN_H


#include "Fan_Hw.h"


void Fan_Initial(void);
void Fan_Control(void);
void Fan_SetControl(FAN_CTRL by_Dat);
FAN_CTRL Fan_GetControl(void);
void Fan_Erp(void);


#endif


