#include "keyboard.h"
#include "Beep.h"
#include "Keyboard_Hw.h"
#include "LCB.h"


#define C_MEM_SIZE              8       //扫描口的个数
#define C_MAX_PRESS_COUNT       (3+1)   //最大同时按键数

#define C_MAX_TIME              20000
#define C_TRUE_TIME             5       //0.1s
#define C_HOLD_TIME_04S         20      //0.4s
#define C_HOLD_TIME_3S          150     //3s
#define C_HOLD_TIME_5S          250     //5s
#define C_HOLD_TIME_10S         500     //10s
#define C_HOLD_TIME_30S         1500    //30s  超过30s则认为按键卡死，需要屏蔽
#define C_REPLAY_TIME_SLOW      25      //0.5s
#define C_REPLAY_TIME_MID       12      //0.24s
#define C_REPLAY_TIME_FAST      3       //0.06s


//1.26.2016 应吴刚要求修改 + & -  //经过JHT1-28号确认完毕．
const UCHAR aby_KeyTable[8][8] =
{
    //DA0       DA1             DA2             DA3             DA4         DA5         DA6         DA7
    {KB_NONE,   KB_NONE,        KB_NONE,        KB_HOME,        KB_CC_MUTE, KB_DASH,    KB_0,       KB_DISPLAY_MODE,}, 	//SC0
    {KB_NONE,   KB_LEVEL_UP,    KB_NONE,        KB_TV_LAST,     KB_2,       KB_3,       KB_TV_CU,   KB_INCLINE_UP,  },	//SC1
    {KB_NONE,   KB_LEVEL_UP,    KB_LEVEL_DOWN,  KB_TV_VU,       KB_1,       KB_6,       KB_SPEED_UP,KB_SPEED_DOWN,  }, 	//SC2
    {KB_NONE,   KB_NONE,        KB_NONE,        KB_NONE,        KB_4,       KB_5,       KB_9,       KB_TV_CD,       }, 	//SC3
    {KB_NONE,   KB_NONE,        KB_NONE,        KB_NONE,        KB_7,       KB_NONE,    KB_NONE,    KB_8,           }, 	//SC4
    {KB_NONE,   KB_NONE,        KB_NONE,        KB_NONE,        KB_NONE,    KB_NONE,    KB_TV_VD,   KB_ENTER,       }, 	//SC5
    {KB_NONE,   KB_NONE,        KB_NONE,        KB_NONE,        KB_NONE,    KB_NONE,    KB_NONE,    KB_NONE,        }, 	//SC6
    {KB_NONE,   KB_NONE,        KB_NONE,        KB_NONE,        KB_NONE,    KB_PAUSE,   KB_STOP,    KB_INCLINE_DOWN,}, 	//SC7
};


__no_init static UINT16 w_KBTime;
__no_init static UCHAR by_ReplayCounter;
//__no_init static UCHAR bBeepEnable;
__no_init static UCHAR aby_KeyMask;
__no_init static UCHAR aby_KeyBuffer[C_MEM_SIZE];
__no_init static UCHAR aby_KeyPressed[C_MAX_PRESS_COUNT];
__no_init static struct
{
    unsigned Flag           : 1;
}KB;


UCHAR Keyboard_Encoder(void);
UCHAR Keyboard_Decode(UCHAR by_Key1, UCHAR by_Key2, UCHAR by_Key3);


void Keyboard_Initial(void)
{
    memset(aby_KeyBuffer, 0, sizeof(aby_KeyBuffer));
    memset(aby_KeyPressed, 0, sizeof(aby_KeyPressed));
    memset(&KB, 0, sizeof(KB));
    
    w_KBTime = 0;
    //bBeepEnable = TRUE;
    by_ReplayCounter = 0;
}

void Keyboard_EnableBeep(UCHAR bStatus)
{
    //bBeepEnable = bStatus ? TRUE : FALSE;
}

void Keyboard_ResetMask(void)
{
    aby_KeyMask = 0;
}

void Keyboard_Flag_20ms_Int(void)
{
    KB.Flag = 1;
}

