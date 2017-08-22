#include "Trace.h"


#define TRACE_BUFFER_MAX	    200
#define TRACE_TX_TIMEOUT        50     //unit ms


//#define USE_TRACE

__no_init static UCHAR  by_TxPointer;
__no_init static UCHAR  by_TxLength;
__no_init static UCHAR  aby_TxBuffer[TRACE_BUFFER_MAX];
__no_init static UINT16 w_CountMsTx;
__no_init static struct
{
    unsigned CmdTxBusy        : 1;
} Uart;


void Trace_Initial(void)
{
#ifdef USE_TRACE
    Trace_Hw_Init();

    memset(aby_TxBuffer, 0, sizeof(aby_TxBuffer));
    memset(&Uart       , 0, sizeof(Uart));

    by_TxPointer = 0;
    by_TxLength = 0;
    w_CountMsTx = 0;
    
    Trace_Hw_Set_RxFuct(Trace_Rx_Int);
    Trace_Hw_Set_TxFuct(Trace_Tx_Int);
#endif
}

//非阻塞发送
void Trace(CHAR *p_fmt, ...)
{
#ifdef USE_TRACE
    if(Uart.CmdTxBusy) return;
    Uart.CmdTxBusy = 1;

    CHAR    str[TRACE_BUFFER_MAX + 1];
    UCHAR   len = 0;
    memset(str, 0, sizeof(str));
    va_list vArgs;

    va_start(vArgs, p_fmt);
    vsprintf((char *)str, (char const *)p_fmt, vArgs);
    va_end(vArgs);

    len = strlen((char const *)str);
    Trace_Tx_String((UCHAR *)str, len);
#endif
}

//阻塞发送
void TraceW(CHAR *p_fmt, ...)
{
#ifdef USE_TRACE
    while(Uart.CmdTxBusy);                                                      //等待上个数据发送完毕再开始
    Uart.CmdTxBusy = 1;

    CHAR    str[TRACE_BUFFER_MAX + 1];
    UCHAR   len = 0;
    memset(str, 0, sizeof(str));
    va_list vArgs;

    va_start(vArgs, p_fmt);
    vsprintf((char *)str, (char const *)p_fmt, vArgs);
    va_end(vArgs);

    len = strlen((char const *)str);
    Trace_Tx_String((UCHAR *)str, len);
    while(Uart.CmdTxBusy);                                                      //等待发送完毕再出函数
#endif
}

void Trace_Tx_String(UCHAR *pString, UCHAR byLength)
{
    memcpy(aby_TxBuffer, pString, byLength);
    by_TxPointer = 0;
    by_TxLength = byLength;
    Trace_Tx_Int();
}

void Trace_Rx_Int(UCHAR by_Data)
{
    //丢失不用
}

void Trace_Tx_Int(void)
{
    if(by_TxPointer >= by_TxLength)
    {
        Uart.CmdTxBusy = 0;
        by_TxPointer = 0;
        by_TxLength = 0;
        w_CountMsTx = 0;
    }
    else
    {
        Trace_Hw_Send_Byte(aby_TxBuffer[by_TxPointer++]);
    }
}

void Trace_10ms_Int(void)
{
    if(Uart.CmdTxBusy)
    {
        if(++w_CountMsTx > TRACE_TX_TIMEOUT)
        {
            Uart.CmdTxBusy = 0;
            by_TxPointer = 0;
            by_TxLength = 0;
            w_CountMsTx = 0;
        }
    }
    else
    {
        w_CountMsTx = 0;
    }
}

void Trace_Erp(void)
{
#ifdef USE_TRACE
    Trace_Hw_Erp();
#endif
}

