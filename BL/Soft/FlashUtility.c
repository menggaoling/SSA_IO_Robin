/* Includes ------------------------------------------------------------------*/
#include "bootloader.h"
#include "MyType.h"
#include "MyStruct.h"
#include "stm32f10x_lib.h"
//#include "main.h"
#include "FlashUtility.h"
#include "UpdateUart.h"
#ifdef  _Debug_BSL_
//#include "debuguart5.h"
#endif

vu32 NbrOfPage = 0;
volatile FLASH_Status FLASHStatus;
U32  FlashAddress    = StartAddr;

U8   ProgramLock = 0x01;
UC8  GUC8_EraseMark;
U32  GU32_ReadFlashAddress;
U32  GU32_FlashEraseIndex[DFlashEraseMaskBufferSize];

const unsigned char CU8_BslMessage[20] =
{
    DeviceFamilyType_LO,    // 0x6F
    DeviceFamilyType_HI,    // 0xF2
    Process_LO,             // 0x60
    Process_HI,             // 0x04
    0,
    0,
    0,
    0,
    0,
    0,
    BSLVersion_LO,          // 0x12
    BSLVersion_HI,          // 0x02
    'J',
    'o',
    'h',
    'n',
    's',
    'o',
    'n',
    0
};

CU32 GU32_FlashBitMask[] =
{
    DBit0,  DBit1,  DBit2,  DBit3,  DBit4,  DBit5,  DBit6,  DBit7, \
    DBit8,  DBit9, DBit10, DBit11, DBit12, DBit13, DBit14, DBit15, \
    DBit16, DBit17, DBit18, DBit19, DBit20, DBit21, DBit22, DBit23, \
    DBit24, DBit25, DBit26, DBit27, DBit28, DBit29, DBit30, DBit31
};

//*****************************************************************************
// Function Name  : EnterFlashProgramming
// Description    : Enter Flash Programming
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void EnterFlashProgramming(void)
{
    U32 index = 0;

    FlashAddress          = StartAddr;
    GU32_ReadFlashAddress = FlashAddress;

    for ( index = 0; index < (U32)DFlashEraseMaskBufferSize; index++ )
    {
        GU32_FlashEraseIndex[index] = 0;
    }
    GUC8_EraseMark = 0;
}

//*****************************************************************************
// Function Name  : CheckFlashEraseStatus
// Description    : Flash Erase 1 Page (2KB)
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void CheckFlashEraseStatus( U32 PU32_Address )
{
    U32     LU32_Address, LU32_PageIndex;

    LU32_Address   = PU32_Address;
    LU32_Address  &= ((EndAddr - 0x8000000) - 1);
    LU32_Address  /= (U32)PageSize;             // PageSize = 2K
    LU32_PageIndex = LU32_Address / 32;
    LU32_Address  &= (32 - 1);
    if( !(GU32_FlashEraseIndex[LU32_PageIndex] & GU32_FlashBitMask[LU32_Address]) )
    {
        FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        FLASH_Unlock();
        FLASH_ErasePage( PU32_Address );
        FLASH_Lock();
        GU32_FlashEraseIndex[LU32_PageIndex] |= GU32_FlashBitMask[LU32_Address];
    }
}

//*****************************************************************************
// Function Name  : FlashErase
// Description    : Flash Erase
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
U8   FlashErase(U32 EraseAddr, U32 Length)
{
    U32  EraseCounter, MaxAddr, TopAddr;
    U32  LU32_Address, LU32_PageIndex;
    vu32 NbrOfPage = 0;
    Byte16  LB16_WriteData;

    if (ProgramLock == 0x01)
        return  DFalse;
#if 0
    // 當執行過Erase command後，執行下列Command將FLASH MARK破壞，用以保證更新失敗也不會跑至AP
    if (CheckFlashMark())
    {
        FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR );
        FLASH_Unlock();
        LB16_WriteData.Word = 0x0000;
        FLASHStatus = FLASH_ProgramHalfWord(EndAddr - 16, LB16_WriteData.Word);
        FLASH_Lock();
    }
#endif
    MaxAddr = EraseAddr + Length;

    if (MaxAddr > EndAddr)
    {
#ifdef  _Debug_BSL_
        puts("\n> Error: MaxAddr > Flash Size !! ");
        puts("\n> MaxAddr: ");
        vPut4HEX(MaxAddr);
#endif
        return  DFalse;
    }

    /* Define the number of page to be erased */
    NbrOfPage = EraseAddr / (U32) PageSize;
    TopAddr   = PageSize * NbrOfPage ;

    NbrOfPage = MaxAddr / (U32) PageSize;
    MaxAddr   = PageSize * NbrOfPage ;

#ifdef  _Debug_BSL_
    puts("\n> TopAddr: ");
    vPut4HEX(TopAddr);
    puts("\n> MaxAddr: ");
    vPut4HEX(MaxAddr);
