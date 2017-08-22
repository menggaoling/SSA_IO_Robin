#include "LCB.h"
#include "WatchDog.h"
#include "FIFO.h"


#define LCB_ERP_LEAVE           0x00
#define LCB_ERP_ENTER           0xFF
#define LCB_ERP_ACK_OK          0x01
#define LCB_ERP_ACK_FAIL        0x00
#define LCB_ERP_ACK_WAIT        0xA1

#define LCB_SPEED_TIMER         600
#define LCB_SPEED_KEEP_TIMER    1000
#define LCB_RPM_MAX             5000
#define LCB_RPM_MIN             85
#define LCB_RPM_ADD_STEP        100         //4200/100 = 42*600 = 25.2s
#define LCB_RPM_DEC_STEP        130         //4200/130 = 32*600 = 19.3s
#define DEFAULT_TOPTEK_MAX_INC  205
#define DEFAULT_TOPTEK_MIN_INC  20

#define LCB_ADDRS               0xFF
#define IR_ADDRS                0x26
#define NO_PAR                  0
#define C_TX_BUFF_SIZE          20          //命令缓存个数
#define C_DATA_LEN              512         //接收数据保存buff大小

#define C_KEEP_ONLINE           500         //ms
#define C_KEEP_ONLINE_IDLE      250         //ms
#define C_LOST_TIME             5000        //ms


typedef enum{ErrGet = 0, ErrHdle, ErrSkip, ErrWaitSkipACK, ErrIDLE}  ERR_STEP;


typedef struct
{
    UCHAR  AddStep;
    UCHAR  DecStep;
} LCB_SPEED_REF;


typedef struct
{
    UCHAR       Cmd;          //对应的Cmd
    UCHAR       RxOffset;     //
    UCHAR       RxLen;
} RX_DATA_OFFSET;


typedef struct
{
    UCHAR Cmd;
    UCHAR TxLen;
    UCHAR RxLen;
} CMD_PROPERTY;


typedef struct
{
    COM_TYPE    CmdType;
    UINT32      Data;
} CMD_BUFF;


typedef struct
{
    COM_TYPE    CmdType;   //对应的Cmd种类
    UCHAR       RxLen;
} RX_DATA_PROPERTY;


