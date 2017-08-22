/******************** (C) COPYRIGHT 2013 ***************************************
* File Name         : main.h
* Author            : FW / Taiwan	--Peter Huang
* Version           : V1.0
* Date              : 03/28/2013
* Description       :

*******************************************************************************/
#ifndef _BootLoader_H_
#define _BootLoader_H_

//====== Defintion Tool chain       ======
//#define __C51__                                     // Keil C51
#define __ICC8051__                                 // IAR 8051

//====== Defintion Project          ======
#define _PROJECT_BF535_

//====== Defintion Firmware Version ======
#define REV_PCB                     1               // 0~9     PCB Ver.B
#define REV_OWNER                   1               // 0~9,A~Z (0: nexcom, 1: OEM/ODM)
#define REV_RELEASE                 0               // 0~9  if REV_BETA is zero for release  (ex. 0010)
#define REV_BETA                    2               // 0~9  if REV_BETA is not zero for test (ex. 0001)

//====== Defintion System option    ======
//#define SoftWaveUART
//#define _Debug_BSL_

#endif  // _BootLoader_H_
