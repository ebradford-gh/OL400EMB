//-----------------------------------------------------------------------------
// 400PCB.h
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: Hardware-specific definitions and function prototypes
// AUTH: Emmett Bradford
// DATE: 3-17-06
//
//
// Target: C8051F32x
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//
// REVISIONS:
//	1.00	03/17/06	Initial Release
//	2.40	01/28/16	Added Delay_StopWatch_BuildReport
//
//

// Key Identities
#define	LAMPOFFKEY	0x01
#define	ENTERKEY 		0x02
#define	CURRKEY 		0x04
#define	MENUKEY 		0x08
#define	LAMPONKEY 	0x10
#define	ZEROKEY 		0x20
#define	ESCKEY			0x40
#define	GAINKEY 		0x80

#define	UPKEY 			LAMPONKEY
#define	DOWNKEY			LAMPOFFKEY
#define	LEFTKEY 		GAINKEY
#define	RIGHTKEY 		ZEROKEY



// Preamp Identities
#define	IE3					0x01
#define	IE5					0x02
#define	IE7					0x04
#define	IE9					0x00
#define	VE0					0x20
#define	VE1					0x40
#define	VE2					0x80

//// Event Detector Identities
#define	VCMPL 			0x01			// Low = bad
#define	UNDER				0x02			// Low = bad
#define	RNGOKN			0x04			// Low = OK
#define	OVLD				0x08			// Low = Bad
//#define	NEWGAIN			0x10			// New Gain Range
#define	NEWADC			0x80			// New VFC Total



#define	AUTORNG			0x08			// 1 = Auto
#define	AUTOUPDATE	0x10			// 1 = Auto, 0 = wait for OUT report 
#define	RAWUNITS		0x20			// 1 = Raw, 0 = calibrated
#define	CMPL				0x40			// Low = Bad
#define	LAMPSTAT		0x80			// 1 = On, 0 = Off
/*
B7 	Lamp Status: 0=Standby, 1=On
B6	Regulation Compliance: 0=OK, 1= VUNREG too low
B5	Raw Units
B4	Auto-Update
B3	Auto-Range
B2  Range msb
B1	Range lsb
B0	Range lsb
*/

/*  BYTE Registers  */
//sfr VDM0CN   = 0xFF;    /* VDD MONITOR CONTROL                                      */

/*  BIT Registers  */

// PCSRAD PCB PORT ASSIGNMENTS
sbit	SDEN0		= P1^4;		//P14
sbit	SDEN1		= P1^5;		//P15
sbit	SDEN2		= P1^6;		//P16
sbit	SDEN3		= P1^7;		//P17


#define	LEDON		1			// 1 = On
#define	LEDOFF	0			// 0 = Off

sbit	USBTXLED= P2^2;		//P22 0=on
sbit	USBRXLED= P2^3;		//P23 0=on

sbit	MSTEP		= P0^4;		//P04
sbit	MDRXN		= P0^7;		//P25
sbit	ME0			= P2^0;		//P20
sbit	PDWN		= P2^1;		//P21

sbit	GPI0		= P0^5;		//P05
sbit	GPI1		= P1^1;		//P11
sbit	GPI2		= P1^2;		//P12
sbit	GPI3		= P1^3;		//P13

sbit	GPO0		= P2^4;		//P24
sbit	GPO1		= P2^5;		//P25
sbit	GPO2		= P2^6;		//P26
sbit	GPO3		= P2^7;		//P27


//------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------

//Functions in 400MAIN.C
void Suspend_Device(void);
void ChangeGain (unsigned char Range);
void Update (void);
void BuildReport (void);
void LampAmpsTarget2RawTarget (void);
float Curr2CTemp (float CurrVal);
float CTemp2Curr (float CTempVal);

//Functions in 400DISP.C
void Delay (unsigned int time);
void Display_Init (void);
void ClearDisplayBuff (void);
void Display (char DisplayPosition, char *string);
void Display_Keypad_Update (void);
void Display_Keypad_Transfer (char Display, char Data, char Command);
void Keypad_Rapidfire (unsigned int Limit);
void Write_Preamp (char Data);
void Write_LampDAC (unsigned int Data);
void Delay_StopWatch_BuildReport (unsigned int time);

//Functions in 400INIT.C
void Timer_Init (void);                       // Start timer 2 for use by ADC and to check switches
void Adc_Init (void);  
void Port_Init (void);
void Initialize (void);
void SPI0_Init (void);
void Check_Power(void);


//Functions in 400MENU.C
void Check_Keypad (void);
void Menu (void);
void SetResponseTime (void);
void SelectGain (void);
void SelectMonitorUnits (void);
void SelectLampUnits (void);
void SetLampTimer (void);
void ViewLampTimer (void);
void ZeroLum (void);
void ShowDeviceInfo (void);
void COMMMonitor (void);
void ViewCTempCal (void);
void ViewLumCal (void);


void AdjustLampTarget (void);
float NumericEntry (float Number, float Min, float Max, char DisplayPos, char width, char precision);


