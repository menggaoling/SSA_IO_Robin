#ifndef __FLASH_H__
#define __FLASH_H__

#include <includes.h>
#include "hw.h"


//数据长度: 一个块为1024byte,size = 一页大小/分为几个块/ = 1024/4=256
#define C_PAGE_SIZE             1024

//一个mass分为几个block轮用,如果要改为轮用，则写入数据要与各C_MASSx_BLOCK_SIZE 相同,在Flash_Init() and Flash_Buffer_Write_Mass() 做相应修改即可
#define C_MASS1_BLOCK           4

#define C_MASS1_BLOCK_SIZE      (C_PAGE_SIZE/C_MASS1_BLOCK)
#define C_BUFF_SIZE             C_MASS1_BLOCK_SIZE


#define ERROR                   0
#define RIGHT                   1
#define ERR_ERASE               97  //a
#define ERR_WRITE               98  //b
#define ERR_READADDR            99  //c
#define ERR_USEREXIST           100 //d
#define ERR_NOSTORE             101 //e
#define ERR_USERNOT             102 //f


#define D_FLASH_FLAG            0xA5A5
#define D_UPDATE_FLAG           0xa5a55a5a


void Flash_Init(void);
UCHAR* Flash_Get_Point(void);
void Flash_Buffer_Read_Mass(UCHAR by_Page);
UINT32 Flash_Buffer_Write_Mass(UCHAR by_Page);
void Flash_Read_Page(UINT32 wAdr, UCHAR *pData, UINT16 wSize);
UCHAR Flash_Write_Page(UINT32 wAdr, UCHAR *pData, UINT16 wSize);
void Flash_WriteUpdateFlag(void);


#endif


