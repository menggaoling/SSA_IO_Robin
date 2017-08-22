#include "LCB_Hal.h"


#define TX_START            0x00
#define RX_START            0x01

#define ADR_START           0
#define ADR_ADDRESS         1
#define ADR_STATUS          1
#define ADR_COMMAND         2
#define ADR_LENGTH          3
#define ADR_DATA	    4

#define C_TX_TIMEOUT        50      //ms
#define C_RX_TIMEOUT        50      //ms
#define C_ACK_TIMEOUT       150     //ms
#define C_OK_INTERVAL       50      //ms
#define C_INTERVAL_TIME     220     //ms 200 + 20(发送所需时间)
#define C_RETX_TIMES        4       //24 //重发次数 24+1=25次 protocol page 6. If console retry 25 times after, Console will have 04B0 error code happening


typedef enum {RStart = 0, RStatus, RCommd, RLen, RData, RCRC}   RX_STEP;


typedef enum
{
    TRAN_IDLE = 0,
    TRAN_TX,        //在发送阶段
    TRAN_ACK,       //在等待回复阶段
    TRAN_OK,        //一次传输成功
} TRANSTEP;


__no_init static UCHAR aby_RxBuffer[RX_BUFFER_MAX + 5];                         //串口接收buff
__no_init static UCHAR aby_RxCopy[RX_BUFFER_MAX + 5];                           //串口接收buff
__no_init static UCHAR aby_TxBuffer[TX_BUFFER_MAX + 5];                         //串口发送buff
__no_init static UCHAR by_RxPointer;
__no_init static UCHAR by_RxSize;
__no_init static UCHAR by_TxPointer;
__no_init static UCHAR by_TxLengthCopy;
__no_init static UCHAR by_TxLength;
__no_init static UCHAR by_ReTxTimes;                                            //重发次数计数
__no_init static UCHAR by_RxTimeOutCounter;
__no_init static UINT16 w_TimerCounter;                                         //计时器
__no_init static UINT16 w_TimerInterval;                                        //两次数据发送之间计时器
__no_init RX_STEP RxState;
__no_init TRANSTEP TranState;
__no_init static struct
{
    unsigned RxStart        : 1;
    unsigned RxOK           : 1;
    unsigned RxIRError      : 1;
} Uart;


const UCHAR aby_CRC8_Table[16] =
{
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E
};

void LCB_Hal_Initial(void)
{
    LCB_Hw_Init(9600);
    memset(aby_RxCopy,   0, sizeof(aby_RxCopy));
    memset(aby_RxBuffer, 0, sizeof(aby_RxBuffer));
    memset(aby_TxBuffer, 0, sizeof(aby_TxBuffer));
    memset(&Uart,        0, sizeof(Uart));

    by_RxSize = 0;
    by_RxPointer = 0;
    by_TxPointer = 0;
    by_TxLengthCopy = 0;
    by_TxLength = 0;
    by_ReTxTimes = 0;
    by_RxTimeOutCounter = 0;
    w_TimerCounter = 0;
    w_TimerInterval = 0;
    RxState = RStart;
    TranState = TRAN_IDLE;

    LCB_Hw_Set_RxFuct(LCB_Hal_Rx_Int);
    LCB_Hw_Set_TxFuct(LCB_Hal_Tx_Int);
}

UCHAR LCB_Hal_Get_Cmd(CMD_RX *CmdRx)
{
    UCHAR by_Out = 0;

    if(Uart.RxOK)
    {
        Uart.RxOK = 0;

        UCHAR by_CRC = LCB_Hal_CRC8_Check(aby_RxCopy, by_RxSize);
        if(by_CRC == aby_RxBuffer[aby_RxBuffer[ADR_LENGTH] + 4])
        {
            memset(CmdRx, 0, sizeof(CmdRx));
            CmdRx->Status = aby_RxCopy[ADR_STATUS];
            CmdRx->Cmd = aby_RxCopy[ADR_COMMAND];
            CmdRx->Len = aby_RxCopy[ADR_LENGTH];
            CmdRx->Crc = by_CRC;
            
            if(CmdRx->Len > 0)
            {
                memcpy(CmdRx->Data, &aby_RxCopy[ADR_DATA], CmdRx->Len);
            }

            w_TimerCounter = 0;
            TranState = TRAN_OK;
            by_Out = 1;
        }
    }

    return by_Out;
}

