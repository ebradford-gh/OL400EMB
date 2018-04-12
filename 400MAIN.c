//-----------------------------------------------------------------------------
// 400MAIN.c
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: Main
// AUTH: Emmett Bradford
// DATE: 3-17-06
//
//
// Target: C8051F32x
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//
// REVISIONS:
//	1.00	03/17/06	Initial Release
//	2.00	02/14/09 	Added Power-Down Detection and V1.10 PLD features
//	2.01	02/18/09 	Added Power-up detection of power down
//	2.10	03-17-09	F347 Flash, Improved USB compliance and keypad debouncing
//	2.20	12-15-09	Control Xfer Flash Read, 2 byte flash addressing
//	2.21	01-25-10	Fixed LumUnits[4] bug
//	2.30	03/17/13	Added support for covert LifeTIme Lamphours and Newhaven OLED or Optrex LCD using J6-4
//	2.40	01/28/16	Added Delay_StopWatch_BuildReport
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
#include "400FVAR.h"               
#include "FlashF340.h"               
#include "USB_REGISTER.h"
#include "USB_MAIN.h"
#include "USB_DESCRIPTOR.h"

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F32x
//-----------------------------------------------------------------------------
sfr16 TMR2RL   = 0xca;                   // Timer2 reload value
sfr16 TMR2     = 0xcc;                   // Timer2 counter     
sfr16 ADC0     = 0xbe;

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------

code const BYTE  Gain[10]={IE3|VE0,IE3|VE1,IE5|VE0,IE5|VE1,IE7|VE0,IE7|VE1,IE9|VE0,IE9|VE1,IE9|VE1,IE9|VE1};
code const float LampAmpsTargetMax=6.600;
code const float FullScaleCalPoint=6.500;
code const float MidScaleCalPoint=4.000;

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
BYTE bdata Status;	// 0 @ reset
sbit LampStat 	= Status^7;
sbit VCMPLStat 	= Status^6;
sbit RawStat 		= Status^5;
sbit UpdateStat = Status^4;
//sbit AutoRange 	= Status^3;

BYTE bdata EventFlags;	// 0 @ reset
sbit NewADC 		= EventFlags^7;
sbit NewUpdate	= EventFlags^6;
sbit NewGain 		= EventFlags^4;
sbit LumOvld 		= EventFlags^3;
sbit RngOKN			= EventFlags^2;
sbit LumUnder		= EventFlags^1;
sbit Vcmpl			= EventFlags^0;

BYTE bdata EventFlg100ms;	// 0 @ reset
sbit NewADC100 	= EventFlg100ms^7;
sbit NewUpd100	= EventFlg100ms^6;			// This is to append a color temp report to the main update
//sbit NewGain100 = EventFlg100ms^4;
sbit LumOvld100 = EventFlg100ms^3;
sbit RngOKN100	= EventFlg100ms^2;
sbit LumUnder100 = EventFlg100ms^1;
sbit Vcmpl100		= EventFlg100ms^0;

//// Event Detector Identities
//#define	VCMPL 			0x01			// Low = bad
//#define	UNDER				0x02			// Low = bad
//#define	RNGOKN			0x04			// Low = OK
//#define	OVLD				0x08			// Low = Bad
//#define	NEWGAIN			0x10			// New Gain Range
//#define	NEWADC			0x80			// New VFC Total


BYTE data OutStatus;
BYTE data InPktRdy=0;
BYTE data OutPktRdy=0;
BYTE data Out_Packet[11] = {0,0,0,0,0,0,0,0,0,0,0}; // Last packet received from host (1 extra pad)
BYTE data In_Packet[11]  = {0,0,0,0,0,0,0,0,0,0,0}; // Next packet to sent to host (1 extra pad)


char xdata cBuff[50];
char xdata DisplayBuff[50];
char xdata DisplayOff=0;
char xdata DisplayPtr=0;
 int xdata itemp;
unsigned int xdata utemp, LastLampTargetOut;

unsigned char data SPIOUTH,SPIOUTL,SPIINH,SPIINL, SPIIN_ID;
unsigned char xdata BounceRate=100;
unsigned char data Bounces,NowKey,NewKey,LastKey,NoKeyCtr;

unsigned int  xdata LampDAC=0;
unsigned int  data T0Now,T0Last,T0Diff,T1Now,T1Last,T1Diff,TPNow,TPLast,TPDiff,Blinks;
unsigned char data T0Ext,T1Ext,TPExt,THTemp,TLTemp;
unsigned char data T2Count,T2Count2,OVLDcnt,UNDERcnt,RNGOKNcnt,VCMPLcnt,RngWtCnt;
unsigned char data USBRXCtr=0;
unsigned char data USBTXCtr=0;

unsigned int xdata LumBuff[100];
unsigned int xdata LampAmpsBuff[11];
unsigned int xdata LampVoltsBuff[11];
unsigned char xdata BuffPtr, LumBuffPtr, LampAmpsBuffPtr, LampVoltsBuffPtr;
unsigned char xdata ResponseTime;
unsigned char xdata LumUnits, LampUnits;
unsigned char xdata Range,NewRange,AutoRange;

char xdata LumAmpsExpString[6]; 
char xdata LumUserExpString[6]; 
char xdata LumAmpsString[20]; 
char xdata LumUserString[20]; 
char xdata LampAmpsString[16]; 
char xdata LampVoltsString[16]; 
char xdata ColorTempString[16];
char xdata LampHourString[16];

int xdata LumAmpsInt, LampAmpsInt, LampVoltsInt, ColorTempInt;

float xdata LumAmps, LampAmps, LampVolts;
float xdata LumAmpsZero[9];
float xdata LumUser;
float xdata LampAmpsTarget;
float xdata ftemp;
unsigned int xdata LampRawTarget;
unsigned int xdata LampRaw;
long xdata LampRawDiff;
unsigned long xdata LampTimer, LampTimerLifeTime;
long xdata ltemp;

long xdata ltempBR;
 int xdata itempBR;
unsigned int xdata utempBR;
unsigned char xdata ctempBR;
float xdata ftempBR;

float xdata ColorTemp;
float xdata cto[6], CPart[6];

unsigned int  xdata StopWatch;

