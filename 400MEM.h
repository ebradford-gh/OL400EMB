//-----------------------------------------------------------------------------
// 400MEM.h
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: Extern Memory definitions
// AUTH: Emmett Bradford
// DATE: 3-17-06
//
//
// Target: C8051F32x
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//
// REVISIONS:
//	1.00	03/17/06	Initial Release
//	2.00	02/14/09	Added OL401, OL410 Support
//	2.40	01/28/16	Added Delay_StopWatch_BuildReport
//
//
/*
; MEMORY MAP:
;	INTERNAL DATA ADDRESS SPACE
; 0000 - 001F		General Purpose Registers (R0-R7)
;	0020 - 002F		Bit Addressable RAM
; 0030 - 007F		Direct and Indirect RAM
; 0080 - 00FF		Indirect-Only Addressable RAM
;
; SFR SPACE
; 0080 - 00FF		Special Function Registers
;	
; EXTERNAL DATA ADDRESS SPACE
; 0000 - 03FF		XRAM (1024 bytes)
; 0400 - 07FF		USB FIFO (1024 bytes)
;
; PROGRAM/DATA ADDRESS SPACE FOR MCU = F321 or F347
; 0000 - 3BFF		FLASH (15872 bytes)
; 3A00 - 3BFF		Cal Factors
; 3C00 - 3FFF		LOCK BYTE page can't be used, C2 programming only
; 
*/
#include "400FVAR.h"               // The flash variables as defined in 400FVAR.c

//External Global Variables
extern unsigned char data InPktRdy;
extern unsigned char data OutPktRdy;
extern unsigned char data Out_Packet[];   // Last packet received from host
extern unsigned char data In_Packet[];   // Next packet to sent to host
//extern unsigned char data Status, OutStatus;
extern unsigned char data OutStatus;

extern char bdata Status;
extern bit LampStat;
extern bit VCMPLStat;
extern bit RawStat;
extern bit UpdateStat;
//extern bit AutoRange;

extern char bdata EventFlags;
extern bit NewADC;
extern bit NewGain;
extern bit LumOvld;
extern bit RngOKN;
extern bit LumUnder;
extern bit Vcmpl;

extern char bdata EventFlg100ms;
extern bit NewADC100;
extern bit NewGain100;
extern bit LumOvld100;
extern bit RngOKN100;
extern bit LumUnder100;
extern bit Vcmpl100;

extern char xdata cBuff[50];
extern char xdata DisplayBuff[50];
//extern char xdata DisplayBuffPtr;
extern char xdata DisplayOff;
extern char xdata DisplayPtr;
extern  int xdata itemp;

extern unsigned char data SPIOUTH,SPIOUTL,SPIINH,SPIINL,SPIIN_ID;
extern unsigned char xdata BounceRate;
extern unsigned char data Bounces,NowKey,NewKey,LastKey,NoKeyCtr;

extern unsigned int xdata LampDAC;
extern unsigned int  data T0Now,T0Last,T0Diff,T1Now,T1Last,T1Diff,TPNow,TPLast,TPDiff,Blinks;
extern unsigned char data T0Ext,T1Ext,TPExt,THTemp,TLTemp;
extern unsigned char data T2Count,T2Count2,OVLDcnt,UNDERcnt,RNGOKNcnt,VCMPLcnt,RngWtCnt;

extern unsigned int xdata LumBuff[100];
extern unsigned int xdata LampAmpsBuff[11];
extern unsigned int xdata LampVoltsBuff[11];
extern unsigned char xdata BuffPtr, LumBuffPtr, LampAmpsBuffPtr, LampVoltsBuffPtr;
extern unsigned char xdata ResponseTime;
extern unsigned char xdata LumUnits, LampUnits;

extern unsigned char xdata Range,NewRange,AutoRange;
extern char xdata LumAmpsExpString[6]; 
extern char xdata LumUserExpString[6]; 
extern char xdata LumAmpsString[20]; 
extern char xdata LumUserString[20]; 
extern char xdata LampAmpsString[16]; 
extern char xdata LampVoltsString[16]; 
extern char xdata LampWattsString[16]; 
extern char xdata ColorTempString[16]; 
extern char xdata LampHourString[16];

extern float xdata ftemp;
extern float xdata LampVolts;
extern float xdata LampAmps;
extern float xdata LumAmps;
extern float xdata LumAmpsZero[9];
extern float xdata LumUser;
extern float xdata LampAmpsTarget;
//extern float code  LampAmpsTargetMax[8];
extern float code  LampAmpsTargetMax;
extern unsigned int xdata LampRawTarget;
extern unsigned int xdata LampRaw;
extern long xdata LampRawDiff;
extern unsigned long xdata LampTimer;
extern float xdata cto[6], CPart[6];

extern unsigned int  xdata StopWatch;

//extern BYTE code *FlashPtr;
extern unsigned int FlashPtr;