void LCB_Hal_1ms_Int(void)
{
    //接收超时处理
    if(Uart.RxStart)
    {
        if(by_RxTimeOutCounter++ > C_RX_TIMEOUT)
        {
            by_RxTimeOutCounter = 0;
            Uart.RxStart = 0;
            RxState = RStart;
        }
    }
    else
    {
        by_RxTimeOutCounter = 0;
    }

    if(w_TimerInterval < 0xffff) w_TimerInterval++;
    if(w_TimerCounter < 0xffff) w_TimerCounter++;                               //通讯状态机处理
    
    
    switch(TranState)
    {
    default:
    case TRAN_IDLE:
        break;
    case TRAN_TX:
        if(w_TimerCounter >= C_TX_TIMEOUT)                                      //发送超时重发
        {
            w_TimerCounter = 0;
            LCB_Hal_TxBuff(by_TxLengthCopy);
        }
        break;
    case TRAN_ACK:
        if(w_TimerCounter >= C_ACK_TIMEOUT)                                     //未收到回复重发,根据通讯协议第6页
        {
            w_TimerCounter = 0;
            if(by_ReTxTimes >= C_RETX_TIMES)
            {
                if(aby_TxBuffer[ADR_ADDRESS] == 0x26)
                {
                    Uart.RxIRError = 1;
                }
                
                TranState = TRAN_IDLE;                                          //重发达到3次后复位状态机
            }
            else
            {
                if(by_ReTxTimes < 0xFF) by_ReTxTimes++;
                LCB_Hal_TxBuff(by_TxLengthCopy);
            }
        }
        break;
    case TRAN_OK:
        //if(w_TimerCounter >= C_OK_INTERVAL && w_TimerInterval > C_INTERVAL_TIME)//收到回复后等待50ms复位状态机,通讯协议第6页
        if(w_TimerCounter >= C_OK_INTERVAL)
        {
            w_TimerInterval = 0;
            w_TimerCounter = 0;
            TranState = TRAN_IDLE;
        }
        break;
    }

    LCB_Hw_1ms_Int();
}

UCHAR LCB_Hal_Tx_Busy(void)
{
    return (UCHAR) TranState == TRAN_IDLE ? 0 : 1;
}

void LCB_Hal_Send_Cmd(CMD_TX *CmdTx)
{
    if(LCB_Hal_Tx_Busy()) return;
    if(CmdTx->Adress == 0x26 && Uart.RxIRError) return;
    
    aby_TxBuffer[ADR_START] = TX_START;
    aby_TxBuffer[ADR_ADDRESS] = CmdTx->Adress;
    aby_TxBuffer[ADR_COMMAND] = CmdTx->Cmd;
    aby_TxBuffer[ADR_LENGTH] = CmdTx->Len;

    if(CmdTx->Len <= TX_BUFFER_MAX)                                             //超过数组最大长度,退出
    {
        for(UCHAR i = 0; i < CmdTx->Len; i++)
        {
            aby_TxBuffer[ADR_DATA + i] = *(CmdTx->Data + i);
        }
    }
    else
    {
        return;
    }

    aby_TxBuffer[CmdTx->Len + 4] = LCB_Hal_CRC8_Check(aby_TxBuffer, CmdTx->Len + 4);
    by_TxLengthCopy = CmdTx->Len + 5;

    by_ReTxTimes = 0;
    w_TimerCounter = 0;
    LCB_Hal_TxBuff(by_TxLengthCopy);
}

