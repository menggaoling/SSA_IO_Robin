#ifndef __HEART_H
#define __HEART_H

#include "Heart_Hw.h"


#define HEART_TM    1
#define HEART_EP    0
#define HEART_BK    HEART_EP
#define HEART_BIKE  HEART_BK


void Heart_Initial(void);
void Heart_Process(void);
UCHAR Heart_Get_Source(void);
UCHAR Heart_Get_Blink(void);
UCHAR Heart_Get_Heart(void);
void Heart_Polar_Int(void);
void Heart_HR_Int(void);
void Heart_1ms_Int(void);
void Heart_Erp(void);


#endif
