#ifndef __COM_H__
#define __COM_H__

#include "Com_Hw.h"


#define COM_MAX_LENGTH	                        (512+32)
#define COM_WRITE                               0x01
#define COM_READ                                0x02

// ==============================================
// RESULT
// ==============================================
#define RESULT_SUCCESS					        0x0000
#define RESULT_INVALID_REGISTER			        0x0001
#define RESULT_INVALID_DATA				        0x0002
#define RESULT_INVALID_COMMAND			        0x0003

//---------------- DEVICE TYPE -----------------------//
#define DEVICE_TYPE_SMM                         0x0001
#define DEVICE_TYPE_MMM                         0x0002
#define DEVICE_TYPE_LMM                         0x0003

#define STATUS_BIT_ERROR_FLAG                   0x0001
#define STATUS_BIT_KEY_FLAG                     0x0002
#define STATUS_BIT_WDT_OVERFLOW                 0x0004
#define STATUS_BIT_SAFEKEY                      0x0008
#define STATUS_BIT_ERP                          0x0020
#define STATUS_BIT_HR_DETECT                    0x0100
#define STATUS_BIT_IPOD_READY                   0x0200
#define STATUS_BIT_SAFE_REC                     0x0400
#define STATUS_BIT_LCB_CALIBRATION              0x0800

#define DSPI_OPTION_INTERRUPT_ENABLE            0x0001
#define DSPI_OPTION_WATCHDOG_ENABLE             0x0002
#define DSPI_OPTION_WATCHDOG_RESET              0x0004
#define DSPI_OPTION_HEARTRATE_PRIORITY          0x0100
#define DSPI_OPTION_CSAFE_RADIO_RESET_LINE      0x1000