void LCB_Hal_Send_Buff(UCHAR *by_Data, UCHAR by_Len)
{
    if(LCB_Hal_Tx_Busy()) return;
    if(Uart.RxIRError) return;
    
    if(by_Len <= TX_BUFFER_MAX)                                                 //超过数组最大长度,退出
    {
        memcpy(aby_TxBuffer, by_Data, by_Len);
    }
    else
    {
        return;
    }

    by_TxLengthCopy = (aby_TxBuffer[ADR_LENGTH] + 5);
    
    by_ReTxTimes = 0;
    w_TimerCounter = 0;
    LCB_Hal_TxBuff(by_TxLengthCopy);
}

void LCB_Hal_TxBuff(UCHAR by_Len)
{
    by_TxPointer = 0;
    by_TxLength = by_Len;
    TranState = TRAN_TX;
    LCB_Hw_RS485_SetToTx();
    LCB_Hal_Tx_Int();
}

void LCB_Hal_Tx_Int(void)
{
    if(by_TxPointer >= by_TxLength)
    {
        by_TxLength = 0;
        TranState = TRAN_ACK;                                                   //发送完毕等待下控回复
        LCB_Hw_RS485_SetToRx();
    }
    else
    {
        LCB_Hw_Send_Byte(aby_TxBuffer[by_TxPointer++]);
    }
}

void LCB_Hal_Rx_Int(UCHAR by_Data)
{
    UCHAR by_RxBuff = by_Data;
    
    switch(RxState)
    {
    case RStart:
        if(by_RxBuff != RX_START)
        {
            RxState = RStart;
        }
        else
        {
            Uart.RxStart = 1;
            by_RxTimeOutCounter = 0;
            by_RxPointer = 0;
            aby_RxBuffer[by_RxPointer++] = by_RxBuff;
            RxState = RStatus;
        }
        break;
    case RStatus:
        aby_RxBuffer[by_RxPointer++] = by_RxBuff;
        RxState = RCommd;
        break;
    case RCommd:
        aby_RxBuffer[by_RxPointer++] = by_RxBuff;
        RxState = RLen;
        break;
    case RLen:
        aby_RxBuffer[by_RxPointer++] = by_RxBuff;
        
        if(by_RxBuff == 0)
            RxState = RCRC;
        else if(by_RxBuff >= RX_BUFFER_MAX)
            RxState = RStart;
        else
            RxState = RData;
        break;
    case RData:
        if((by_RxPointer - 4 + 1) >= aby_RxBuffer[ADR_LENGTH])                  //接收指定长度的Data
        {
            RxState = RCRC;
        }
        
        aby_RxBuffer[by_RxPointer++] = by_RxBuff;
        break;
    case RCRC:
        aby_RxBuffer[by_RxPointer] = by_RxBuff;
        memcpy(aby_RxCopy, aby_RxBuffer, sizeof(aby_RxCopy));
        Uart.RxStart = 0;
        by_RxTimeOutCounter = 0;
        Uart.RxOK = 1;
        RxState = RStart;
        by_RxSize = by_RxPointer;
        by_RxPointer = 0;
        break;
    default:
        RxState = RStart;
        break;
    }
}

UCHAR LCB_Hal_CRC8_Check(UCHAR *pby_Data, UCHAR by_Length)
{
    UCHAR by_CRC = 0;
    UCHAR by_Temp = 0;
    UCHAR by_Data = 0;

    for(UCHAR by_Loop = 0; by_Loop < by_Length; by_Loop++)
    {
        by_Data  = *(pby_Data + by_Loop);

        by_Temp  = (by_CRC >> 4);
        by_CRC <<= 4;
        by_CRC  ^= aby_CRC8_Table[by_Temp ^ (by_Data >> 4)];

        by_Temp  = (by_CRC >> 4);
        by_CRC <<= 4;
        by_CRC  ^= aby_CRC8_Table[by_Temp ^ (by_Data & 0x0F)];
    }

    return (by_CRC);
}

UCHAR LCB_Hal_Get_Rx(void)
{
    return LCB_Hw_Get_Rx();
}

void LCB_Hal_Erp(void)
{
    LCB_Hw_Erp();
}



