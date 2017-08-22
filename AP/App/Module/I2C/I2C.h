#ifndef _I2C_H_
#define _I2C_H_

#include "includes.h"


void I2C_Initail(void);
UCHAR I2C_Read(UCHAR by_Address, UCHAR by_Length, UCHAR by_Offset, UCHAR *Aby_TX_Buffer);
UCHAR I2C_Write(UCHAR by_Adress, UCHAR by_Len, UCHAR by_Offset, UCHAR *Aby_TX_Buffer);
void I2C_Error_Clear(void);
UCHAR I2C_Get_Error(void);
void I2C_Erp(void);


#endif

