#ifndef _LCB_UPDATE__
#define _LCB_UPDATE__

#include "LCB_Hw.h"


void LCB_Update_Initial(void);
void LCB_Update_Write(UCHAR by_Command, UINT16 w_Length, UCHAR *pData);
UCHAR LCB_Update_Read(UCHAR *pby_Data, UINT16 *wLength);
void LCB_Update_10msInt(void);


#endif