//code const BYTE LockByte _at_ 0x7FFF;
BYTE code *FlashPtr;
FLADDR FlashAddr;
char code *FlashReadPtr;

//-----------------------------------------------------------------------------
// Main Routine
//-----------------------------------------------------------------------------
void main(void) 
{
	Initialize();

	for(itemp=0; itemp<8; itemp++)	// Initialize the zeroes
	{
		LumAmpsZero[itemp]=0;
	}

	NewKey=NewRange=0;
	if(AutoRange || Range>7)
		Range=0;
  NewRange=Range+1;					// Cause a gain change from the T2 ISR

		//Clear out invalid LumUnits states
	if(LumUnits>4) 
		LumUnits=0;
	if(LumUnits>0 && ((UserCalName[LumUnits-1][0] & 0x80) != 0)) //if blank
		LumUnits=0;

	if(LampUnits>2) LampUnits=0;
	if(ResponseTime>100) ResponseTime=100;
	if(ResponseTime<1) ResponseTime=1;
	if(LampTimer>(9999.99*36000)) LampTimer=(10001*36000);
	if(LampAmpsTarget>LampAmpsTargetMax) LampAmpsTarget=0;

	LampStat=RawStat=VCMPLStat=UpdateStat=0;
	LastLampTargetOut=0;

	T2Count=T2Count2=2;
	Blinks=LampDAC=0;
	Write_LampDAC(LampDAC);
	LampAmpsTarget2RawTarget();


	// Display the startup message with revision level version
//	sprintf(cBuff," OPTRONIC LABS, INC.");
	sprintf(cBuff," GOOCH AND HOUSEGO  ");
	Display(0,cBuff);
	sprintf(cBuff,"  OL 400-C   V%4.2f  ", FIRMWARE_REVISION);
	Display(20,cBuff);
//	Delay(2000);
	Delay_StopWatch_BuildReport(2000);
	ClearDisplayBuff();



	while (1)
	{
		Check_Keypad();			// Monitor for key presses
		Update();						// Convert and sprintf the raw results
		BuildReport();			// Build an input report

		// Display Results on the LED display
		if(LumUnits==0)
		{
			Display(2, LumAmpsString);
			Display(15, "A    ");
		}
		else
		{
			Display(2, LumUserString);
			Display(15, UserCalName[LumUnits-1]);
		}

		Display(20, LampAmpsString);

		if(VCMPLStat)
			Display(30, LampVoltsString);
		else if(!LampStat || LampUnits==2)
			Display(30, LampHourString);
		else if (LampUnits==1)
			Display(30, ColorTempString);
		else
			Display(30, LampVoltsString);
  }
}

