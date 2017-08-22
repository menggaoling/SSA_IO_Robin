#ifndef __RF_HW_H
#define __RF_HW_H

#include "includes.h"
#include "hw.h"


void RF_Hw_Init(void);
void RF_Hw_Send_Byte(UCHAR by_Data);
void RF_Hw_Set_RxFuct(FNCT_UCHAR Fnct);
void RF_Hw_Set_TxFuct(FNCT_VOID Fnct);
void RF_Hw_ISR_Handler(void);
void RF_Hw_ERP(void);


#endif