static const CMD_PROPERTY Cmd_Talbe[CMD_LEN] =
{
    //cmd,  Tx, Rx,  Index
    //Common Command
    {0x70,  0,  0 },//cmd initial                   DS_INITIAL,
    {0x71,  0,  0 },//get status                    DG_STATUS,
    {0x72,  0,  2 },//get err code                  DG_ERRCODE,
    {0x73,  0,  2 },//get version                   DG_VERSION,
    {0x74,  0,  0 },//calibrate                     DS_CALIBRATE,
    {0x75,  2,  1 },//update program                DS_UPDATE,
    {0x76,  0,  0 },//skio err                      DS_ErrSkip,
    {0x77,  1,  1 },//ext cmd                       DS_EXTCMD,
    {0x78,  0,  27},//DCI Version                   DG_DCI_VER,
    {0x79,  0,  84},//DG_DCI_ENVIRONMENT            DG_DCI_ENVIRONMENT,
    {0x7B,  2,  0 },//Console Power                 DS_CONSOLE_PWR,
    
    //TM Command
    {0xf0,  2,  2 },//set Rpm max                   DS_RPM_RTN,
    {0xf1,  2,  0 },//set Motor Rpm                 DS_MOT_RPM,
    {0xf2,  2,  0 },//set add step                  DS_ADD_STEP,
    {0xf3,  2,  0 },//set dec step                  DS_DEC_STEP,
    {0xf4,  2,  0 },//set stop step                 DS_STOP_STEP,
    {0xf5,  1,  0 },//set inc action                DS_INC_ACTION,
    {0xf6,  2,  0 },//set inc location              DS_INC_LOCATION,
    {0xf7,  1,  0 },//set work status               DS_WORK_STATUS,
    {0xf8,  0,  2 },//get roller rpm                DG_ROLLER_RPM,
    {0xf9,  0,  2 },//get motor rpm                 DG_MOT_RPM,
    {0xfa,  0,  2 },//get inc location              DG_INC_LOCATION,
    {0xfb,  2,  0 },//set inc tune1                 DS_INC_TUNE1,
    {0xfc,  2,  0 },//set inc tune2                 DS_INC_TUNE2,
    {0xfd,  2,  0 },//set inc stroke                DS_INC_STROKE,
    {0x90,  2,  0 },//set motor hp                  DS_MOTOR_HP,
    {0x91,  0,  1 },//get Driver type               DG_DRVE_TYPE,
    {0x94,  1,  0 },//force inc operation           DS_FORCE_INC_OPERATION,
    {0x96,  0,  10},//get main motor infor          DG_MAIN_MOTOR_INF,
    {0xA0,  0,  4 },//get inc range                 DG_INC_RANGE,
    {0xA1,  4,  0 },//set inc range                 DS_INC_RANGE,
    {0xA6,  0,  4 },//get rpm speed range           DG_RPM_SPEED_RANGE,
    {0xA7,  8,  0 },//set rpm speed range           DS_RPM_SPEED_RANGE,
    {0xAB,  0,  0 },//set manualcalibration         DS_MANUAL_CALIBRATION,
    
    //EP/BI Command
    {0x61,  0,  0 },//set ECB Zero                  DS_ECB_ZERO,
    {0x62,  2,  0 },//set ECB Location              DS_ECB_LOCATION,
    {0x63,  0,  2 },//get EPBI Rpm                  DG_EPBI_RPM,
    {0x64,  0,  2 },//get ecb adc                   DG_ECB_ADC,
    {0x65,  2,  0 },//set inductor pwm percent      DS_IDCT_PWM_PERCENT,
    {0x66,  0,  2 },//battery status                DG_BATTERY_STATUS,
    {0x67,  2,  0 },//watts                         DS_WATTS,
    {0x68,  2,  0 },//rpm gear ratio                DS_RPM_GEAR_RATIO,
    {0x69,  1,  0 },//generate pole pair            DS_GEN_POLE_PAIR,
    {0x6A,  2,  0 },//limit rpm for change          DS_LMT_RPM_FOR_CHANGE,
    {0x6B,  2,  0 },//limit rpm for res             DS_LMT_RPM_FOR_RES,
    {0x6C,  1,  0 },//machine type                  DS_MACHINE_TYPE,
    {0x6D,  1,  0 },//power off                     DS_POWER_OFF,
    {0x6E,  1,  0 },//battery charge level          DS_BATTERY_CHARGE_LEVEL,
    {0x6F,  1,  0 },//min res when inc work         DS_MIN_RES_WHEN_INC_WORK,
    {0x80,  2,  0 },//set inc percent               DS_INC_PERCENT,
    {0x81,  0,  2 },//get inc percent               DG_INC_PERCENT,
    {0x82,  1,  0 },//set ECB Action                DS_ECB_ACTION,
    {0x83,  0,  1 },//get ECB status                DG_ECB_STATUS,
    {0x84,  0,  2 },//get ECB count                 DG_ECB_COUNT,
    {0x85,  2,  0 },//set elect current             DS_ELECT_CURRENT,
    {0x86,  2,  0 },//set res type                  DS_RES_TYPE,
    {0x87,  2,  0 },//begin battery charge          DS_BEGIN_BATTERY_CHARGE,
    {0x8A,  1,  0 },//set climb mill status         DS_CLIMB_MILL_STATUS,
    {0x8B,  0,  1 },//get climb mill status         DG_CLIMB_MILL_STATUS,
    {0x8C,  1,  0 },//estop para                    DS_ESTOP_PARA,
    
//    {0x88,  2,  2 },//Save Calibration Point 1 PWM  DS_INDUCTION_CALSAVEP1,
//    {0x89,  2,  2 },//Save Calibration Point 2 PWM  DS_INDUCTION_CALSAVEP2,
//    {0x8A,  2,  2 },//Save Calibration Point 3 PWM  DS_INDUCTION_CALSAVEP3,
//    {0x8B,  2,  2 },//Save Calibration Point 4 PWM  DS_INDUCTION_CALSAVEP4,
//    {0x8C,  2,  2 },//Save Calibration Point 5 PWM  DS_INDUCTION_CALSAVEP5,
//    {0x8D,  0,  2 },//Get Calibration Point 1 PWM   DG_INDUCTION_CALP1,
//    {0x8E,  0,  2 },//Get Calibration Point 2 PWM   DG_INDUCTION_CALP2,
//    {0x8F,  0,  2 },//Get Calibration Point 3 PWM   DG_INDUCTION_CALP3,
//    {0x90,  0,  2 },//Get Calibration Point 4 PWM   DG_INDUCTION_CALP4,
//    {0x91,  0,  2 },//Get Calibration Point 5 PWM   DG_INDUCTION_CALP5,
    {0x95,  0,  1 },//get tm in used status         DG_GET_TM_IN_USED,
    {0x7A,  1,  1 },//CmdEUPsMode                   DS_EUP,
    {0x99,  0,  1 },//Get_IR_Statu                  DG_IR_STATUS,
    
    {0x50,  1,  6 },//Get Version New               DG_NEW_VERSION,
    {0x50,  4,  2 },//Save PWM                      DS_SAVE_PWM,
    {0x50,  2,  3 },//Read PWM                      DS_READ_PWM,
    {0x50,  0,  4 },//get lcb data infor            DG_LCB_DATA_INFOR,
    {0x50,  0,  16},//get lcb data content          DG_LCB_DATA_CONTENT,
    {0x00,  0,  1 },//G_STATUS1
    {0x00,  0,  2 },//G_MCB_TYPE
};


__no_init static FIFO_BASE FIFO_Base;
__no_init static UCHAR aby_FIFOBuff[32];                                        //FIFO用Buff
__no_init static UCHAR aby_Total[C_DATA_LEN];                                   //接收数据
__no_init static UCHAR by_Command_Point;
__no_init static UCHAR by_Command_End;                                          //待发送命令buff 最后一个位置
__no_init static UINT16 w_KeepOnlineTimer;
__no_init static RX_DATA_OFFSET Offset_Adress[CMD_LEN];
__no_init static CMD_BUFF Cmd_Buffer[C_TX_BUFF_SIZE];                           //待发送命令buff
__no_init static CMD_RX CmdRx;
__no_init static ERR_STEP ErrStep;
__no_init static UN16 unError;
__no_init static UINT16 w_CommonTimer;
__no_init static UINT16 w_SpecialTimer;
__no_init static UINT16 w_SpeedTimer;
__no_init static UINT16 w_LostTimer;
__no_init static UINT16 w_LCBErrorDetectTimer;
__no_init static UCHAR bInited;                                                 //是否已完成初始化
__no_init static UCHAR by_DriverType;
__no_init static UCHAR by_MCBType;
__no_init static UINT16 w_MCBVersion;
__no_init static LCB_SPEED_REF SpeedRef;
__no_init static UINT16 w_CurrentSpeed;
__no_init static UINT16 w_TargetSpeed;
__no_init static UINT16 w_InductData[5];
__no_init static struct
{
    unsigned Enable         :1;     //模块开关，当更新LCB时，本模块需要停止工作．
    unsigned ERP_Ack        :1;
    unsigned DetectError    :1;
    unsigned ErrSkip_Ack    :1;
    unsigned Cmd_Ack        :1;
} LCB;