//-------------------------
// Average the Readings and Update the output strings
//-------------------------
void Update (void)
{
int dRange;
long LampDACTemp;

	if(!NewADC)
		return;
	NewADC = 0;								// There is new ADC data - this happens @ 10Hz

	// Advance the buffer pointers
	if(++LampAmpsBuffPtr>=10) 			LampAmpsBuffPtr = 0;
	if(++LumBuffPtr>=ResponseTime)	LumBuffPtr = 0;
	if(++LampVoltsBuffPtr>=10)			LampVoltsBuffPtr = 0;

	// Buffer the raw data
	ET2=0;	// Turn off the 500 Hz Interrupt
//	LampAmpsBuff[LampAmpsBuffPtr] = T0Diff;
	LumBuff[LumBuffPtr] = T1Diff;
	LampVoltsBuff[LampVoltsBuffPtr] = TPDiff;
	ET2=1;	// Turn on the 500 Hz Interrupt



//////////////////////////////////
// Process Current Source

	if(!RawStat)
	{

		if(VCMPLStat)	// reading is bogus
			LampAmps=0;
		else
		{ // 3 point calibration conversion
			LampAmps = ((float)((long)LampDAC-(long)LampDACOffset))*LampDACSlope;
			if(MidScaleCalPoint>LampAmpsTarget)
				LampAmps -= LampAmps*LampDACCurve;
			else
				LampAmps -= LampDACCurve*MidScaleCalPoint*(FullScaleCalPoint-LampAmpsTarget)/(FullScaleCalPoint-MidScaleCalPoint);
		}

		if(LampAmps>0)
			LampAmpsInt = (int)((LampAmps * 1000) + 0.5);
		else
			LampAmpsInt = (int)((LampAmps * 1000) - 0.5);

		ColorTemp=Curr2CTemp(LampAmps);

		if((!LampStat && LampDAC<100) || VCMPLStat)// amps reading is bogus
			sprintf(LampAmpsString, " __.___ A ");
		else
			sprintf(LampAmpsString, "%7.3f A ", LampAmps);

		if(ColorTemp < 1999.5)
		{
			ColorTempInt= 0;
			sprintf(ColorTempString, "   ____ K ");
		}

		else
		{
			ColorTempInt= (int)(ColorTemp+0.5);
			sprintf(ColorTempString, "% 7d K ", ColorTempInt);
		}

		if(LampTimer < (36000*9999.99))
			sprintf(LampHourString,"% 7.2f hr", ((float)LampTimer/36000));
		else
			sprintf(LampHourString,"  >10000 hr");
	}

	else
	{
//		LampAmpsInt = (int)(LampAmps);
		LampAmpsInt = LampDAC;
		sprintf(LampAmpsString, "%7u rA", LampAmpsInt);
	}

//////////////////////////////
// Process Timer 1 = Luminance
	if(NewGain)
	{	//	If Gain has changed, reprint the exponent strings
		dRange=(-3-Range);
		sprintf(LumAmpsExpString, "E%+03d", dRange); 
		if(LumUnits>0)
			dRange+=UserCalExp[LumUnits-1];
		sprintf(LumUserExpString, "E%+03d", dRange); 
		NewGain = 0;
	}

	if(!(LumOvld||LumUnder) && RngWtCnt==0)
	{
		LumAmps=0;
		for(BuffPtr=0; BuffPtr<ResponseTime; BuffPtr++)
			LumAmps += LumBuff[BuffPtr];
		LumAmps/=ResponseTime;

		if(!RawStat)
		{
			//LumAmps
			LumAmps= (LumAmps-LumAmpsOffset[Range])*LumAmpsSlope[Range];
			LumAmps -= LumAmpsZero[Range];

			//LumAmpsInt
			if(LumAmps > 0)
				LumAmpsInt = (int)((LumAmps * 10000) + 0.5);
			else
				LumAmpsInt = (int)((LumAmps * 10000) - 0.5);
			if(LumAmps>3.2766) LumAmpsInt = 32767;
			else if(LumAmps<-3.2767) LumAmpsInt = -32768;

			if(LumAmps<9.9999 && LumAmps>-9.9999)	// make sure zeroing won't screw up the display
				sprintf(LumAmpsString, "%7.4f %s ", LumAmps, LumAmpsExpString);
			else
				sprintf(LumAmpsString, "RE-ZERO %s ", LumAmpsExpString);

/*			//LumUnits
			LumUser= LumAmps;
			if(LumUnits)
				LumUser*= UserCalFactor[LumUnits-1];
			if(LumUser<9.9999 && LumUser>-9.9999)	// make sure zeroing won't screw up the display
				sprintf(LumUserString, "%7.4f %s ", LumUser, LumUserExpString);
			else
				sprintf(LumUserString, "RE-ZERO %s ", LumUserExpString);
*/

			//LumUnits
//			if(LumUnits && LumAmpsInt<32767 && LumAmpsInt>-32768)
			if(LumUnits && ((UserCalName[LumUnits-1][0] & 0x80) == 0)) //if valid lumunits
			{
				LumUser = LumAmpsInt * 0.0001;
				LumUser *= UserCalFactor[LumUnits-1];
			}
			else
				LumUser = LumAmps;

			if(LumUser<9.9999 && LumUser>-9.9999)	// make sure zeroing won't screw up the display
				sprintf(LumUserString, "%7.4f %s ", LumUser, LumUserExpString);
			else
				sprintf(LumUserString, "RE-ZERO %s ", LumUserExpString);

		}

		else	//RAWUNITS
		{
			LumUnits=0;
			LumAmpsInt = (int)LumAmps;
			sprintf(LumAmpsString, "%7u %s ", LumAmpsInt, LumAmpsExpString);
		}
	}

	else if(RngWtCnt>0)
	{
		sprintf(LumAmpsString, "------- %s ", LumAmpsExpString);
		sprintf(LumUserString, "------- %s ", LumUserExpString);
		LumAmpsInt = -32767;
	}

	else if(LumOvld)
	{
		sprintf(LumAmpsString, "  OVER  %s ", LumAmpsExpString);
		sprintf(LumUserString, "  OVER  %s ", LumUserExpString);
		LumAmpsInt = 32767;
	}

	else if(LumUnder)
	{
		sprintf(LumAmpsString, " UNDER  %s ", LumAmpsExpString);
		sprintf(LumUserString, " UNDER  %s ", LumUserExpString);
		LumAmpsInt = -32768;
	}



/////////////////////////////////
// Process PCA     = Lamp Voltage
	LampVolts = 0;
	for(BuffPtr=0; BuffPtr<10; BuffPtr++)
		LampVolts += LampVoltsBuff[BuffPtr];
	LampVolts /= 10;

	if(!RawStat)
	{
		LampVolts = ((LampVolts-LampVoltsOffset))*LampVoltsSlope;
//EB 091015
		if(	LampVolts>327.66) LampVoltsInt=32767;
		if(	LampVolts<-327.67) LampVoltsInt=-32768;
		if(LampVolts > 0)
			LampVoltsInt = (int)((LampVolts * 100) + 0.5);
		else
			LampVoltsInt = (int)((LampVolts * 100) - 0.5);

		if(!LampStat && LampDAC<100)
			sprintf(LampVoltsString, "____.__ V ");
		else if(VCMPLStat)
			sprintf(LampVoltsString, " VCMPL! V ");
//EB 091015
		else if(LampVoltsInt==32767)
			sprintf(LampVoltsString, " V OVER V ");
		else if(LampVoltsInt==-32768)
			sprintf(LampVoltsString, " VUNDER V ");
		else
			sprintf(LampVoltsString, "%7.2f V ", LampVolts);

//		if((!LampStat && LampDAC<100) || VCMPLStat)
//			sprintf(LampWattsString, "____.__ W ");
//		else
//			sprintf(LampWattsString, "%7.2f W ", LampWatts);
	}

	else
	{
		LampVoltsInt = (int)(LampVolts);
		sprintf(LampVoltsString, "% 7u rV", LampVoltsInt);
//		sprintf(LampWattsString, "   0.00 W ");
	}



/////////////////////////////////////////
// Do open-loop DAC-based current control
	if(!LampStat)
	{
		if(LampDAC>900)
			LampDACTemp=LampDAC-500;
		else
			LampDACTemp = 0;
	}
	else
	{
		LampTimer++;
		LampTimerLifeTime++;
		LampRawDiff=(long)LampDAC-(long)LampRawTarget;
		LampDACTemp = (long)LampDAC;

		if(LampRawDiff<-100)
			LampDACTemp+=100;
		else if(LampRawDiff<0)
			LampDACTemp=LampRawTarget;
		else if(LampRawDiff>100)
			LampDACTemp-=100;
		else
			LampDACTemp=LampRawTarget;

		if(LampDACTemp>65535) LampDACTemp=65535;
		else if(LampDACTemp<0) LampDACTemp=0;
	}

	if(LampDAC != (unsigned int)LampDACTemp)
	{
		ET2=0;	// Turn off the 500 Hz Interrupt
		LampDAC= (unsigned int)LampDACTemp;
		ET2=1;	// Turn on the 500 Hz Interrupt
	}

	NewUpdate=1;		// Semaphore to AutoUpdate USB
}


//V2.00!!  The current VFC was never used and was permanently removed in rev B PCB
//-------------------------
// Timer0_ISR
// Called when timer 0 overflows for a step rate
// Check the timers:
// Timer 0 = Step Rate
// Timer 0 was initially used for a VFC for current measurement
// never used in a released version
// Current has always been controlled directly by DAC
//-------------------------
void Timer0_ISR(void) interrupt 1
{
	TH0=0xff;
	
	if(P0 & 0x80)
		P0 &= ~0x80;
	else
		P0 |= 0x80;

}


