#include "Com.h"


#define RXBUFF_LENGTH	        540         //512+8+20         //
#define C_TX_TIMEOUT            50          //unit 10ms
#define C_RX_TIMEOUT            50          //unit 10ms
#define C_COM_START0            0x55        //
#define C_COM_START1            0xAA        //


typedef enum
{
    RHead, RWR, RCommd, RLen, RData, RCRC
} REC_STATE;


__no_init static UCHAR aby_RxBuff0[RXBUFF_LENGTH];       //接收暂存BUFF
__no_init static UCHAR aby_RxTemp[RXBUFF_LENGTH];       //接收暂存BUFF
__no_init static UCHAR aby_TxBuff[RXBUFF_LENGTH];       //发送暂存BUFF
__no_init static REC_STATE R_State;                     //接收步骤
__no_init static UINT16 w_RxPointer;                    //接收BUFF位置
__no_init static UINT16 w_RxTempLength;                 //接收BUFF的长度
__no_init static UINT16 w_TxPointer;
__no_init static UINT16 w_TxLength;
__no_init static UCHAR by_TxTimeOut;
__no_init static UCHAR by_RxTimeOut;    
__no_init static UCHAR by_UpdateIOStep;
__no_init static UCHAR by_UpdateRxIndex;
__no_init static UCHAR aby_UpdateIORx[10];
__no_init static struct
{
    unsigned TxBuzy     : 1;    //发送忙中
    unsigned StartRx    : 1;    //传输开始
} Com;


UINT16 Com_Cal_CRC(UCHAR *pby_Buff, UINT16 wSize);
void Com_Tx_Int(void);
void Com_Rx_Int(UCHAR by_Data);


void Com_Initial(void)
{
    Com_Hw_Init();
    by_TxTimeOut = 0;
    by_RxTimeOut = 0;
    w_RxPointer = 0;
    w_RxTempLength = 0;
    w_TxLength = 0;
    w_TxPointer = 0;
    R_State = RHead;
    by_UpdateIOStep = 0;
    by_UpdateRxIndex = 0;
    
    memset(aby_UpdateIORx,   0, sizeof(aby_UpdateIORx));
    memset(aby_RxBuff0,       0, sizeof(aby_RxBuff0));
    memset(aby_RxTemp,       0, sizeof(aby_RxTemp));
    memset(aby_TxBuff,       0, sizeof(aby_TxBuff));
    memset(&Com,             0, sizeof(Com));
    Com_Hw_Set_RxFuct(Com_Rx_Int);
    Com_Hw_Set_TxFuct(Com_Tx_Int);
}

UCHAR Com_GetUpdateIOState(void)
{
    return by_UpdateIOStep;
}

void Com_Write(UCHAR readwrite, UINT16 Command, UINT16 Length, UCHAR *pData)
{
    if(Length > COM_MAX_LENGTH) return; //MAX ALLOWED DATA BYTE SIZE IS 128
    
    //IF THE UART IS BUSY, WAIT FOR IT TO BECOME IDLE
    while(Com.TxBuzy);
    
    UINT16 w_Index = 0;
    w_TxLength = 0;
    
    //ADD PACKET HEAD, SEND SYNC BYTE 0xaa55
    aby_TxBuff[w_Index++] = C_COM_START0;
    aby_TxBuff[w_Index++] = C_COM_START1;
    
    //ADD READ/WRITE INDICATOR
    aby_TxBuff[w_Index++] = readwrite;
    
    //ADD THE COMMAND
    aby_TxBuff[w_Index++] = (UCHAR)Command;
    aby_TxBuff[w_Index++] = (UCHAR)(Command >> 8);
    
    //ADD THE LENGTH
    aby_TxBuff[w_Index++] = (UCHAR)Length;
    aby_TxBuff[w_Index++] = (UCHAR)(Length >> 8);
    
    //ADD THE DATA to BUFFER
    memcpy(&aby_TxBuff[w_Index], pData, Length);
    w_Index += Length;
    
    //CALCULATE THE CHECKSUM AND ADD IT TO THE PACKET
    UINT16 checksum = Com_Cal_CRC(aby_TxBuff, w_Index);
    aby_TxBuff[w_Index++] = checksum;
    aby_TxBuff[w_Index++] = checksum >> 8;
    
    //SET A FLAG THAT ITS NOT OK FOR TRANSMISSION AS IT IS BUSY
    Com.TxBuzy = 1;
    by_TxTimeOut = 0;
    
    //SET THE SIZE OF CURRENT PACKET FOR TRANSMISSION
    w_TxLength = w_Index;
    w_TxPointer = 0;
    
    //ENABLE TRANSMISSION
    Com_Tx_Int();
}

