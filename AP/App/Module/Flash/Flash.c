#include "Flash.h"
#include "WatchDog.h"


/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_FRIST ((UINT32)0x08000000)
#define ADDR_FLASH_SECTOR_1     ((UINT32)0x0801F000)
#define ADDR_FLASH_SECTOR_2     ((UINT32)0x0801F400)
#define ADDR_FLASH_SECTOR_3     ((UINT32)0x0801F800)
#define ADDR_FLASH_BOOTLOADER   ((UINT32)0x0801FC00)


__no_init static UCHAR aby_Info_Buffer[C_BUFF_SIZE];
__no_init static UCHAR by_Mass1_Block;


void Flash_Init(void)
{
    FLASH_EnableWriteProtection(FLASH_WRProt_Pages0to3);                        //�ص㱣����һ����,������д
    
    memset(aby_Info_Buffer, 0xff, sizeof(aby_Info_Buffer));
    
    for(signed char by_Loop = C_MASS1_BLOCK - 1; by_Loop >= 0; by_Loop--)       //��mass1�Ĵ洢��־λ
    {
        by_Mass1_Block = by_Loop;                                               //��¼��ǰΪ�ڼ�С����
        UINT16 *pw_Addr = (UINT16*)(ADDR_FLASH_SECTOR_2 + (C_MASS1_BLOCK_SIZE * by_Loop) + (C_MASS1_BLOCK_SIZE - 2));
        if(*pw_Addr == D_FLASH_FLAG) break;
        
        if(by_Loop == 0)                                                        //��һ���ϵ��趨ʱ���Ҳ���Ŀ��λ������һ��д�룬�趨һ��Ŀ��λ
        {
            by_Mass1_Block = C_MASS1_BLOCK;                                     //��һ���������Ҳ�����־λ��Block��Ϊ���һ�Σ�����һ��д��ʱ��ִ�в���
            Flash_Buffer_Write_Mass(0);
            break;
        }
    }
}

UCHAR* Flash_Get_Point(void)
{
    return aby_Info_Buffer;
}

//��ȡһ��Mass�����ݵ�����
void Flash_Buffer_Read_Mass(UCHAR by_Page)
{
    UINT32 *P32 = NULL;
    UINT16 by_Flash_Length = 0;
    
    switch(by_Page)
    {
    case 0:
        {
            P32 = (UINT32 *)(ADDR_FLASH_SECTOR_2 + by_Mass1_Block * C_MASS1_BLOCK_SIZE);//count address
            by_Flash_Length = C_MASS1_BLOCK_SIZE;
            break;
        }
    default:return;
    }
    
    //ת��32λ��һ��4��BYTE
    UINT32 *Dist = (UINT32 *)aby_Info_Buffer;
    memcpy(Dist, P32, by_Flash_Length);
}

UINT32 Flash_Buffer_Write_Mass(UCHAR by_Page)
{
    UINT32 Address = 0;
    UINT16 by_Flash_Length = 0;
    
    switch(by_Page)
    {
    case 0:
        {
            by_Mass1_Block++;
            Address = ADDR_FLASH_SECTOR_2 + by_Mass1_Block * C_MASS1_BLOCK_SIZE;//count address 5*256
            
            by_Flash_Length = C_MASS1_BLOCK_SIZE;
            
            //���õ�mass��Ҫ��־λ�����ı�־λ
            aby_Info_Buffer[C_MASS1_BLOCK_SIZE-1] = (UCHAR)D_FLASH_FLAG;
            aby_Info_Buffer[C_MASS1_BLOCK_SIZE-2] = (UCHAR)(D_FLASH_FLAG >> 8);
            
            //һ����mass������󣬲������ӵ�һ��block��ʼд��
            if(by_Mass1_Block >= C_MASS1_BLOCK)
            {
                Address = ADDR_FLASH_SECTOR_2;
                by_Mass1_Block = 0;
                if(FLASH_ErasePage(Address) != FLASH_COMPLETE) return ERR_ERASE;
            }
            break;
        }
    default:
        return ERROR;
    }

    UINT32 *pw = (UINT32 *)aby_Info_Buffer;
    UINT32 w_Address_End = Address + by_Flash_Length;
    
    Watchdog_Reset();
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);// | FLASH_FLAG_OPTERR
    
    while(Address < w_Address_End)
    {
        if(FLASH_ProgramWord(Address, *(pw++)) == FLASH_COMPLETE)
        {
            Address += 4;       //ÿ�μ�4
        }
        else
        { 
            return ERR_WRITE;
        }
    }
    
    FLASH_Lock();
    return RIGHT;
}


void Flash_Read_Page(UINT32 wAdr, UCHAR *pData, UINT16 wSize)
{
    //ת��32λ��һ��4��BYTE
    UCHAR *Src  = (UCHAR*)(wAdr);
    UCHAR *Dist = (UCHAR*)pData;
    
    //��Flash��ȡ���ݵ�Buff
    memcpy(Dist, Src, wSize);
}

UCHAR Flash_Write_Page(UINT32 wAdr, UCHAR *pData, UINT16 wSize)
{
    //ת��32λ��һ��4��BYTE
    UINT32 *Dist = (UINT32 *)pData;
    UINT32 Address = wAdr;
    UINT32 w_Address_End = Address + wSize;
    
    if(wAdr < 0x08004000) return 0;    //��Bootloader��, ���ò�д
    
    Watchdog_Reset();
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);// | FLASH_FLAG_OPTERR
    
    while(Address < w_Address_End)
    {
        if(FLASH_ProgramWord(Address, *(Dist++)) == FLASH_COMPLETE)
        {
            Address += 4;       //ÿ�μ�4
        }
        else
        {
            FLASH_Lock();
            return 0;
        }
    }
    
    FLASH_Lock();
    return 1;
}

void Flash_WriteUpdateFlag(void)
{
    UCHAR aby_Buff[C_PAGE_SIZE] = {0};
    
    Flash_Read_Page(ADDR_FLASH_BOOTLOADER, aby_Buff, C_PAGE_SIZE);
    
    UN32 un32;
    un32.DWord = D_UPDATE_FLAG;
    aby_Buff[0] = un32.Byte[0];
    aby_Buff[1] = un32.Byte[1];
    aby_Buff[2] = un32.Byte[2];
    aby_Buff[3] = un32.Byte[3];
    
    Flash_Write_Page(ADDR_FLASH_BOOTLOADER, aby_Buff, C_PAGE_SIZE);
}


