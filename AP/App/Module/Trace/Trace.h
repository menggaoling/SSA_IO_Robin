#ifndef __TRACE_H__
#define __TRACE_H__

#include "Trace_Hw.h"


void Trace_Initial(void);
void Trace(CHAR *p_fmt, ...);
void TraceW(CHAR *p_fmt, ...);
void Trace_Tx_String(UCHAR *pString, UCHAR byLength);
void Trace_Tx_Int(void);
void Trace_Rx_Int(UCHAR by_Data);
void Trace_10ms_Int(void);
void Trace_Erp(void);


#endif

