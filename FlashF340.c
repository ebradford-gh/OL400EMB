//-----------------------------------------------------------------------------
// F320_FlashPrimitives.c
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: Flash Writing Functions
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

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------

#include "FlashF340.h"
#include <c8051F340.h>

//-----------------------------------------------------------------------------
// Structures, Unions, Enumerations, and Type Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

// FLASH read/write/erase routines
//void FLASH_Write (FLADDR dest, char *src, unsigned numbytes);
//void FLASH_ByteWrite (FLADDR addr, char byte);
//void FLASH_PageErase (FLADDR addr);

// Example Calls
//	FLASH_PageErase (&FlashTest);
//	FLASH_Write(&FlashTest, &itemp, 2);

//-----------------------------------------------------------------------------
// FLASH_Write
//-----------------------------------------------------------------------------
//
// This routine copies <numbytes> from <src> to the linear FLASH address
// <dest>.
//
/*
void FLASH_Write (FLADDR dest, char *src, unsigned numbytes)
{
   FLADDR xdata i;

   for (i = dest; i < dest+numbytes; i++) 
	 {
      FLASH_ByteWrite (i, *src++);
   }
}
*/

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// This routine writes <byte> to the linear FLASH address <addr>.
//
void FLASH_ByteWrite (FLADDR addr, char byte)
{
	int x;
   bit EA_SAVE = EA;                  // preserve EA
   char xdata * pwrite; 			        // FLASH write pointer
   EA = 0;                            // disable interrupts
   pwrite = (char xdata *) addr;			// point at flash address parameter

	// Enable the VDDMON for F34X
	VDM0CN=0x80;		
	for(x = 0; x < 195; x)	// 0.770µS * 195 = 150µS
		x++;
	RSTSRC |= 0x02;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x01;                      // PSWE = 1 = write enable

   *pwrite = byte;                     // write the byte

	// Disable the VDDMON for F34X
	RSTSRC &= 0xFD;
	VDM0CN=0x00;

   PSCTL = 0;			                     // PSWE = 0
   EA = EA_SAVE;                       // restore interrupts
}

//-----------------------------------------------------------------------------
// FLASH_PageErase
//-----------------------------------------------------------------------------
//
// This routine erases the FLASH page containing the linear FLASH address <addr>.
//
void FLASH_PageErase (FLADDR addr)
{
	int x;
   bit EA_SAVE = EA;                  // preserve EA
   char xdata * pwrite;								// FLASH write pointer
   EA = 0;														// disable interrupts
   pwrite = (char xdata *) addr;			// point at flash address parameter

	// Enable the VDDMON for F34X
	VDM0CN=0x80;		
//	for(x = 0; x < 195; x)	// 0.770µS * 195 = 150µS
	for(x = 0; x < 400; x)	// 0.770µS * 195 = 150µS
		x++;
	RSTSRC |= 0x02;

   FLKEY  = 0xA5;                     // Key Sequence 1
   FLKEY  = 0xF1;                     // Key Sequence 2
   PSCTL |= 0x03;                     // PSWE = 1; PSEE = 1

   *pwrite = 0;                       // initiate page erase

	// Disable the VDDMON for F34X
	RSTSRC &= 0xFD;
	VDM0CN=0x00;

   PSCTL = 0;                     		// PSWE = 0; PSEE = 0
   EA = EA_SAVE;                      // restore interrupts
}
