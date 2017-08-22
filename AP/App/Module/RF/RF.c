#include "RF.h"
#include "RF_Hw.h"
#include "FIFO.h"


#define RF_MAX_LENGTH	        256
#define C_TX_TIMEOUT            50          //unit 10ms
#define C_RX_TIMEOUT            50          //unit 10ms

#define D_RXBUFFER              0
#define D_RXTARGET              1


__no_init static FIFO_BASE FIFO_Base;
__no_init static UCHAR aby_FIFOBuff[512];                                       //FIFO用Buff
__no_init static UCHAR aby_TxBuff[RF_MAX_LENGTH];
__no_init static UCHAR aby_RxBuff[RF_MAX_LENGTH];
__no_init static UCHAR by_TxTimeOut;
__no_init static UCHAR by_RxTimeOut;
__no_init static UINT16 w_TxLength;
__no_init static UINT16 w_RxPointer;                                            //接收BUFF位置
__no_init static UINT16 w_TxPointer;
__no_init static struct
{
    unsigned RxOk       : 1;    //接收OK
    unsigned TxBuzy     : 1;    //发送忙中
    unsigned StartRx    : 1;    //传输开始
} RF;


void RF_Tx_Int(void);
void RF_Rx_Int(UCHAR by_Data);


void RF_Initial(void)
{
    RF_Hw_Init();
    w_TxLength = 0;
    w_RxPointer = 0;
    w_TxPointer = 0;
    
    memset(aby_RxBuff, 0, sizeof(aby_RxBuff));
    memset(aby_TxBuff, 0, sizeof(aby_TxBuff));
    memset(&RF, 0, sizeof(RF));
    RF_Hw_Set_RxFuct(RF_Rx_Int);
    RF_Hw_Set_TxFuct(RF_Tx_Int);
    
    FIFO_Initial(&FIFO_Base, aby_FIFOBuff, sizeof(aby_FIFOBuff));
}

void RF_GetData(UCHAR* pby, UINT16* Size)
{
    *Size = FIFO_Output(&FIFO_Base, pby, RF_MAX_LENGTH);
}

UINT16 RF_GetSize(void)
{
    return FIFO_Peek(&FIFO_Base);
}

void RF_TxData(UCHAR* by_Data, UINT16 w_Len)
{
    if(w_Len > RF_MAX_LENGTH) return;
    while(RF.TxBuzy);
    
    w_TxLength = w_Len;
    w_TxPointer = 0;
    memcpy(aby_TxBuff, by_Data, w_TxLength);
    
    RF.TxBuzy = 1;
    RF_Tx_Int();
}

void RF_Tx_Int(void)
{
    if(w_TxPointer < w_TxLength)
    {
        RF_Hw_Send_Byte(aby_TxBuff[w_TxPointer++]);
    }
    else
    {
        w_TxLength = 0;
        w_TxPointer = 0;
        RF.TxBuzy = 0;
    }
}

void RF_Rx_Int(UCHAR by_Data)
{
    switch(by_Data)
    {
    case 0xC0:
        if(w_RxPointer == 0)
        {
            RF.StartRx = 1;
            memset(aby_RxBuff, 0, sizeof(aby_RxBuff));
        }
        else
        {
            by_RxTimeOut = 0;
            FIFO_Input(&FIFO_Base, aby_RxBuff, w_RxPointer);
            w_RxPointer = 0;
        }
        break;
    default:
        if(RF.StartRx)
        {
            aby_RxBuff[w_RxPointer++] = by_Data;
        }
        break;
    }
}

void RF_10ms_Int(void)
{
    if(RF.TxBuzy)                                                               //防止发送超时锁死
    {
        if(by_TxTimeOut++ > C_TX_TIMEOUT)
        {
            by_TxTimeOut = 0;
            RF.TxBuzy = 0;
            memset(aby_TxBuff, 0, sizeof(aby_TxBuff));                          //清空发送数据BUFF
        }
    }
    else
    {
        by_TxTimeOut = 0;
    }


    if(RF.StartRx)                                                              //防止接收超时锁死
    {
        if(by_RxTimeOut++ > C_RX_TIMEOUT)
        {
            by_RxTimeOut = 0;
            RF.StartRx = 0;
            memset(aby_RxBuff, 0, sizeof(aby_RxBuff));                          //清空接收数据BUFF
        }
    }
    else
    {
        by_RxTimeOut = 0;
    }
}

void RF_ERP(void)
{
    RF_Hw_ERP();
}



