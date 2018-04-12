//-----------------------------------------------------------------------------
// 400FVAR.h
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: Flash Code
// AUTH: Emmett Bradford
// DATE: 3-17-06
//
//
// Target: C8051F32x
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//
// REVISIONS:
//	1.00	03/17/06	Initial Release
//	2.20	12/15/09	Revision change
//	2.21	01/25/10	?
//	2.20	02/25/13	Added LifeTIme Covert Lamp Timer
//	2.40	01/28/16	Added Delay_StopWatch_BuildReport
//	2.41	01/28/18	Fixed Menu length
//
#define C_DOCUMENT 	190
#define FIRMWARE_REVISION 	2.41

//-----------------------------------------------------------------------------
// Variable names in 400FVAR.c
//-----------------------------------------------------------------------------
extern code const unsigned long SerialNumber;
extern code const unsigned long CalDate;
extern code const unsigned long CalDate2;

extern code const float LumAmpsSlope[9];
extern code const unsigned int LumAmpsOffset[9];
//V2.00
//extern code const float LampAmpsSlope;
//extern code const unsigned int LampAmpsOffset;
extern code const float LampDACCurve;
extern code const unsigned int OldLampAmpsOffset;

extern code const float LampDACSlope;
extern code const int LampDACOffset;
extern code const float LampVoltsSlope;
extern code const unsigned int LampVoltsOffset;

extern code const float FirmwareRevision;

extern code const float UserCalFactor[4];
extern code const int UserCalExp[4];
extern code const char UserCalName[4][6];

extern code const float CurrPnt[6];
extern code const float CTempPnt[6];
extern code const float ktoap[6];
extern code const float atokp[6];

extern code const int FlashTest;	//end of flash




