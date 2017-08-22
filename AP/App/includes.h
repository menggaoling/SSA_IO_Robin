#ifndef  _INCLUDES_PRESENT_H_
#define  _INCLUDES_PRESENT_H_

#include <stdarg.h>         //让函数能够接收可变参数
#include <stdio.h>          //标准输入输出
#include <math.h>           //标准数学函数
#include <stdlib.h>         //标准库
#include <string.h>         //标准字符函数库
#include <stdbool.h>        //可用bool变量
#include <intrinsics.h>



typedef unsigned        short       WORD;
typedef unsigned        long        DWORD;
typedef unsigned        char        BYTE;
typedef signed          char        CHAR;
typedef unsigned        char        UCHAR;
typedef signed          char        INT8;
typedef unsigned        char        UINT8;
typedef signed          short       INT16;
typedef unsigned        short       UINT16;
typedef signed          int         INT32;
typedef unsigned        int         UINT32;
typedef signed    long  long        INT64;
typedef unsigned  long  long        UINT64;

//typedef enum {FALSE = 0, TRUE = !FALSE} bool;
typedef union
{
    UINT16 Word;
    UCHAR  Byte[2];
} UN16;

typedef union
{
    UCHAR  Byte[4];
    UINT16 Word[2];
    UINT32 DWord;
} UN32;


#ifndef BIT0
#define BIT0                (0x0001)
#define BIT1                (0x0002)
#define BIT2                (0x0004)
#define BIT3                (0x0008)
#define BIT4                (0x0010)
#define BIT5                (0x0020)
#define BIT6                (0x0040)
#define BIT7                (0x0080)
#define BIT8                (0x0100)
#define BIT9                (0x0200)
#define BIT10               (0x0400)
#define BIT11               (0x0800)
#define BIT12               (0x1000)
#define BIT13               (0x2000)
#define BIT14               (0x4000)
#define BIT15               (0x8000)
#endif


#ifndef BITSET
#define BITSET(X, MASK)     ( (X) |= (MASK) )
#endif

#ifndef BITCLR
#define BITCLR(X, MASK)     ( (X) &= ~(MASK) )
#endif

#ifndef BITGET
#define BITGET(X, MASK)     ( ( (X) & (MASK) ) ? 1 : 0 )
#endif

#define SWAP8(DATA8)        (((DATA8)  << 4 ) | ((DATA8)  >> 4 ))
#define SWAP16(DATA16)      (((DATA16) << 8 ) | ((DATA16) >> 8 ))
#define SWAP32(DATA32)      (((DATA32) << 16) | ((DATA32) >> 16))

#define MAX(a, b)           ((a) > (b) ? (a) : (b))
#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define MID(a, b)           (((a) + (b)) / 2)

#define DINT()              __disable_interrupt()
#define EINT()              __enable_interrupt()
#define NOP()               __no_operation()
#define SWAP_BYTES(x)       __swap_bytes(x)


typedef void (*FNCT_VOID) (void);
typedef void (*FNCT_UCHAR)(UCHAR);


#ifdef	FALSE
#undef	FALSE
#endif
#define FALSE	(0)

#ifdef	TRUE
#undef	TRUE
#endif
#define	TRUE	(1)

#ifdef	NULL
#undef	NULL
#endif
#define NULL	(0)

#ifdef  ON
#undef  ON
#endif
#define ON      (1)

#ifdef  OFF
#undef  OFF
#endif
#define OFF     (0)


#endif



