#ifndef __TV_HW_H
#define __TV_HW_H

#include "includes.h"
#include "hw.h"


void TV_Hw_Init(void);
void TV_Hw_Send_Byte(UCHAR by_Data);
void TV_Hw_Set_RxFuct(FNCT_UCHAR Fnct);
void TV_Hw_Set_TxFuct(FNCT_VOID Fnct);
void TV_Hw_ISR_Handler(void);
void TV_Hw_Erp(void);


#endif