void LCB_CmdPack(COM_TYPE CmdType, UINT32 w_Data);
void LCB_CmdProcess(void);
void LCB_Speed_Update(void);
UCHAR LCB_MCBHaveIncline(void);


void LCB_Initial(void)
{
    LCB_Hal_Initial();
    
    w_CurrentSpeed = 0;
    w_TargetSpeed = 0;
    by_Command_End = 0;
    by_Command_Point = 0;
    w_KeepOnlineTimer = 0;
    w_CommonTimer = 0;
    w_SpecialTimer = 0;
    w_SpeedTimer = 0;
    w_LostTimer = 0;
    w_LCBErrorDetectTimer = 0;
    unError.Word = 0;
    
    memset(aby_Total, 0, sizeof(aby_Total));
    memset(Cmd_Buffer, CMD_NONE, sizeof(Cmd_Buffer));
    FIFO_Initial(&FIFO_Base, aby_FIFOBuff, sizeof(aby_FIFOBuff));
    
    UCHAR by_Offset = 0;
    for(UCHAR i = 0; i < CMD_LEN; i++)
    {
        Offset_Adress[i].Cmd = Cmd_Talbe[i].Cmd;
        Offset_Adress[i].RxOffset = by_Offset;
        Offset_Adress[i].RxLen = Cmd_Talbe[i].RxLen;
        by_Offset += Offset_Adress[i].RxLen;
    }
    
    Offset_Adress[G_MCB_TYPE].RxOffset = Offset_Adress[DG_NEW_VERSION].RxOffset + 2;
    
    aby_Total[Offset_Adress[G_STATUS1].RxOffset] = C_LCB_INITIALING;            //默认下控在初始化中
    aby_Total[Offset_Adress[DG_DRVE_TYPE].RxOffset] = 0x5a;
    
    LCB.Enable = 1;
}

void LCB_Clear_SendBuff(void)
{
    by_Command_End = 0;
    by_Command_Point = 0;
    memset(Cmd_Buffer, CMD_NONE, sizeof(Cmd_Buffer));
    FIFO_Initial(&FIFO_Base, aby_FIFOBuff, sizeof(aby_FIFOBuff));
}

void LCB_Process(void)
{
    if(!LCB.Enable) return;
    
    if(LCB.DetectError)
    {
        switch(ErrStep)
        {
        case ErrGet:
            LCB_Send_Cmd(DG_ERRCODE, NO_PAR);
            ErrStep = ErrHdle;
            w_SpecialTimer = 0;
            break;
        case ErrHdle:
            {
                LCB_CmdProcess();
                UN16 un16;
                un16.Word = LCB_Get_Data(DG_ERRCODE);
                if(un16.Word != 0x00 || w_SpecialTimer >= 3000)
                {
                    LCB_Clear_Data(DG_ERRCODE);
                    ErrStep = ErrSkip;
                }
                
                switch(un16.Word)
                {
                case 0x0000:
                case 0x0440:
                case 0x0441:
                case 0x0442:
                case 0x0443:
                case 0x04A0:                                                    //bypass
                    break;
                default:
                    FIFO_Input(&FIFO_Base, un16.Byte, 2);
                }
                
                w_LCBErrorDetectTimer = 0;
            }
            break;
        case ErrSkip:
            LCB.ErrSkip_Ack = 0;
            LCB_Send_Cmd(DS_SKIPERR, NO_PAR);
            ErrStep = ErrWaitSkipACK;
            w_SpecialTimer = 0;
            break;
        case ErrWaitSkipACK:
			LCB_CmdProcess();
			if(LCB.ErrSkip_Ack || w_SpecialTimer >= 2000)
			{
				LCB.ErrSkip_Ack = 0;
				ErrStep = ErrGet;
				LCB.DetectError = 0;
			}
			break;
        default:
            LCB.DetectError = 0;
            break;
        }
    }
    else
    {
        if(bInited)
        {
            LCB_CmdProcess();
            
            if(w_LCBErrorDetectTimer >= 2000 && (LCB_Get_Data(G_STATUS1) & C_LCB_ERROR))
            {
                w_LCBErrorDetectTimer = 0;
                LCB.DetectError = 1;
            }
        }
    }
}

