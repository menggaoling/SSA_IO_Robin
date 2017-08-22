#include  "stm32f10x_lib.h"

//##################################################################################
//###                               Project                                      ###
//##################################################################################
#define _PROJECT_BF535_
//#define _DEMO_KIT_
//#define _LCM_SUPPORT_
//#define __C51__                       // Keil C51
#define __ICC8051__                   // IAR 8051

//##################################################################################
//###                               Bit Mask                                     ###
//##################################################################################
// Bit Mask
#define BIT0                        ((u16)0x0001)
#define BIT1                        ((u16)0x0002)
#define BIT2                        ((u16)0x0004)
#define BIT3                        ((u16)0x0008)
#define BIT4                        ((u16)0x0010)
#define BIT5                        ((u16)0x0020)
#define BIT6                        ((u16)0x0040)
#define BIT7                        ((u16)0x0080)
#define BIT8                        ((u16)0x0100)
#define BIT9                        ((u16)0x0200)
#define BIT10                       ((u16)0x0400)
#define BIT11                       ((u16)0x0800)
#define BIT12                       ((u16)0x1000)
#define BIT13                       ((u16)0x2000)
#define BIT14                       ((u16)0x4000)
#define BIT15                       ((u16)0x8000)




//##################################################################################
//###                           ╰参砞﹚篈絏                               ###
//##################################################################################
//#define My_Debug //==>и代刚盎岿ノ

#define C_FAIL                      0
#define C_PASS                      1

#define L_English                   0
#define L_Portugese                 1
#define L_Japanese                  2
#define L_French                    3
//#define L_German                    4
#define L_Italian                   4//5
#define L_Spanish                   5//6
#define L_Dutch                     6//7

#define C_KM                        1
#define C_MILE                      0

#define D_Male                      1
#define D_Female                    0

#define Time_UP                     1
#define Time_DOWN                   0

#define Fan_Close                   0
#define Fan_LSP                     1
#define Fan_MSP                     2
#define Fan_HSP                     3

#define D_Order_ON                  1
#define D_Order_OFF                 0
//##################################################################################
//###                          PROGRAM嘿絏                               ###
//##################################################################################
#define TM_QuickStart               0
#define TM_Manual                   1
#define TM_5K                       2
#define TM_Rolling                  3
#define TM_FatBurn                  4
#define TM_TargetHR                 5
/*Fitness test program*/
#define TM_Gerkin                   6
#define TM_PEB                      7
#define TM_Army                     8
#define TM_Navy                     9
#define TM_Airforce                 10
#define TM_Marine                   11
//##################################################################################
//###                             瑈祘篈絏                                 ###
//##################################################################################
#define M_NOVE                      0xa0
#define M_RESET                     0xa1
#define M_START                     0xa2
#define M_NEXT                      0xa3
#define M_PROGRAM                   0xa4
#define M_ENGINEER                  0xa5
#define M_ReWork                    0xa6
#define M_CSAFE                     0xa7
//##################################################################################
//###                        EEPROM 纗                                   ###
//##################################################################################
#define EE_CheckSum                 0xa55aa5a5
//#define EE_CheckSum               0xa5a9a5a9
#define EE_CHECK                    0
#define EE_MAX_TIME                 1
#define EE_USER_TIME                2
#define EE_WEIGHT                   3
#define EE_LEVEL                    4
#define EE_AGE                      5
#define EE_UNIT                     6
#define EE_LOW_ELEVATION            7
#define EE_MAX_ELEVATION            8
#define EE_ACCUMULATED_TIME         9
#define EE_ACCUMULATED_TIMEA        10
#define EE_ACCUMULATED_DISTANCE     11
#define EE_ACCUMULATED_DISTANCEA    12
#define EE_LANGUAGE                 13
#define EE_ORDER                    14
#define EE_StartSpeed               15
#define EE_DigitalError_1           16
#define EE_DigitalError_2           17
#define EE_DigitalError_3           18
#define EE_DisableError             19
#define EE_MaxSpeed                 20
#define EE_PauseTime                21
#define EE_CHANNEL                  22
#define EE_VOLUME                   23
#define EE_TimerMode                24
#define EE_GENDER                   25
#define EE_OutOfOrder               26
#define EE_RPM_Parameter            27

