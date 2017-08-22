#ifndef __TRACE_HW_H__
#define __TRACE_HW_H__

#include "includes.h"


void Trace_Hw_Init(void);
void Trace_Hw_Send_Byte(UCHAR by_Data);
void Trace_Hw_Set_RxFuct(FNCT_UCHAR Fnct);
void Trace_Hw_Set_TxFuct(FNCT_VOID Fnct);
void Trace_Hw_ISR_Handler(void);
void Trace_Hw_Erp(void);


#endif

