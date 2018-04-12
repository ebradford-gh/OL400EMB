//-----------------------------------------------------------------------------
// 400FVAR.c
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: Flash Variable Declaration Code
// AUTH: Emmett Bradford
// DATE: 3-17-06
//
//
// Target: C8051F32x
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//
// REVISIONS:
//	1.00	03/17/06	Initial Release
//
// This file contains the declaration for the non-volatile calibration data
// and other data stored in FLASH memory.
// It is in a separate file in order to auto-generate a section name 
// that can be physically located by a  linker directive
// These variables are initialized in code when downloaded from the C2 interface.
// This means that a firmware update will overwrite cal data
// Calibration Software needs to keep contents on file and upload/download

#include "400MEM.h"

code const unsigned long SerialNumber=1;
code const unsigned long CalDate=100;
code const unsigned long CalDate2=0;

code const float LumAmpsSlope[9] = 
{0.0000622, 0.0000622, 0.0000622, 0.0000622, 0.0000622, 0.0000622, 0.0000622, 0.0000622, 0.0000622};

code const unsigned int LumAmpsOffset[9] = {5279, 5272, 5280, 5273, 5279, 5273, 5280, 5289, 0};

//V2.00 code const float LampAmpsSlope =0.000215;
code const float LampDACCurve =0.000;
code const unsigned int OldLampAmpsOffset = 4885;

code const float LampDACSlope =0.000104;
code const int LampDACOffset = 500;

code const float LampVoltsSlope =0.0008307;
code const unsigned int LampVoltsOffset =4985;

//code const float FirmwareRevision =2.010;
code const float FirmwareRevision = FIRMWARE_REVISION;

code const float UserCalFactor[4] = {1.0,3.426,0,0};
code const int UserCalExp[4] = {8,8,0,0};
code const char UserCalName[4][6] = {"fL   ","cd/m2", 0xff,0xff,0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff,0xff,0xff};


code const float CurrPnt[6]  = {0,0,0,0,0,0};
code const float CTempPnt[6] = {0,0,0,0,0,0};
code const float ktoap[6] = {0,0,0,0,0,0};
code const float atokp[6] = {0,0,0,0,0,0};

/*
code const float CurrPnt[6]  = {4.0, 4.5, 5.0, 5.5, 6.0, 6.5};
code const float CTempPnt[6] = {2000,2200,2400,2600,2800,3000};
code const float ktoap[6] = {-1.042E-13, -5.859E-13, -1.302E-12, -1.432E-12, -7.813E-13, -1.693E-13};
code const float atokp[6] = {-533.333, -2933.333, -6400.000, -6933.333, -3733.333, -800.000};
*/

code const int FlashTest = 0x7fff;		//end of flash