//-------------------------
// Timer2_ISR
// Called when timer 2 overflows every 2ms
// Check the timers:
// Timer 0 = Was initially Current Sense, now Step Rate
// Timer 1 = Luminance
// PCA     = Lamp Voltage
// Update the Display and Keypad
//
//-------------------------
void Timer2_ISR(void) interrupt 5
{
	// Check the timer overflows and inc extensions
//V2.00	if(TF0) {	T0Ext++;	TF0=0;}
	if(TF1) {	T1Ext++;	TF1=0;}
	if(CF)  {	TPExt++;	CF=0;}

	// Decrement the real time Stopwatch
	if(StopWatch>1)
		StopWatch-=2;
	else
		StopWatch=0;

	if(--T2Count==0)		//decimate to 100ms
	{
		T2Count=50;				// reset counter
		if(--T2Count2==0)	//decimate to 500ms
		{
			T2Count2=5;			// reset counter
			Blinks++;				// inc the blink counter
		}


//V2.00!!  The current VFC was never used and was permanently removed in rev B PCB
// Read Timer 0 = Current Sense
//		THTemp = TH0;
//		if(TF0) {	T0Ext++;	TF0=0;}
//		TLTemp = TL0;
//		if(THTemp!=TH0 || TF0)	// if TL rolled over
//		{												// Read TH, TL and TF again
//			TLTemp = TL0;
//			THTemp = TH0;
//			if(TF0) {	T0Ext++;	TF0=0;}
//		}
//		T0Now=(int)THTemp;
//		T0Now<<=8;
//		T0Now+=(int)TLTemp;
//		T0Diff = T0Now-T0Last;
//		T0Diff >>= 1;
//		if(T0Ext>1 || (T0Ext==1 && (T0Now>=T0Last)))
//			T0Diff |= 0x8000;
//		T0Last = T0Now;
//		T0Last &= 0xFFFE;				// preserve lost resolution in next pass
//		T0Ext=0;


		// Read Timer 1 = Luminance
		THTemp = TH1;
		if(TF1) {	T1Ext++;	TF1=0;}
		TLTemp = TL1;
		if(THTemp!=TH1 || TF1)	// if TL rolled over
		{												// Read TH, TL and TF again
			TLTemp = TL1;
			THTemp = TH1;
			if(TF1)
			{
				T1Ext++;
				TF1=0;
			}
		}
		T1Now=(int)THTemp;
		T1Now<<=8;
		T1Now+=(int)TLTemp;
		T1Diff = T1Now-T1Last;
		T1Diff >>= 1;
		if(T1Ext>1 || (T1Ext==1 && (T1Now>=T1Last)))
			T1Diff |= 0x8000;
		T1Last = T1Now;
		T1Last &= 0xFFFE;				// preserve lost resolution in next pass
		T1Ext=0;


		// Read Timer P = PCA = Lamp Voltage
		TLTemp = PCA0L;					// This latches PCA0H
		THTemp = PCA0H;
		if(CF)  {	TPExt++;	CF=0;}
		TPNow=(int)THTemp;
		TPNow<<=8;
		TPNow+=(int)TLTemp;
		TPDiff = TPNow-TPLast;
		TPDiff >>= 1;
		if(TPExt>1 || (TPExt==1 && (TPNow>=TPLast)))
			TPDiff |= 0x8000;
		TPLast = TPNow;
		TPLast &= 0xFFFE;				// preserve lost resolution in next pass
		TPExt=0;
		NewADC = 1; 						// Signal new results to update routine
	}

	else if(T2Count==25)						// Count down events on another time slice
	{
//#define	VCMPL 			0x01			// Low = bad
//#define	UNDER				0x02			// Low = bad
//#define	RNGOKN			0x04			// Low = OK
//#define	OVLD				0x08			// Low = Bad

		if(RngWtCnt>8)		// Ignore events just after range change
			EventFlg100ms &= ~(OVLD|RNGOKN|UNDER);

		EventFlags |= EventFlg100ms;

		if(LumOvld100)
			OVLDcnt=5;
		else if(--OVLDcnt==0)
			LumOvld=0;

		if(LumUnder100)
			UNDERcnt=5;
		else if(--UNDERcnt==0)
			LumUnder=0;

		if(RngOKN100)
			RNGOKNcnt=5;
		else if(--RNGOKNcnt==0)
			RngOKN = 0;

		if(Vcmpl100)
			VCMPLcnt=5;
		else if(--VCMPLcnt==0)
			Vcmpl = 0;


		if(Vcmpl)
			VCMPLStat=1;
		else
			VCMPLStat=0;

		EventFlg100ms = 0;

		if(RngWtCnt>0)
			RngWtCnt--;

		if(AutoRange)
		{
			if((LumOvld||LumUnder) && Range>0 && RngWtCnt==0)
			{
				Range--;
				ChangeGain(Range);
			}
			else if(!(RngOKN||LumOvld||LumUnder) && Range<7 && RngWtCnt==0)
			{
				Range++;
				ChangeGain(Range);
			}
		}
		else if(NewRange)
		{
			Range=NewRange-1;		// Range will become NewRange-1
			NewRange=0;
			ChangeGain(Range);
		}

		// Turn the USB LEDs off
		if(USBRXCtr)
			USBRXCtr--;
		else
			USBRXLED=LEDOFF;		// Turn off the USB LED
		if(USBTXCtr)
			USBTXCtr--;
		else
			USBTXLED=LEDOFF;		// Turn off the USB LED
	}

	Check_Power();
	Write_LampDAC(LampDAC);
	Display_Keypad_Update();
	TF2H = 0;								// Clear Timer2 interrupt flag
}



