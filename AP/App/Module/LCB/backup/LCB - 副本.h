#ifndef __LCB_H__
#define __LCB_H__

#include "LCB_Hal.h"


#define LCB_EP_BIKE             0
#define LCB_TM                  1

// define ECB status
#define ECB_STATUS_UP           BIT0
#define ECB_STATUS_DOWN         BIT1
#define ECB_STATUS_STOP         BIT2

#define C_LCB_INC_DOWN          BIT0    //1 = Incline Running Down
#define C_LCB_INC_UP            BIT1    //1 = Incline Running Up
#define C_LCB_DRIVE_MOTOR       BIT2    //1 = Main Motor is Running
#define C_LCB_RPM_DETECTOR      BIT3    //1 = OSP Detected Pulse
#define C_LCB_BATTERY_LOW       BIT4    //1 = 0:enougt 1:not enough
#define C_LCB_CMD_ERROR         BIT5    //1 = Received Error Command from UCB
#define C_LCB_ERROR             BIT6    //1 = MCB Found Error(s)
#define C_LCB_INITIALING        BIT7    //1 = MCB is Initializing


typedef enum
{
    //Common Command
    DS_INITIAL = 0,             //0x70
    DG_STATUS,                  //0x71
    DG_ERRCODE,                 //0x72
    DG_VERSION,                 //0x73
    DS_CALIBRATE,               //0x74
    DS_UPDATE,                  //0x75
    DS_SKIPERR,                 //0x76
    DS_EXTCMD,                  //0x77
    DG_DCI_VER,                 //0x78--
    DG_DCI_ENVIRONMENT,         //0x79--
    DS_CONSOLE_PWR,             //0x7B--

    //TM COMMAND
    DS_RPM_RTN,                 //0xf0
    DS_MOT_RPM,                 //0xf1
    DS_ADD_STEP,                //0xf2
    DS_DEC_STEP,                //0xf3
    DS_STOP_STEP,               //0xf4
    DS_INC_ACTION,              //0xf5
    DS_INC_LOCATION,            //0xf6
    DS_WORK_STATUS,             //0xf7
    DG_ROLLER_RPM,              //0xf8
    DG_MOT_RPM,                 //0xf9
    DG_INC_LOCATION,            //0xfA
    DS_INC_TUNE1,               //0xFB--
    DS_INC_TUNE2,               //0xFC--
    DS_INC_STROKE,              //0xFD--
    DS_MOTOR_HP,                //0x90
    DG_DRVE_TYPE,               //0x91
    DS_FORCE_INC_OPERATION,     //0x94--
    DG_MAIN_MOTOR_INF,          //0x96--
    DG_INC_RANGE,               //0xA0--
    DS_INC_RANGE,               //0xA1--
    DG_RPM_SPEED_RANGE,         //0xA6
    DS_RPM_SPEED_RANGE,         //0xA7
    DS_MANUAL_CALIBRATION,      //0xAB--

    //EP/BI COMMAND
    DS_ECB_ZERO,                //0x61
    DS_ECB_LOCATION,            //0x62
    DG_EPBI_RPM,                //0x63
    DG_ECB_ADC,                 //0x64
    DS_IDCT_PWM_PERCENT,        //0x65
    DG_BATTERY_STATUS,          //0x66
    DS_WATTS,                   //0x67
    DS_RPM_GEAR_RATIO,          //0x68
    DS_GEN_POLE_PAIR,           //0x69
    DS_LMT_RPM_FOR_CHANGE,      //0x6A
    DS_LMT_RPM_FOR_RES,         //0x6B
    DS_MACHINE_TYPE,            //0x6C
    DS_POWER_OFF,               //0x6D
    DS_BATTERY_CHARGE_LEVEL,    //0x6E
    DS_MIN_RES_WHEN_INC_WORK,   //0x6F
    DS_INC_PERCENT,             //0x80
    DG_INC_PERCENT,             //0x81
    DS_ECB_ACTION,              //0x82
    DG_ECB_STATUS,              //0x83
    DG_ECB_COUNT,               //0x84
    DS_ELECT_CURRENT,           //0x85
    DS_RES_TYPE,                //0x86
    DS_BEGIN_BATTERY_CHARGE,    //0x87
    DS_CLIMB_MILL_STATUS,       //0x8A
    DG_CLIMB_MILL_STATUS,       //0x8B
    DS_ESTOP_PARA,              //0x8C

//    DS_INDUCTION_CALSAVEP1,     //0x88
//    DS_INDUCTION_CALSAVEP2,     //0x89
//    DS_INDUCTION_CALSAVEP3,     //0x8A
//    DS_INDUCTION_CALSAVEP4,     //0x8B
//    DS_INDUCTION_CALSAVEP5,     //0x8C
//    DG_INDUCTION_CALP1,         //0x8D
//    DG_INDUCTION_CALP2,         //0x8E
//    DG_INDUCTION_CALP3,         //0x8F
//    DG_INDUCTION_CALP4,         //0x90
//    DG_INDUCTION_CALP5,         //0x91
    DG_GET_TM_IN_USED,          //0x95
    DS_EUP,                     //0x7A
    DG_IR_STATUS,               //0x99
    DG_NEW_VERSION,             //0x50 0xF0
    DS_SAVE_PWM,                //0x50
    DS_READ_PWM,                //0x50 0x8100 + 0x0001/2/3/4/5
    DG_LCB_DATA_INFOR,          //0x50 0x00--
    DG_LCB_DATA_CONTENT,        //0x50 0x02--
    G_STATUS1,                  //
    G_MCB_TYPE,                 //
    CMD_LEN,                    //0x
    CMD_NONE = 0XFF,
} COM_TYPE;


