#include "LCB.h"
#include "WatchDog.h"
#include "FIFO.h"
#include "Timer.h"

__no_init static FIFO_BASE FIFO_Base;
__no_init static UCHAR aby_FIFOBuff[512];                                       //FIFO用Buff
__no_init static CMD_RX CmdRx;
static UCHAR lcbType = 0;
static UINT16 lcbVersion = 0;

void LCB_CmdProcess(void);


void LCB_Initial(void)
{
    LCB_Hal_Initial();
    
    FIFO_Initial(&FIFO_Base, aby_FIFOBuff, sizeof(aby_FIFOBuff));
    memset(&CmdRx, 0, sizeof(CmdRx));
}

void LCB_Process(void)
{
    if(LCB_HW_GET_MODE() == 1) return;
    
    LCB_CmdProcess();
}

void LCB_CmdProcess(void)
{
    Watchdog_Reset();
    
    //Data send if bus is idle and FIFO have data
    if(!LCB_Hal_Tx_Busy())
    {
        UCHAR aby_TxBuff[128];
        memset(aby_TxBuff, 0, sizeof(aby_TxBuff));
    
        UCHAR by_TxLength = FIFO_Output(&FIFO_Base, aby_TxBuff, sizeof(aby_TxBuff));
        if(by_TxLength)
        {
            LCB_Hal_Send_Buff(aby_TxBuff, by_TxLength);
        }
    }
}

void LCB_Tx_Data(UCHAR *by_Data, UCHAR by_Len)
{
    if(LCB_HW_GET_MODE() == 1) return;
    if(by_Len <= 128)
    {
        FIFO_Input(&FIFO_Base, by_Data, by_Len);
        LCB_CmdProcess();
    }
}

UCHAR LCB_Rx_Data(UCHAR *by_Data, UCHAR *by_Len)
{
    if(LCB_HW_GET_MODE() == 1) return 0;
    if(LCB_Hal_Get_Cmd(&CmdRx))                                                 //检查是否有收到新的数据
    {
        UCHAR aby_RxBuff[128];
        memset(aby_RxBuff, 0, sizeof(aby_RxBuff));
        
        aby_RxBuff[0] = 0x01;                                                   //RX_START
        aby_RxBuff[1] = CmdRx.Status;
        aby_RxBuff[2] = CmdRx.Cmd;
        aby_RxBuff[3] = CmdRx.Len;
        memcpy(&aby_RxBuff[4], CmdRx.Data, CmdRx.Len);
        aby_RxBuff[CmdRx.Len+4] = CmdRx.Crc;
        
        *by_Len = CmdRx.Len+5;
        memcpy(by_Data, aby_RxBuff, CmdRx.Len+5);
        
        return 1;
    }
    else
    {
        *by_Len = 0;
        return 0;
    }    
}

void LCB_1ms_Int(void)
{
    if(LCB_HW_GET_MODE() == 1) return;
       LCB_Hal_1ms_Int();
}

void LCB_Get_DataPointer(COM_TYPE en_Index, UCHAR *pby, UINT16 *size)
{
    
}

UINT16 LCB_Get_Error(UCHAR bClear)
{
    return 0;
}

UINT16 LCB_Get_Data(COM_TYPE en_Index)
{
    return 0;
}

void LCB_Set_DriverType(UCHAR DriverType)
{
    
}

UCHAR LCB_Get_DriverType(void)
{
    return 0;
}

UCHAR LCB_Get_MCBType(void)
{
    return lcbType;
}

UINT16 LCB_Get_Version(void)
{
    return lcbVersion;
}

void LCB_Set_SpeedRef(UCHAR by_AddStep, UCHAR by_DecStep)
{
    
}

void LCB_GetMCBInfor(void)
{
  UCHAR data = 0xf0;
  UCHAR Rxdata[20];
  UCHAR by_Length;
  UCHAR reTry = 3;
  
  LCB_Send_Cmd2(0x50, &data, 1);
  Timer_Clear(12);
  while(1)
  {
    if(LCB_Rx_Data(Rxdata, &by_Length) > 0)
    {
      lcbType = Rxdata[6];
      lcbVersion = ((UINT16)Rxdata[8]) << 8;
      lcbVersion += Rxdata[9];
      break;
    }
    else
    {
      if(Timer_Counter(12, 10))
      {
        Timer_Clear(12);
        if(reTry > 0)
        {
          --reTry;
          LCB_Send_Cmd2(0x50, &data, 1);
        }
        else
          break;
      }
      
    }
  }
}

void LCB_Get_Induct(UINT16 *pw)
{

}

UCHAR LCB_Erp(void)
{
    return 0;
}

void LCB_Disable(void)
{
    
}

void LCB_Clear_Data(COM_TYPE en_Index)
{
    
}

void LCB_Send_Cmd(COM_TYPE CmdType, UINT32 w_Data)
{
  
}


void LCB_Send_Cmd2(UCHAR CmdType, UCHAR *data, UCHAR by_Len)
{
  UCHAR by_Data[20];
  UCHAR by_index = 0;
   by_Data[by_index++] = 0x00;
   by_Data[by_index++] = 0xff;
   by_Data[by_index++] = CmdType;
   by_Data[by_index++] = by_Len;
   for(UCHAR i = 0; i < by_Len; i++)
     by_Data[by_index++] = data[i];
   by_Data[by_index] = LCB_Hal_CRC8_Check(by_Data, by_index);
   
   ++by_index;
  LCB_Tx_Data(by_Data, by_index);
}


void LCB_Power_On(UCHAR bWakeUP)
{
    
}
