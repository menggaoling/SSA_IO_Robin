#ifndef __FLASH_H__
#define __FLASH_H__

#include <includes.h>
#include "hw.h"


//���ݳ���: һ����Ϊ1024byte,size = һҳ��С/��Ϊ������/ = 1024/4=256
#define C_PAGE_SIZE             1024

//һ��mass��Ϊ����block����,���Ҫ��Ϊ���ã���д������Ҫ���C_MASSx_BLOCK_SIZE ��ͬ,��Flash_Init() and Flash_Buffer_Write_Mass() ����Ӧ�޸ļ���
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