UCHAR Keyboard_Get_Code(void)
{
    UCHAR by_Data = KB_NA;
    UCHAR by_Out = KB_NA;
    
    if(!KB.Flag) return(by_Out);
    KB.Flag = 0;
    
    by_Data = Keyboard_Encoder();
    if(by_Data == KB_NONE)
    {
        w_KBTime = 0;
        by_ReplayCounter = 0;
        return(by_Data);
    }
    
    //*******************************************************************
    if(w_KBTime < C_MAX_TIME)
    {
        ++w_KBTime;
    }
    
    if(w_KBTime == C_TRUE_TIME)
    {
        if(by_Data == KB_CD_VU_VD || by_Data == KB_VD_CU_CD || by_Data == KB_CD_CU || by_Data == KB_TIMEUP_TIMEDOWN) 
        {
            Beep_Proce(1,3,1);
        }
        by_Out = by_Data;
    }
    
    /*********************************连发功能*********************************/  
    if(w_KBTime > C_HOLD_TIME_04S)                                              //0.4s
    {
        if(by_Data != KB_CD_VU_VD && by_Data != KB_VD_CU_CD && by_Data != KB_CD_CU)
        {
            UCHAR by_Replay = C_REPLAY_TIME_MID;
            
//            if(w_KBTime >= C_HOLD_TIME_04S * 4)
//            {
//                by_Replay = C_REPLAY_TIME_FAST;
//            }
            
            if(by_ReplayCounter >= by_Replay)
            {
                by_Out = by_Data | 0x80;                                        //JHT工程师要求，如果是重发按键的话加0x80
                by_ReplayCounter = 0;
            }
            else
            {
                by_ReplayCounter++;
            }
        }
    }
    
    /*******************************按住超过3秒区******************************/
    if(w_KBTime == C_HOLD_TIME_3S)
    {
        if(by_Data == KB_CD_VU_VD || by_Data == KB_VD_CU_CD || by_Data == KB_CD_CU)
        {
            by_Out = by_Data + KB_HOLD_3S;
        }
    }
    
    /****************按住超过30秒,待屏蔽区,需重新启动机台有效******************/
    if(w_KBTime == C_HOLD_TIME_30S)
    {
        aby_KeyMask = by_Data;
    }
    
    return(by_Out);
}

UCHAR Keyboard_Encoder(void)
{   
    UCHAR by_Out = KB_NONE;
    UCHAR by_Count = 0;
    
    memset(aby_KeyPressed, 0, sizeof(aby_KeyPressed));
    
    for(UCHAR i=0; i<C_MEM_SIZE; i++)
    {
        for(UCHAR k=0; k<C_MEM_SIZE; k++)
        {
            if(aby_KeyBuffer[i] & (1 << k) && aby_KeyTable[i][k] != aby_KeyMask)//如果沒有在卡鍵值內
            {
                aby_KeyPressed[by_Count++] = aby_KeyTable[i][k];
                if(by_Count >= C_MAX_PRESS_COUNT) break;
            }
        }
    }
    

    if(Keyboard_Decode(KB_STOP, KB_NONE, KB_NONE))                              //stop键直接輸出
    {
        return KB_STOP;
    }
    
    if(Keyboard_Decode(KB_PAUSE, KB_NONE, KB_NONE))                             //pause键直接輸出
    {
        return KB_PAUSE;
    }
    
    if(by_Count >= C_MAX_PRESS_COUNT)
    {
        by_Out = KB_NONE;
    }
    else
    {
        if(by_Count == 1)
        {
            by_Out = aby_KeyPressed[0];                                         //单个按键直接输出
        }
        else if(Keyboard_Decode(KB_START, KB_NONE, KB_NONE))
        {
            by_Out = KB_START;
        }
        else if(Keyboard_Decode(KB_TV_CD, KB_TV_VU, KB_TV_VD))
        {
            by_Out = KB_CD_VU_VD;
        }
        else if(Keyboard_Decode(KB_TV_VD, KB_TV_CU, KB_TV_CD))
        {
            by_Out = KB_VD_CU_CD;
        }
        else if(Keyboard_Decode(KB_TV_CU, KB_TV_CD, KB_NONE))
        {
            by_Out = KB_CD_CU;
        }
        else
        {
            by_Out = KB_NONE;                                                   //超过两个以上按键无效
        }
    }
    
    return(by_Out);
}

UCHAR Keyboard_Decode(UCHAR by_Key1, UCHAR by_Key2, UCHAR by_Key3)
{
    UCHAR by_Key1Statu = (by_Key1 == KB_NONE) ? TRUE : FALSE;
    UCHAR by_Key2Statu = (by_Key2 == KB_NONE) ? TRUE : FALSE;
    UCHAR by_Key3Statu = (by_Key3 == KB_NONE) ? TRUE : FALSE;
    
    
    for(UCHAR i=0; i<(C_MAX_PRESS_COUNT-1); i++)
    {
        if(aby_KeyPressed[i] == by_Key1) by_Key1Statu = TRUE;
        if(aby_KeyPressed[i] == by_Key2) by_Key2Statu = TRUE;
        if(aby_KeyPressed[i] == by_Key3) by_Key3Statu = TRUE;
    }
    
    return (by_Key1Statu && by_Key2Statu && by_Key3Statu);
}

void Keyboard_1ms_Int(void)
{
    static UCHAR by_Index = 0;
    
    aby_KeyBuffer[by_Index++] = Keyboard_Hw_ReadData();
    
    if(by_Index >= C_MEM_SIZE) by_Index = 0;
    
    Keyboard_Hw_ScanOut(by_Index);                                              //键盘扫描口输出，跟上面的取值是对应的。
}

void Keyboard_Erp(void)
{
    Keyboard_Hw_Erp();
}



