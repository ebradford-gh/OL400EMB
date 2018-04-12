//-----------------------------------------------------------------------------
// 410DISP.c
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: Display Functions
// AUTH: Emmett Bradford
// DATE: 3-17-06
//
//
// Target: C8051F32x
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//
// REVISIONS:
//	1.00	03/17/09	Initial Release including pwrdwn and V2.00 PLD
//	1.10	03/17/13	Including support for OLED w/ LCD option using J6-4 = SPIIN_ID(7)
//	2.40	01/28/16	Added Delay_StopWatch_BuildReport
//
//

// Includes
#include "c8051f340.h"
#include "400PCB.h"            
#include "400MEM.h"


//-----------------------------------------------------------------------------
// LCD Display Subroutines
//-----------------------------------------------------------------------------


//-------------------------
// Display
// Write a string to a position of the DisplayBuff
//-------------------------
void Display (char DisplayPosition, char *string)
{
  int i=0;
  while(string[i])
  {
    DisplayBuff[DisplayPosition]=string[i];
    DisplayPosition++;
    i++;
  }
}


//-------------------------
// ClearDisplayBuff
// Fill Display Buff with spaces
//-------------------------
void ClearDisplayBuff (void)
{
	int i;
	for(i=0; i<40; i++)
	{
		DisplayBuff[i]=' ';
	}
}



//-------------------------
// Delay
//-------------------------
void Delay (unsigned int time)
{
	int x;
	while(time>0)
	{
		for(x = 0;x < 1298;x)	// 0.770µS * 1298 = 1mS
			x++;
		time--;
	}
}

//-------------------------
// Delay_StopWatch_BuildReport
//-------------------------
void Delay_StopWatch_BuildReport (unsigned int time)
{
	StopWatch=time;
	while(StopWatch>0)
	{
		BuildReport();			// Build an input report
	}
}

//-------------------------
// Display_Init
//-------------------------
// Display Initialization
// - Configure the Optrex LCD Display.
//
void Display_Init (void)
{  
	int i=0;
	char j=0;

	Delay(100);		//100ms
	ClearDisplayBuff();				//Clear DisplayBuffer!!!

//	j='A';
//	for(i=0; i<40; i++)
//	{
//		DisplayBuff[i]=j;
//		j++;
//	}

	Display_Keypad_Transfer(0x30, 0, 1);	// 8 bit interface
	Delay(10);
	Display_Keypad_Transfer(0x30, 0, 1);	// 8 bit interface
	Delay(1);
	Display_Keypad_Transfer(0x30, 0, 1);	// 8 bit interface
	Delay(1);
	Display_Keypad_Transfer(0x38, 0, 1);	// 8 bit interface, 2 lines, 5x7 dots
	Delay(1);
	Display_Keypad_Transfer(0x08, 0, 1);	// Display, cursor, blinking off
	Delay(1);
	Display_Keypad_Transfer(0x0C, 0, 1);	// Display on, cursor off, blinking off
	Delay(1);
	Display_Keypad_Transfer(0x06, 0, 1);	// Set entry mode to increment cursor
	Delay(1);
}


//-------------------------
// Display_Keypad_Update
// Write a character from the DisplayBuff to the display
// This is done continually every 2ms
// THE BUSY FLAG IS ASSUMED TO BE CLEAR, SINCE AT LEAST 1mS HAS ELAPSED SINCE THE LAST DISPLAY OPERATION
//-------------------------