void LCB_CmdProcess(void)
{
    Watchdog_Reset();
    
    if(LCB_Hal_Get_Cmd(&CmdRx))                                                 //检查是否有收到新的数据
    {
        aby_Total[Offset_Adress[G_STATUS1].RxOffset] = CmdRx.Status;
        w_LostTimer = 0;
        
        LCB.Cmd_Ack = 1;
        if(CmdRx.Cmd == 0x7A) LCB.ERP_Ack = 1;                                  //说明LCB有回复ERP执行状态
        if(CmdRx.Cmd == 0x76) LCB.ErrSkip_Ack = 1;
        
        for(UCHAR i = 0; i < CMD_LEN; i++)
        {
            if(CmdRx.Cmd == Offset_Adress[i].Cmd)
            {
                if(CmdRx.Cmd == 0x50 && CmdRx.Len == 5)                         //用来支持JHT读下控版本号时恢复5个数据
                {
                    aby_Total[Offset_Adress[i].RxOffset] = CmdRx.Data[0];
                    aby_Total[Offset_Adress[i].RxOffset+1] = 0;
                    aby_Total[Offset_Adress[i].RxOffset+2] = CmdRx.Data[1];
                    aby_Total[Offset_Adress[i].RxOffset+3] = CmdRx.Data[2];
                    aby_Total[Offset_Adress[i].RxOffset+4] = CmdRx.Data[3];
                    aby_Total[Offset_Adress[i].RxOffset+5] = CmdRx.Data[4];
                }
                else if(CmdRx.Len == Offset_Adress[i].RxLen)
                {
                    memcpy(&aby_Total[Offset_Adress[i].RxOffset], CmdRx.Data, CmdRx.Len);
                    return;
                }
            }
        }
    }
    
    
    //数据Buff发送
    if(!LCB_Hal_Tx_Busy())
    {
        if(by_Command_Point != by_Command_End)
        {
            if(by_Command_Point < (C_TX_BUFF_SIZE - 1))                         //发送下一个命令
                ++by_Command_Point;
            else
                by_Command_Point = 0;
            
            LCB_CmdPack(Cmd_Buffer[by_Command_Point].CmdType, Cmd_Buffer[by_Command_Point].Data);
            memset(&Cmd_Buffer[by_Command_Point], CMD_NONE, sizeof(Cmd_Buffer[by_Command_Point]));
        }
    }
    
    
    if(bInited)
    {
        //Speed Update
        if(by_DriverType == LCB_TM)
        {
            LCB_Speed_Update();
        }
        
        
        //保持通讯,并获取RPM
        if((by_Command_Point == by_Command_End && w_KeepOnlineTimer > C_KEEP_ONLINE_IDLE)   //忙时500ms插入一个,不忙时250ms插入一个
           || w_KeepOnlineTimer > C_KEEP_ONLINE)
        {
            w_KeepOnlineTimer = 0;
            
            static UCHAR by_Shift = 0;
            
            switch(by_Shift)
            {
            default:
            case 0:
                if(by_DriverType == LCB_TM)
                {
                    LCB_Send_Cmd(DG_MOT_RPM, 0);
                }
                else
                {
                    LCB_Send_Cmd(DG_EPBI_RPM, 0);
                }
                
                by_Shift = 1;
                break;
            case 1:
                if(LCB_MCBHaveIncline())
                {
                    LCB_Send_Cmd(DG_INC_LOCATION, 0);
                }
                
                by_Shift = 2;
                break;
            case 2:
                if(by_DriverType == LCB_TM)
                {
                    LCB_Send_Cmd(DG_IR_STATUS, 0);
                }
                
                if(by_MCBType == EP_0B_JIS)                                     //Climbmill
                {
                    LCB_Send_Cmd(DG_CLIMB_MILL_STATUS, 0);
                }
                by_Shift = 0;
                break;
            }
        }
    }
}

void LCB_Get_DataPointer(COM_TYPE en_Index, UCHAR *pby, UINT16 *size)
{
    *size = Offset_Adress[en_Index].RxLen;
    memcpy(pby, &aby_Total[Offset_Adress[en_Index].RxOffset], Offset_Adress[en_Index].RxLen);
}

UINT16 LCB_Get_Data(COM_TYPE en_Index)
{
    UINT16 w_Data = 0;
    
    switch(en_Index)
    {
    case DG_NEW_VERSION:
        {
            UCHAR by_Len = aby_Total[Offset_Adress[DG_NEW_VERSION].RxOffset + 1];
            if(by_Len) by_Len--;
            w_Data |= aby_Total[Offset_Adress[DG_NEW_VERSION].RxOffset + 2 * by_Len + 4];
            w_Data <<= 8;
            w_Data += aby_Total[Offset_Adress[DG_NEW_VERSION].RxOffset + 2 * by_Len + 5];
        }
        break;
    case G_MCB_TYPE:
        w_Data = aby_Total[Offset_Adress[DG_NEW_VERSION].RxOffset + 2];
        break;
    case DG_MOT_RPM:
    case DG_EPBI_RPM:
        if(by_DriverType == LCB_TM)
        {
            w_Data  |= aby_Total[Offset_Adress[DG_MOT_RPM].RxOffset];
            w_Data <<= 8;
            w_Data  += aby_Total[Offset_Adress[DG_MOT_RPM].RxOffset + 1];
        }
        else
        {
            w_Data  |= aby_Total[Offset_Adress[DG_EPBI_RPM].RxOffset];
            w_Data <<= 8;
            w_Data  += aby_Total[Offset_Adress[DG_EPBI_RPM].RxOffset + 1];
        }
        break;
    default:
        switch(Offset_Adress[en_Index].RxLen)
        {
        case 1:
            w_Data  |= aby_Total[Offset_Adress[en_Index].RxOffset];
            break;
        case 2:
            w_Data  |= aby_Total[Offset_Adress[en_Index].RxOffset];
            w_Data <<= 8;
            w_Data  += aby_Total[Offset_Adress[en_Index].RxOffset + 1];
            break;
        case 3:
            w_Data   = 0;
            w_Data  |= aby_Total[Offset_Adress[en_Index].RxOffset + 1];
            w_Data <<= 8;
            w_Data  += aby_Total[Offset_Adress[en_Index].RxOffset + 2];
            break;
        default:
            break;
        }
        break;
    }
    
    return w_Data;
}

void LCB_Clear_Data(COM_TYPE en_Index)
{
    memset(&aby_Total[Offset_Adress[en_Index].RxOffset], 0, Offset_Adress[en_Index].RxLen);
}

void LCB_Send_Cmd(COM_TYPE CmdType, UINT32 w_Data)
{
    if(CmdType == DS_MOT_RPM)                                                   //如果是发送Speed的话,把Speed存入Buff中
    {
        w_TargetSpeed = w_Data;
    }
    else
    {
        if(CmdType == DS_INC_LOCATION)
        {
            if(by_MCBType == MCB_DELTA_COMMOCIAL || by_MCBType == MCB_TOPTEK)
            {
//                if(w_Data > DEFAULT_TOPTEK_MAX_INC)
//                {
//                    w_Data = DEFAULT_TOPTEK_MAX_INC;
//                }
//                
//                if(w_Data < DEFAULT_TOPTEK_MIN_INC)
//                {
//                    w_Data = DEFAULT_TOPTEK_MIN_INC;
//                }
                
//                if(w_Data < 2560) w_Data = 2560;
//                if(w_Data > 26240) w_Data = 26240;
//                //w_Data = w_Data * 128;
//                w_Data |= 0x8000;
            }
        }
        
        //待发送Buff中已有相同命令,则丢弃
        for(UCHAR i = 0; i < C_TX_BUFF_SIZE; i++)
        {
            if(Cmd_Buffer[i].CmdType == CmdType && Cmd_Buffer[i].Data == w_Data)
            {
                return;
            }
        }
        
        //存入Buff中
        if(by_Command_End < (C_TX_BUFF_SIZE - 1))
            by_Command_End++;
        else
            by_Command_End = 0;
        
        
        Cmd_Buffer[by_Command_End].CmdType  = CmdType;
        Cmd_Buffer[by_Command_End].Data = w_Data;
    }
}