//#################################################################################
//####################             IO Map                  ########################
//#################################################################################
//====> USB Charger
#define IO_Set_USB_CHARGER_ON()               {GPIO_SetBits  (GPIOD, GPIO_Pin_9);}
#define IO_Set_USB_CHARGER_OFF()              {GPIO_ResetBits(GPIOD, GPIO_Pin_9);}
//=================================================================
//====> BEEP 北竲
//=================================================================
#define DTurnOnBuzzer               {GPIO_SetBits  (GPIOA, GPIO_Pin_4); }
#define DTurnOffBuzzer              {GPIO_ResetBits(GPIOA, GPIO_Pin_4); }

//=================================================================
//====> BURN-IN 北竲
//=================================================================
#ifndef _PROJECT_BF535_             // BF535_R01, BF535burn-in
#define IO_Set_Burn_in()            {GPIO_SetBits  (GPIOA, GPIO_Pin_8); }
#define IO_Clear_Burn_in()          {GPIO_ResetBits(GPIOA, GPIO_Pin_8); }
#endif

//=================================================================
//====> TV TUNER POWER 北竲
//=================================================================
#define IO_Set_TV_TUNER_ON()        {GPIO_SetBits  (GPIOB, GPIO_Pin_6); }
#define IO_Set_TV_TUNER_OFF()       {GPIO_ResetBits(GPIOB, GPIO_Pin_6); }

//=================================================================
//====> Keypad龄北竲
//=================================================================

#define IO_Set_SK1()                {GPIO_SetBits  (GPIOE, GPIO_Pin_7);  }
#define IO_Set_SK2()                {GPIO_SetBits  (GPIOE, GPIO_Pin_8);  }
#define IO_Set_SK3()                {GPIO_SetBits  (GPIOE, GPIO_Pin_9); }
#define IO_Set_SK4()                {GPIO_SetBits  (GPIOE, GPIO_Pin_10); }
#define IO_Set_SK5()                {GPIO_SetBits  (GPIOE, GPIO_Pin_11); }
#define IO_Set_SK6()                {GPIO_SetBits  (GPIOE, GPIO_Pin_12); }
#define IO_Set_SK7()                {GPIO_SetBits  (GPIOE, GPIO_Pin_13); }
#define IO_Set_SK8()                {GPIO_SetBits  (GPIOE, GPIO_Pin_14); }
#define IO_Clear_SK1()              {GPIO_ResetBits(GPIOE, GPIO_Pin_7);  }
#define IO_Clear_SK2()              {GPIO_ResetBits(GPIOE, GPIO_Pin_8);  }
#define IO_Clear_SK3()              {GPIO_ResetBits(GPIOE, GPIO_Pin_9); }
#define IO_Clear_SK4()              {GPIO_ResetBits(GPIOE, GPIO_Pin_10); }
#define IO_Clear_SK5()              {GPIO_ResetBits(GPIOE, GPIO_Pin_11); }
#define IO_Clear_SK6()              {GPIO_ResetBits(GPIOE, GPIO_Pin_12); }
#define IO_Clear_SK7()              {GPIO_ResetBits(GPIOE, GPIO_Pin_13); }
#define IO_Clear_SK8()              {GPIO_ResetBits(GPIOE, GPIO_Pin_14); }
//=================================================================
//====> LCM北竲
//=================================================================
#ifndef _PROJECT_BF535_                 // BF535_R01,パBF535 LCMэノMODULEΑ硓筁UART北LCM
//==>LCD_RS
#define LCD_RS_Lo                   {GPIO_ResetBits(GPIOE, GPIO_Pin_4); }
#define LCD_RS_Hi                   {GPIO_SetBits  (GPIOE, GPIO_Pin_4); }
//==>LCD_RW
#define LCD_RW_Lo                   {GPIO_ResetBits(GPIOE, GPIO_Pin_5); }
#define LCD_RW_Hi                   {GPIO_SetBits  (GPIOE, GPIO_Pin_5); }
//==>LCD_ENTER
#define LCD_E_Lo                    {GPIO_ResetBits(GPIOE, GPIO_Pin_6); }
#define LCD_E_Hi                    {GPIO_SetBits  (GPIOE, GPIO_Pin_6); }
//==>LCD_RST
#define LCD_RESET_Lo                {GPIO_ResetBits(GPIOE, GPIO_Pin_7); }
#define LCD_RESET_Hi                {GPIO_SetBits  (GPIOE, GPIO_Pin_7); }
#else
#define LCM_SW_Lo()                 GPIO_ResetBits(GPIOD, GPIO_Pin_7)
#define LCM_SW_Hi()                 GPIO_SetBits  (GPIOD, GPIO_Pin_7)
#endif

