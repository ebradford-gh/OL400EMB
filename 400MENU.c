//-----------------------------------------------------------------------------
// 400MENU.c
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: Menu Code
// AUTH: Emmett Bradford
// DATE: 3-17-06
//
//
// Target: C8051F32x
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//
// REVISIONS:
//	1.00	03/17/06	Initial Release
//	2.41	01/28/18	Fixed Menu length
//
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "c8051f340.h"
#include "stddef.h"
#include "stdio.h"
#include "400PCB.h"
#include "400MEM.h"
#include "FlashF340.h"               



//-----------------------------------------------------------------------------
// String Constants
//-----------------------------------------------------------------------------
code const char LUMUNITA[] 					=	"A    ";
code const char GAINRANGE[] 				=	"GAIN RANGE = ";
code const char RESPTIME[]			    =	"RESPONSE TIME =   S";
code const char MENUOPTION[] 				=	"MENU OPTION";

code const char SETRESPTIME[]				= " SET RESPONSE TIME  ";
code const char SELECTMONUNITS[]		=	"SELECT MONITOR UNITS";
code const char SELECTLMPUNITS[]		=	" SELECT LAMP UNITS  ";
code const char VIEWLAMPTIME[] 			=	"  VIEW LAMP TIMER   ";
code const char TURNDISPLAYOFF[] 		=	"  TURN DISPLAY OFF  ";
code const char SHOWDEVICEINFO_[]		=	"  SHOW DEVICE INFO  ";
code const char USBMMONITOR[] 		  =	"  USB COMM MONITOR  ";
code const char VIEWCTEMPCOEFF[] 	  =	"VIEW CTEMP CAL COEFF";
//code const char SETLAMPTIME[] 			=	"   SET LAMP TIMER   ";

// The flash of the original F320 MCU is too small to include SHOWDEVICEINFO_ !!!!
//#define	MENULENGTH	6
//code const char *OptionTitle[MENULENGTH] = {SETRESPTIME, SELECTMONUNITS, SELECTLMPUNITS, VIEWLAMPTIME, TURNDISPLAYOFF, SHOWDEVICEINFO_};
#define	MENULENGTH	5
code const char *OptionTitle[MENULENGTH] = {SETRESPTIME, SELECTMONUNITS, SELECTLMPUNITS, VIEWLAMPTIME, TURNDISPLAYOFF};
//#define	MENULENGTH	8
//code const char *OptionTitle[MENULENGTH] = {SETRESPTIME, SELECTMONUNITS, SELECTLMPUNITS, VIEWLAMPTIME, TURNDISPLAYOFF, SHOWDEVICEINFO_, USBMMONITOR, VIEWCTEMPCOEFF};







//-------------------------
// Check and Parse the Keypad
//-------------------------
void Check_Keypad (void)
{
//#define	ESCKEY			0x20;
//#define	LAMPONKEY 	0x40;
//#define	MENUKEY 		0x80;
//#define	GAINKEY 		0x08;
//#define	ZEROKEY 		0x10;
//#define	CURRKEY 		0x01;
//#define	LAMPOFFKEY	0x02;
//#define	ENTERKEY 		0x04;

	if(!NewKey)
		return;

/*// TEST KEYPAD
	sprintf(cBuff,"%02X",(int)NewKey);
	Display(0, cBuff);
	Delay(500);
	Display(0, "   ");
	NewKey=0;
	return;
*/

	if(NewKey==LAMPONKEY)
		LampStat=1;

	else if(NewKey==MENUKEY)
		Menu();

	else if(NewKey==GAINKEY)
		SelectGain();

	else if(NewKey==ZEROKEY)
		ZeroLum();

	else if(NewKey==CURRKEY)
		AdjustLampTarget();

	else if(NewKey==LAMPOFFKEY)
		LampStat=0;

	NewKey=0;
}