void LCB_CmdPack(COM_TYPE CmdType, UINT32 w_Data)
{
    CMD_TX CmdTx;
    memset(&CmdTx, 0, sizeof(CmdTx));
    CmdTx.Cmd = Cmd_Talbe[CmdType].Cmd;
    if(CmdType == DG_IR_STATUS)
    {
        CmdTx.Adress = IR_ADDRS;
    }
    else
    {
        CmdTx.Adress = LCB_ADDRS;
    }
    
    CmdTx.Len = Cmd_Talbe[CmdType].TxLen;
    
    if(CmdTx.Len)
    {
        for(UCHAR i = 0; i < CmdTx.Len; i++)
        {
            CmdTx.Data[i] = (UCHAR)(w_Data >> ((CmdTx.Len - 1 - i) * 8));       //仅可发送0-4个byte的数据
        }
    }
    
    LCB_Hal_Send_Cmd(&CmdTx);
}

UINT16 LCB_Get_Error(UCHAR bClear)
{
    UINT16 wOut = unError.Word;
    
    if(bClear) unError.Word = 0;
    
    if(unError.Word == 0 && FIFO_Peek(&FIFO_Base))
    {
        if(FIFO_Output(&FIFO_Base, unError.Byte, 2) != 2)
        {
            unError.Word = 0;
        }
    }
    
    if(w_LostTimer >= C_LOST_TIME && wOut == 0)
    {
        wOut = 0x04B0;
    }
    
    return wOut;
}

void LCB_1ms_Int(void)
{
    if(!LCB.Enable) return;
    
    LCB_Hal_1ms_Int();
    
    if(w_LostTimer < 0xFFFF) w_LostTimer++;
    if(w_SpeedTimer < 0xFFFF) w_SpeedTimer++;
    if(w_CommonTimer < 0xFFFF) w_CommonTimer++;
    if(w_SpecialTimer < 0xFFFF) w_SpecialTimer++;
    if(w_KeepOnlineTimer < 0xFFFF) w_KeepOnlineTimer++;
    if(w_LCBErrorDetectTimer < 0xFFFF) w_LCBErrorDetectTimer++;
}