UCHAR Com_Read(COM_PACKAGE *stData)
{
    UCHAR by_Out = 0;
    
    if(w_RxTempLength)
    {
        UINT16 wCRCValue = aby_RxTemp[w_RxTempLength - 2] + (aby_RxTemp[w_RxTempLength - 1] << 8);
        UINT16 wCRC = Com_Cal_CRC(aby_RxTemp, w_RxTempLength - 2);
        
        if(wCRCValue == wCRC)                                                   //CRC校验
        {
            stData->WR = aby_RxTemp[2];
            stData->Command = aby_RxTemp[3] + (aby_RxTemp[4] << 8);
            stData->Length  = aby_RxTemp[5] + (aby_RxTemp[6] << 8);
            memcpy(stData->Data, &aby_RxTemp[7], stData->Length);
        }
        
        memset(aby_RxTemp, 0, sizeof(aby_RxTemp));                              //清空接收数据BUFF
        w_RxTempLength = 0;                                                     //清零长度
        by_Out = 1;
    }
    
    return by_Out;
}

void Com_Tx_Int(void)
{
    if(w_TxPointer < w_TxLength)
    {
        Com_Hw_Send_Byte(aby_TxBuff[w_TxPointer++]);
    }
    else
    {
        w_TxLength = 0;
        w_TxPointer = 0;
        Com.TxBuzy = 0;
    }
}