typedef enum
{
    REGISTER_DEVICE_TYPE                        = 0x0000,
    REGISTER_STATUS                             = 0x0001,
    REGISTER_OPTION                             = 0x0003,
    REGISTER_SOFT_VERSION                       = 0x0005,
    REGISTER_KEY_CODE                           = 0x0006,
    REGISTER_ERROR                              = 0x0007,
    REGISTER_WDT_RESET                          = 0x0009,
    REGISTER_DATE                               = 0x000A,
    REGISTER_MESSAGE_UPDATE                     = 0x000C,
    REGISTER_GET_MACHINE_INFO                   = 0x000D,
    REGISTER_DCIVERSION                         = 0x0020, 
    REGISTER_DCIENVIRONMENT_RETRIEVE            = 0x0021, 
    REGISTER_MCB_VERSION                        = 0x0022, 
    REGISTER_DB_PERSONINPLACE                   = 0x0023, 
    REGISTER_CONTROL_TYPE                       = 0x0024,
    REGISTER_GETMCBINFORMATION                  = 0x0025, 
    REGISTER_VERSION_DB_NEW			= 0x0027,
    REGISTER_RPM_STEP                           = 0x0029,
    REGISTER_RPM_TARGET                         = 0x0030, 
    REGISTER_RPM_CURRENT                        = 0x0032, 
    REGISTER_RPM_CUREENT_ROLLER                 = 0x0034, 
    REGISTER_USER_RPM                           = 0x0035, 
    REGISTER_ADC_TARGET1                        = 0x0040,
    REGISTER_ADC_CURRENT1                       = 0x0042, 
    REGISTER_ADC_TARGET2                        = 0x0044, 
    REGISTER_ADC_CURRENT2                       = 0x0046, 
    REGISTER_INCLNE_ACTION                      = 0x0048, 
    REGISTER_TUNEEND_POINT_INCLINE1             = 0x0049, 
    REGISTER_TUNEEND_POINT_INCLINE2             = 0x004A, 
    REGISTER_LCB_CALIBRATE                      = 0x004B, 
    REGISTER_INCLINE_PERCENT                    = 0x004C, 
    REGISTER_ECB_ACTION                         = 0x0050, 
    REGISTER_ECB_COUNTER_TRAGET                 = 0x0051, 
    REGISTER_ECB_COUNTER_CURRENT                = 0x0053, 
    REGISTER_SET_INCLINE_STROKE                 = 0x0054, 
    REGISTER_SET_INDCT_BRK_PWM		        = 0x0056,
    REGISTER_SAVE_INDCT_BRK_CLB		        = 0x0057, 
    REGISTER_GET_INDCT_BRK_CLB		        = 0x0058,
    REGISTER_HR_HAND                            = 0x0060, 
    REGISTER_HR_TELEMETRIC                      = 0x0061, 
    REGISTER_SET_USB_CHARGE                     = 0x0065,
    REGISTER_BTCONTROL         			= 0x0066, 
    REGISTER_BTPOWERON         			= 0x0067, 
    REGISTER_RFIDWRITE         			= 0x0068, 
    REGISTER_RFIDREAD         			= 0x0069,
    REGISTER_LCB_BT_VOLTAGE                     = 0x0070, 
    REGISTER_LCB_GTOR_VOLTAGE                   = 0x0072, 
    REGISTER_LCB_DC_VOLTAGE                     = 0x0074, 
    REGISTER_LCB_DC_CURRENT                     = 0x0076, 
    REGISTER_SET_ELECT_ROMAGNETIC_CURRENT       = 0x0077, 
    REGISTER_BUZZER_CNTRL                       = 0x0080, 
    REGISTER_BUZZER_SQNCE                       = 0x0081, 
    REGISTER_FAN_CONTROL                        = 0x0086, 
    REGISTER_KEYPAD_SOUNDS_ENABLE               = 0x0087, 
    REGISTER_EXTERNAL_TV_AUDIO_ENABLE           = 0x0088, 
    REGISTER_EARPHONE_VOLUME                    = 0x008A, 
    REGISTER_EUP			         = 0x008C,
    REGISTER_ENTER_UPDATE_MODE                  = 0x0090, 
    REGISTER_UPDATE_DATA                        = 0x0091, 
    REGISTER_RPM_ACCELERATION                   = 0x0092,
    REGISTER_RPM_DEACCELERATION                 = 0x0093,
    REGISTER_QUICK_SEQUENCE                     = 0x0094, 
    REGISTER_KEYPAD_VERSION                     = 0x0095, 
    REGISTER_LCD_CONTROL                        = 0x00A0, 
    REGISTER_LCD_LOCATION                       = 0x00A2, 
    REGISTER_LCD_DATA                           = 0x00A6,
    REGISTER_LCD_FONTBANK                       = 0x00A7, 
    REGISTER_LCD_BACKLIGHT                      = 0x00A8,
    REGISTER_AUTOTEST_STAT                      = 0x00AA, 
    REGISTER_AUTOTEST_FLUSH                     = 0x00AC, 
    REGISTER_TV_TUNER_CONTROL                   = 0x00B0, 
    REGISTER_TV_TUNER_STATUS                    = 0x00B1, 
    REGISTER_TV_TUNER_POWER                     = 0x00B2, 
    REGISTER_SET_IPOD_POWER                     = 0x00B3, 
    REGISTER_GET_IOBOARD_STATUS                 = 0x00B4, 
    REGISTER_SET_LCB_POWER_OFFTIME              = 0x00B5, 
    REGISTER_GET_TVTUNER_TYPE                   = 0x00B6, 
    REGISTER_GET_GUIPOWER_STATUS                = 0x00B7, 
    REGISTER_CLOSE_BATTERY                      = 0x00B8, 
    REGISTER_CSAFE_RADIO                        = 0x00C0,
    REGISTER_CSAFE_PACKET_LENGTH                = 0x00C1, 
    REGISTER_RF_SETSTATUS                       = 0x00CA, 
    REGISTER_RF_GETSTATUS                       = 0x00CB, 
    REGISTER_RF_SETCHANNEL                      = 0x00CC, 
    REGISTER_CURRENT_RESISTANCE                 = 0x00D0, 
    REGISTER_PWM_RESISTANCE                     = 0x00D1, 
    REGISTER_RPM_GEARRATIO                      = 0x00D2,
    REGISTER_GENERATOR_POLEPAIR                 = 0x00D3, 
    REGISTER_RPM_LOW_LIMIT_CHARGE               = 0x00D4, 
    REGISTER_RPM_LOW_LIMIT_RESISTANCE           = 0x00D5, 
    REGISTER_RESISTANCE_TYPE                    = 0x00D6, 
    REGISTER_SET_WATT				= 0x00D7,	
    REGISTER_SET_CLIMBMILL_COMMAND              = 0x00E1, 
    REGISTER_GET_CLIMBMILL_STATUS               = 0x00E2, 
    REGISTER_SET_ESTOP_PARAMETER                = 0x00E3, 
    REGISTER_SET_EUPS_MODE                      = 0x00E4, 
    REGISTER_LCB_DEVICE_DATA                    = 0x00E5, 
    REGISTER_SET_CONSOLE_POWER                  = 0x00E6, 
    REGISTER_SET_SPIIDLE_TIMER                  = 0x00E7, 
    REGISTER_SET_VA_POWER                       = 0x00E8, 
    REGISTER_SET_MCUTURNOFF_LCB_TIME            = 0x00E9, 
    REGISTER_DCIIOBOARDINCLINERANGE             = 0x004D, 
    REGISTER_TVTUNNERTYPECMD                    = 0x00EC, 
    REGISTER_SETLCBFWUPDATE_START               = 0x00F1,
    REGISTER_GETLCBFWUPDATE_START               = 0x00F2,
    REGISTER_SETLCBFWUPDATE_DATAERASE           = 0x00F3,
    REGISTER_GETLCBFWUPDATE_DATAERASE           = 0x00F4,
    REGISTER_SETLCBFWUPDATE_DATAWRITE           = 0x00F5,
    REGISTER_GETLCBFWUPDATE_DATAWRITE           = 0x00F6,
    REGISTER_SETLCBFWUPDATE_DATAREAD            = 0x00F7,
    REGISTER_GETLCBFWUPDATE_DATAREAD            = 0x00F8,
    REGISTER_SETLCBFWUPDATE_DATACHECKCODE       = 0x00F9,
    REGISTER_GETLCBFWUPDATE_DATACHECKCODE       = 0x00FA,
    REGISTER_SETLCBFWUPDATE_DATAPROGRAMSTATE    = 0x00FB,
    REGISTER_GETLCBFWUPDATE_DATAPROGRAMSTATE    = 0x00FC,
    REGISTER_SETLCB_INFORMATION_DATAREAD        = 0x00FD,
    REGISTER_GETLCB_INFORMATION_DATAREAD        = 0x00FE,
    REGISTER_PLAYKEYTUNE                        = 0x0190,
    REGISTER_ERRORCODERESEND                    = 0x0107,
    REGISTER_WRITE_RAW_LCB                      = 0x0100,
    REGISTER_READ_RAW_LCB                       = 0x0101,
} CMD_TYPE;                                          


typedef struct tag_COM_PACKAGE
{
    UINT16 PacketHead;
    UCHAR  WR;
    UINT16 Command;
    UINT16 Length;
    UCHAR  Data[COM_MAX_LENGTH];                                                //EARLIER WAS SET TO 100, BUT SHOULD BE 128
} COM_PACKAGE;


void Com_Initial(void);
void Com_Write(UCHAR readwrite, UINT16 Command, UINT16 Length, UCHAR *pData);
UCHAR Com_Read(COM_PACKAGE *stData);
UCHAR Com_GetUpdateIOState(void);
UCHAR Com_IsBuzy(void);
void Com_10msInt(void);
void Com_Erp(void);


#endif