void LCB_Power_On(UCHAR bWakeUP)
{
    by_DriverType = LCB_TM;
    by_MCBType = 0;
    w_MCBVersion = 0;
    ErrStep = ErrGet;
    
    memset(&LCB, 0, sizeof(LCB));
    memset(&SpeedRef, 0, sizeof(SpeedRef));
    memset(w_InductData, 0, sizeof(w_InductData));
    
    SpeedRef.AddStep = LCB_RPM_ADD_STEP;
    SpeedRef.DecStep = LCB_RPM_DEC_STEP;
    
    LCB.Enable = 1;
    
    if(bWakeUP)
    {
        LCB_Hw_WakeUp_LCB();
        
        bInited = 0;
        LCB_Hal_Initial();
        LCB_Clear_Data(DS_EUP);
        LCB_Send_Cmd(DS_EUP, LCB_ERP_LEAVE);
        LCB.ERP_Ack = 0;
        LCB.Cmd_Ack = 0;
        w_CommonTimer = 0;
        
        while(1)
        {
            LCB_CmdProcess();
            
            if(LCB.ERP_Ack || LCB.Cmd_Ack)
            {
                LCB.ERP_Ack = 0;
                LCB.Cmd_Ack = 0;
                if(LCB_Get_Data(DS_EUP) == LCB_ERP_ACK_WAIT)
                {
                    LCB_Clear_Data(DS_EUP);
                    LCB_Send_Cmd(DS_EUP, LCB_ERP_LEAVE);
                }
                else
                {
                    break;
                }
            }
            
            if(w_CommonTimer >= 5000) break;
        }
    }
    
    
    LCB_Hal_Initial();
    LCB_Initial();
    w_CommonTimer = 0;
    w_SpecialTimer = 0;
    LCB_Send_Cmd(DS_INITIAL, 0);
    
    while(aby_Total[Offset_Adress[G_STATUS1].RxOffset] & C_LCB_INITIALING)
    {
        LCB_CmdProcess();
        
        if(w_CommonTimer >= 300)
        {
            w_CommonTimer = 0;
            LCB_Send_Cmd(DS_INITIAL, 0);
        }
        
        if(w_SpecialTimer >= 5000) break;                                       //5秒后退出
    }
    
    LCB_Send_Cmd(DS_WORK_STATUS, 0);
    LCB_Send_Cmd(DS_WORK_STATUS, 1);
    w_CommonTimer = 0;
    while(w_CommonTimer < 500) LCB_CmdProcess();
    
    //读版本号
    UINT16 wVersion = 0;
    LCB_Send_Cmd(DG_VERSION, 0);
    w_CommonTimer = 0;
    
    while(w_CommonTimer < 2000)
    {
        LCB_CmdProcess();
        if(LCB_Get_Data(DG_VERSION) != 0x00) break;
    }
    
    wVersion = LCB_Get_Data(DG_VERSION);
    
    if(wVersion == 0xFF00)
    {
        LCB_Send_Cmd(DG_NEW_VERSION, 0xf0);
        w_CommonTimer = 0;
        while(w_CommonTimer < 2000)
        {
            LCB_CmdProcess();
            if(LCB_Get_Data(DG_NEW_VERSION) != 0x00) break;
        }
        
        by_MCBType = (UCHAR)LCB_Get_Data(G_MCB_TYPE);
        w_MCBVersion = LCB_Get_Data(DG_NEW_VERSION);
    }
    else
    {
        by_MCBType = wVersion >> 8;
        w_MCBVersion = wVersion;
    }
    
    
    //针对特定机种功能初始化
    switch(by_MCBType)
    {
    case EP_0B_JIS:
        //LCB_Send_Cmd(DS_CLIMB_MILL_STATUS, 0xA2);
        break;
    case DCI_DAUGHTER_BOARD:
        {
            //inc range
            LCB_Send_Cmd(DS_MANUAL_CALIBRATION, 0);
            LCB_Send_Cmd(DG_INC_RANGE, 0);
            LCB_Send_Cmd(DG_RPM_SPEED_RANGE, 0);
            
            UCHAR aby_Buff[RX_BUFFER_MAX];
            UINT16 w_Size = 0;
            UN16 un16;
            UN16 unTemp;
            un16.Word = 0;
            unTemp.Word = 0;
            w_CommonTimer = 0;
            w_SpecialTimer = 0;
            
            while(!(un16.Word == 0x0500 && unTemp.Word == 0x7800) && w_SpecialTimer < 3000)
            {
                LCB_CmdProcess();
                LCB_Get_DataPointer(DG_INC_RANGE, aby_Buff, &w_Size);
                
                un16.Byte[0] = aby_Buff[1];
                un16.Byte[1] = aby_Buff[0];
                
                unTemp.Byte[0] = aby_Buff[3];
                unTemp.Byte[1] = aby_Buff[2];
                
                if(w_CommonTimer >= 300)
                {
                    w_CommonTimer = 0;
                    LCB_Send_Cmd(DS_INC_RANGE, 0);
                    LCB_Send_Cmd(DG_INC_RANGE, 0);
                }
            }
            
            
            //rpm speed range
            un16.Word = 0;
            unTemp.Word = 0;
            w_CommonTimer = 0;
            w_SpecialTimer = 0;
            
            while(!(un16.Word == 0x00a5 && unTemp.Word == 0x14b4) && w_SpecialTimer < 3000)
            {
                LCB_CmdProcess();
                LCB_Get_DataPointer(DG_RPM_SPEED_RANGE, aby_Buff, &w_Size);
                un16.Byte[0] = aby_Buff[1];
                un16.Byte[1] = aby_Buff[0];
                
                unTemp.Byte[0] = aby_Buff[3];
                unTemp.Byte[1] = aby_Buff[2];
                
                if(w_CommonTimer >= 300)
                {
                    w_CommonTimer = 0;
                    LCB_Send_Cmd(DS_RPM_SPEED_RANGE, 0);
                    LCB_Send_Cmd(DG_RPM_SPEED_RANGE, 0);
                }
            }
            
            LCB_Send_Cmd(DG_DCI_VER, 0);
        }
        break;
    case ECB_JIS:
    case BIKE_0C_JIS:
    case ECB_INDUCTOR_BRAKE:
    case INDUCTOR_BRAKE_INCLINE:
        {
            memset(w_InductData, 0, sizeof(w_InductData));
            
            if(wVersion == 0xFF00)
            {
                LCB_Clear_Data(DS_READ_PWM);
                LCB_Send_Cmd(DS_READ_PWM, 0x8100|0x0001);
                w_CommonTimer = 0;
                while(w_CommonTimer < 500)
                {
                    LCB_CmdProcess();
                    w_InductData[0] = LCB_Get_Data(DS_READ_PWM);
                    if(w_InductData[0] != 0x0000) break;
                }
                
                LCB_Clear_Data(DS_READ_PWM);
                LCB_Send_Cmd(DS_READ_PWM, 0x8100|0x0002);
                w_CommonTimer = 0;
                while(w_CommonTimer < 500)
                {
                    LCB_CmdProcess();
                    w_InductData[1] = LCB_Get_Data(DS_READ_PWM);
                    if(w_InductData[1] != 0x0000) break;
                }
                
                LCB_Clear_Data(DS_READ_PWM);
                LCB_Send_Cmd(DS_READ_PWM, 0x8100|0x0003);
                w_CommonTimer = 0;
                while(w_CommonTimer < 500)
                {
                    LCB_CmdProcess();
                    w_InductData[2] = LCB_Get_Data(DS_READ_PWM);
                    if(w_InductData[2] != 0x0000) break;
                }
                
                LCB_Clear_Data(DS_READ_PWM);
                LCB_Send_Cmd(DS_READ_PWM, 0x8100|0x0004);
                w_CommonTimer = 0;
                while(w_CommonTimer < 500)
                {
                    LCB_CmdProcess();
                    w_InductData[3] = LCB_Get_Data(DS_READ_PWM);
                    if(w_InductData[3] != 0x0000) break;
                }
                
                LCB_Clear_Data(DS_READ_PWM);
                LCB_Send_Cmd(DS_READ_PWM, 0x8100|0x0005);
                w_CommonTimer = 0;
                while(w_CommonTimer < 500)
                {
                    LCB_CmdProcess();
                    w_InductData[4] = LCB_Get_Data(DS_READ_PWM);
                    if(w_InductData[4] != 0x0000) break;
                }
                
                LCB_Clear_Data(DS_READ_PWM);
                
                if(w_InductData[0] == 0 && w_InductData[1] == 0 &&
                   w_InductData[2] == 0 && w_InductData[3] == 0 &&
                       w_InductData[4] == 0)
                {
                    w_InductData[0] = 1;
                    w_InductData[1] = 1;
                    w_InductData[2] = 1;
                    w_InductData[3] = 1;
                    w_InductData[4] = 1;
                }
            }
//        else
//        {
//            LCB_Send_Cmd(DG_INDUCTION_CALP1,NO_PAR);                            //Get induction brake calibration point 1 data
//            LCB_Send_Cmd(DG_INDUCTION_CALP2,NO_PAR);                            //Get induction brake calibration point 2 data
//            LCB_Send_Cmd(DG_INDUCTION_CALP3,NO_PAR);                            //Get induction brake calibration point 3 data
//            LCB_Send_Cmd(DG_INDUCTION_CALP4,NO_PAR);                            //Get induction brake calibration point 4 data
//            LCB_Send_Cmd(DG_INDUCTION_CALP5,NO_PAR);                            //Get induction brake calibration point 5 data
//            
//            w_CommonTimer = 0;
//            while(w_CommonTimer < 3000)
//            {
//                LCB_CmdProcess();
//                w_InductData[0] = LCB_Get_Data(DG_INDUCTION_CALP1);   
//                w_InductData[1] = LCB_Get_Data(DG_INDUCTION_CALP2);     
//                w_InductData[2] = LCB_Get_Data(DG_INDUCTION_CALP3);   
//                w_InductData[3] = LCB_Get_Data(DG_INDUCTION_CALP4);   
//                w_InductData[4] = LCB_Get_Data(DG_INDUCTION_CALP5); 
//                
//                if(w_InductData[0] != 0 && w_InductData[1] != 0 && 
//                   w_InductData[2] != 0 && w_InductData[3] != 0 && 
//                  w_InductData[4] != 0) break;
//            }
//            
//            if(w_InductData[0] == 0 && w_InductData[1] == 0 &&
//               w_InductData[2] == 0 && w_InductData[3] == 0 &&
//               w_InductData[4] == 0)
//            {
//                w_InductData[0] = 2;
//                w_InductData[1] = 2;
//                w_InductData[2] = 2;
//                w_InductData[3] = 2;
//                w_InductData[4] = 2;
//            }
//        }
        }
    default:
        break;
    }
    
    by_DriverType = LCB_GetMCBType();
    bInited = 1;
}