typedef enum
{
    NO_MCB                          = 0x00,
    AC_LCB_JIS                      = 0x01,
    LCB1_JHT                        = 0x02,
    LCB3_JHT                        = 0x03,
    DC_LCB_JIS                      = 0x04,
    LCB_CD_COSTDN_JHT               = 0x05,
    TM_LCB_PANATTA                  = 0x06,
    LCB1_PWN_JHT                    = 0x07,
    LCB3_CURRENT_JHT                = 0x08,
    LCB1_PANATTA                    = 0x09,
    LCB2_A5X_REDESIGN               = 0x0A,
    EP_0B_JIS                       = 0x0B,     //JHT Climbmill
    BIKE_0C_JIS                     = 0x0C,
    DC_LCB_20_275HP_JIS             = 0x0D,
    DC_LCB_30_325HP_JIS             = 0x0E,
    AC_LCB_20_275HP_JIS             = 0x0F,
    AC_LCB_30_325HP_JIS             = 0x10,
    AD_BOARD_JIS                    = 0x11,
    DIGITAL_ECB                     = 0x12,
    LCBA                            = 0x13,
    BIKE_LCB_1X                     = 0x14,
    INDUCTOR_BRAKE_INCLINE          = 0x15,
    GLOBAL_15_20HP_110              = 0xB0,
    GLOBAL_15_20HP_220              = 0xB1,
    GLOBAL_20_25HP_110              = 0xB2,
    GLOBAL_20_25HP_220              = 0xB3,
    ECB_JIS                         = 0xB4,
    ECB_INDUCTOR_BRAKE              = 0xB5,
    GLOBAL_LCB_ECB_INCLINE          = 0xB6,
    GLOBAL_LCB_INDUCTION            = 0xB7,
    GLOBAL_LCB_INDUCTION_INCLINE    = 0xB8,
    GLOBAL_LCB_FULL                 = 0xB9,
    DCI_DAUGHTER_BOARD              = 0xC0,
    MCB_TOPTEK                      = 0xC1,
    MCB_DELTA                       = 0xC2,
    MCB_DELTA_COMMOCIAL             = 0xC3,
    GLOBAL_20_325HP_110             = 0xD0,
    GLOBAL_20_325HP_220             = 0xD1,
} MCB_TYPE;


// TM: 
// 0xC1
// 0xC2
// 0xC3  
// 0xC4
// 
// Bike:
// 0x07
// 0x08
// 0x13
// 0x14
// 
// Elliptical:
// 0x07
// 
// Ascent:
// 0x0A
// 
// Stepper
// 0x07
// 
// Climb:
// 0x0B
//

#define CmdLCBReadUpdateMode                    0x40
#define CmdLCBReturnUpdateMode                  0x41
#define CmdLCBFlashUnlock                       0x24    
#define CmdLCBReturnFlashState                  0x25    
#define CmdLCBEraseFlash                        0x10    //REGISTER_SETLCBFWUPDATE_DATAERASE
#define CmdLCBReturnEraseFlash                  0x11    //REGISTER_GETLCBFWUPDATE_DATAERASE
#define CmdLCBWriteFlash                        0x12    //REGISTER_SETLCBFWUPDATE_DATAWRITE
#define CmdLCBReturnWriteFlash                  0x13    //REGISTER_GETLCBFWUPDATE_DATAWRITE
#define CmdLCBReadFlash                         0x14    //REGISTER_SETLCBFWUPDATE_DATAREAD
#define CmdLCBReturnReadFlash                   0x15    //REGISTER_GETLCBFWUPDATE_DATAREAD
#define CmdLCBWriteCheckCode                    0x26    //REGISTER_SETLCBFWUPDATE_DATACHECKCODE
#define CmdLCBReturnCheckCode                   0x27    //REGISTER_GETLCBFWUPDATE_DATACHECKCODE
#define CmdLCBReadProgramState                  0x20    //REGISTER_SETLCBFWUPDATE_DATAPROGRAMSTATE
#define CmdLCBReturnProgramState                0x21    //REGISTER_GETLCBFWUPDATE_DATAPROGRAMSTATE

#define CmdLCBReadProduceID                     0x30
#define CmdLCBReturnProduceID                   0x31
#define CmdLCBReadMcuType                       0x32
#define CmdLCBReturnMcuType                     0x33
#define CmdLCBReadMcuID                         0x34
#define CmdLCBReturnMcuID                       0x35
#define CmdLCBIsRequestUpdate                   0x42
#define CmdLCBReturnRequestUpdate               0x43


void LCB_Initial(void);
void LCB_Power_On(UCHAR bWakeUP);
void LCB_Process(void);
UINT16 LCB_Get_Error(UCHAR bClear);
void LCB_Get_DataPointer(COM_TYPE en_Index, UCHAR *pby, UINT16 *size);
UINT16 LCB_Get_Data(COM_TYPE en_Index);
void LCB_Clear_Data(COM_TYPE en_Index);
void LCB_Send_Cmd(COM_TYPE CmdType, UINT32 w_Data);
void LCB_1ms_Int(void);
void LCB_Set_DriverType(UCHAR DriverType);
UCHAR LCB_Get_DriverType(void);
UCHAR LCB_Get_MCBType(void);
UINT16 LCB_Get_Version(void);
void LCB_Set_SpeedRef(UCHAR by_AddStep, UCHAR by_DecStep);
UCHAR LCB_GetMCBType(void);
void LCB_Get_Induct(UINT16 *pw);
UCHAR LCB_Erp(void);
void LCB_Disable(void);


#endif



