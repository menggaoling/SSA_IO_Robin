#ifndef __TV_H
#define __TV_H

#include "includes.h"


typedef enum
{
    TV_CMD_POWER_ON         = 0,        
    TV_CMD_POWER_OFF        = 1,        
    TV_CMD_SOURCE           = 2,          
    TV_CMD_MUTE             = 3,            
    TV_CMD_MENU             = 4,            
    TV_CMD_ENTER            = 5,           
    TV_CMD_EXIT             = 6,            
    TV_CMD_AUDIO_DEC_LEFT   = 7,   
    TV_CMD_AUDIO_INC_RIGHT  = 8,  
    TV_CMD_CHANNEL_UP       = 9,    
    TV_CMD_CHANNEL_DOWN     = 10,
    TV_CMD_NUMBER0          = 11,         
    TV_CMD_NUMBER1          = 12,         
    TV_CMD_NUMBER2          = 13,         
    TV_CMD_NUMBER3          = 14,         
    TV_CMD_NUMBER4          = 15,         
    TV_CMD_NUMBER5          = 16,         
    TV_CMD_NUMBER6          = 17,         
    TV_CMD_NUMBER7          = 18,         
    TV_CMD_NUMBER8          = 19,        
    TV_CMD_NUMBER9          = 20,         
    TV_CMD_NUMBER100        = 21,       
    TV_CMD_CHANNEL_RETURN   = 22,   
    TV_CMD_CAPTION_DISPLAY  = 23,  
    TV_CMD_ASPECT_RATIO     = 24,     
    TV_CMD_VCHIP            = 25,           
    TV_CMD_COMPOSITEIN      = 26,     
    TV_CMD_SVIDOE_IN        = 27,        
    TV_CMD_COMPONENT        = 28,       
    TV_CMD_VGAIN            = 29,           
    TV_CMD_TUNER_IN         = 30,         
    TV_CMD_HDMI_IN          = 31,          
    TV_CMD_STATUS           = 32,          
    TV_CMD_EPG              = 33,             
    TV_CMD_TEL_TEXT         = 34,        
    TV_CMD_RED              = 35,             
    TV_CMD_GREEN            = 36,           
    TV_CMD_YELLOW           = 37,          
    TV_CMD_BLUE             = 38,            
    TV_CMD_ADI              = 39,             
    //TV_CMD_CLOSE_CAPTION    = 40,    
    //TV_CMD_HOME             = 41,            
    //TV_CMD_ASPECT           = 42,          
    TV_CMD_LEN              = 40,
} TV_COM_TYPE;


typedef struct
{
    unsigned TV_Rx_Timeout : 1;
    unsigned TV_Tx_Timeout : 1;
    unsigned TV_CRC        : 1;
    unsigned TV_TimeOut    : 1;
} TV_ERROR;


void TV_Initial(UCHAR bFirst);
void TV_SendCommand(TV_COM_TYPE Cmd);
UCHAR TV_Process(void);
void TV_1ms_Int(void);
UCHAR TV_TVTunerPowerStatus(void);
UCHAR TV_TVTunerType(void);
void TV_Erp(void);


#endif