void LCB_Set_DriverType(UCHAR DriverType)
{
    by_DriverType = DriverType;
}

UCHAR LCB_Get_DriverType(void)
{
    return by_DriverType;
}

UCHAR LCB_Get_MCBType(void)
{
    return by_MCBType;
}

UINT16 LCB_Get_Version(void)
{
    return w_MCBVersion;
}

void LCB_Set_SpeedRef(UCHAR by_AddStep, UCHAR by_DecStep)
{
    SpeedRef.AddStep = by_AddStep;
    SpeedRef.DecStep = by_DecStep;
}

void LCB_Speed_Update(void)
{
    UINT16 wTargetSpeed = w_TargetSpeed & 0x7FFF;
    
    if(w_SpeedTimer >= (w_CurrentSpeed != wTargetSpeed ? LCB_SPEED_TIMER : LCB_SPEED_KEEP_TIMER))
    {
        w_SpeedTimer = 0;
        
        if(wTargetSpeed > w_CurrentSpeed)
        {
            if((w_CurrentSpeed + SpeedRef.AddStep) <= wTargetSpeed)
            {
                w_CurrentSpeed += SpeedRef.AddStep;
            }
            else
            {
                w_CurrentSpeed = wTargetSpeed;
            }
            
            if(w_CurrentSpeed < LCB_RPM_MIN)  w_CurrentSpeed = LCB_RPM_MIN;
        }
        else if(wTargetSpeed < w_CurrentSpeed)
        {
            if((w_CurrentSpeed - wTargetSpeed) >= SpeedRef.DecStep)
            {
                w_CurrentSpeed -= SpeedRef.DecStep;
            }
            else
            {
                w_CurrentSpeed = wTargetSpeed;
            }
            
            if(w_CurrentSpeed < LCB_RPM_MIN)  w_CurrentSpeed = 0;
        }
        else
        {
            w_CurrentSpeed = wTargetSpeed;
        }
        
        
        UINT16 wSendSpeed = 0;
        if(BITGET(w_TargetSpeed, 0x8000))
        {
            wSendSpeed = w_CurrentSpeed | 0x8000;
        }
        else
        {
            wSendSpeed = w_CurrentSpeed;
        }
        
        
        COM_TYPE CmdType = DS_MOT_RPM;
        
        //待发送Buff中已有相同命令,则丢弃
        for(UCHAR i = 0; i < C_TX_BUFF_SIZE; i++)
        {
            if(Cmd_Buffer[i].CmdType == CmdType && Cmd_Buffer[i].Data == wSendSpeed)
            {
                return;
            }
        }
        
        //存入Buff中
        if(by_Command_End < (C_TX_BUFF_SIZE - 1))
            by_Command_End++;
        else
            by_Command_End = 0;
        
        Cmd_Buffer[by_Command_End].CmdType  = CmdType;
        Cmd_Buffer[by_Command_End].Data = wSendSpeed;
    } 
}

