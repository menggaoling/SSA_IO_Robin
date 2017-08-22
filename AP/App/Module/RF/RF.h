#ifndef __RF_H
#define __RF_H

#include "includes.h"


void RF_Initial(void);
void RF_10ms_Int(void);
void RF_TxRxInt(void);
void RF_TxData(UCHAR* by_Data, UINT16 w_Len);
void RF_GetData(UCHAR* pby, UINT16* Size);
UINT16 RF_GetSize(void);
void RF_ERP(void);


#endif