//=================================================================
//====> 北竲
//=================================================================
#define IO_Clear_PAN_LSP()          {GPIO_ResetBits(GPIOA, GPIO_Pin_5); }
#define IO_Set_PAN_LSP()            {GPIO_SetBits  (GPIOA, GPIO_Pin_5); }
#define IO_Clear_PAN_MSP()          {GPIO_ResetBits(GPIOA, GPIO_Pin_6); }
#define IO_Set_PAN_MSP()            {GPIO_SetBits  (GPIOA, GPIO_Pin_6); }
#define IO_Clear_PAN_HSP()          {GPIO_ResetBits(GPIOA, GPIO_Pin_7); }
#define IO_Set_PAN_HSP()            {GPIO_SetBits  (GPIOA, GPIO_Pin_7); }

//=================================================================
//====> 计硄癟 TX/RX よ北竲 (RS485 REEN 北竲)
//=================================================================
#define IO_Set_Digital_CTL()        {GPIO_SetBits  (GPIOD, GPIO_Pin_5); }
#define IO_Clear_Digital_CTL()      {GPIO_ResetBits(GPIOD, GPIO_Pin_5); }

//=================================================================
//====> Voice 北竲
//=================================================================
#define IO_Set_Voice1()             {GPIO_SetBits  (GPIOE, GPIO_Pin_0); }
#define IO_Clear_Voice1()           {GPIO_ResetBits(GPIOE, GPIO_Pin_0); }
#define IO_Set_Voice2()             {GPIO_SetBits  (GPIOE, GPIO_Pin_1); }
#define IO_Clear_Voice2()           {GPIO_ResetBits(GPIOE, GPIO_Pin_1); }
#define IO_Set_Voice3()             {GPIO_SetBits  (GPIOE, GPIO_Pin_2); }
#define IO_Clear_Voice3()           {GPIO_ResetBits(GPIOE, GPIO_Pin_2); }

//=================================================================
//====> RTC 北竲     TW8806  IIC 北竲
//=================================================================
#define IO_Set_HT1381_SCK()         {GPIO_SetBits  (GPIOC, GPIO_Pin_3); }
#define IO_Clear_HT1381_SCK()       {GPIO_ResetBits(GPIOC, GPIO_Pin_3); }
#define IO_Set_HT1381_Data()        {GPIO_SetBits  (GPIOA, GPIO_Pin_0); }
#define IO_Clear_HT1381_Data()      {GPIO_ResetBits(GPIOA, GPIO_Pin_0); }
//#define IO_Set_HT1381_RST()         {GPIO_SetBits  (GPIOA, GPIO_Pin_1); }
//#define IO_Clear_HT1381_RST()       {GPIO_ResetBits(GPIOA, GPIO_Pin_1); }