UCHAR LCB_GetMCBType(void)
{
    switch(by_MCBType)
    {
    case 0x02:          // LCB1 PWM(JHT)
    case 0x03:          // LCB3 PWM(JHT)
    case 0x05:          // LCB CD (JHT)
    case 0x07:          // LCB1 Current(JHT)
    case 0x08:          // LCB3 Current(JHT)
    case 0x09:          // LCB1 (Panatta)
    case 0x0A:          // LCB2 A5x Redesign(Self power)
    case 0x0B:          // ClimbMill(JHT)/EP(JIS)
    case 0x0C:          // Bike Electromagnetic (JIS)
    case 0x11:          // AD Board(JIS)
    case 0x12:          // ECB Board(JIS)
    case 0x13:          // LCBA  Chopper(HUR) <Bike>
    case 0x14:          // BIKE_LCB_1X(Watt control)
    case 0x15:          // Electromagnetic brake + Incline LCB(JIS)
    case 0xB4:          // ECB_JIS(JIS)
    case 0xB5:          // ECB_INDUCTOR_BRAKE(JIS)
    case 0xB6:          // Global LCB for ECB+Incline(JIS)
    case 0xB7:          // Global LCB for Induction(JIS)
    case 0xB8:          // Global LCB for Induction+Incline(JIS)
    case 0xB9:          // Global LCB for Full Function(JIS)
        return LCB_EP_BIKE;
    case 0x01:          // AC LCB(JIS)
    case 0x04:          // DC LCB(JIS)
    case 0x06:          // TM LCB PANATTA(JIS)
    case 0x0D:          // TM 2.0~2.75HP DC LCB (JIS)
    case 0x0E:          // TM 3.0~3.25HP DC LCB (JIS)
    case 0x0F:          // TM 2.0~2.75HP AC LCB (JIS)
    case 0x10:          // TM 3.0~3.25HP AC LCB (JIS)
    case 0xB0:          // Global 1.5-2.0HP/110V
    case 0xB1:          // Global 1.5-2.0HP/220V
    case 0xB2:          // Global 2.0-2.5HP/110V
    case 0xB3:          // Global 2.0-2.5HP/220V
    case 0xD0:          // Global 2.0-3.25HP/110V
    case 0xD1:          // Global 2.0-3.25HP/220V
    case 0xC0:          // DCI daughter board
    case 0xC1:          // Toptek
    case 0xC2:          // DELTA
    case 0xC3:          // DELTA DABU AC 1.1KW
    case 0xC4:          // DELTA Hybrid
    default:
        return LCB_TM;
    }
}

UCHAR LCB_MCBHaveIncline(void)
{
    switch(by_MCBType)
    {
    case 0x02:          // LCB1 PWM(JHT)
    case 0x03:          // LCB3 PWM(JHT)
    case 0x05:          // LCB CD (JHT)
    case 0x07:          // LCB1 Current(JHT)
    case 0x08:          // LCB3 Current(JHT)
    case 0x09:          // LCB1 (Panatta)
    case 0x0B:          // ClimbMill(JHT)/EP(JIS)
    case 0x0C:          // Bike Electromagnetic (JIS)
    case 0x12:          // ECB Board(JIS)
    case 0x13:          // LCBA  Chopper(HUR) <Bike>
    case 0x14:          // BIKE_LCB_1X(Watt control)
    case 0xB4:          // ECB_JIS(JIS)
    case 0xB5:          // ECB_INDUCTOR_BRAKE(JIS)
    case 0xB7:          // Global LCB for Induction(JIS)
        return 0;
    case 0x0A:          // LCB2 A5x Redesign(Self power)
    case 0x11:          // AD Board(JIS)
    case 0x15:          // Electromagnetic brake + Incline LCB(JIS)
    case 0xB6:          // Global LCB for ECB+Incline(JIS)
    case 0xB8:          // Global LCB for Induction+Incline(JIS)
    case 0xB9:          // Global LCB for Full Function(JIS)
    case 0x01:          // AC LCB(JIS)
    case 0x04:          // DC LCB(JIS)
    case 0x06:          // TM LCB PANATTA(JIS)
    case 0x0D:          // TM 2.0~2.75HP DC LCB (JIS)
    case 0x0E:          // TM 3.0~3.25HP DC LCB (JIS)
    case 0x0F:          // TM 2.0~2.75HP AC LCB (JIS)
    case 0x10:          // TM 3.0~3.25HP AC LCB (JIS)
    case 0xB0:          // Global 1.5-2.0HP/110V
    case 0xB1:          // Global 1.5-2.0HP/220V
    case 0xB2:          // Global 2.0-2.5HP/110V
    case 0xB3:          // Global 2.0-2.5HP/220V
    case 0xD0:          // Global 2.0-3.25HP/110V
    case 0xD1:          // Global 2.0-3.25HP/220V
    case 0xC0:          // DCI daughter board
    case 0xC1:          // Toptek
    case 0xC2:          // DELTA
    case 0xC3:          // DELTA DABU AC 1.1KW
    case 0xC4:          // DELTA Hybrid
    default:
        return 1;
    }
}

void LCB_Get_Induct(UINT16 *pw)
{
    for(UCHAR i=0; i<5; i++)
    {
        pw[i] = w_InductData[i];
    }
}

UCHAR LCB_Erp(void)
{
    UCHAR by_Out = 0;
    
    LCB_Clear_Data(DS_EUP);
    LCB_Send_Cmd(DS_EUP, LCB_ERP_ENTER);
    LCB.ERP_Ack = 0;
    w_CommonTimer = 0;
    while(1)
    {
        LCB_CmdProcess();
        if(LCB.ERP_Ack)
        {
            LCB.ERP_Ack = 0;
            if(LCB_Get_Data(DS_EUP) == LCB_ERP_ACK_WAIT)
            {
                LCB_Clear_Data(DS_EUP);
                LCB_Send_Cmd(DS_EUP, LCB_ERP_ENTER);
                LCB.ERP_Ack = 0;
            }
            else
            {
                by_Out = 1;
                break;
            }
        }
        
        if(w_CommonTimer >= 5000)
        {
            by_Out = 2;
            break;
        }
    }
    
    LCB_Hal_Erp();
    
    return by_Out;
}

void LCB_Disable(void)
{
    LCB.Enable = 0;
}