void Com_Rx_Int(UCHAR by_Data)
{
    static UCHAR by_IndexCounter = 0;
    static UINT16 w_DataLength = 0;
    
    if(by_UpdateIOStep != 2)
    {
        if(by_UpdateRxIndex >= 10) by_UpdateRxIndex = 0;
        
        aby_UpdateIORx[by_UpdateRxIndex++] = by_Data;
        
        if(aby_UpdateIORx[0] != 0xE1)
        {
            by_UpdateRxIndex = 0;
        }
        else
        {
            if(by_UpdateIOStep == 0 && aby_UpdateIORx[1] == 0x40 && aby_UpdateIORx[2] == 0xE2)
            {
                by_UpdateIOStep = 1;
                
                aby_TxBuff[0] = 0xE1;
                aby_TxBuff[1] = 0x41;
                aby_TxBuff[2] = 0x00;
                aby_TxBuff[3] = 0xE2;
                
                by_UpdateRxIndex = 0;
                memset(aby_UpdateIORx, 0, sizeof(aby_UpdateIORx));
                Com.TxBuzy = 1;
                by_TxTimeOut = 0;
                w_TxLength = 4;
                w_TxPointer = 0;
                Com_Tx_Int();
            }
            
            if(by_UpdateIOStep == 1 && aby_UpdateIORx[1] == 0x42 && aby_UpdateIORx[2] == 0xE2)
            {
                by_UpdateIOStep = 2;
                
                aby_TxBuff[0] = 0xE1;
                aby_TxBuff[1] = 0x43;
                aby_TxBuff[2] = 0xE2;
                
                by_UpdateRxIndex = 0;
                memset(aby_UpdateIORx, 0, sizeof(aby_UpdateIORx));
                Com.TxBuzy = 1;
                by_TxTimeOut = 0;
                w_TxLength = 3;
                w_TxPointer = 0;
                Com_Tx_Int();
            }
        }
    }
    
    
    if(by_UpdateIOStep != 2)
    {
        switch(R_State)
        {
        case RHead:
            w_RxPointer = 0;
            if(by_IndexCounter == 1)
            {
                if(by_Data == C_COM_START1)
                {
                    Com.StartRx = 1;
                    by_RxTimeOut = 0;
                    aby_RxBuff0[w_RxPointer++] = C_COM_START0;
                    aby_RxBuff0[w_RxPointer++] = C_COM_START1;
                    R_State = RWR;
                }
            }
            else if(by_Data == C_COM_START0)
            {
                by_IndexCounter = 1;
                break;
            }
            
            by_IndexCounter = 0;
            w_DataLength = 0;
            break;
        case RWR:
            if(by_Data == COM_READ || by_Data == COM_WRITE)
            {
                aby_RxBuff0[w_RxPointer++] = by_Data;
                by_IndexCounter = 0;
                R_State = RCommd;
            }
            else
            {
                R_State = RHead;
            }
            break;
        case RCommd:
            aby_RxBuff0[w_RxPointer++] = by_Data;
            if(++by_IndexCounter >= 2)
            {
                by_IndexCounter = 0;
                R_State = RLen;
            }
            break;
        case RLen:
            aby_RxBuff0[w_RxPointer++] = by_Data;
            if(++by_IndexCounter >= 2)
            {
                by_IndexCounter = 0;
                w_DataLength = aby_RxBuff0[w_RxPointer - 2] + (by_Data << 8);
                
                if(w_DataLength >= RXBUFF_LENGTH)                               //超过最大长度回到同步头
                {
                    Com.StartRx = 0;
                    w_RxPointer = 0;
                    R_State = RHead;
                }
                else if(w_DataLength == 0)
                {
                    R_State = RCRC;
                }
                else
                {
                    R_State = RData;
                }
            }
            break;
        case RData:
            aby_RxBuff0[w_RxPointer++] = by_Data;
            if(--w_DataLength == 0)
            {
                R_State = RCRC;
            }
            break;
        case RCRC:
            aby_RxBuff0[w_RxPointer++] = by_Data;
            if(++by_IndexCounter >= 2)
            {
                memcpy(aby_RxTemp, aby_RxBuff0, sizeof(aby_RxBuff0));
                w_RxTempLength = w_RxPointer;
                by_IndexCounter = 0;
                R_State = RHead;
                Com.StartRx = 0;
                by_RxTimeOut = 0;
                w_RxPointer = 0;
            }
            break;
        default:
            R_State = RHead;
            break;
        }
    }
}

void Com_10msInt(void)
{
    if(Com.TxBuzy)                                                              //防止发送超时锁死
    {
        if(by_TxTimeOut++ > C_TX_TIMEOUT)
        {
            by_TxTimeOut = 0;
            Com.TxBuzy = 0;
            memset(aby_TxBuff, 0, sizeof(aby_TxBuff));                        //清空发送数据BUFF
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
            memset(aby_RxBuff0, 0, sizeof(aby_RxBuff0));                        //清空接收数据BUFF
        }
    }
    else
    {
        by_RxTimeOut = 0;
    }
}

UCHAR Com_IsBuzy(void)
{
    return Com.TxBuzy;
}

//CRC16-CCITT algorithm
UINT16 Com_Cal_CRC(UCHAR *pby_Buff, UINT16 wSize)
{
    UINT16 crc = 0xFFFF;
    
    if(pby_Buff && wSize)
    {
        while (wSize--)
        {
            crc  = (crc >> 8) | (crc << 8);
            crc ^= *pby_Buff++;
            crc ^= ((UCHAR) crc) >> 4;
            crc ^= crc << 12;
            crc ^= (crc & 0xFF) << 5;
        }
    }
    
    return crc;
}

void Com_Erp(void)
{
    Com_Hw_Erp();
}