void Display_Keypad_Update (void)
{
	if(!DisplayOff)
	{
		if(DisplayPtr>=40)
		{
			DisplayPtr=0;
			Display_Keypad_Transfer(0x02, SPIOUTH, 1);
		}
		else if(DisplayPtr==20)
		{
			DisplayPtr=-1;
			if(SPIIN_ID & 0x80)		// if J6-4 is not shorted
				Display_Keypad_Transfer(0xC0, SPIOUTH, 1); // for Newhaven OLED		
			else
				Display_Keypad_Transfer(0xA8, SPIOUTH, 1);	// for Optrex LCD
		}
		else if(DisplayPtr<0)
		{
			DisplayPtr=20;
			Display_Keypad_Transfer(DisplayBuff[DisplayPtr++], SPIOUTH, 0);
		}
		else
			Display_Keypad_Transfer(DisplayBuff[DisplayPtr++], SPIOUTH, 0);
	}

	else	// if DisplayOff
	{
		if(DisplayOff==1)
			Display_Keypad_Transfer(0x08, SPIOUTH, 1);
		else
		{
			Display_Keypad_Transfer(0x0C, SPIOUTH, 1);
			DisplayOff=0;
		}
//		Delay(1);
//		DisplayPtr=40;
	}

	// Debounce the Key Presses
	if(SPIINL!=0)
	{	// A key is pressed

		if(NowKey==SPIINL)
		{	// The key is the same as last time
			if(Bounces>0)
			{
//				if(--Bounces==0)
				if(--Bounces==0 && NoKeyCtr>50)
				{
					NewKey=NowKey;
					NoKeyCtr=0;
					if(DisplayOff==1) DisplayOff++; // Cause the Display to be turned back on
				}
			}
		}
		else if(NewKey==0)
		{	// The key is different from last time
			NowKey=SPIINL;
			Bounces=10;
			BounceRate=250;
		}
	}

	else
	{	// No Key is pressed
		NowKey=0;
		Bounces=0;
		BounceRate=250;
		if(NoKeyCtr<255)	NoKeyCtr++;	
	}

	// Pass the event flags
	EventFlg100ms |= SPIINH;
}


//-------------------------
// Transfer the display&GPO data and retrieve the Keypad & LimSW data
//-------------------------
// - Write the next character to the LCD Display.
//

void Display_Keypad_Transfer (char Display, char Data, char Command)
{
	if(!Command)
		Data |= 0x20;		// DDRS is b5
	else
		Data &= ~0x20;		// DDRS is b5
	SDEN0=0;				// Enable

//V2.00 !!
//	SPI0DAT = Data;
//	SPIF =0;				// Necessary!
//	while(!SPIF) {}			// Wait for done	
//	SPIINH=SPI0DAT;			// Get the event-latched LimSW data
//	
//	SPI0DAT = Display;
//	SPIF =0;				// Necessary!
//	while(!SPIF) {}			// Wait for done		
//	SPIINL=SPI0DAT;			// Get the event-latched keypad data

	SPI0DAT = 0;
	SPIF =0;				// Necessary!
	while(!SPIF) {}			// Wait for done	
	SPIINH=SPI0DAT;			// Get the event-latched LimSW data
	
	SPI0DAT = Data;
	SPIF =0;				// Necessary!
	while(!SPIF) {}			// Wait for done		
	SPIINL=SPI0DAT;			// Get the event-latched keypad data

	SPI0DAT = Display;
	SPIF =0;				// Necessary!
	while(!SPIF) {}			// Wait for done		
	SPIIN_ID=SPI0DAT;		// Get the event-latched keypad data

	SDEN0=1;				// Disable
	SPI0DAT = 0;			// Dummy to make DDE toggle
	SPIF =0;				// Necessary!
}



//-------------------------
// Debounce the Key Presses at a faster rate while holding the key down
// High BounceRate = slower
// High BounceAccel = slower
//-------------------------
//
void Keypad_Rapidfire (unsigned int Limit)
{
unsigned int itemp;
#define BounceAccel 240

	NewKey=0;
	itemp=((unsigned int)BounceRate*BounceAccel)>>8;
	if(itemp< Limit/2)	itemp=Limit/2;		// Limit to 1/(Limit(ms))
	BounceRate= (unsigned char)itemp;
	Bounces=BounceRate;

	NoKeyCtr=255;
}



//-------------------------
// Write the preamp setting
//-------------------------

void Write_Preamp (char Data)
{
	while(!SPIF) {}			// Wait for done (keypad dummy might be transmitting)

	SPI0DAT = Data;
	SPIF =0;						// Necessary!
	while(!SPIF) {}			// Wait for done	
	
	SDEN1=1;						// Latch Enable
	SDEN1=0;						// Disable
}

//-------------------------
// Write the lamp current DAC setting
//-------------------------

void Write_LampDAC (unsigned int Data)
{
	SPI0DAT = (unsigned char)(Data>>8);
	SPIF =0;						// Necessary!
	while(!SPIF) {}			// Wait for done	
	
	SDEN2=0;						// Latch Setup DAC7731

	SPI0DAT = (unsigned char)(Data & 0x00FF);
	SPIF =0;						// Necessary!
	while(!SPIF) {}			// Wait for done	

	SDEN2=1;						// Latch Active Edge DAC7731
}