//-------------------------
// Menu options
//-------------------------
void Menu (void)
{
int xdata Option=1;
int xdata i =1;

/*TEST!!!!*/
//char xdata ctemp;
//int xdata itemp;
//code const BYTE LockByte _at_ 0x7FFF;
//char code *FlashTestPtr;
//FLADDR FlashAddr;
//char code *FlashReadPtr;


	NewKey=0;
	ClearDisplayBuff();
	Display(3,MENUOPTION);
	sprintf(cBuff,"%2d",Option);
	Display(15,cBuff);
	Display(20,OptionTitle[Option-1]);
	
	while(i)
	{
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		if(NewKey)
		{
			if(NewKey==UPKEY)
			{
				if(Option<MENULENGTH)
					Option++;
				else
					Option=1;
			}
	
			else if(NewKey==DOWNKEY)
			{
				if(Option>1)
					Option--;
				else
					Option=MENULENGTH;
			}

			else if(NewKey==ESCKEY)
			{
				i=0;
			}

			else if(NewKey==ENTERKEY)
			{
				if(Option==1)
					SetResponseTime();
				else if(Option==2)
					SelectMonitorUnits();
				else if(Option==3)
					SelectLampUnits();
				else if(Option==4)
					ViewLampTimer();
				else if(Option==5)
				{

//TEST!!!!!!
					if(!DisplayOff)
						DisplayOff=1;			// Cause the display to be turned off until next key
					NewKey=0;

/*
FlashAddr = FLASH_START + 0x0215;
sprintf(cBuff,"FLASH ADDR %04X     ", FlashAddr);
Display(0,cBuff);
Delay(1000);


FlashReadPtr = (char code *)FlashAddr;
sprintf(cBuff,"FLASHREADPTR %04X   ", (int)FlashReadPtr);
Display(0,cBuff);
Delay(1000);


ctemp=*FlashReadPtr;
//itemp=(int)ctemp;
sprintf(cBuff,"FLASH VALUE IN %04X ", (int)ctemp);
Display(0,cBuff);
Delay(1000);

ctemp++;
//itemp=(int)ctemp;
sprintf(cBuff,"FLASH VALUE NEW %04X", (int)ctemp);
Display(0,cBuff);
FLASH_ByteWrite(FlashAddr, ctemp);
Delay(1000);

ctemp=*FlashReadPtr;
//itemp=(int)ctemp;
sprintf(cBuff,"FLASH VALUE NOW %04X", (int)ctemp);
Display(0,cBuff);
Delay(1000);
*/

				}


// The flash of the original F320 MCU is too small to include SHOWDEVICEINFO_ !!!!
//				else if(Option==6)
//					ShowDeviceInfo();
//TEST!!!!!!
//				else if(Option==7)
//					COMMMonitor();
//				else if(Option==8)
//					ViewCTempCal();

				ClearDisplayBuff();
				Display(3,MENUOPTION);
			}

		sprintf(cBuff,"%2d",Option);
		Display(15,cBuff);
		Display(20,OptionTitle[Option-1]);
		NewKey=0;
		}

	}
	ClearDisplayBuff();
}