//=================================================================
//====> 本EEPROM北竲
//=================================================================
//#define IO_Clear_EEPROM_CS()      {GPIO_ResetBits(GPIOC, GPIO_Pin_0);  }
//#define IO_Set_EEPROM_CS()        {GPIO_SetBits  (GPIOC, GPIO_Pin_0);  }
//==>SK竲籔HT1381_SCK()ノ
//#define IO_Clear_EEPROM_SK()      {GPIO_ResetBits(GPIOD, GPIO_Pin_14); }
//#define IO_Set_EEPROM_SK()        {GPIO_SetBits  (GPIOD, GPIO_Pin_14); }
//#define IO_Clear_EEPROM_DI()      {GPIO_ResetBits(GPIOC, GPIO_Pin_1);  }
//#define IO_Set_EEPROM_DI()        {GPIO_SetBits  (GPIOC, GPIO_Pin_1);  }

//=================================================================
//====> IPOD 北竲
//=================================================================
#ifndef _PROJECT_BF535_
#define IO_Set_IPOD_POWER_OFF()     {GPIO_SetBits  (GPIOB, GPIO_Pin_7); }
#define IO_Clear_IPOD_POWER_OFF()   {GPIO_ResetBits(GPIOB, GPIO_Pin_7); }
#define IO_Set_IPOD_RESET_CP()      {GPIO_SetBits  (GPIOC, GPIO_Pin_6); }
#define IO_Clear_IPOD_RESET_CP()    {GPIO_ResetBits(GPIOC, GPIO_Pin_6); }
#define IO_Set_IPOD_LED()           {GPIO_SetBits  (GPIOC, GPIO_Pin_7); }
#define IO_Clear_IPOD_LED()         {GPIO_ResetBits(GPIOC, GPIO_Pin_7); }
#endif

//=================================================================
//====> LEDボ縊北竲		
//      Ver.07  穎皌 E-LEAD layout
//              э﹚竡; RED ==> PC8эPC9 ;    Green ==> PC9эPC8    //  101007
//      Ver.09  101013 穦某い矗璶э LAYOUT ﹚竡  祘Α IO ﹚竡惠璶эㄓ
//              э﹚竡; RED ==> PC9эPC8 ;    Green ==> PC8эPC9    //  101014 //
//=================================================================
#define IO_Set_RED()                {GPIO_SetBits  (GPIOC, GPIO_Pin_8); }
#define IO_Clear_RED()              {GPIO_ResetBits(GPIOC, GPIO_Pin_8); }
#define IO_Set_GREEN()              {GPIO_SetBits  (GPIOC, GPIO_Pin_9); }
#define IO_Clear_GREEN()            {GPIO_ResetBits(GPIOC, GPIO_Pin_9); }

//=================================================================
//====> ARM_INT北竲
//=================================================================
#define DInterruptArmReqL()           {GPIO_ResetBits(GPIOD, GPIO_Pin_3); }
#define DInterruptArmReqH()           {GPIO_SetBits  (GPIOD, GPIO_Pin_3); }
#define DResetArm_Active()            {GPIO_ResetBits(GPIOD, GPIO_Pin_4); }
#define DResetArm_DeActive()          {GPIO_SetBits  (GPIOD, GPIO_Pin_4); }

//=================================================================
//====> HPLINE北竲
//=================================================================
#ifndef _PROJECT_BF535_
#define IO_Set_HPLINE()             {GPIO_SetBits  (GPIOD, GPIO_Pin_6); }
#define IO_Clear_HPLINE()           {GPIO_ResetBits(GPIOD, GPIO_Pin_6); }
#endif

//=================================================================
//====> RF北竲
//=================================================================
#ifdef _PROJECT_BF535_
#define Read_RF_Reset_is_Hi()       (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1 ))
#define IO_Set_RF_CE_Hi()           GPIO_SetBits  (GPIOD, GPIO_Pin_0)
#define IO_Set_RF_CE_Low()          GPIO_ResetBits(GPIOD, GPIO_Pin_0)
#define IO_Set_RF_Reset_Hi()        GPIO_SetBits  (GPIOD, GPIO_Pin_1)
#define IO_Set_RF_Reset_Low()       GPIO_ResetBits(GPIOD, GPIO_Pin_1)
#endif

