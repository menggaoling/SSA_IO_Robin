#include "Main.h"


#define TYPE                    "Smart Strength IO"
#define VER_MAJOR               3
#define VER_MINOR               3

#define BootLoaderAddress	    ((UINT32)0x08000000)
#define C_FLASH_MARK1		    0x55AA5AA5
#define C_FLASH_MARK2		    0x24681357
#define C_FLASH_MARK3		    0x9BDEACF0
#define C_FLASH_MARK4			0x0FF0FF00

__root const UCHAR DeviceType[]@ ".ModelType" = TYPE;
__root const UCHAR DeviceVerMajor@ ".VerMajor" = VER_MAJOR;
__root const UCHAR DeviceVerMinor@ ".VerMinor" = VER_MINOR;
__root const UINT32 EndMark[] @ ".EndMark" = { C_FLASH_MARK1, C_FLASH_MARK2, C_FLASH_MARK3, C_FLASH_MARK4 };
__no_init volatile static UINT32 w_UpdateMark @ 0x20007FF0;


__no_init static UN16 un16;
__no_init static UINT16 w_Size;
__no_init static UCHAR aby_Buff[RX_BUFFER_MAX];
__no_init static ERP_STATUS ErpStatus;
__no_init static COM_PACKAGE stDataRx;
__no_init static COM_PACKAGE stDataTx; 
__no_init UCHAR by_LCBUpdataStep;
__no_init UCHAR updateRetryCnt;
static UCHAR lcbSts = 0;

__no_init static struct
{
  unsigned UpdateIOFlag       : 1;
  unsigned CheckSafekey       : 1;
  unsigned SendSafekeyRemove  : 1;        //每次切换状态只发送一次
  unsigned SendSafekeyPlug    : 1;        //每次切换状态只发送一次
  unsigned IRStatuRelease     : 1;        //IR触发后,2秒后才读取是否有释放
  unsigned SendIRRemove       : 1;        //每次切换状态只发送一次
  unsigned SendIRPlug         : 1;        //每次切换状态只发送一次
  unsigned StartRecovery      : 1;        //
  unsigned ResetPress         : 1;
  
  UN32  un32_LCBUpdateLength;
  UCHAR by_LCBReadBuff[60];
  UCHAR by_LCBUpdataPassword[20];
  UCHAR by_LCBReadStep;
  UCHAR by_LCBUpdateCRC;
  UCHAR by_IOType;
  UINT16 LastSendErrorCode;
  UINT16 DelayReset;
} Main;


extern UINT16 __checksum;

void main(void)
{
  Main_Init();
  
  while(1)
  {
    Watchdog_Reset();
    TV_Process();
    LCB_Process();
    Led_Process();
    Heart_Process();
    Main_KeyProcess();
    Main_ComProcess();
    Main_ERP_Process();
    Main_UpdateProcess();
    Main_SafeKeyProcess();
    HW_Test_IO_Process();
    
    //Main_Update_Test();
    
    if(Main.ResetPress == 1 && Timer_Counter(TIMER_RESET_SOC, Main.DelayReset))
    {
      Main.ResetPress = 0;
      Timer_Clear(TIMER_RESET_SOC);
      System_Reset();
    }
  }
}


void Main_KeyProcess(void)
{
  static UCHAR by_KeyRelease = 0;
  UCHAR by_Key = KB_NONE;
  
  by_Key = Keyboard_Get_Code();
  
  if(by_Key != KB_NONE && by_Key != KB_NA)
  {
    System_Wakeup();
  }
  
  switch(by_Key)
  {
  case KB_ENTER_RECOVER:
    Beep_Proce(2,4,2);
    while(!Beep_Get_OK());
    Main.StartRecovery = 1;
    System_OSEnterRecoveryMode();
    break;
  case KB_RESET_SYSTEM:
    Beep_Proce(2,4,2);
    while(!Beep_Get_OK());
    if(Main.StartRecovery)
    {
      Main.StartRecovery = 0;
      Power_UI_Off();
      Power_Set_Power_On(0);
      System_Reset();
    }
    break;
  case KB_NONE:
    if(by_KeyRelease)
    {
      by_KeyRelease = 0;
      Com_Write(COM_READ, REGISTER_KEY_CODE, 1, &by_Key);                 //Send Key Release
    }
    break;
  case KB_CD_VU_VD:
  case KB_VD_CU_CD:
  case KB_CD_CU:
  case KB_TIMEUP_TIMEDOWN:
  case KB_RESET_SOC2:
  case KB_NA:
    break;
  case KB_RESET_SOC1:
    Main.ResetPress = 1;
    break;
  case KB_TV_LAST:                                                            //如果是按过Reset组合键则复位，否则往下执行
    if(Main.ResetPress)
    {
      System_Reset();
    }
  default:
    by_KeyRelease = 1;
    Com_Write(COM_READ, REGISTER_KEY_CODE, 1, &by_Key);
    break;
  }
}

