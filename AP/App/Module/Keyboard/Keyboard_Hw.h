#ifndef __KEYBOARD_HW_H
#define __KEYBOARD_HW_H

#include "includes.h"


void Keyboard_HW_Initial(void);
void Keyboard_Hw_ScanOut(UCHAR by_Index);
UCHAR Keyboard_Hw_ReadData(void);
void Keyboard_Hw_Erp(void);


#endif