//=================================================================
//====> EuPS北竲
//=================================================================
#ifndef _PROJECT_BF535_             // BF535_R01,パBF535 LCMэノMODULEΑ硓筁UART北LCM
#define IO_Set_TV_VA_Power()        {GPIO_SetBits  (GPIOA, GPIO_Pin_1); }
#define IO_Clear_TV_VA_Power()      {GPIO_ResetBits(GPIOA, GPIO_Pin_1); }
#else
//#define IO_Set_TV_VA_Power()        {GPIO_ResetBits(GPIOA, GPIO_Pin_1); }
//#define IO_Clear_TV_VA_Power()      {GPIO_SetBits  (GPIOA, GPIO_Pin_1); }
#define IO_Set_PMIC_High()          {GPIO_SetBits  (GPIOA, GPIO_Pin_1); }
#define IO_Set_PMIC_Low()           {GPIO_ResetBits(GPIOA, GPIO_Pin_1); }
#endif
#define IO_Set_System_Power()       {GPIO_SetBits  (GPIOB, GPIO_Pin_2); }
#define IO_Clear_System_Power()     {GPIO_ResetBits(GPIOB, GPIO_Pin_2); }

#ifdef _PROJECT_BF535_
//=================================================================
//====> 匡拒竲
//=================================================================
#define READ_HAERT_OPTION()         GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)
#define READ_OTHER_OPTION()         GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)

//=================================================================
//====> 筿方北竲
//=================================================================
/*For Maxheadroom power port  V4*/
#define POWER_UI_PORT_EN()          GPIO_SetBits  (GPIOB, GPIO_Pin_2 )	//PORTB.2
#define POWER_UI_PORT_DIS()         GPIO_ResetBits  (GPIOB, GPIO_Pin_2 )//PORTB.2

#define POWER_ERP_5V_PORT_EN()      GPIO_SetBits  (GPIOE, GPIO_Pin_4 )	//PORTE.4
#define POWER_ERP_5V_PORT_DIS()     GPIO_ResetBits  (GPIOE, GPIO_Pin_4 )//PORTE.4

#define POWER_9V_PORT_EN()	    GPIO_SetBits  (GPIOD, GPIO_Pin_6 )	//PORTD.6
#define POWER_9V_PORT_DIS()         GPIO_ResetBits  (GPIOD, GPIO_Pin_6 )//PORTD.6

#define POWER_USB_PORT_EN()	    GPIO_SetBits  (GPIOB, GPIO_Pin_0 )	//PORTB.0
#define POWER_USB_PORT_DIS()        GPIO_ResetBits  (GPIOB, GPIO_Pin_0 )//PORTB.0
                                    
#define POWER_USB1_PORT_EN()        GPIO_SetBits  (GPIOD, GPIO_Pin_7 )	//PORTD.7
#define POWER_USB1_PORT_DIS()       GPIO_ResetBits  (GPIOD, GPIO_Pin_7 )//PORTD.7

#define POWER_USB2_PORT_EN()        GPIO_SetBits  (GPIOD, GPIO_Pin_1 )	//PORTD.1
#define POWER_USB2_PORT_DIS()       GPIO_ResetBits  (GPIOD, GPIO_Pin_1 )//PORTD.1
                                    
#define POWER_USB3_PORT_EN()        GPIO_SetBits  (GPIOD, GPIO_Pin_0 )	//PORTD.0
#define POWER_USB3_PORT_DIS()       GPIO_ResetBits  (GPIOD, GPIO_Pin_0 )//PORTD.0
									
#define POWER_HR_PORT_EN()	    GPIO_SetBits  (GPIOB, GPIO_Pin_9 )	//PORTB.9
#define POWER_HR_PORT_DIS()         GPIO_ResetBits  (GPIOB, GPIO_Pin_9 )//PORTB.9
                                    
#define POWER_TV_PORT_EN()          GPIO_SetBits  (GPIOB, GPIO_Pin_6 )	//PORTB.6
#define POWER_TV_PORT_DIS()         GPIO_ResetBits  (GPIOB, GPIO_Pin_6 )//PORTB.6

#define POWER_BT_PORT_EN()	    GPIO_SetBits  (GPIOD, GPIO_Pin_15 )	 //PORTD.15
#define POWER_BT_PORT_DIS()         GPIO_ResetBits  (GPIOD, GPIO_Pin_15 )//PORTD.15
 

