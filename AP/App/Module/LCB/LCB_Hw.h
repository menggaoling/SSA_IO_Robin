#ifndef __DIGITAL_HW_H
#define __DIGITAL_HW_H

#include "includes.h"


void LCB_Hw_Init(UINT32 Baud);
void LCB_Hw_Send_Byte(UCHAR by_Data);
void LCB_Hw_Set_RxFuct(FNCT_UCHAR Fnct);
void LCB_Hw_Set_TxFuct(FNCT_VOID Fnct);
void LCB_Hw_ISR_Handler(void);
void LCB_Hw_RS485_SetToRx(void);
void LCB_Hw_RS485_SetToTx(void);
void LCB_Hw_WakeUp_LCB(void);
void LCB_Hw_Delay(UINT16 i);
void LCB_Hw_1ms_Int(void);
UCHAR LCB_Hw_Get_Rx(void);
void LCB_Hw_Erp(void);
UCHAR LCB_HW_GET_MODE(void);

#endif