/*
// The flash of the original F320 MCU is too small to include SHOWDEVICEINFO_ !!!!
//-------------------------
// ShowDeviceInfo - Display the firmware C#, version, serial number, cal date
//-------------------------
void ShowDeviceInfo (void)
{
unsigned char xdata i=1;
unsigned char LeapYear=0;
unsigned long CalMonth, CalDay, CalYear, LeapDays;
float CalDateTemp;

	NewKey=0;
	ClearDisplayBuff();

	sprintf(cBuff,"   OL 400C V%4.2f    ", FIRMWARE_REVISION);
	Display(0, cBuff);
	sprintf(cBuff,"CALIBRATION STANDARD");
	Display(20, cBuff);

	while(i)
	{
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		if(NewKey)
		{
			if(NewKey==ESCKEY)
				i=0;

			else if(NewKey==UPKEY)
			{
				i++;
				if(i>3)
					i=1;
			}
			else if(NewKey==DOWNKEY)
			{
				i--;
				if(i<1)
					i=3;
			}

			if(i>3)
				i=0;
			NewKey=0;


			if(i==1)
			{
				sprintf(cBuff,"   OL 400C V%4.2f    ", FIRMWARE_REVISION);
				Display(0, cBuff);
				sprintf(cBuff,"CALIBRATION STANDARD");
				Display(20, cBuff);
			}

			else if(i==2)
			{
				if(CalDate < 146874882)
				{
					CalYear=0;
					CalMonth=0;
					CalDay=0;
				}
				else
				{
					CalDateTemp = (float)(CalDate-146874882+1);	// "gTicks" since 2000 (lower long insignificant) (plus a little offset)
//TEST
// 			if(i==2) CalDateTemp= (float)(0x08C12202-146874882+1);	//1/1/2000
// else if(i==3) CalDateTemp= (float)(0x08C122CB-146874882+1);	//1/2/2000
// else if(i==4) CalDateTemp= (float)(0x08CC5955-146874882+1);	//1/2/2010
// else if(i==5) CalDateTemp= (float)(0x08CCCFFD-146874882+1);	//6/2/2010
// else if(i==6) CalDateTemp= (float)(0x08D78E4D-146874882+1);	//1/1/2020
// else if(i==7) CalDateTemp= (float)(0x08D7BD73-146874882+1);	//3/1/2020
// else if(i==8) CalDateTemp= (float)(0x08D7D5CF-146874882+1);	//4/1/2020
// else if(i==9) CalDateTemp= (float)(0x08D7ED62-146874882+1);	//5/1/2020
// else if(i==10) CalDateTemp= (float)(0x08D805BE-146874882+1);	//6/1/2020
// else if(i==11) CalDateTemp= (float)(0x08D81D51-146874882+1);	//7/1/2020	
// else if(i==12) CalDateTemp= (float)(0x08D835AD-146874882+1);	//8/1/2020


					CalDateTemp /= 201.165676116943;		// days since 2000
					CalDay = (long)(CalDateTemp);				// days since 2000

					LeapDays = CalDay/1461;							// leap days prior to this year since 2000
	
					if (CalDay % 1461 <366)
						LeapYear=1;
					else
					{
						LeapYear=0;
						LeapDays++;
					}

					CalYear = (CalDay-LeapDays) / 365;	// Full Years since 1/1/2000

					CalDay -= LeapDays;									// Regular days since 2000
					CalDay -= CalYear*365;							// Days since New Year

					if ((LeapYear==0) && (CalDay>59))		// If not leap year and after February
						CalDay++;
					CalDay++;														// Why??

					CalYear += 2000;										// Final Displayed Calendar Year



					if (CalDay > (31+29+31+30+31+30+31+31+30+31+30))
					{
						CalMonth=12;
						CalDay -= (31+29+31+30+31+30+31+31+30+31+30);
					}
					else if (CalDay > (31+29+31+30+31+30+31+31+30+31))
					{
						CalMonth=11;
						CalDay -= (31+29+31+30+31+30+31+31+30+31);
					}
					else if (CalDay > (31+29+31+30+31+30+31+31+30))
					{
						CalMonth=10;
						CalDay -= (31+29+31+30+31+30+31+31+30);
					}
					else if (CalDay > (31+29+31+30+31+30+31+31))
					{
						CalMonth=9;
						CalDay -= (31+29+31+30+31+30+31+31);
					}
					else if (CalDay > (31+29+31+30+31+30+31))
					{
						CalMonth=8;
						CalDay -= (31+29+31+30+31+30+31);
					}
					else if (CalDay > (31+29+31+30+31+30))
					{
						CalMonth=7;
						CalDay -= (31+29+31+30+31+30);
					}
					else if (CalDay > (31+29+31+30+31))
					{
						CalMonth=6;
						CalDay -= (31+29+31+30+31);
					}
					else if (CalDay > (31+29+31+30))
					{
						CalMonth=5;
						CalDay -= (31+29+31+30);
					}
					else if (CalDay > (31+29+31))
					{
						CalMonth=4;
						CalDay -= (31+29+31);
					}
					else if (CalDay > (31+29))
					{
						CalMonth=3;
						CalDay -= (31+29);
					}
					else if (CalDay > (31))
					{
						CalMonth=2;
						CalDay -= (31);
					}
					else
					{
						CalMonth=1;
					}
				}

				sprintf(cBuff,"    S/N %08ld    ", SerialNumber);
				Display(0, cBuff);
				sprintf(cBuff," Cal Date %02ld/%02ld/%04ld", CalMonth, CalDay, CalYear);
				Display(20, cBuff);

////CALDATE TEST
//sprintf(cBuff,"%08lX  %02ld/%02ld/%04ld", CalDate, CalMonth, CalDay, CalYear);
//Display(20, cBuff);


			}

			else if(i==3)
			{
				Display(0, "      FIRMWARE      ");
				sprintf(cBuff,"   C%06d V%4.2f    ", (unsigned int)C_DOCUMENT, (float)FIRMWARE_REVISION);
				Display(20, cBuff);
			}
		}


	}
	ClearDisplayBuff();
}
*/


