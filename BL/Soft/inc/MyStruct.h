#ifndef __STRUCT_H
#define __STRUCT_H

//#include "MyType.H"

#define DMaxSystemErrorBuffer                4
#define DProgramProfileBufferSize           24

/*
struct STreadmillRecord
{
    U16 U16_Mark;
    U16 U16_Model;
    U16 U16_SpeedMode;
    U32 U32_Time;
    U32 U32_Distance;
    U16 U16_Weight;
    U16 U16_MaxTime;
    U16 U16_LowSpeed;
    U16 U16_HightSpeed;
    U16 U16_MidSpeed;
    U16 U16_MaxIncline;
    U16 U16_MinIncline;
    U16 U16_CRC;
};
*/

// total information block (A+B) size = 256byte
typedef struct
{
    U16     U16_RecordMark;
    U32     U32_SystemCount;
    U32     U32_Time;
    U32     U32_Distance;
    U32     U32_InclineWorkoutTime;
    U32     U32_InclineHomingTime;
    U16     U16_MaxTime;
    U16     U16_DefaultTime;
    U16     U16_DefaultUserWeight;
    U16     U16_ShutDownTime;
    U16     U16_TelemetricThresholdL;
    U16     U16_TelemetricThresholdH;
    U16     U16_HandgripThresholdL;
    U16     U16_HandgripThresholdH;
    U16     U16_InclineTotalStroke; // 2006/11/10 新增
    U16     U16_Flag1;
    U8      U8_MachineType;
    U8      U8_DefaultUserAge;
    U8      U8_Language;
    U8      U8_FanLowSpeedPercent;
    U8      U8_FanHighSpeedPercent;
    U8      U8_MainLcdContrastPercent;
    U8      U8_ReadoutLcdContrastPercent;
    U8      U8_MaxHeartRateFormula;
    U8      U8_SmartResistanceControl;
    U8      U8_CautionLevel;
    U8      U8_InclineHomingLevel;
    U8      U8_ShutDownVoltage;
    U8      U8_ErrorCodeCount;
    U8      U8_ErrorCodeIndex;
    U8      Reverse[30];
    U16     U16_ErrorCode[DMaxSystemErrorBuffer];
    U8      CustomResistanceLevel[5][16];
    U8      CustomInclineLevel[5][16];
    U16     U16_CheckCRC;
} SBikeRecord;

typedef struct
{
    U16     U16_ProgramStatusFlag;
    U8      U8_ProgramStatusFlag;
    U8      U8_ProfileColumn;
    U8      U8_ProfileColumnIndex;
    U8      U8_ColumnBlinkIndex;
    //      主要為 resistance level, 當在 GLUTE BIKE, SUMMIT HIKE 則為 incline level
    U8      U8_ResistanceInclineLevel[DProgramProfileBufferSize];
    //      主要為 incline level, 當在 GLUTE BIKE, SUMMIT HIKE 則為 resistance level
    U8      U8_InclineResistanceLevel[DProgramProfileBufferSize];
    U16     U16_Time[DProgramProfileBufferSize];
} SBikeAndEp;

typedef struct
{
    U8      U8_KeyCode;
    U8      U8_KeyRepeatTimes;
} SKeyEvent;

//-----------------------------------------------------------------------------
#ifdef  __ICC8051__
typedef union
{
    struct
    {
        U8 LByte;
        U8 HByte;
    };
    struct
    {
        U16 Word;
    };
} Byte16;
#endif

#ifdef  __C51__
#pragma anon_unions
typedef union
{
    U16 Word;

    struct
    {
        U8 LByte;
        U8 HByte;
    } bytes;
} Byte16;
#endif
//-----------------------------------------------------------------------------
#ifdef  __ICC8051__
typedef union
{
    struct
    {
        U8      LLByte;
        U8      LHByte;
        U8      HLByte;
        U8      HHByte;
    };
    struct
    {
        U16     LWord;
        U16     HWord;
    };
    struct
    {
        U32     DWord;
    };
} Byte32;
#endif