#endif
    for (EraseCounter = TopAddr; EraseCounter < MaxAddr ; EraseCounter += (U32)PageSize)
    {
#ifdef  _Debug_BSL_
        puts("\n> EraseAddr: ");
        vPut4HEX(EraseCounter);
        vDBGUartSendTx();
#endif
        LU32_Address   = EraseCounter;
        LU32_Address  &= ((EndAddr - 0x8000000) - 1);
        LU32_Address  /= (U32)PageSize;             // PageSize = 2K
        LU32_PageIndex = LU32_Address / 32;
        LU32_Address  &= (32 - 1);
        if ( !(GU32_FlashEraseIndex[LU32_PageIndex] & GU32_FlashBitMask[LU32_Address]) )
        {
            /* Clear All pending flags */
            FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
            /* Unlock the Flash Program Erase controller */
            FLASH_Unlock();
            FLASHStatus = FLASH_ErasePage( EraseCounter );
            FLASH_Lock();
            GU32_FlashEraseIndex[LU32_PageIndex] |= GU32_FlashBitMask[LU32_Address];

            if (FLASHStatus != FLASH_COMPLETE)
            {
                return  DFalse;
            }
        }
    }
    return  DTrue;
}

//*****************************************************************************
// Function Name  : FlashWrite
// Description    : Flash Write
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
U8  FlashWrite(U32  WriteAddr, U16 Length, U8 *updbuf)
{
    U16     i;
    Byte16  LB16_WriteData;

    if (ProgramLock == 0x01)
        return  DFalse;
#if 0
    // 當執行過Wrie command後，執行下列Command將FLASH MARK破壞，用以保證更新失敗也不會跑至AP
    if (CheckFlashMark())
    {
        FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR );
        FLASH_Unlock();
        LB16_WriteData.Word = 0x0000;
        FLASHStatus = FLASH_ProgramHalfWord(EndAddr - 16, LB16_WriteData.Word);
        FLASH_Lock();
    }
#endif
    //
    FlashAddress = WriteAddr;
    if ((FlashAddress < EndAddr) ||
            ((FlashAddress == DFlashMarkAddress) || (FlashAddress == DAPVersionAddress)))
    {
        i = 0;
        while (i < Length)
        {
#ifdef  __ICC8051__
            LB16_WriteData.LByte = updbuf[i++];
            LB16_WriteData.HByte = updbuf[i++];
#endif
#ifdef  __C51__
            LB16_WriteData.bytes.LByte = updbuf[i++];
            LB16_WriteData.bytes.HByte = updbuf[i++];
#endif
            if ( FlashAddress >= StartAddr )
            {
                CheckFlashEraseStatus( FlashAddress );
                FLASH_ClearFlag( FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR );
                FLASH_Unlock();
                FLASHStatus = FLASH_ProgramHalfWord(FlashAddress, LB16_WriteData.Word);
                FLASH_Lock();
                FlashAddress = FlashAddress + 2;
            }
            else
            {
                return  DFalse;
            }
        }
    }
    else
    {
        return  DFalse;
    }
    return  DTrue;
}


//*****************************************************************************
// Function Name  : FlashRead
// Description    : Flash Read
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
U8  FlashRead(U32 ReadAddr, U16 Length, U8 *pbuf)
{
    U8      *src_ptr = (U8 *) ReadAddr;
    U16     i;

    if ((ReadAddr < EndAddr) ||
            ((ReadAddr == DFlashMarkAddress) || (ReadAddr == DAPVersionAddress)))
    {
        i = 0;
        while (i < Length)
        {
            pbuf[i] = *src_ptr;
            i++;
            src_ptr++;
        }
    }
    else
    {
        return  DFalse;
    }
    return  DTrue;
}


//*****************************************************************************
// Function Name  : ShowFlashContent
// Description    : Show Flash Content
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void ShowFlashContent(U8 *updbuf)
{
#ifdef  _Debug_BSL_
    U16     i;

    puts("\n> updbuf: ");
    vDBGUartSendTx();
    i = 0;
    while (i < 512)
    {
        if ((i % 16) == 0)
        {
            puts("\n");
        }
        vPutHEX(updbuf[i]);
        puts(" ");
        vDBGUartPollTx();
        i++;
    }
    puts("\n");
    vDBGUartSendTx();
#endif
}


//*****************************************************************************
// Function Name  : ShowFlashIndex
// Description    :
// Input          : None
// Output         : None
// Return         : None
//*****************************************************************************
void  ShowFlashIndex(void)
{
    U32     index;
#ifdef  _Debug_BSL_
    puts("\n> FlashEraseIndex: ");
#endif
    for ( index = 0; index < (U32)DFlashEraseMaskBufferSize; index++ )
    {
      #ifdef  _Debug_BSL_
        vPut4HEX(GU32_FlashEraseIndex[index]);
      #endif
    }
}



