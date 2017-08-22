#ifndef  __RFID_H
#define  __RFID_H


#include "includes.h"


void RFID_Init(void);
void RFID_TxRx_Process();
void RFID_SendCommand(char *string, UINT16 str_size);
void RFID_ReceiveCommand(char *string);
void RFID_Erp(void);


#endif

