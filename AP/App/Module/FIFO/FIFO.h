#ifndef __FIFO_H__
#define __FIFO_H__


#include "includes.h"


typedef enum
{
    FIFO_ERROR_NONE     = 0,
    FIFO_ERROR_FULL,
    FIFO_ERROR_NOSPACE,         //无空间错误
} FIFO_ERROR;


typedef struct
{
    UCHAR  *pBuff;      //Buff区首地址
    UCHAR  *ReadAdr;    //读地址指针
    UCHAR  *WriteAdr;   //写地址指针
    UINT16  BuffSize;   //Buff区大小
} FIFO_BASE;


FIFO_ERROR FIFO_Initial(FIFO_BASE *FIFO_Base, UCHAR *pAdr, UINT16 Size);
FIFO_ERROR FIFO_Input(FIFO_BASE *FIFO_Base, UCHAR *pAdr, UINT16 Size);
UINT16 FIFO_Output(FIFO_BASE *FIFO_Base, UCHAR *pAdr, UINT16 Size);
UINT16 FIFO_Peek(FIFO_BASE *FIFO_Base);


#endif