void Main_SafeKeyProcess(void)
{
  un16.Word = LCB_Get_Error(TRUE);
  if(un16.Word != 0x00 && un16.Word != 0x02B2 && un16.Word != Main.LastSendErrorCode)          //只发送一次
  {
    Main.LastSendErrorCode = un16.Word;
    Com_Write(COM_READ, REGISTER_ERROR, 2, &un16.Byte[0]);
  }
  
  if(Timer_Counter(TIMER_SAFEKEY_CHECK_DELAY, 50))                            //Start check safe key after booting 5 secs.
  {
    Main.CheckSafekey = 1;
  }
  
  if(Main.CheckSafekey && LCB_Get_DriverType() == LCB_TM)                     //When MachineType is TM, check safe key after booting 5 secs.
  {
    //IR Sense
    if(LCB_Get_Data(DG_IR_STATUS))                                          //IRSense 触发
    {
      if(!Main.SendIRRemove)
      {
        printf("Send IRSense is remove\n");
        Main.SendIRRemove = 1;
        Main.SendIRPlug = 0;
        
        un16.Word = 0x02C8;
        Main.LastSendErrorCode = un16.Word;
        Com_Write(COM_READ, REGISTER_ERROR, 2, &un16.Byte[0]);
        LCB_Send_Cmd(DS_MOT_RPM, 0);
        Main.IRStatuRelease = 0;
        Timer_Clear(TIMER_IR_CLEAR_DIGITAL);
      }
    }
    else                                                                    //IRSense 正常
    {
      if(!Main.IRStatuRelease && Timer_Counter(TIMER_IR_CLEAR_DIGITAL, 20))
      {
        Main.IRStatuRelease = 1;
      }
      
      if(Main.IRStatuRelease && !Main.SendIRPlug)
      {
        printf("Send IR is plug\n");
        printf("Initial LCB\n");
        Main.SendIRPlug = 1;
        Main.SendIRRemove = 0;
        LCB_Power_On(FALSE);
        
        un16.Word = 0x02C9;
        Main.LastSendErrorCode = un16.Word;
        Com_Write(COM_READ, REGISTER_ERROR, 2, &un16.Byte[0]);
      }
    }
    
    
    //Safekey
    if(LCB_Get_Error(FALSE) == 0x02B2)
    {
      printf("Set Digital State\n");
      Timer_Clear(TIMER_SAFEKEY_CLEAR_DIGITAL);
      Safekey_Set_LCB_Remove();
    }
    else
    {
      if(Timer_Counter(TIMER_SAFEKEY_CLEAR_DIGITAL, 20))                  //到了2s后就清除Digital Safekey remove状态
      {
        Safekey_Clear_LCB_Remove();
        printf("Clear Digital State\n");
      }
    }
    
    if(Safekey_IsRemove())							                        //安全开关被移除
    {
      if(!Main.SendSafekeyRemove)
      {
        Main.SendSafekeyRemove = 1;
        un16.Word = 0x02B1;
        Main.LastSendErrorCode = un16.Word;
        Com_Write(COM_READ, REGISTER_ERROR, 2, &un16.Byte[0]);
        LCB_Send_Cmd(DS_MOT_RPM, 0);
        printf("Send Safekey is remove\n");
      }
      
      Main.SendSafekeyPlug = 0;
      Timer_Clear(TIMER_SAFEKEY_CLEAR);
    }
    else
    {
      if(!Main.SendSafekeyPlug && Timer_Counter(TIMER_SAFEKEY_CLEAR, 20))
      {
        Main.SendSafekeyPlug = 1;
        Main.SendSafekeyRemove = 0;
        LCB_Power_On(FALSE);
        
        un16.Word = 0x02B2;
        Main.LastSendErrorCode = un16.Word;
        Com_Write(COM_READ, REGISTER_ERROR, 2, &un16.Byte[0]);
        printf("Send Safekey is plug\n");
        printf("Initial LCB\n");
      }
    }
  }
}



UINT16 Main_GetStatus(void)
{
  UINT16 w_Out = 0;
  UCHAR reTry = 3;
  UCHAR by_Length;
  UCHAR RxData[20];
    
  LCB_Send_Cmd2(0x70,0,0);
  
  Timer_Clear(TIMER_TEST);
  while(1)
  {
    if(LCB_Rx_Data(RxData, &by_Length) > 0)
    {
      if(RxData[4] & 0x80)
      {
        BITSET(w_Out, STATUS_BIT_LCB_CALIBRATION);
      }
      lcbSts = RxData[4];
      break;
    }
    else
    {
      if(Timer_Counter(TIMER_TEST, 10))
      {
        Timer_Clear(TIMER_TEST);
        if(reTry > 0)
        {
          --reTry;
          LCB_Send_Cmd2(0x70,0,0);
        }
        else
          break;
      }
      
    }
  }
  
  if(Heart_Get_Blink())
  {
    BITSET(w_Out, STATUS_BIT_HR_DETECT);
  }
  
  if(LCB_Get_Error(FALSE))
  {
    BITSET(w_Out, STATUS_BIT_ERROR_FLAG);
  }
  
  return w_Out;
}


void Main_ERP_Process(void)
{
  if(ErpStatus == ERP_ENTER && Timer_Counter(TIMER_SLEEP_DELAY, 5))           //收到命令后等待500ms,留够时间让回复命令发送到UI
  {
    printf("ERP Close Devices\n");
    Com_Erp();
    I2C_Erp();
    Beep_Erp();
    Fan_Erp();
    Led_Erp();
    TV_Erp();
    Heart_Erp();
    HW_Empty_IO_Erp();
    LCB_Erp();
    Safekey_Erp();
    Power_Erp();
    Power_Set_Power_On(0);
    printf("Erp Delay 500ms\n");
    Timer_Clear(TIMER_COMMON);
    while(!Timer_Counter(TIMER_COMMON, 5)) Watchdog_Reset();
    printf("Erp Enter\n");
    Keyboard_Erp();
    System_Erp();
    
    //已被唤醒;
    Main_Init();
  }
}

