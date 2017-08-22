#include "LCB_Update.h"


#define LCB_UPDATE_MAX_LENGTH	(512+8+20)  //512+8+20          //
#define C_TX_TIMEOUT            100         //unit 10ms
#define C_RX_TIMEOUT            200         //unit 10ms
#define C_LCB_UPDATE_START      0xD1        //
#define C_LCB_UPDATE_END        0xD2        //


__no_init static UCHAR aby_RxBuffSave[LCB_UPDATE_MAX_LENGTH];   //接收暂存BUFF
__no_init static UCHAR aby_RxBuff2[LCB_UPDATE_MAX_LENGTH];       //接收暂存BUFF
__no_init static UCHAR aby_TxBuff[LCB_UPDATE_MAX_LENGTH];       //发送暂存BUFF
__no_init static UINT16 w_RxPointer;                            //接收BUFF位置
__no_init static UINT16 w_RxLength;                             //本次接收的数据长度
__no_init static UINT16 w_TxPointer;
__no_init static UINT16 w_TxLength;
__no_init static UCHAR by_TxTimeOut;
__no_init static UCHAR by_RxTimeOut;
__no_init static struct
{
    unsigned RxOk       : 1;    //接收OK
    unsigned TxBuzy     : 1;    //发送忙中
    unsigned StartRx    : 1;    //传输开始
} Com;


void LCB_Update_Tx_Int(void);
void LCB_Update_Rx_Int(UCHAR by_Data);


void LCB_Update_Initial(void)
{
    LCB_Hw_Init(115200);
    by_TxTimeOut = 0;
    by_RxTimeOut = 0;
    w_RxPointer = 0;
    w_RxLength = 0;
    w_TxPointer = 0;
    w_TxLength = 0;
    
    memset(aby_RxBuffSave,   0, sizeof(aby_RxBuffSave));
    memset(aby_RxBuff2,       0, sizeof(aby_RxBuff2));
    memset(aby_TxBuff,       0, sizeof(aby_TxBuff));
    memset(&Com,             0, sizeof(Com));
    LCB_Hw_Set_RxFuct(LCB_Update_Rx_Int);
    LCB_Hw_Set_TxFuct(LCB_Update_Tx_Int);
}

void LCB_Update_Write(UCHAR by_Command, UINT16 w_Length, UCHAR *pData)
{
    if(LCB_HW_GET_MODE() == 0) return;
    if(w_Length > LCB_UPDATE_MAX_LENGTH) return;
    
    while(Com.TxBuzy);
    
    UINT16 w_Index = 0;
    w_TxLength = 0;
    
    aby_TxBuff[w_Index++] = C_LCB_UPDATE_START;
    aby_TxBuff[w_Index++] = by_Command;
    
    for(UINT16 loop = 0; loop < w_Length; loop++)
    {
        if(pData[loop] >= 0xD0 && pData[loop] <= 0XD3)
        {
           aby_TxBuff[w_Index++] = 0xD0;
           aby_TxBuff[w_Index++] = pData[loop] - 0xD0;
        }
        else
           aby_TxBuff[w_Index++] = pData[loop];
    }
    
    aby_TxBuff[w_Index++] = C_LCB_UPDATE_END;
    
    Com.TxBuzy = 1;
    by_TxTimeOut = 0;
    w_TxLength = w_Index;
    w_TxPointer = 0;
    LCB_Hw_RS485_SetToTx();
    LCB_Update_Tx_Int();
}

void LCB_Update_Tx_Int(void)
{
    if(w_TxPointer < w_TxLength)
    {
        LCB_Hw_Send_Byte(aby_TxBuff[w_TxPointer++]);
    }
    else
    {
        w_TxLength = 0;
        w_TxPointer = 0;
        Com.TxBuzy = 0;
        LCB_Hw_RS485_SetToRx();
    }
}

UCHAR LCB_Update_Read(UCHAR *pby_Data, UINT16 *wLength)
{
    UCHAR by_Cmd = 0;
    UINT16 lenChange = 0;
    if(LCB_HW_GET_MODE() == 0) return 0;
    if(Com.RxOk)
    {
        by_Cmd = aby_RxBuff2[0];
        for(UINT16 loop = 1; loop < w_RxLength; loop++)
        {
           if(aby_RxBuff2[loop] == 0xD0 && aby_RxBuff2[loop + 1] <= 3)
           {
              pby_Data[lenChange++] = aby_RxBuff2[loop] + aby_RxBuff2[loop + 1];
              loop += 1;
           }
           else
              pby_Data[lenChange++] = aby_RxBuff2[loop];
        }
        *wLength = lenChange;
        memset(aby_RxBuff2, 0, sizeof(aby_RxBuff2));
        Com.RxOk = 0;
    }
    
    return by_Cmd;
}

void LCB_Update_Rx_Int(UCHAR by_Data)
{
    if(LCB_HW_GET_MODE() == 0) return ;
    if(!Com.RxOk)
    {
        if(by_Data == C_LCB_UPDATE_START)
        {
            w_RxPointer = 0;
            Com.StartRx = 1;
        }
        else if(by_Data == C_LCB_UPDATE_END)
        {
            if(w_RxPointer >= LCB_UPDATE_MAX_LENGTH-1)
            {
                w_RxPointer = LCB_UPDATE_MAX_LENGTH-1;
            }
            w_RxLength = w_RxPointer;
            memcpy(aby_RxBuffSave, aby_RxBuff2, sizeof(aby_RxBuffSave));
            Com.StartRx = 0;
            w_RxPointer = 0;
            by_RxTimeOut = 0;
            Com.RxOk = 1;
        }
        else
        {
            aby_RxBuff2[w_RxPointer++] = by_Data;
            if(w_RxPointer >= LCB_UPDATE_MAX_LENGTH-1)
            {
                w_RxPointer = LCB_UPDATE_MAX_LENGTH-1;
            }
        }
    }
}

void LCB_Update_10msInt(void)
{
    if(LCB_HW_GET_MODE() == 0) return;
    if(Com.TxBuzy)                                                              //防止发送超时锁死
    {
        if(by_TxTimeOut++ > C_TX_TIMEOUT)
        {
            by_TxTimeOut = 0;
            Com.TxBuzy = 0;
            LCB_Hw_RS485_SetToRx();
            memset(aby_TxBuff, 0, sizeof(aby_TxBuff));                          //清空发送数据BUFF
        }
    }
    else
    {
        by_TxTimeOut = 0;
    }
    
    
    if(Com.StartRx)                                                             //防止接收超时锁死
    {        
        if(by_RxTimeOut++ > C_RX_TIMEOUT)
        {
            by_RxTimeOut = 0;
            Com.StartRx = 0;
            w_RxPointer = 0;
            memset(aby_RxBuff2, 0, sizeof(aby_RxBuff2));                          //清空接收数据BUFF
        }
    }
    else
    {
        by_RxTimeOut = 0;
    }
}