#ifdef  __C51__
#pragma anon_unions
typedef union
{
    struct
    {
        U8      LLByte;
        U8      LHByte;
        U8      HLByte;
        U8      HHByte;
    } bytes;

    struct
    {
        U16     LWord;
        U16     HWord;
    } words;

    U32     DWord;
} Byte32;
#endif
//-----------------------------------------------------------------------------
#ifdef  __ICC8051__
typedef union
{
    struct
    {
        U8      CRC_CCITT_L;
        U8      CRC_CCITT_H;
    };
    struct
    {
        U16     CRC_CCITT;
    };
} CRC_CCITT16;
#endif

#ifdef  __C51__
#pragma anon_unions
typedef union
{
    U16     CRC_CCITT;

    struct
    {
        U8      CRC_CCITT_L;
        U8      CRC_CCITT_H;
    } TwoCRC8;

} CRC_CCITT16;
#endif
//-----------------------------------------------------------------------------
typedef struct
{
    TCallF  CallFunction;
    U8      U8_ProcessTime;
    U8      U8_CountTime;
    U8      U8_ProcessStatus;
} SCallF;

typedef struct
{
    U8      Seconds;
    U8      Minutes;
    U8      Hours;
    U8      Date;
    U8      Month;
    U8      Day;
    U8      Year;
    U8      WriteProtect;
} SHT1381;

#define DSkipInputHint2_IVF                     0x0001          // 是否顯示 index 2 message
#define DProcessEvent_IVF                       0x0002          // 是否執行自訂資料變更程序
#define DSubProgramKey_IVF                      0x0004          // 按 program key 是否可以執行類似 up 功能
#define DEnableProgramKey_IVF                   0x0008          // 按 program key 是否可以返回
#define DSkipInputHint1_IVF                     0x0010          // 是否顯示 index 1 message
#define DStartSimulationSelect_IVF              0x0020          // start 是否等於 select 功能
#define DCheckManagerSelectItem_IVF             0x0040          // 判斷輸入選項是否與原先值相同
#define DCheckExtFlag_IVF                       0x0080          // 檢察擴充旗標
#define DEnableUnPressKeyTimeout_IVF            0x0100          // 無按鍵逾時是否直接返回
#define DDisableSelectLed_IVF                   0x0200          // select led 不閃爍
#define DSpecialFormat_IVF                      0x0400          // 特殊輸出格式
#define DDisableShowVariable_IVF                0x0800          // 不顯示輸入數值
#define DDisableOffset2_IVF                     0x1000          // 取消 offset2 變數功能
#define DVariableLoop_IVF                       0x2000          // 數值是否循環 ( max -> min )
#define DVariableGap_IVF                        0x4000          // 是否輸入數值有不連續 ( 0, 5, 6.... )
#define DShowTitle_IVF                          0x8000          // 是否顯示 title message

#define DManagerItem2Format_IVF_F2              0x0100          // M 輸出
#define DSwitchOnOff_IVF_F2                     0x0200          // ON or OFF 輸出
#define DManagerItemFormat_IVF_F2               0x0400          // P 輸出
#define DProgramItemFormat_IVF_F2               0x0800          // # 輸出 , danny 建議 program 選擇時加 #1
#define DGenderFormat_IVF_F2                    0x1000          // 男女生符號輸出
#define DDifficultyLevelFormat_IVF_F2           0x2000          // Lx 輸出
#define DPercentOutputFotmat_IVF_F2             0x4000          // % 輸出
#define DFloatOutputtFotmat_IVF_F2              0x8000          // 浮點輸出

#define DEnableEraseFunction_IVF_ExtF           0x0001          // 允許 incline up + speed up
#define DEnableManager2Function_IVF_ExtF        0x0002          // 允許 1 + 3
#define DSkipSelectionLoadDefault_IVF_ExtF      0x0004          // 直接載入預設值後返回

typedef struct
{
    U16     U16_InputFlag;
    U16     U16_SpecialFormatFlag;
    U16     U16_ExtFlag;
    U16     U16_InputMaxVar;
    U16     U16_InputCurrentVar;
    U16     U16_InputMinVar;
    U16     U16_InputVariableOffset1;
    U16     U16_InputVariableOffset2;
    U16     U16_MessageIndex1;
    U16     U16_MessageIndex2;
    U16     U16_TitleIndex;
    U16     U16_VariableGap;
    TCallF_U16 FC_InputVariable;
    U8      U8_SubProgramKey;
} SInputVariable;
#endif