//-------------------------
// ChangeGain
// This should only be called from Timer2_ISR
// Otherwise it could conflict with other SPIO activity
//-------------------------
void ChangeGain (unsigned char Range)
{
//// Preamp Identities
//#define	IE3					0x01
//#define	IE5					0x02
//#define	IE7					0x04
//#define	IE9					0x00
//#define	VE0					0x20
//#define	VE1					0x40
//#define	VE2					0x80
//
//Gain[]={IE3|VE0,IE3|VE1,IE5|VE0,IE5|VE1,IE7|VE0,IE7|VE1,IE9|VE0,IE9|VE1,IE9|VE1};

 	if(Range>7) Range=0;
	Write_Preamp(Gain[Range]);
	RngWtCnt=10;
	NewGain = 1;
	Status &= 0xf8;
	Status |= Range;
}



/********************************************************************
*********************************************************************
* This routine calculates DAC counts from the current target value


void LampAmpsTarget2RawTarget (void)
{

float ftemp;

	// Calculate the Raw equivalent
	if(LampAmpsTargetMax-LampAmpsTarget>LampAmpsTarget)
		ftemp = LampAmpsTarget+(LampAmpsTarget*LampDACCurve);
	else
		ftemp = LampAmpsTarget+((LampAmpsTargetMax-LampAmpsTarget)*LampDACCurve);

	ftemp = (ftemp/LampDACSlope)+LampDACOffset;

	if(ftemp<0) ftemp=0;
	if(ftemp>65535) ftemp=65535;
	LampRawTarget = (unsigned int)ftemp;
}
*/

/********************************************************************
*********************************************************************
* This routine calculates DAC counts from the current target value

//code const float FullScaleCalPoint=6.500;
//code const float MidScaleCalPoint=4.000;
*/

void LampAmpsTarget2RawTarget (void)
{

float ftemp;

	// Check Target against Limits
//	ftemp = (float)LampLibTargetMax[LampLibSelection]/1000;
//	if(ftemp>0 && ftemp<=LampAmpsTargetMax[ModelType])
//		LampLibAmpsLimit = ftemp;
//	else
//		LampLibAmpsLimit = LampAmpsTargetMax[ModelType];
//
	if(LampAmpsTarget>LampAmpsTargetMax) LampAmpsTarget=0;
//	if(LampAmpsTarget>LampLibAmpsLimit) LampAmpsTarget=0;
//	if(LampAmpsTarget<0) LampAmpsTarget=0;


	// Calculate the mid-scale correction
	if(MidScaleCalPoint>LampAmpsTarget)
		ftemp = LampAmpsTarget+(LampAmpsTarget*LampDACCurve);
	else
	{
		ftemp = (FullScaleCalPoint-LampAmpsTarget)/(FullScaleCalPoint-MidScaleCalPoint);
		ftemp *= LampDACCurve;
		ftemp *= MidScaleCalPoint;
		ftemp += LampAmpsTarget;
	}

	// Calculate the Raw Target
	ftemp = (ftemp/LampDACSlope)+LampDACOffset;

	if(ftemp<0) ftemp=0;
	if(ftemp>65535) ftemp=65535;
	LampRawTarget = (unsigned int)ftemp;
}



//float code CurrPnt[6], CTempPnt[6], ktoap[6], atokp[6];
//float xdata cto[6], CPart[6];
/********************************************************************
*********************************************************************
* This routine calculates color temperature from the current value
*/

float Curr2CTemp (float CurrVal)
{
	if(CurrVal<3)
	  return(0);

	CPart[0]=CurrVal-CurrPnt[0];
	CPart[1]=CurrVal-CurrPnt[1];
	CPart[2]=CurrVal-CurrPnt[2];
	CPart[3]=CurrVal-CurrPnt[3];
	CPart[4]=CurrVal-CurrPnt[4];
	CPart[5]=CurrVal-CurrPnt[5];

	cto[0]=CPart[1]*CPart[2]*CPart[3]*CPart[4]*CPart[5]*atokp[0];
	cto[1]=CPart[0]*CPart[2]*CPart[3]*CPart[4]*CPart[5]*atokp[1];        // - 
	cto[2]=CPart[0]*CPart[1]*CPart[3]*CPart[4]*CPart[5]*atokp[2];
	cto[3]=CPart[0]*CPart[1]*CPart[2]*CPart[4]*CPart[5]*atokp[3];        // - 
	cto[4]=CPart[0]*CPart[1]*CPart[2]*CPart[3]*CPart[5]*atokp[4];
	cto[5]=CPart[0]*CPart[1]*CPart[2]*CPart[3]*CPart[4]*atokp[5];        // - 

  return((cto[0]-cto[1])+(cto[2]-cto[3])+(cto[4]-cto[5]));
}




/********************************************************************
*********************************************************************
* This routine calculates a current target from the color temperature
*/
/*
float CTemp2Curr (float CTempVal)
{
float initcurr,diff;
char i;

CPart[0]=CTempVal-CTempPnt[0];
CPart[1]=CTempVal-CTempPnt[1];
CPart[2]=CTempVal-CTempPnt[2];
CPart[3]=CTempVal-CTempPnt[3];
CPart[4]=CTempVal-CTempPnt[4];
CPart[5]=CTempVal-CTempPnt[5];

cto[0]=CPart[1]*CPart[2]*CPart[3]*CPart[4]*CPart[5]*ktoap[0];
cto[1]=CPart[0]*CPart[2]*CPart[3]*CPart[4]*CPart[5]*ktoap[1];        // - 
cto[2]=CPart[0]*CPart[1]*CPart[3]*CPart[4]*CPart[5]*ktoap[2];
cto[3]=CPart[0]*CPart[1]*CPart[2]*CPart[4]*CPart[5]*ktoap[3];        // - 
cto[4]=CPart[0]*CPart[1]*CPart[2]*CPart[3]*CPart[5]*ktoap[4];
cto[5]=CPart[0]*CPart[1]*CPart[2]*CPart[3]*CPart[4]*ktoap[5];        // - 

initcurr=((cto[0]-cto[1])+(cto[2]-cto[3])+(cto[4]-cto[5]));


// this loop checks that the specified temp will be produced by
// the  calculated current, if error is too great, it adjusts the calculated
// current target until it agrees

i=1;
while(i)
  {
  ftemp=Curr2CTemp(initcurr);
  if(ftemp>CTempVal)
    {
    diff=ftemp-CTempVal;
    if(diff>=1 && initcurr>0)
      initcurr-=0.001;
    else
      i=0;
    }
  else
    {
    diff=CTempVal-ftemp;
    if(diff>=1 && initcurr<LampAmpsTargetMax)
      initcurr+=0.001;
    else
      i=0;
    }
  }
return(initcurr);
}
*/