/*For Generation power port*/
#define IO_Set_9V_EN()              GPIO_SetBits  (GPIOA, GPIO_Pin_8 )
#define IO_Clear_9V_EN()            GPIO_ResetBits(GPIOA, GPIO_Pin_8 )
#define IO_Set_5V_EN()              GPIO_SetBits  (GPIOE, GPIO_Pin_15)      //Changed pin from PB1 to PE15
#define IO_Clear_5V_EN()            GPIO_ResetBits(GPIOE, GPIO_Pin_15)      //Changed pin from PB1 to PE15
#define IO_Set_3V3_IO_EN()          GPIO_SetBits  (GPIOC, GPIO_Pin_6 )
#define IO_Clear_3V3_IO_EN()        GPIO_ResetBits(GPIOC, GPIO_Pin_6 )
#define IO_Set_DDR3_VTT_EN()        GPIO_SetBits  (GPIOC, GPIO_Pin_13)
#define IO_Clear_DDR3_VTT_EN()      GPIO_ResetBits(GPIOC, GPIO_Pin_13)
#define READ_1V8_POK_STATUS()       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7)

#define IO_Set_HR_EN()              GPIO_SetBits  (GPIOB, GPIO_Pin_9)      //Added Heart Beat power enable pin
#define IO_Clear_HR_EN()            GPIO_ResetBits(GPIOB, GPIO_Pin_9)      //Added Heart Beat power enable pin

#define IO_Set_C_SAFE_EN()          GPIO_SetBits  (GPIOD, GPIO_Pin_6)       //Added C-SAFE Enable pin
#define IO_Clear_C_SAFE_EN()        GPIO_ResetBits(GPIOD, GPIO_Pin_6)       //Added C-SAFE Enable pin

#define IO_Set_E_STOP_EN()          GPIO_SetBits  (GPIOE, GPIO_Pin_4)       //Added SAFE Enable pin
#define IO_Clear_E_STOP_EN()        GPIO_ResetBits(GPIOE, GPIO_Pin_4)       //Added SAFE Enable pin

#define IO_Set_Soc_Wakeup()         {GPIO_SetBits  (GPIOA, GPIO_Pin_11); }
#define IO_Set_Soc_Sleep()          {GPIO_ResetBits(GPIOA, GPIO_Pin_11); }

#define Read_PWRSTB_is_High()       GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)
#define Read_PWRSTB_is_Low()        (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12))

#define READ_MCU_OFF()              GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5 )


//=================================================================
//====>   DEBUG LED北竲       // 2012/11/13
//=================================================================
//#define IO_Set_DEBUG_LED1_ON()      GPIO_ResetBits(GPIOC, GPIO_Pin_14)        //Cancel DEBUG_LED1 pin
//#define IO_Set_DEBUG_LED1_OFF()     GPIO_SetBits  (GPIOC, GPIO_Pin_14)        //Cancel DEBUG_LED1 pin
//#define IO_Set_DEBUG_LED2_ON()      GPIO_ResetBits(GPIOC, GPIO_Pin_15)        //Cancel DEBUG_LED2 pin
//#define IO_Set_DEBUG_LED2_OFF()     GPIO_SetBits  (GPIOC, GPIO_Pin_15)        //Cancel DEBUG_LED2 pin
//#define IO_Set_DEBUG_LED3_ON()      GPIO_ResetBits(GPIOE, GPIO_Pin_15)        //Cancel DEBUG_LED3 pin
//#define IO_Set_DEBUG_LED3_OFF()     GPIO_SetBits  (GPIOE, GPIO_Pin_15)        //Cancel DEBUG_LED3 pin
#define IO_Set_DEBUG_LED4_ON()      GPIO_ResetBits(GPIOB, GPIO_Pin_9 )
#define IO_Set_DEBUG_LED4_OFF()     GPIO_SetBits  (GPIOB, GPIO_Pin_9 )
#endif

/****************************** END OF FILE *********************************/