void Main_UpdateProcess(void)
{
  UCHAR by_Length = 0;
  switch(by_LCBUpdataStep)
  {
  case 1:
    if(LCB_Rx_Data(stDataTx.Data, &by_Length) > 0)
    {
      if(stDataTx.Data[2] == 0x75 && stDataTx.Data[4] == 'Y')
      {
        LCB_Disable();                                                      //关闭LCB模块
        LCB_Update_Initial();                                               //初始化LCB_Update模块并进入工作状态
        Timer_Clear(TIMER_UPDATE_MCB_RESENT);
        memset(aby_Buff, 0, sizeof(aby_Buff));
        while(1)
        {
          if(Timer_Counter(TIMER_UPDATE_MCB_RESENT, 40))
          {
            Timer_Clear(TIMER_UPDATE_MCB_RESENT);
            break;
          }
        }
        LCB_Update_Write(CmdLCBReadUpdateMode, 0, aby_Buff);
        by_LCBUpdataStep = 2;
        updateRetryCnt = 3;
      }
    }
    break;
  case 2:
    if(LCB_Update_Read(aby_Buff, &un16.Word) == CmdLCBReturnUpdateMode)
    {
      Timer_Clear(TIMER_UPDATE_MCB_RESENT);
      LCB_Update_Write(CmdLCBFlashUnlock, 20, Main.by_LCBUpdataPassword);
      by_LCBUpdataStep = 3;
      updateRetryCnt = 3;
    }
    else
    {
      if(Timer_Counter(TIMER_UPDATE_MCB_RESENT, 15))                      //超时未回复重发
      {
        Timer_Clear(TIMER_UPDATE_MCB_RESENT);
        if(updateRetryCnt > 0)
        {
          --updateRetryCnt;
          memset(aby_Buff, 0, sizeof(aby_Buff));
          LCB_Update_Write(CmdLCBReadUpdateMode, 0, aby_Buff);
        }
        else
        {
          UCHAR data = 0x00;
          Com_Write(COM_WRITE, REGISTER_GETLCBFWUPDATE_START, 1, &data);
        }
      }
    }
    break;
  case 3:
    if(LCB_Update_Read(aby_Buff, &un16.Word) == CmdLCBReturnFlashState)
    {
      UCHAR data = 0x01;
      by_LCBUpdataStep = 4;
      Com_Write(COM_WRITE, REGISTER_GETLCBFWUPDATE_START, 1, &data);
    }
    else
    {
      if(Timer_Counter(TIMER_UPDATE_MCB_RESENT, 15))                      //超时未回复重发
      {
        Timer_Clear(TIMER_UPDATE_MCB_RESENT);
        if(updateRetryCnt > 0)
        {
          --updateRetryCnt; 
          LCB_Update_Write(CmdLCBFlashUnlock, 20, Main.by_LCBUpdataPassword);
        }
        else
        {
          UCHAR data = 0x00;
          Com_Write(COM_WRITE, REGISTER_GETLCBFWUPDATE_START, 1, &data);
        }
      }
    }
    break;
  default:
    break;
  }
  
  
  switch(Main.by_LCBReadStep)
  {
  case 1:
    Timer_Clear(TIMER_READ_MCB_RESENT);
    memset(aby_Buff, 0, sizeof(aby_Buff));
    LCB_Update_Write(CmdLCBReadProduceID, 0, aby_Buff);
    Main.by_LCBReadStep = 2;
    updateRetryCnt = 3;
    break;
  case 2:
    if(LCB_Update_Read(aby_Buff, &un16.Word) == CmdLCBReturnProduceID)
    {
      Timer_Clear(TIMER_READ_MCB_RESENT);
      memcpy(Main.by_LCBReadBuff, aby_Buff, 20);
      memset(aby_Buff, 0, sizeof(aby_Buff));
      LCB_Update_Write(CmdLCBReadMcuType, 0, aby_Buff);
      Main.by_LCBReadStep = 3;
      updateRetryCnt = 3;
    }
    else
    {
      if(Timer_Counter(TIMER_READ_MCB_RESENT, 10))                        //超时未回复重发
      {
        Timer_Clear(TIMER_READ_MCB_RESENT);
        if(updateRetryCnt > 0)
        {
          --updateRetryCnt; 
          memset(aby_Buff, 0, sizeof(aby_Buff));
          LCB_Update_Write(CmdLCBReadProduceID, 0, aby_Buff);
        }
        else
        {
          UCHAR data = 0x00;
          Com_Write(COM_WRITE, REGISTER_GETLCB_INFORMATION_DATAREAD, 1, &data);
        }
      }
    }
    break;
  case 3:
    if(LCB_Update_Read(aby_Buff, &un16.Word) == CmdLCBReturnMcuType)
    {
      Timer_Clear(TIMER_READ_MCB_RESENT);
      memcpy(&Main.by_LCBReadBuff[20], aby_Buff, 20);
      memset(aby_Buff, 0, sizeof(aby_Buff));
      LCB_Update_Write(CmdLCBReadMcuID, 0, aby_Buff);
      Main.by_LCBReadStep = 4;
      updateRetryCnt = 3;
    }
    else
    {
      if(Timer_Counter(TIMER_READ_MCB_RESENT, 10))                        //超时未回复重发
      {
        Timer_Clear(TIMER_READ_MCB_RESENT);
        if(updateRetryCnt > 0)
        {
          --updateRetryCnt; 
          memset(aby_Buff, 0, sizeof(aby_Buff));
          LCB_Update_Write(CmdLCBReadMcuType, 0, aby_Buff);
        }
        else
        {
          UCHAR data = 0x00;
          Com_Write(COM_WRITE, REGISTER_GETLCB_INFORMATION_DATAREAD, 1, &data);
        }
      }
    }
    break;
  case 4:
    if(LCB_Update_Read(aby_Buff, &un16.Word) == CmdLCBReturnMcuID)
    {
      Timer_Clear(TIMER_READ_MCB_RESENT);
      memcpy(&Main.by_LCBReadBuff[40], aby_Buff, 20);
      Main.by_LCBReadStep = 5;
      memcpy(stDataTx.Data, Main.by_LCBReadBuff, 60);
      stDataTx.Length = 60;
      Com_Write(COM_WRITE, REGISTER_GETLCB_INFORMATION_DATAREAD, stDataTx.Length, stDataTx.Data);
    }
    else
    {
      if(Timer_Counter(TIMER_READ_MCB_RESENT, 10))                        //超时未回复重发
      {
        Timer_Clear(TIMER_READ_MCB_RESENT);
        if(updateRetryCnt > 0)
        {
          --updateRetryCnt; 
          memset(aby_Buff, 0, sizeof(aby_Buff));
          LCB_Update_Write(CmdLCBReadMcuID, 0, aby_Buff);
        }
        else
        {
          UCHAR data = 0x00;
          Com_Write(COM_WRITE, REGISTER_GETLCB_INFORMATION_DATAREAD, 1, &data);
        }
      }
    }
    break;
  default:
    break;
  }
  
  
  if(Main.UpdateIOFlag && Timer_Counter(TIMER_ENTER_UPDATE_DELAY, 10))
  {
    Main.UpdateIOFlag = 0;
    Power_Set_Power_On(0);
    Main_EnterBSLMode();
  }
}

