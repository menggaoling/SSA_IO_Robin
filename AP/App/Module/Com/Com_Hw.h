#ifndef __COM_UART_H
#define __COM_UART_H

#include "includes.h"
#include "hw.h"


void Com_Hw_Init(void);
void Com_Hw_Send_Byte(UCHAR by_Data);
void Com_Hw_Set_RxFuct(FNCT_UCHAR Fnct);
void Com_Hw_Set_TxFuct(FNCT_VOID Fnct);
void Com_Hw_ISR_Handler(void);
void Com_Hw_Erp(void);


#endif