/*
//-------------------------
// ViewCtempCal - Display the color temp cal coeeff in flash
//-------------------------
void ViewCTempCal (void)
{
unsigned char xdata i=1;
unsigned int xdata j=0;

	ClearDisplayBuff();

	j=i-1;
	sprintf(cBuff,"%d C=%5.3f T=%5.1f   ", j, CurrPnt[j], CTempPnt[j]);
	Display(0, cBuff);
	sprintf(cBuff,"a=%5.1E k%5.1E   ", atokp[j], ktoap[j]);
	Display(20, cBuff);


	while(i)
	{
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report


		if(NewKey)
		{
			if(NewKey==ESCKEY)
				i=0;
			if(NewKey==UPKEY)
			{
				if(i==6) i=1;
				else i++;
			}
			if(NewKey==DOWNKEY)
			{
				if(i==1) i=6;
				else i--;
			}

			j=i-1;
			sprintf(cBuff,"%d C=%5.3f T=%5.1f   ", j, CurrPnt[j], CTempPnt[j]);
			Display(0, cBuff);
			sprintf(cBuff,"a=%5.1E k%5.1E   ", atokp[j], ktoap[j]);
			Display(20, cBuff);

			NewKey=0;
		}

	}
	ClearDisplayBuff();
}


//-------------------------
// COMM Monitor - Display the OutBuff in the top line and the InBuff in the bottom line
//-------------------------
void COMMMonitor (void)
{
unsigned char xdata i=1;

	ClearDisplayBuff();

	while(i)
	{
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		sprintf(cBuff,"%02X,%02X,%02X,%02X,%02X,%02X,%02X",(int)Out_Packet[0],(int)Out_Packet[1],(int)Out_Packet[2],(int)Out_Packet[3],(int)Out_Packet[4],(int)Out_Packet[5],(int)Out_Packet[6]);
		Display(0, cBuff);
		sprintf(cBuff,"%02X,%02X,%02X,%02X,%02X,%02X,%02X",(int)In_Packet[0],(int)In_Packet[1],(int)In_Packet[2],(int)In_Packet[3],(int)In_Packet[4],(int)In_Packet[5],(int)In_Packet[6]);
		Display(20, cBuff);

//		sprintf(cBuff,"%02X,%02X,%02X,%02X,%02X,%02X,%02X",(int)Out_Packet[0],(int)Out_Packet[1],(int)Out_Packet[2],(int)Out_Packet[3],(int)Out_Packet[4],((int)Status & 0x00ff),(int)OutPktRdy);
//		Display(0, cBuff);
//		sprintf(cBuff,"%02X,%02X,%02X,%02X,%02X,  ,%02X",(int)In_Packet[0],(int)In_Packet[1],(int)In_Packet[2],(int)In_Packet[3],(int)In_Packet[4],(int)InPktRdy);
//		Display(20, cBuff);

		if(NewKey)
		{
			if(NewKey==ESCKEY)
				i=0;
			NewKey=0;
		}

	}
	ClearDisplayBuff();
}
*/




//-------------------------
// Set Response Time for the Luminance Reading
//-------------------------
void SetResponseTime (void)
{
unsigned char xdata OldResponseTime;
unsigned char xdata i=1;
float xdata RT;

	NewKey=0;
	OldResponseTime=ResponseTime;

	ClearDisplayBuff();
	Display(20, RESPTIME);

	while(i)
	{
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		Display(4, LumAmpsString);
//	Display(4, LumUserString);

		RT = ResponseTime;
		RT /= 10;
		sprintf(cBuff, "%4.1f", RT);
		Display(35,cBuff);


		if(NewKey)
		{
			if(NewKey==UPKEY)
			{
				Keypad_Rapidfire(100);		// Limit to 1/100ms = 10Hz
				if(ResponseTime<100)
				{
					ResponseTime++;
					LumBuff[ResponseTime-1] = LumBuff[0];// initialize the buffer member
				}
			}
	
			else if(NewKey==DOWNKEY)
			{
				Keypad_Rapidfire(100);		// Limit to 1/100ms = 10Hz
				if(ResponseTime>1)
					ResponseTime--;
			}

			else if(NewKey==ESCKEY)
			{
				ResponseTime = OldResponseTime;
				i=0;
			}

			else if(NewKey==ENTERKEY)
			{
				i=0;
			}

			NewKey=0;
		}
	}
	ClearDisplayBuff();
}



