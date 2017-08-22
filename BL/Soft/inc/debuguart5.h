//*****************************************************************************/*-----------------------------------------------------------------------------
//* Copyright (c) 2012, Nexcom International Co.,Ltd. All Rights Reserved.
//*
//* Filename : HED_UART.H
//* Project  : NEXCOM PROJECT
//* Created  : 27 MAR 2012 / PKC
//* Devices  : LPC1114
//* Toolchain: Keil ARM
//*
//* Description: Header files for core source code
//*
//* $Log: HED_UART.H,v $
//*       27MAR2012-PKC: Initial revision.
//*
//*****************************************************************************

#ifndef __DBG_UART_H__
#define __DBG_UART_H__
//***********************************************************************
//*                             INCLUDE                                 *
//***********************************************************************

//***********************************************************************
//*                             DEFINE                                  *
//***********************************************************************
#define _RX_BUFFER_SIZE_                 15
#define _TX_BUFFER_SIZE_               1024

//***********************************************************************
//*                             REGISTER                                *
//***********************************************************************

extern  u8  DBGUart_TxBuf[_TX_BUFFER_SIZE_ + 1];
extern  u8  DBGUart_RxBuf[_RX_BUFFER_SIZE_ + 1];
extern  u8  DBGUart_RxBufTemp[_RX_BUFFER_SIZE_ + 1];

//***********************************************************************
//*                             DECLARE                                 *
//***********************************************************************
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 Init                            ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void DebugUartInit(void);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~                 DRV                             ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern void vShowBootLoaderInfo(void);
extern void vDBGUartPollTx(void);
extern void vDBGUartSendTx(void);
extern void vDBGUart_Poll(void);
extern void vDBGUartCheckTX(void);
extern void vDBGUartCheckRX(void);
extern void vPutChar(u8 bHEX);
extern void puts(u8 *pbString);
extern void vPutDec(u8 bHex);
extern void vPut2Dec(u16 wHex);
extern void vPut4Dec(u32 dwHex);
extern void vPutHEX(u8 bHEX);
extern void vPut2HEX(u16 wHEX);
extern void vPut4HEX(u32 dwHEX);
extern void vParseCommand(void);
extern void vDBGUart_Rx_Process(void);
extern void vDBGUart_Poll(void);

extern void TxStart(void);

//***********************************************************************
//*                             END                                     *
//***********************************************************************
#endif  // __DBG_UART_H__