void Main_ComProcess(void)
{
  un16.Word = 0;
  UINT16 w_Index = 0;
  UCHAR noReply = 0;
  
  if(!Main.UpdateIOFlag && Com_GetUpdateIOState() == 2)                       //Enter Update流程
  {
    Timer_Clear(TIMER_ENTER_UPDATE_DELAY);
    Main.UpdateIOFlag = 1;
  }
  
  if(Com_Read(&stDataRx))
  {
    memset(&stDataTx, 0, sizeof(stDataTx));
    
    if(stDataRx.WR == COM_WRITE)
    {
      un16.Byte[0] = stDataRx.Data[0];
      un16.Byte[1] = stDataRx.Data[1];
      
      switch(stDataRx.Command)
      {
      case REGISTER_WRITE_RAW_LCB:
        LCB_Tx_Data(stDataRx.Data, stDataRx.Length);
        break;
      case REGISTER_SET_USB_CHARGE:
        if(un16.Byte[0])
        {
          Power_USB1_On();
        }
        else
        {
          Power_USB1_Off();
        }
        break;
      case REGISTER_ENTER_UPDATE_MODE:
        //                Timer_Clear(TIMER_ENTER_UPDATE_DELAY);
        //                Main.UpdateIOFlag = 1;
        break;
      case REGISTER_CSAFE_RADIO:
        break;
      case REGISTER_DCIVERSION:
      case REGISTER_MCB_VERSION:
        break;
      case REGISTER_LCB_CALIBRATE:
        LCB_Send_Cmd(DS_CALIBRATE, 0);
        break;
      case REGISTER_DCIIOBOARDINCLINERANGE:
        break;
      case REGISTER_TVTUNNERTYPECMD:
        break;
      case REGISTER_BTPOWERON:
        break;
      case REGISTER_BTCONTROL:
        break;
      case REGISTER_RFIDWRITE:
        break;
      case REGISTER_PLAYKEYTUNE:
        Beep_Proce(1,3,1);
        break;
      case REGISTER_CONTROL_TYPE:
        Main.by_IOType = stDataRx.Data[0];
        LCB_Send_Cmd(DS_MACHINE_TYPE, stDataRx.Data[0]);
        break;
      case REGISTER_INCLNE_ACTION:
        LCB_Send_Cmd(DS_INC_ACTION, stDataRx.Data[0]);
        break;
      case REGISTER_KEYPAD_SOUNDS_ENABLE:
        Keyboard_EnableBeep(stDataRx.Data[0]);
        break;
      case REGISTER_BUZZER_CNTRL:
        switch(stDataRx.Data[0])
        {
        case 0:
          Beep_Enable(FALSE);
          break;
        case 1:
          Beep_Enable(TRUE);
          break;
        case 2:
          Beep_Proce(1,6,1);
          break;
        default:
          break;
        }
        break;
      case REGISTER_BUZZER_SQNCE:
        switch(stDataRx.Data[0])
        {
        case 0:
          Beep_Proce(1,3,1);
          break;
        case 1:
          Beep_Proce(1,6,1);
          break;
        case 2:
          Beep_Proce(1,9,1);
          break;
        case 3:
          Beep_Proce(2,4,2);
          break;
        default:
          break;
        }
        break;
      case REGISTER_FAN_CONTROL:
        Fan_SetControl((FAN_CTRL)(stDataRx.Data[0]));
        break;
      case REGISTER_SET_VA_POWER:
        if(stDataRx.Data[0] == 0x00)
        {
          Power_TV_Off();
        }
        else if(stDataRx.Data[0] == 0xff)
        {
          Power_TV_On();
          TV_Initial(1);
        }
        break;
      case REGISTER_TV_TUNER_POWER:
        if(stDataRx.Data[0] == 0)
        {
          Power_TV_Off();
        }
        else 
        {
          Power_TV_On();
        }
        
        TV_SendCommand(TV_CMD_STATUS);
        TV_SendCommand(TV_CMD_ADI);
        break;
      case REGISTER_TV_TUNER_CONTROL:
        TV_SendCommand((TV_COM_TYPE)stDataRx.Data[0]);
        break;
      case REGISTER_GENERATOR_POLEPAIR:
        LCB_Send_Cmd(DS_GEN_POLE_PAIR, un16.Word);
        break;
      case REGISTER_SET_CLIMBMILL_COMMAND:
        LCB_Send_Cmd(DS_CLIMB_MILL_STATUS, un16.Word);
        break;
      case REGISTER_SET_ESTOP_PARAMETER:
        LCB_Send_Cmd(DS_ESTOP_PARA, un16.Word);
        break;
      case REGISTER_EUP:
      case REGISTER_SET_EUPS_MODE:
        if(stDataRx.Data[0] == 0xff)
        {
          ErpStatus = ERP_ENTER;
        }
        else if(stDataRx.Data[0] == 0x00)
        {
          ErpStatus = ERP_LEAVE;
        }
        
        Timer_Clear(TIMER_SLEEP_DELAY);
        break;
      case REGISTER_SET_IPOD_POWER:
        break;
      case REGISTER_SET_LCB_POWER_OFFTIME:
        LCB_Send_Cmd(DS_POWER_OFF, un16.Word);
        break;
      case REGISTER_CLOSE_BATTERY:
        break;
      case REGISTER_SET_WATT:
        LCB_Send_Cmd(DS_WATTS, un16.Word);
        break;
      case REGISTER_SET_ELECT_ROMAGNETIC_CURRENT:
        LCB_Send_Cmd(DS_ELECT_CURRENT, un16.Word);
        break;
      case REGISTER_RPM_LOW_LIMIT_CHARGE:
        LCB_Send_Cmd(DS_LMT_RPM_FOR_CHANGE, un16.Word);
        break;
      case REGISTER_RPM_LOW_LIMIT_RESISTANCE:
        LCB_Send_Cmd(DS_LMT_RPM_FOR_RES, un16.Word);
        break;
      case REGISTER_RESISTANCE_TYPE:
        LCB_Send_Cmd(DS_RES_TYPE, un16.Word);
        break;
      case REGISTER_RPM_GEARRATIO:
        LCB_Send_Cmd(DS_RPM_GEAR_RATIO, un16.Word);
        break;
      case REGISTER_TUNEEND_POINT_INCLINE1:
        LCB_Send_Cmd(DS_INC_TUNE1, un16.Word);
        break;
      case REGISTER_TUNEEND_POINT_INCLINE2:
        LCB_Send_Cmd(DS_INC_TUNE2, un16.Word);
        break;
      case REGISTER_INCLINE_PERCENT:
        LCB_Send_Cmd(DS_INC_PERCENT, un16.Word);
        break;
      case REGISTER_SET_INCLINE_STROKE:
        LCB_Send_Cmd(DS_INC_STROKE, un16.Word);
        break;
      case REGISTER_CURRENT_RESISTANCE:
        LCB_Send_Cmd(DS_ELECT_CURRENT, un16.Word);
        break;
      case REGISTER_PWM_RESISTANCE:
        LCB_Send_Cmd(DS_IDCT_PWM_PERCENT, un16.Word);
        break;
      case REGISTER_RPM_STEP:
        LCB_Set_SpeedRef(stDataRx.Data[0], stDataRx.Data[1]);
        break;
      case REGISTER_RPM_TARGET:
        LCB_Send_Cmd(DS_MOT_RPM, un16.Word);
        break;
      case REGISTER_ADC_TARGET1:
        LCB_Send_Cmd(DS_INC_LOCATION, un16.Word);
        break;
      case REGISTER_ADC_TARGET2:
        break;
      case REGISTER_ECB_ACTION:
        if(stDataRx.Data[0] <= 2)
        {
          if(stDataRx.Data[0] == 2)
            stDataRx.Data[0] = 0x80;
          LCB_Send_Cmd(DS_ECB_ACTION, stDataRx.Data[0]);
        }
        break;
      case REGISTER_ECB_COUNTER_TRAGET:
        LCB_Send_Cmd(DS_ECB_LOCATION, un16.Word);
        break;
      case REGISTER_SET_CONSOLE_POWER:                                    // 110209 set console power
        LCB_Send_Cmd(DS_CONSOLE_PWR, un16.Word);
        break;
      case REGISTER_LCB_DEVICE_DATA:
        LCB_Send_Cmd(DG_LCB_DATA_INFOR, 0);
        LCB_Send_Cmd(DG_LCB_DATA_CONTENT, stDataRx.Data[0]);
        break;
      case REGISTER_SETLCBFWUPDATE_START:
        {
          un16.Word = 0;
          
          switch(stDataRx.Data[0])
          {
          case 1:
            un16.Byte[0] = 'S';
            un16.Byte[1] = 'J';
            break;
          case 2:
            un16.Byte[0] = 'T';
            un16.Byte[1] = 'J';
            break;
          case 3:
            un16.Byte[0] = 'A';
            un16.Byte[1] = 'J';
            break;
          case 4:
            un16.Byte[0] = 'B';
            un16.Byte[1] = 'J';
            break;
          default:
            break;
          }
          
          memcpy(Main.by_LCBUpdataPassword, &stDataRx.Data[1], 20);
          
          stDataRx.Data[0] = 0x00;
          stDataRx.Data[1] = 0x00;
          stDataRx.Data[2] = 0x75;
          stDataRx.Data[3] = 0x02;
          stDataRx.Data[4] = 'S';
          stDataRx.Data[5] = 'J';
          stDataRx.Data[6] = LCB_Hal_CRC8_Check(stDataRx.Data, 6);
          
          stDataRx.Length = 7;
          LCB_Tx_Data(stDataRx.Data, stDataRx.Length);
          
          by_LCBUpdataStep = 1;
          updateRetryCnt = 3;
          noReply = 1;
        }
        break;
      case REGISTER_SETLCBFWUPDATE_DATAERASE:
        if(by_LCBUpdataStep == 4)
        {
          LCB_Update_Write(CmdLCBEraseFlash, stDataRx.Length, stDataRx.Data);
          Timer_Clear(TIMER_UPDATE_MCB_RESENT);
          stDataTx.Length = 1;
          stDataTx.Data[0] = 0;
          while(1)
          {
            if(LCB_Update_Read(aby_Buff, &un16.Word) == CmdLCBReturnEraseFlash)
            {
              stDataTx.Data[0] = 0x01;
              break;
            }
            if(Timer_Counter(TIMER_UPDATE_MCB_RESENT, 30))
              break;
          }
          stDataRx.Command += 1;
        }
        break;
      case REGISTER_SETLCBFWUPDATE_DATAWRITE:
        if(by_LCBUpdataStep == 4)
        {
          memcpy(&stDataTx.Data[0], &stDataRx.Data[0], 4);            //重新组织数据，把长度清除
          memcpy(&stDataTx.Data[4], &stDataRx.Data[8], stDataRx.Length - 8);
          memcpy(&Main.un32_LCBUpdateLength.Byte[0], &stDataRx.Data[4], 4);                   //保存长度
          Main.by_LCBUpdateCRC = LCB_Hal_CRC8_Check(&stDataTx.Data[4], stDataRx.Length - 8);  //保存CRC校验值
          LCB_Update_Write(CmdLCBWriteFlash, stDataRx.Length - 4, stDataTx.Data);
          Timer_Clear(TIMER_UPDATE_MCB_RESENT);
          stDataTx.Length = 1;
          stDataTx.Data[0] = 0;
          while(1)
          {
            if(LCB_Update_Read(aby_Buff, &un16.Word) == CmdLCBReturnWriteFlash)
            {
              stDataTx.Data[0] = 0x01;
              break;
            }
            if(Timer_Counter(TIMER_UPDATE_MCB_RESENT, 30))
              break;
          }
          stDataRx.Command += 1;
        }
        break;
      case REGISTER_SETLCBFWUPDATE_DATAREAD:
        if(by_LCBUpdataStep == 4)
        {
          LCB_Update_Write(CmdLCBReadFlash, stDataRx.Length, stDataRx.Data);
          
          Timer_Clear(TIMER_UPDATE_MCB_RESENT);
          while(1)
          {
            if(LCB_Update_Read(stDataTx.Data, &stDataTx.Length) == CmdLCBReturnReadFlash)
              break;
            if(Timer_Counter(TIMER_UPDATE_MCB_RESENT, 30))
              break;
          }
          stDataRx.Command += 1;
        }
        break;
      case REGISTER_SETLCBFWUPDATE_DATACHECKCODE:
        if(by_LCBUpdataStep == 4)
        {
          LCB_Update_Write(CmdLCBWriteCheckCode, stDataRx.Length, stDataRx.Data);
          Timer_Clear(TIMER_UPDATE_MCB_RESENT);
          while(1)
          {
            if(LCB_Update_Read(stDataTx.Data, &stDataTx.Length) == CmdLCBReturnCheckCode)
              break;
            if(Timer_Counter(TIMER_UPDATE_MCB_RESENT, 30))
              break;
          }
          stDataRx.Command += 1;
        }
        break;
      case REGISTER_SETLCBFWUPDATE_DATAPROGRAMSTATE:
        if(by_LCBUpdataStep == 4)
        {
          LCB_Update_Write(CmdLCBReadProgramState, 0, stDataRx.Data); 
          Timer_Clear(TIMER_UPDATE_MCB_RESENT);
          while(1)
          {
            if(LCB_Update_Read(stDataTx.Data, &stDataTx.Length) == CmdLCBReturnProgramState)
            {
              LCB_Initial();
              LCB_Power_On(FALSE);
              break;
            }
            if(Timer_Counter(TIMER_UPDATE_MCB_RESENT, 30))
              break;
          }
          stDataRx.Command += 1;
        }
        break;
      case REGISTER_SETLCB_INFORMATION_DATAREAD:
        Main.by_LCBReadStep = 1;                                        //进入读取下控Information流程
        noReply = 1;
        break;
      case REGISTER_SET_SPIIDLE_TIMER:
        if(stDataRx.Length == 2)
        {
          if(un16.Word > 9000) un16.Word = 9000;
          Main.DelayReset = un16.Word;
          stDataTx.Data[0] = un16.Byte[0];
          stDataTx.Data[1] = un16.Byte[1];
          stDataTx.Length = stDataRx.Length;
          Timer_Clear(TIMER_RESET_SOC);
          Power_Set_Power_On(0);
          Main.ResetPress = 1;
          if(Main.DelayReset == 0)
            System_Reset();
        }
        break;
      default:
        break;
      }
      
      if(noReply == 0)
        Com_Write(COM_WRITE, stDataRx.Command, stDataTx.Length, stDataTx.Data);
    }
    else if(stDataRx.WR == COM_READ)
    {
      switch(stDataRx.Command)
      {
      case REGISTER_READ_RAW_LCB:
        
        break;
      case REGISTER_GETMCBINFORMATION:
        LCB_Get_DataPointer(DG_MAIN_MOTOR_INF, aby_Buff, &w_Size);
        
        stDataTx.Length = w_Size;
        stDataTx.Data[w_Index++] = aby_Buff[1];
        stDataTx.Data[w_Index++] = aby_Buff[0];
        stDataTx.Data[w_Index++] = aby_Buff[3];
        stDataTx.Data[w_Index++] = aby_Buff[2];
        stDataTx.Data[w_Index++] = aby_Buff[5];
        stDataTx.Data[w_Index++] = aby_Buff[4];
        stDataTx.Data[w_Index++] = aby_Buff[7];
        stDataTx.Data[w_Index++] = aby_Buff[6];
        stDataTx.Data[w_Index++] = aby_Buff[9];
        stDataTx.Data[w_Index++] = aby_Buff[8];
      case REGISTER_USER_RPM:
        stDataTx.Length = 2;
        un16.Word = LCB_Get_Data(DG_EPBI_RPM);                          // EPBIKE and TM use sharable
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_DEVICE_TYPE:
        stDataTx.Length = 1;
        stDataTx.Data[w_Index++] = DEVICE_TYPE_LMM;
        break;
      case REGISTER_STATUS:
        stDataTx.Length += 2;
        un16.Word = Main_GetStatus();
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_KEY_CODE:
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = Keyboard_Get_Code() & 0x7F;          //去除repeat信号
        break;
      case REGISTER_ERROR:
        un16.Word = LCB_Get_Error(TRUE);
        stDataTx.Length += 2;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_DATE:
        break;
      case REGISTER_MESSAGE_UPDATE:
        stDataTx.Length += 14;
        un16.Word = Main_GetStatus();
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        
        un16.Word = LCB_Get_Data(G_STATUS1);
        stDataTx.Data[w_Index++] = un16.Byte[0];
        
        un16.Word = Heart_Get_Heart();
        stDataTx.Data[w_Index++] = un16.Byte[0];
        
        un16.Word = Heart_Get_Blink();
        stDataTx.Data[w_Index++] = un16.Byte[0];
        
        un16.Word = LCB_Get_Data(DG_MOT_RPM);                           //EPBIKE and TM use sharable
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        
        un16.Word = LCB_Get_Data(DG_INC_LOCATION);
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        
        un16.Word = LCB_Get_Data(DG_GET_TM_IN_USED);
        stDataTx.Data[w_Index++] = un16.Byte[0];
        
        un16.Word = LCB_Get_Error(TRUE);
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        
        un16.Word = LCB_Get_Data(DG_BATTERY_STATUS);
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_GET_MACHINE_INFO:
        stDataTx.Length += 8;
        un16.Word = Main_GetStatus();
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        
        un16.Word = lcbSts;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        
        LCB_GetMCBInfor();
        un16.Word = LCB_Get_MCBType();
        stDataTx.Data[w_Index++] = un16.Byte[0];
        
        un16.Word = LCB_Get_Version();
        stDataTx.Data[w_Index++] = un16.Byte[0];
        
        un16.Word = VER_MAJOR;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        
        un16.Word = LCB_Get_Error(TRUE);
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
        
      case REGISTER_DCIENVIRONMENT_RETRIEVE:
        stDataTx.Length += 84;
        LCB_Get_DataPointer(DG_DCI_ENVIRONMENT, stDataTx.Data, &stDataTx.Length);
        break;
      case REGISTER_VERSION_DB_NEW:
        stDataTx.Data[w_Index++] = VER_MINOR;
        stDataTx.Data[w_Index++] = VER_MAJOR;
        stDataTx.Length = 2;
        break;
      case REGISTER_DCIVERSION:
        //by_Sum = LCB_Get_Version() / 1000;
      case REGISTER_MCB_VERSION: 
        un16.Word = LCB_Get_MCBType();
        stDataTx.Length += 4;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        
        un16.Word = LCB_Get_Version();
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_SOFT_VERSION:
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = VER_MAJOR;
        break;
      case REGISTER_CONTROL_TYPE:
        stDataTx.Length = 2;
        un16.Word = Main.by_IOType;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_RPM_TARGET:
        stDataTx.Length += 2;
        un16.Word = LCB_Get_Data(DG_MOT_RPM);
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_DB_PERSONINPLACE:
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = LCB_Get_Data(DG_GET_TM_IN_USED);
        break;
      case REGISTER_QUICK_SEQUENCE:
      case REGISTER_RPM_CURRENT:
      case REGISTER_RPM_CUREENT_ROLLER:
        un16.Word = LCB_Get_Data(DG_MOT_RPM);
        stDataTx.Length += 2;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_ADC_CURRENT1:
        un16.Word = LCB_Get_Data(DG_INC_LOCATION);
        stDataTx.Length += 2;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_HR_HAND:
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = Heart_Get_Heart();
        break;
      case REGISTER_ADC_TARGET1:
        stDataTx.Length += 2;
        un16.Word = LCB_Get_Data(DG_INC_LOCATION);
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_ADC_TARGET2:
      case REGISTER_ADC_CURRENT2:
        un16.Word = 0;
        stDataTx.Length += 2;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_INCLNE_ACTION:
        un16.Byte[0] = LCB_Get_Data(DG_INC_LOCATION);
        un16.Byte[1] = 0;
        if(un16.Byte[0] & C_LCB_INC_DOWN)
          un16.Byte[1] |= 0x02;
        if(un16.Byte[0] & C_LCB_INC_UP)
          un16.Byte[1] |= 0x01;
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_ECB_ACTION:
        un16.Byte[0] = LCB_Get_Data(DG_ECB_STATUS);
        un16.Byte[1] = 0;
        if(un16.Byte[0] & ECB_STATUS_DOWN)
          un16.Byte[1] |= 0x02;
        if(un16.Byte[0] & ECB_STATUS_UP)
          un16.Byte[1] |= 0x01;
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_ECB_COUNTER_TRAGET:
        stDataTx.Length += 2;
        un16.Word = LCB_Get_Data(DG_ECB_COUNT);
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_ECB_COUNTER_CURRENT:
        un16.Word = LCB_Get_Data(DG_ECB_COUNT);
        stDataTx.Length += 2;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_GET_CLIMBMILL_STATUS:
        un16.Word = LCB_Get_Data(DG_CLIMB_MILL_STATUS);
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        break;
      case REGISTER_LCB_BT_VOLTAGE:
        un16.Word = LCB_Get_Data(DG_BATTERY_STATUS);
        stDataTx.Length += 2;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_LCB_GTOR_VOLTAGE:
      case REGISTER_LCB_DC_CURRENT:
      case REGISTER_LCB_DC_VOLTAGE:
        un16.Word = 0x00;
        stDataTx.Length += 2;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_FAN_CONTROL:
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = Fan_GetControl();
        break;
      case REGISTER_LCB_DEVICE_DATA:
        LCB_Get_DataPointer(DG_LCB_DATA_INFOR, aby_Buff, &w_Size);
        memcpy(stDataTx.Data, aby_Buff, w_Size);
        stDataTx.Length += w_Size;
        
        LCB_Get_DataPointer(DG_LCB_DATA_CONTENT, aby_Buff, &w_Size);
        memcpy(&stDataTx.Data[w_Size], aby_Buff, w_Size);
        stDataTx.Length += w_Size;
        break;
      case REGISTER_CSAFE_RADIO:
        break;
      case REGISTER_CSAFE_PACKET_LENGTH:
        break;
      case REGISTER_TV_TUNER_STATUS:
        stDataTx.Length += 1;
        break;
      case REGISTER_TV_TUNER_POWER:
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6);
        break;
      case REGISTER_SET_IPOD_POWER:
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_7);
        break;
      case REGISTER_GET_IOBOARD_STATUS:
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = LCB_Get_Data(G_STATUS1);
        break;
      case REGISTER_INCLINE_PERCENT:
        un16.Word = LCB_Get_Data(DG_INC_PERCENT);
        stDataTx.Length += 2;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        break;
      case REGISTER_GET_TVTUNER_TYPE:
        stDataTx.Length += 1;
        stDataTx.Data[w_Index++] = TV_TVTunerType();
        break;
      case REGISTER_TVTUNNERTYPECMD:
        break;
      case REGISTER_ERRORCODERESEND:
        un16.Word = Main.LastSendErrorCode;
        stDataTx.Data[w_Index++] = un16.Byte[0];
        stDataTx.Data[w_Index++] = un16.Byte[1];
        stDataTx.Length += 2;
        break;
      case REGISTER_RFIDREAD:
        break;
      default:
        break;
      }
      
      Com_Write(COM_READ, stDataRx.Command, stDataTx.Length, stDataTx.Data);
    }
  }
  
  
  UCHAR by_Length = 0;
  if(stDataRx.Command == REGISTER_WRITE_RAW_LCB &&  LCB_Rx_Data(stDataTx.Data, &by_Length))                                  //upload data
  {
    stDataTx.Length = by_Length;
    Com_Write(COM_READ, REGISTER_READ_RAW_LCB, stDataTx.Length, stDataTx.Data);
  }
}