//-------------------------
// SelectGain
//-------------------------
void SelectGain (void)
{
unsigned char xdata OldRange;
unsigned char xdata OldAutoRange;
unsigned char xdata i=1;

	NewKey=0;
	OldRange=Range;
	OldAutoRange=AutoRange;
	ClearDisplayBuff();
	Display(20, GAINRANGE);

	while(i)
	{
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		Display(4, LumAmpsString);
//	Display(4, LumUserString);
		if(AutoRange)
			Display(33, "AUTO");
		else
			Display(33, LumAmpsExpString);


		if(NewKey)
		{
			if(NewKey==UPKEY)
			{
				if(!AutoRange && Range<7)
					NewRange=Range+2;				// Range will become NewRange-1 in T2 ISR
				else if(Range==7)
					AutoRange=1;
			}
	
			else if(NewKey==DOWNKEY)
			{
				if(AutoRange)
				{
					AutoRange=0;
					NewRange=Range+1;				// Range will become NewRange-1 in T2 ISR
				}
				else if(Range>0)
					NewRange=Range;					// Range will become NewRange-1 in T2 ISR
			}

			else if(NewKey==ESCKEY)
			{
				AutoRange=OldAutoRange;
				if(!AutoRange)
					NewRange=OldRange+1;		// Range will become NewRange-1 in T2 ISR
				i=0;
			}

			else if(NewKey==ENTERKEY)
			{
				i=0;
			}

		NewKey=0;
		}
	}
	ClearDisplayBuff();
}


					
//-------------------------
// SelectMonitorUnits
//-------------------------
void SelectMonitorUnits (void)
{
unsigned char xdata OldUnits;
unsigned char xdata i=1;
int dRange;

	//Clear out invalid LumUnits states
	if(LumUnits>4) 
		LumUnits=0;
	if(LumUnits>0 && ((UserCalName[LumUnits-1][0] & 0x80) != 0)) //if blank
		LumUnits=0;

	NewKey=0;
	OldUnits=LumUnits;
	ClearDisplayBuff();
	Display(20, SELECTMONUNITS);

	while(i)
	{
		dRange=(-3-Range);

		if(LumUnits>0)
			dRange+=UserCalExp[LumUnits-1];
		sprintf(LumUserExpString, "E%+03d", dRange); 

		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		if(LumUnits>0)
			cBuff[0] = (LumUnits | 0x30);
		else
			cBuff[0] = 0x20;
		cBuff[1] = 0;
		Display(0, cBuff);

		Display(2, LumUserString);

		if(!LumUnits)
			Display(15, LUMUNITA);
		else if((UserCalName[LumUnits-1][0] & 0x80) != 0) //if blank
			Display(15, "-----");
		else
			Display(15, UserCalName[LumUnits-1]);
			


		if(NewKey)
		{
			if(NewKey==UPKEY)
			{
				if(LumUnits<4)
					LumUnits++;
			}
	
			else if(NewKey==DOWNKEY)
			{
				if(LumUnits>0)
					LumUnits--;
			}

			else if(NewKey==ESCKEY)
			{
				LumUnits=OldUnits;
				i=0;
			}

			else if(NewKey==ENTERKEY)
			{
				if((UserCalName[LumUnits-1][0] & 0x80) != 0) //if blank
					LumUnits=OldUnits;
				i=0;
			}

		NewKey=0;
		}
	}
	ClearDisplayBuff();
}

//-------------------------
// SelectLampUnits
//-------------------------
void SelectLampUnits (void)
{
unsigned char xdata OldUnits;
unsigned char xdata i=1;

	if(LampUnits>2) LampUnits=0;
	NewKey=0;
	OldUnits=LampUnits;
	ClearDisplayBuff();
	Display(20, SELECTLMPUNITS);

	while(i)
	{
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		// Display the format on the top line
		Display(0, LampAmpsString);
		if(LampUnits==0)
			Display(10, LampVoltsString);
		else if(LampUnits==1)
			Display(10, ColorTempString);
		else
			Display(10, LampHourString);


		if(NewKey)
		{
			if(NewKey==UPKEY)
			{
				if(LampUnits<2)
					LampUnits++;
			}
	
			else if(NewKey==DOWNKEY)
			{
				if(LampUnits>0)
					LampUnits--;
			}

			else if(NewKey==ESCKEY)
			{
				LampUnits=OldUnits;
				i=0;
			}

			else if(NewKey==ENTERKEY)
			{
				i=0;
			}

		NewKey=0;
		}
	}
	ClearDisplayBuff();
}



//-------------------------
// Zero the Luminance Monitor
//-------------------------
void ZeroLum (void)
{
	ftemp = LumAmps+LumAmpsZero[Range];
	for(itemp=Range; itemp>0; itemp--)
	{
		ftemp/=10;
	}
	for(itemp=0; itemp<8; itemp++)
	{
		LumAmpsZero[itemp] = ftemp;
		ftemp*=10;
	}
}


