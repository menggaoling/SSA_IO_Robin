#ifndef __MyType_H
#define __MyType_H

#define DOn                     1
#define DOff                    0
#define DTrue                   1
#define DFalse                  0
#define DEnable                 1
#define DDisable                0
#define DUp                     1
#define DDown                   0
#define DMale                   1   // 與 CSAFE 定義相同
#define DFemale                 2
#define DGood                   1
#define DFail                   0
#define DRun                    1
#define DStop                   0

#define DBit0                   0x0001
#define DBit1                   0x0002
#define DBit2                   0x0004
#define DBit3                   0x0008
#define DBit4                   0x0010
#define DBit5                   0x0020
#define DBit6                   0x0040
#define DBit7                   0x0080
#define DBit8                   0x0100
#define DBit9                   0x0200
#define DBit10                  0x0400
#define DBit11                  0x0800
#define DBit12                  0x1000
#define DBit13                  0x2000
#define DBit14                  0x4000
#define DBit15                  0x8000
#define DBit16                  0x00010000
#define DBit17                  0x00020000
#define DBit18                  0x00040000
#define DBit19                  0x00080000
#define DBit20                  0x00100000
#define DBit21                  0x00200000
#define DBit22                  0x00400000
#define DBit23                  0x00800000
#define DBit24                  0x01000000
#define DBit25                  0x02000000
#define DBit26                  0x04000000
#define DBit27                  0x08000000
#define DBit28                  0x10000000
#define DBit29                  0x20000000
#define DBit30                  0x40000000
#define DBit31                  0x80000000
#define DBitA                   0x0400
#define DBitB                   0x0800
#define DBitC                   0x1000
#define DBitD                   0x2000
#define DBitE                   0x4000
#define DBitF                   0x8000

typedef unsigned char           U8;
typedef const unsigned char     CU8;
typedef unsigned short          U16;
typedef signed short            S16;
typedef const unsigned short    CU16;
typedef unsigned long           U32;
typedef const unsigned long     CU32;

typedef unsigned short          UC8;

typedef unsigned char*          UPtr8;
typedef unsigned short*         UPtr16;
typedef unsigned long*          UPtr32;

typedef unsigned short          BOOL;

typedef unsigned int            UP;
typedef unsigned int            UX;

typedef void(*TCallF)(void);
typedef void(*TCallF_U16)(U16 X);
typedef void(*TCallF_voidPtr)( void* );

#endif