void Main_EnterBSLMode(void)
{
  DINT();                                                                     //Disable interruputs for avoiding interrupts destory image download.
  
  w_UpdateMark = 0x5AA5F00F;
  UINT32 JumpAddress = *(UINT32 *)(BootLoaderAddress + 4);
  FNCT_VOID Jump_To_BootLoader = (FNCT_VOID) JumpAddress;
  
  __set_MSP(*(UINT32 *) BootLoaderAddress);                                   //Initialize user application's Stack Pointer
  Jump_To_BootLoader();
}

void Main_Init(void)
{
  DINT();
  if(__checksum == 0) __checksum = 1;
  if(w_UpdateMark == 1) w_UpdateMark = 0;
  
  System_Initial(Power_Get_Power_On() != C_PASSWORD);
  Information_Init();
  Power_Initial();
  TV_Initial(Power_Get_Power_On() != C_PASSWORD);
  Safekey_Initial();
  LCB_Initial();
  I2C_Initail();
  Com_Initial();
  Beep_Initial();
  Fan_Initial();
  Timer_Initial();
  Keyboard_Initial();
  Heart_Initial();
  Led_Initial();
  HW_Empty_IO_Initial();
  HW_Test_IO_Initial();
  
  Main_Reset_Log(0);
  Watchdog_Initial();
  Watchdog_Reset();
  
  EINT();
  
  memset(&Main, 0, sizeof(Main));
  memset(&stDataTx, 0, sizeof(stDataTx));
  memset(&stDataRx, 0, sizeof(stDataRx));
  memset(aby_Buff, 0, sizeof(aby_Buff));
  
  by_LCBUpdataStep = 0;
  updateRetryCnt = 0;
  un16.Word = 0;
  w_Size = 0;
  Main.SendSafekeyPlug = 1;
  ErpStatus = ERP_NONE;
  
  if(Power_Get_Power_On() != C_PASSWORD)
  {
    Power_Set_Power_On(C_PASSWORD);
    Keyboard_ResetMask();
    Beep_Proce(1,3,1);
    
    while(!Timer_Counter(TIMER_COMMON, 5)) 
    {
      Watchdog_Reset();                                                   //电源起来后再等0.5秒,防止掉电时重复复位
    }
    
    while(!Beep_Get_OK());
    LCB_Power_On(TRUE);
  }
}

void Main_Reset_Log(UINT16 wNum)
{
  if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)                             //Check if the Power On Reset flag is set
  {
    printf("Power On Reset occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)                             //Check if the Pin Reset flag is set
  {
    printf("External Reset occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
  {
    printf("RCC_FLAG_SFTRST occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    printf("RCC_FLAG_IWDGRST occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
  {
    printf("RCC_FLAG_WWDGRST occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET)
  {
    printf("RCC_FLAG_LPWRRST occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_LSERDY) != RESET)
  {
    printf("RCC_FLAG_LSERDY occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)
  {
    printf("RCC_FLAG_HSERDY occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET)
  {
    printf("RCC_FLAG_PLLRDY occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)
  {
    printf("RCC_FLAG_HSIRDY occurred\r\n");
  }
  if(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != RESET)
  {
    printf("RCC_FLAG_LSIRDY occurred\r\n");
  }
  
  RCC_ClearFlag();                                                            //Clear reset flags
}



