//-----------------------------------------------------------------------------
// FLASHF340.h
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
//	1.00	03/17/09	Initial Release
//
//

#ifndef FLASHF340_H
#define FLASHF340_H
//#define LockByte 0x3DFF
//-----------------------------------------------------------------------------
// Variable names in 457FVAR
//-----------------------------------------------------------------------------
/*extern code const unsigned long SerialNumber;
extern code const unsigned long CalDate;
extern code const unsigned long CalDate2;
extern code const float LumAmpsSlope[9];
extern code const unsigned int LumAmpsOffset[9];
extern code const float LampAmpsSlope;
extern code const unsigned int LampAmpsOffset;
extern code const float LampVoltsSlope;
extern code const unsigned int LampVoltsOffset;
extern code const float LampDACSlope;
extern code const int LampDACOffset;

extern code const float FirmwareRevision;

extern code const CurrPnt[6];
extern code const CTempPnt[6];
extern code const ktoap[6];
extern code const atokp[6];

extern code const float UserCalFactor[4];
extern code const int UserCalExp[4];
extern code const char UserCalName[4][6];

extern code const int FlashTest;
*/
//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

typedef unsigned long ULONG;
//typedef unsigned int UINT;
typedef unsigned char UCHAR;

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//#ifndef FLASH_PAGESIZE
//#define FLASH_PAGESIZE 512
//#endif

//#ifndef FLASH_TEMP
//#define FLASH_TEMP 0x03a00L
//#endif

#ifndef FLASH_START
#define FLASH_START 0x03a00L
//#define FLASH_START 0x07800L
#endif

//#ifndef FLASH_LAST
//#define FLASH_LAST 0x03c00L
//#endif

//typedef UINT FLADDR;
typedef unsigned int FLADDR;

//-----------------------------------------------------------------------------
// Exported Function Prototypes
//-----------------------------------------------------------------------------
// FLASH read/write/erase routines
//void FLASH_Write (FLADDR dest, char *src, unsigned numbytes);
void FLASH_ByteWrite (FLADDR addr, char byte);
void FLASH_PageErase (FLADDR addr);

//// FLASH read/write/erase routines
//extern void FLASH_Write (FLADDR dest, char *src, unsigned numbytes);
//extern void FLASH_ByteWrite (FLADDR addr, char byte);
//extern void FLASH_PageErase (FLADDR addr);

#endif // FLASHF340_H