//-------------------------
// BuildReport
// This should only be called Main Loop
//-------------------------
void BuildReport (void)
{
/*
BYTE Out_Packet[9] = {0,0,0,0,0,0,0,0,0}; // Last packet received from host (1 extra pad)
BYTE In_Packet[9]  = {0,0,0,0,0,0,0,0,0}; // Next packet to sent to host (1 extra pad)

#define	AUTORNG			0x08			// 1 = Auto
#define	AUTOUPDATE	0x10			// 1 = Auto, 0 = wait for OUT report 
#define	RAWUNITS		0x20			// 1 = Raw, 0 = calibrated
#define	CMPL				0x40			// Low = Bad
#define	LAMPSTAT		0x80			// 1 = On, 0 = Off

B7 	Lamp Status: 0=Standby, 1=On
B6	Regulation Compliance: 0=OK, 1= VUNREG too low
B5	Raw Units
B4	Auto-Update
B3	Auto-Range
B2  Range msb
B1	Range lsb
B0	Range lsb
*/

		// AutoRange is a byte not a bit
		Status &= ~AUTORNG;
		if(AutoRange)
			Status |= AUTORNG;

	if(OutPktRdy)
	{	// if Packet received from host set In_Packet defaults;
		In_Packet[0] = 0;
		In_Packet[1] = 0;
		In_Packet[2] = 0;
		In_Packet[3] = 0;
		In_Packet[4] = 0;
		In_Packet[5] = 0;
		In_Packet[6] = 0;
		In_Packet[7] = 0;
		In_Packet[8] = 0;
		In_Packet[9] = 0;
		In_Packet[10] = 0;

		// Out Report 1 = Signal		
		if(Out_Packet[0] == 1)
		{
			OutStatus = Out_Packet[1];		// Save the OutStatus =  Commanded Status

			// AutoRange is a byte not a bit
			if(OutStatus & AUTORNG)
			{
				AutoRange=1;
				Status |= AUTORNG;
			}
			else
			{
				AutoRange=0;
				Status &= ~AUTORNG;
				if(Range != (OutStatus & 0x07))
					NewRange = (OutStatus & 0x07) +1;	// Cause a gain change from the T2 ISR
			}

			if(OutStatus & LAMPSTAT)		LampStat=1;
			else												LampStat=0;
			if(OutStatus & RAWUNITS)		RawStat=1;
			else												RawStat=0;
			if(OutStatus & AUTOUPDATE)	UpdateStat=1;
			else												UpdateStat=0;

			// Get the new lamp Target
			utempBR = (unsigned int)Out_Packet[3];
			utempBR <<= 8;
			utempBR |= (unsigned int)Out_Packet[2];
			if(utempBR!= LastLampTargetOut)
			{
				LastLampTargetOut = utempBR;
				if(RawStat)
					LampRawTarget = utempBR;
				else
				{
					ftempBR = (float)utempBR;
					ftempBR /= 1000;
					LampAmpsTarget = ftempBR;
					LampAmpsTarget2RawTarget();
				}
			}
	
			if(InPktRdy==0)		// If the last In packet is gone
			{									// Build the In Report #1
				In_Packet[0] = 1;
				In_Packet[1] = Status;
				In_Packet[2] = (LumAmpsInt & 0x00ff);		// Detector Current LSB
				In_Packet[3] = (LumAmpsInt>>8);					// Detector Current MSB
				In_Packet[4] = (LampAmpsInt & 0x00ff);	// Lamp Current LSB
				In_Packet[5] = (LampAmpsInt>>8);				// Lamp Current MSB
				In_Packet[6] = (LampVoltsInt & 0x00ff);	// Lamp Voltage LSB
				In_Packet[7] = (LampVoltsInt>>8);				// Lamp Voltage MSB
//				In_Packet[8] = (ColorTempInt & 0x00ff);	// Color Temp LSB
//				In_Packet[9] = (ColorTempInt>>8);				// Color Temp MSB
				InPktRdy=1;
				USBTXLED=LEDON;	// Turn on the USB LED to show activity
				USBTXCtr=10;		// Set counter for T2 ISR to turn off 10 x 0.1s later
			}
		}


//2 byte Flash Address !!

		// Out Report 2 = Read Flash
		else if(Out_Packet[0] == 2)
		{
			utempBR = (unsigned int)Out_Packet[2];
			utempBR <<= 8;
			utempBR |= (unsigned int)Out_Packet[1];
			FlashAddr = (FLASH_START + utempBR);

			if(InPktRdy==0)		// If the last In packet is gone
			{									// Build the In Report #2
				In_Packet[0] = 2;
				In_Packet[1] = Out_Packet[1];
				In_Packet[2] = Out_Packet[2];
				FlashReadPtr = (char *)FlashAddr;
				In_Packet[3] = *FlashReadPtr;
				InPktRdy=1;
				USBTXLED=LEDON;	// Turn on the USB LED to show activity
				USBTXCtr=10;		// Set counter for T2 ISR to turn off 10 x 0.1s later
			}
		}


		// Out Report 3 = Write Flash
		else if(Out_Packet[0] == 3)
		{	
			utempBR = (unsigned int)Out_Packet[2];
			utempBR <<= 8;
			utempBR |= (unsigned int)Out_Packet[1];
			FlashAddr = (FLASH_START + utempBR);
			FLASH_ByteWrite(FlashAddr, Out_Packet[3]);

			if(InPktRdy==0)		// If the last In packet is gone
			{									// Build the In Report #3
				In_Packet[0] = 3;
				In_Packet[1] = Out_Packet[1];
				In_Packet[2] = Out_Packet[2];
				In_Packet[3] = Out_Packet[3];
				InPktRdy=1;
				USBTXLED=LEDON;			// Turn on the USB LED to show activity
				USBTXCtr=10;		// Set counter for T2 ISR to turn off 10 x 0.1s later
			}
		}


		// Out Report 4 = Erase Flash Page pointed at
		else if(Out_Packet[0] == 4)
		{	
			utempBR = (unsigned int)Out_Packet[2];
			utempBR <<= 8;
			utempBR |= (unsigned int)Out_Packet[1];
			FlashAddr = (FLASH_START + utempBR);
			FLASH_PageErase(FlashAddr);	// Erase the page


			if(InPktRdy==0)		// If the last In packet is gone
			{									// Build the In Report #4
				In_Packet[0] = 4;
				In_Packet[1] = Out_Packet[1];
				In_Packet[2] = Out_Packet[2];
				InPktRdy=1;
				USBTXLED=LEDON;	// Turn on the USB LED to show activity
				USBTXCtr=10;		// Set counter for T2 ISR to turn off 10 x 0.1s later
			}
		}


		// Out Report 5 = Read Setup Info
		// Info Type 
		//	1		Response Time		(unsigned char)		sec/10
		//	2		Monitor Units		(unsigned char)		400C: 0-4, 730E: 0-3
		//	3		Lamp Timer 		(unsigned long)		(0-ffffffff (sec/10))
		//	4		Display Off 		(unsigned char)		(0/1)
		//	5		Status	 		(unsigned char)		(write / read status byte)
		//	6		Lamp Current / PMTHV	(unsigned int)		(0-ffff mA / V)
		//	7		Lamp Units Display	(unsigned char)		(0-2, volts/°K/hours)
		//	8		Aux I/O			(unsigned int)		(write 00-0f, read 0000-0f0f)
		//NA	9		Model Type (read only)	(unsigned char)		(401 lamp, 410 model, 730E HV)
		//NA	10	Library Selection	(unsigned char)		730E detector or 410 Lamp
		//NA	11	730E Integration Mode	(unsigned char)		0=off, 1= clear, 2=run, 3= hold 
		//NA	12	730E Aux I/O Mode	(unsigned char)		0=TTL, 1= trig'd reading, 2=integration 
		//NA	13	Flux Overload Value	(unsigned int)		(1-0xC350 (µA/100))
		//NA	14	Normalize Signal Inv	(float)			(>= 0 (A))
		//	15	Firmware Version	(unsigned int)		read only
		//	254	Lamp Timer LifeTime	(unsigned long)		(0-ffffffff (sec/10))

		else if(Out_Packet[0] == 5)
		{
			if(InPktRdy==0)		// If the last In packet is gone
			{									// Build the In Report #2
				In_Packet[0] = 5;
				In_Packet[1] = Out_Packet[1];
	
				if(In_Packet[1] == 1)
					In_Packet[2] = ResponseTime;
				else if(In_Packet[1] == 2)
					In_Packet[2] = LumUnits;
				else if(In_Packet[1] == 3)
				{
					ltempBR=LampTimer;
					In_Packet[2] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[3] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[4] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[5] = (unsigned char)(ltempBR & 0x000000ff);
				}
				else if(In_Packet[1] == 4)
					In_Packet[2] = DisplayOff;
				else if(In_Packet[1] == 5)
					In_Packet[2] = Status;
				else if(In_Packet[1] == 6)
				{
					utempBR = (unsigned int)((LampAmpsTarget*1000)+0.5);
					In_Packet[2] = (unsigned char)(utempBR & 0x000000ff);
					utempBR>>=8;
					In_Packet[3] = (unsigned char)(utempBR & 0x000000ff);
				}
				else if(In_Packet[1] == 7)
					In_Packet[2] = LampUnits;
				else if(In_Packet[1] == 8)
				{
					In_Packet[2] = P1 & 0x0E;
					if(GPI0)
						In_Packet[2] |= 0x01;				
					In_Packet[3] = P2 & 0xf0;
					In_Packet[3] >>=4;
				}

				else if(In_Packet[1] == 15)
				{
					utempBR=(unsigned int)(FirmwareRevision*100);
					In_Packet[2] = (unsigned char)(utempBR & 0x000000ff);
					utempBR>>=8;
					In_Packet[3] = (unsigned char)(utempBR & 0x000000ff);
				}

				else if(In_Packet[1] == 254)
				{
					ltempBR=LampTimerLifeTime;
					In_Packet[2] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[3] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[4] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[5] = (unsigned char)(ltempBR & 0x000000ff);
				}

				InPktRdy=1;
				USBTXLED=LEDON;	// Turn on the USB LED to show activity
				USBTXCtr=10;		// Set counter for T2 ISR to turn off 10 x 0.1s later
			}
		}


		// Out Report 6 = Write Setup Info
		else if(Out_Packet[0] == 6)
		{	
			if(Out_Packet[1] == 1)
			{
				if(Out_Packet[2]<=100 && Out_Packet[2]>0)
					ResponseTime = Out_Packet[2];
			}
			else if(Out_Packet[1] == 2)
			{	
				if(Out_Packet[2]==0)
					LumUnits = Out_Packet[2];
				else if((Out_Packet[2]<5) && ((UserCalName[Out_Packet[2]-1][0] & 0x80) == 0))
					LumUnits = Out_Packet[2];

				NewGain=1;		// causes lum exponent to be reprinted
			}
			else if(Out_Packet[1] == 3)
			{
				ltempBR = Out_Packet[5];
				ltempBR<<=8;
				ltempBR |= (unsigned long)Out_Packet[4];
				ltempBR<<=8;
				ltempBR |= (unsigned long)Out_Packet[3];
				ltempBR<<=8;
				ltempBR |= (unsigned long)Out_Packet[2];
				LampTimer=ltempBR;
			}
			else if(Out_Packet[1] == 4)
			{	// DisplayOff can't be cleared directly, it must be done by Display_Keypad_Update
				if(Out_Packet[2]==1)
					DisplayOff = 1;
				else if(DisplayOff!=0)
					DisplayOff++;
			}
			else if(Out_Packet[1] == 5)
			{
				if(Out_Packet[2] == 0)
						UpdateStat=0;			// Turn AUTOUPDATE off
				else
					UpdateStat=1;			// Turn AUTOUPDATE on
			}
			else if(Out_Packet[1] == 6)
			{
				// Get the new lamp Target
				utempBR = (unsigned int)Out_Packet[3];
				utempBR <<= 8;
				utempBR |= (unsigned int)Out_Packet[2];
				if(utempBR!= LastLampTargetOut)
				{
					LastLampTargetOut = utempBR;
					if(RawStat)
						LampRawTarget = utempBR;
					else
					{
						ftempBR = (float)utempBR;
						ftempBR /= 1000;
						LampAmpsTarget = ftempBR;
						LampAmpsTarget2RawTarget();
					}
				}
			}
			else if(Out_Packet[1] == 7)
			{
				if(Out_Packet[2] <3)
					LampUnits = Out_Packet[2];
			}
			else if(Out_Packet[1] == 8)
			{
				P2 =  (Out_Packet[2]<<4) | (P2 & 0x0f); // Write GPO data to port 2
			}
			else if(Out_Packet[1] == 254)
			{
				ltempBR = Out_Packet[5];
				ltempBR<<=8;
				ltempBR |= (unsigned long)Out_Packet[4];
				ltempBR<<=8;
				ltempBR |= (unsigned long)Out_Packet[3];
				ltempBR<<=8;
				ltempBR |= (unsigned long)Out_Packet[2];
				LampTimerLifeTime=ltempBR;
			}


			if(InPktRdy==0)		// If the last In packet is gone
			{									// Build the In Report #6
				In_Packet[0] = 6;
				In_Packet[1] = Out_Packet[1];
	
				if(In_Packet[1] == 1)
					In_Packet[2] = ResponseTime;
				else if(In_Packet[1] == 2)
					In_Packet[2] = LumUnits;
				else if(In_Packet[1] == 3)
				{
					ltempBR=LampTimer;
					In_Packet[2] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[3] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[4] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[5] = (unsigned char)(ltempBR & 0x000000ff);
				}
				else if(In_Packet[1] == 4)
					In_Packet[2] = DisplayOff;
				else if(In_Packet[1] == 5)
				{
					if(UpdateStat)
						In_Packet[2] = 1;
					else
						In_Packet[2] = 0;
				}
				else if(In_Packet[1] == 6)
				{
					utempBR = (unsigned int)((LampAmpsTarget*1000)+0.5);
					In_Packet[2] = (unsigned char)(utempBR & 0x000000ff);
					utempBR>>=8;
					In_Packet[3] = (unsigned char)(utempBR & 0x000000ff);
				}
				else if(In_Packet[1] == 7)
				{
					In_Packet[2] = LampUnits;
				}
				else if(In_Packet[1] == 8)
				{
					In_Packet[2] = P1 & 0x0E;
					if(GPI0)
						In_Packet[2] |= 0x01;				
					In_Packet[3] = P2 & 0xf0;
					In_Packet[3] >>=4;
				}

				else if(In_Packet[1] == 15)
				{
					utempBR=(unsigned int)(FirmwareRevision*100);
					In_Packet[2] = (unsigned char)(utempBR & 0x000000ff);
					utempBR>>=8;
					In_Packet[3] = (unsigned char)(utempBR & 0x000000ff);
				}

				else if(In_Packet[1] == 254)
				{
					ltempBR=LampTimerLifeTime;
					In_Packet[2] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[3] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[4] = (unsigned char)(ltempBR & 0x000000ff);
					ltempBR>>=8;
					In_Packet[5] = (unsigned char)(ltempBR & 0x000000ff);
				}

				InPktRdy=1;
				USBTXLED=LEDON;	// Turn on the USB LED to show activity
				USBTXCtr=10;		// Set counter for T2 ISR to turn off 10 x 0.1s later
			}
		}


		// Out Report 7 = Zero the Luminance Monitor
		else if(Out_Packet[0] == 7)
		{	
			ZeroLum();
		}

		// Out Report 8 = Read ColorTemp
		else if(Out_Packet[0] == 8)
		{

			if(InPktRdy==0)		// If the last In packet is gone
			{									// Build the In Report #8
				In_Packet[0] = 8;
				In_Packet[1] = (ColorTempInt & 0x00ff);	// Color Temp LSB
				In_Packet[2] = (ColorTempInt>>8);				// Color Temp MSB
				InPktRdy=1;
				USBTXLED=LEDON;	// Turn on the USB LED to show activity
				USBTXCtr=10;		// Set counter for T2 ISR to turn off 10 x 0.1s later
			}
		}

		// Out Report 0 = Flush Comm
		else if(OutPktRdy && (Out_Packet[0] == 0))
		{	
			UpdateStat=0;			// Turn AUTOUPDATE off
			InPktRdy=0;
		}

		// Turn off the semaphore before exit
		OutPktRdy=0;
	}



// When autoupdate, send a report 1 and 8 for signal monitoring every 100mS
	if(UpdateStat && NewUpdate && InPktRdy==0)
	{									// Build the In Report #1
		In_Packet[0] = 1;
		In_Packet[1] = Status;
		In_Packet[2] = (LumAmpsInt & 0x00ff);		// Detector Current LSB
		In_Packet[3] = (LumAmpsInt>>8);					// Detector Current MSB
		In_Packet[4] = (LampAmpsInt & 0x00ff);	// Lamp Current LSB
		In_Packet[5] = (LampAmpsInt>>8);				// Lamp Current MSB
		In_Packet[6] = (LampVoltsInt & 0x00ff);	// Lamp Voltage LSB
		In_Packet[7] = (LampVoltsInt>>8);				// Lamp Voltage MSB
		NewUpdate=0;
		NewUpd100=1;
		InPktRdy=1;
		USBTXLED=LEDON;	// Turn on the USB LED to show activity
		USBTXCtr=10;		// Set counter for T2 ISR to turn off 10 x 0.1s later
	}

	else if(UpdateStat && NewUpd100 && InPktRdy==0)
	{									// Build the In Report #8
		In_Packet[0] = 8;
		In_Packet[1] = (ColorTempInt & 0x00ff);	// Color Temp LSB
		In_Packet[2] = (ColorTempInt>>8);				// Color Temp MSB
		NewUpd100=0;
		InPktRdy=1;
		USBTXLED=LEDON;	// Turn on the USB LED to show activity
		USBTXCtr=10;		// Set counter for T2 ISR to turn off 10 x 0.1s later
	}

}












