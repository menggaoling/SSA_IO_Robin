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
    FLASH_EnableWriteProtection(FLASH_WRProt_Pages0to3);                        //重点保护第一块区,不被改写
    
    memset(aby_Info_Buffer, 0xff, sizeof(aby_Info_Buffer));
    
    for(signed char by_Loop = C_MASS1_BLOCK - 1; by_Loop >= 0; by_Loop--)       //找mass1的存储标志位
    {
        by_Mass1_Block = by_Loop;                                               //记录当前为第几小分区
        UINT16 *pw_Addr = (UINT16*)(ADDR_FLASH_SECTOR_2 + (C_MASS1_BLOCK_SIZE * by_Loop) + (C_MASS1_BLOCK_SIZE - 2));
        if(*pw_Addr == D_FLASH_FLAG) break;
        
        if(by_Loop == 0)                                                        //第一次上电设定时，找不到目标位，则做一次写入，设定一个目标位
        {
            by_Mass1_Block = C_MASS1_BLOCK;                                     //第一次启动，找不到标志位则将Block设为最后一段，再做一次写入时会执行擦除
            Flash_Buffer_Write_Mass(0);
            break;
        }
    }
}

UCHAR* Flash_Get_Point(void)
{
    return aby_Info_Buffer;
}

//读取一个Mass的数据到缓存
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
    
    //转成32位读一次4个BYTE
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
            
            //轮用的mass需要标志位，更改标志位
            aby_Info_Buffer[C_MASS1_BLOCK_SIZE-1] = (UCHAR)D_FLASH_FLAG;
            aby_Info_Buffer[C_MASS1_BLOCK_SIZE-2] = (UCHAR)(D_FLASH_FLAG >> 8);
            
            //一整块mass都用完后，擦除，从第一个block开始写入
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
            Address += 4;       //每次加4
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
    //转成32位读一次4个BYTE
    UCHAR *Src  = (UCHAR*)(wAdr);
    UCHAR *Dist = (UCHAR*)pData;
    
    //从Flash读取数据到Buff
    memcpy(Dist, Src, wSize);
}

UCHAR Flash_Write_Page(UINT32 wAdr, UCHAR *pData, UINT16 wSize)
{
    //转成32位读一次4个BYTE
    UINT32 *Dist = (UINT32 *)pData;
    UINT32 Address = wAdr;
    UINT32 w_Address_End = Address + wSize;
    
    if(wAdr < 0x08004000) return 0;    //低Bootloader区, 不让擦写
    
    Watchdog_Reset();
    FLASH_Unlock();
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);// | FLASH_FLAG_OPTERR
    
    while(Address < w_Address_End)
    {
        if(FLASH_ProgramWord(Address, *(Dist++)) == FLASH_COMPLETE)
        {
            Address += 4;       //每次加4
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