/*//-------------------------
// Set Lamp Timer
//-------------------------
void SetLampTimer (void)
{
float LT;

	NewKey=0;
	ClearDisplayBuff();
	Display(20, SETLAMPTIME);
	Display(15, "HOURS");

	LT = (float)LampTimer;
	LT /= 36000;
	LT=NumericEntry(LT,0,1000,5,7,2);
	LT *= 36000;
	LampTimer = (unsigned long)LT;

	ClearDisplayBuff();
}*/

//-------------------------
// View Lamp Timer
//-------------------------
void ViewLampTimer (void)
{
float LT;
unsigned char xdata i=1;

	NewKey=0;
	ClearDisplayBuff();
	Display(20, VIEWLAMPTIME);
	Display(13, "HOURS");
	while(i)
	{
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		LT = (float)LampTimer;
		LT /= 36000;
		sprintf(cBuff,"%7.2f",LT);
		Display(5,cBuff);

		if(NewKey)
		{
			if(NewKey==ESCKEY)
				i=0;
		NewKey=0;
		}
	}
	ClearDisplayBuff();
}

				

//-------------------------
// Adjust Lamp Current
//-------------------------
void AdjustLampTarget (void)
{
	NewKey=0;
	ClearDisplayBuff();
	Display(22,"EDIT LAMP TARGET");

// float NumericEntry (float Number, float Min, float Max, char DisplayPos, char width, char precision)
	LampAmpsTarget=NumericEntry(LampAmpsTarget,0,LampAmpsTargetMax,5,6,3);

//V2.00
//	ftemp = (LampAmpsTarget/LampDACSlope)+LampDACOffset;
//	if(ftemp<0) ftemp=0;
//	if(ftemp>65535) ftemp=65535;
//	LampRawTarget = (unsigned int)ftemp;

	LampAmpsTarget2RawTarget();

//	sprintf(cBuff,"%u",LampRawTarget);
//	Display(0, cBuff);
//	Delay(3000);

	ClearDisplayBuff();
}


//-------------------------
// Numeric Entry
// Use the cursor keys to edit a decimal number
//-------------------------
float NumericEntry (float Number, float Min, float Max, char DisplayPos, char width, char precision)
{
char Format[10];
char CursorPos,cp;
float IncVal;
float OldNumber=Number;
int i=1;

	CursorPos= width-precision-2;
	IncVal = 1.0;
	sprintf(Format,"%%%d.%df",(int)width,(int)precision);

	while(i)
	{
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		// Display the number with a blinking cursor
		sprintf(cBuff,Format,Number);
		if(cBuff[CursorPos]==' ')						// if the digit at cursor is blank
		{																		// Make it a zero so it will blink
			cp=CursorPos;											// Fill in zeroes to the 1st non-zero digit
			while(cBuff[cp]==' ')
			{
				cBuff[cp++]='0';
			}
		}
		if(NowKey==0 && (Blinks & 0x0001))	// Don't blink when keypad active	
			cBuff[CursorPos]=' ';
		Display(DisplayPos, cBuff);


		if(NewKey)
		{
			if(NewKey==UPKEY)
			{
				Keypad_Rapidfire(100);		// Limit to 1/100ms = 10Hz
				Number+=IncVal;
			}
	
			else if(NewKey==DOWNKEY)
			{
				Keypad_Rapidfire(100);		// Limit to 1/100ms = 10Hz
				Number-=IncVal;
			}

			else if(NewKey==LEFTKEY)
			{
				if(CursorPos>1)
				{
					CursorPos--;
					if(CursorPos==width-precision-1) // if on decimal point, move again
						CursorPos--;
					IncVal*=10.0;
				}
			}

			else if(NewKey==RIGHTKEY)
			{
				if(CursorPos<width-1)
				{
					CursorPos++;
					if(CursorPos==width-precision-1) // if on decimal point, move again
						CursorPos++;
					IncVal/=10.0;
				}
			}

			else if(NewKey==ESCKEY)
			{
				NewKey=0;
				return(OldNumber);
			}

			else if(NewKey==ENTERKEY)
			{
				NewKey=0;
				return(Number);
//				i=0;
			}

			if(Number>Max)
				Number=Max;
			else if(Number<Min)
				Number=Min;

			NewKey=0;
		}

	}
}


