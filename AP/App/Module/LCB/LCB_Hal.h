#ifndef _LCB_HAL__
#define _LCB_HAL__

#include "LCB_Hw.h"


#define RX_BUFFER_MAX	128
#define TX_BUFFER_MAX	128


typedef struct
{
    UCHAR       Cmd;
    UCHAR       Status;             //Status command
    UCHAR       Len;                //Length of data bytes £¬max.255
    UCHAR       Crc;
    UCHAR       Data[RX_BUFFER_MAX];//Data bytes£¬max.255Bytes
} CMD_RX;

typedef struct
{
    UCHAR       Cmd;
    UCHAR       Adress;             //0xFF used for only one receiver condition
    UCHAR       Len;                //Length of data bytes £¬max.255
    UCHAR       Data[TX_BUFFER_MAX];//Data bytes£¬max.255Bytes
} CMD_TX;


void LCB_Hal_Initial(void);
UCHAR LCB_Hal_Get_Cmd(CMD_RX *CmdRx);
void LCB_Hal_Send_Cmd(CMD_TX *CmdTx);
void LCB_Hal_Send_Buff(UCHAR *by_Data, UCHAR by_Len);
void LCB_Hal_TxBuff(UCHAR by_Len);
UCHAR LCB_Hal_Tx_Busy(void);
void LCB_Hal_Tx_Int(void);
void LCB_Hal_Rx_Int(UCHAR by_Data);
UCHAR LCB_Hal_CRC8_Check(UCHAR *pby_Data, UCHAR by_Length);
void LCB_Hal_1ms_Int(void);
UCHAR LCB_Hal_Get_Rx(void);
void LCB_Hal_Erp(void);


#endif


