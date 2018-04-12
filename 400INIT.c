//-----------------------------------------------------------------------------
// 400INIT.c
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: MCU Initialization Functions
// AUTH: Emmett Bradford
// DATE: 3-17-06
//
//
// Target: C8051F32x
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//
// REVISIONS:
//	1.00	03/17/06	Initial Release
//	2.00	02/14/09	Added power down detection and V2.00 PLD features
//	2.10	03/17/09	Added FLSCL setting for F347
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "c8051f340.h"
#include "stddef.h"
#include "400PCB.h"
#include "400MEM.h"
#include "USB_REGISTER.h"
#include "USB_MAIN.h"
#include "USB_DESCRIPTOR.h"


//-----------------------------------------------------------------------------
// Initialization Functions
//-----------------------------------------------------------------------------

//-------------------------
// Initialize
//-------------------------
// Called when a DEV_CONFIGURED interrupt is received.
// - Enables all peripherals needed for the application
//
void Initialize (void)
{
	PCA0MD &= ~0x40;				// Disable Watchdog timer
	RSTSRC = 0x00;					// Disable all reset sources 
	Check_Power();					// Check for power-down before initialization
	Sysclk_Init();					// Initialize System and USB Clock	
	Port_Init();						// Initialize crossbar and GPIO
  Usb0_Init();						// Initialize USB0
	SPI0_Init();						// Initialize SPIO
	Delay(200);
	Display_Init();					// Initialize LCD Display
	Write_Preamp(IE3|VE0);	// Initialize Preamp
	Timer_Init();						// Initialize timers (Will begin periodic updates)
  EA = 1;                 // Global Interrupt enable
}

//-------------------------
// Sysclk_Init
//-------------------------
// SYSLCK Initialization
// - Initialize the system clock and USB clock
//
void Sysclk_Init(void)
{
#ifdef _USB_LOW_SPEED_
   OSCICN |= 0x03;							// Int Osc = Max = 12MHz, enable missing clock detector
   CLKSEL  = SYS_INT_OSC;				// Select System clock
   CLKSEL |= USB_INT_OSC_DIV_2;	// Select USB clock
#else
   OSCICN |= 0x03;							// Int Osc = Max = 12MHz, enable missing clock detector
   CLKMUL  = 0x00;							// Int Osc = multiplier input 
   CLKMUL |= 0x80;							// Enable clock multiplier
   CLKMUL |= 0xC0;							// Initialize the clock multiplier
   Delay(1);										// Delay for clock multiplier to begin
   while(!(CLKMUL & 0x20));			// Wait for multiplier to lock
   CLKSEL  = SYS_INT_OSC;				// Select system clock  
   CLKSEL |= USB_4X_CLOCK;			// Select USB clock
#endif  /* _USB_LOW_SPEED_ */ 

	FLSCL=0x90;										// Set the Flash Read Time for 48MHz

}


//-------------------------
// Usb0_Init
//-------------------------
// USB Initialization
// - Initialize USB0
// - Enable USB0 interrupts
// - Enable USB0 transceiver
// - Enable USB0 with suspend detection
//
void Usb0_Init(void)
{
   POLL_WRITE_BYTE(POWER,  0x08);          // Force Asynchronous USB Reset
   POLL_WRITE_BYTE(IN1IE,  0x07);          // Enable Endpoint 0-2 in interrupts
   POLL_WRITE_BYTE(OUT1IE, 0x07);          // Enable Endpoint 0-2 out interrupts
   POLL_WRITE_BYTE(CMIE,   0x07);          // Enable Reset, Resume, and Suspend interrupts

#ifdef _USB_LOW_SPEED_
   USB0XCN = 0xC0;                         // Enable transceiver; select low speed
   POLL_WRITE_BYTE(CLKREC, 0xA0);          // Enable clock recovery; single-step mode
                                           // disabled; low speed mode enabled
#else                                      
   USB0XCN = 0xE0;                         // Enable transceiver; select full speed
   POLL_WRITE_BYTE(CLKREC, 0x80);          // Enable clock recovery, single-step mode
                                           // disabled
#endif /* _USB_LOW_SPEED_ */

   EIE1 |= 0x02;                           // Enable USB0 Interrupts
   EA = 1;                                 // Global Interrupt enable
                                           // Enable USB0 by clearing the USB Inhibit bit
   POLL_WRITE_BYTE(POWER,  0x01);          // and enable suspend detection
}


/*-------------------------
// Port Initialization
//-------------------------
// Configure the Crossbar and GPIO ports.
//
OL457 C8051F320 Peripheral I/O						
PIO#	Pin#	I/O	Fxn Used				
0.0		2		O		SCLK				SSI CLK
0.1		1		I		SDIN				SSI Data In
0.2		32		O		SDOUT				SSI Data Out
0.3		31		I		2MHz				VF Master Clock (T2)
0.4		30		I		LMPTYP2				Lamp Type Bit 2
0.5		29		I		GPI0				Aux Input 0
0.6		28		O		VF2					Lamp Voltage ECI
0.7		27		I		LMPTYP3				Lamp Type Bit 3
1.0		26		O		VF1					Luminance T1
1.1		25		I		GPI1				Aux Input 1
1.2		24		I		GPI2				Aux Input 2
1.3		23		I		GPI3				Aux Input 3
1.4		22		O		SDEN0				PLD/display'595
1.5		21		O		SDEN1				RELAYS'595
1.6		20		O		SDEN2				DAC I
1.7		19		O		SDEN3				Spare
2.0		18		I		LMPTYP0				Lamp Type Bit 0
2.1		17		I		LMPTYP1				Lamp Type Bit 1
2.2		16		O		USB Tx LED	SW-driven indicator
2.3		15		O		USB Rx LED	SW-driven indicator
2.4		14		O		MSTEP				Motor Step Output		
2.5		13		O		MDRXN				Motor Direction Output
2.6		12		O		ME0					Motor Enable 0
2.7		11		O		ME1					Motor Enable 1

Port 2   Port 1   Port 0
OOOOOOII OOOOIIIO IOIIIOIO
11111100 11110001 01000101
*/					

/*-------------------------
// Port Initialization
//-------------------------
// Configure the Crossbar and GPIO ports.
//

OL457 C8051F320 Peripheral I/O						
PIO#	Pin#	I/O	Fxn Used				
0.0		2		O		SCLK				SSI CLK
0.1		1		I		SDIN				SSI Data In
0.2		32		O		SDOUT				SSI Data Out
0.3		31		I		2MHz				VF Master Clock (T2)
0.4		30		O		MDRXN				Motor Direction
0.5		29		I		GPI0				Aux Input 0
0.6		28		O		VF2					Lamp Voltage ECI
0.7		27		O		MSTEP				Steps for ME0 or ME1
1.0		26		O		VF1					Luminance T1
1.1		25		I		GPI1				Aux Input 1
1.2		24		I		GPI2				Aux Input 2
1.3		23		I		GPI3				Aux Input 3
1.4		22		O		SDEN0				PLD/display'595
1.5		21		O		SDEN1				RELAYS'595
1.6		20		O		SDEN2				DAC I
1.7		19		O		SDEN3				Spare
2.0		18		O		ME0					Motor Enable 0
2.1		17		O		PWDN				Power-Down Detect
2.2		16		O		USB Tx LED			SW-driven indicator
2.3		15		O		USB Rx LED			SW-driven indicator
2.4		14		O		GPO0				Aux Output 0
2.5		13		O		GPO1				Aux Output 1
2.6		12		O		GPO2				Aux Output 2
2.7		11		O		GPO3				Aux Output 3


Port 2   Port 1   Port 0
OOOOOOOO OOOOIIIO OOI0IOIO
11111111 11110001 11010101
FF       F1       D5
*/					



void Port_Init(void)
{  
// PxMDIN Port pin input type 0=Analog, 1=Digital (default)
//	P0MDIN		= 0xFF;		// Port 0 pins 0-7
//	P1MDIN		= 0xFF;		// Port 1 pins 0-7
//	P2MDIN		= 0xFF;		// Port 2 pins 0-7

//V1.00 PxMDOUT Port pin output type 0=OD (default), 1=Push-Pull
//	P0MDOUT = 0xD5;			// Port 0 pins 1,3,5=0=OD, 0,2,4,6,7=1=PP
//	P1MDOUT = 0xF1;			// Port 1 pins 1-3=0=OD, 0,4-7=1=PP
//	P2MDOUT = 0xFE;			// Port 2 pins 0=0=OD, 1-7=1=PP

//V2.00 PxMDOUT Port pin output type 0=OD (default), 1=Push-Pull
	P0MDOUT = 0xD5;			// Port 0 pins 1,3,5=0=OD, 0,2,4,6,7=1=PP
	P1MDOUT = 0xF1;			// Port 1 pins 1-3=0=OD, 0,4-7=1=PP
	P2MDOUT = 0xFD;			// Port 2 pins 0-7=1=PP, 1=0=OD


// PxSKIP  Port pins skipped by crossbar 0=no skip (default), 1=skip
//V1.00	
//	P0SKIP   = 0x08;		// Port 0 pins 0-7 not skipped by crossbar
//V2.00
	P0SKIP   = 0x88;		// Port 0 pins 0-7 not skipped by crossbar
	P1SKIP   = 0x00;		// Port 1 pins 0-7 skipped by crossbar
	P2SKIP   = 0x00;		// Port 2 pins 0-7 skipped by crossbar

// Crossbar Init
	XBR0     = 0x03;		// Enable SPIO and UART
//V1.00	XBR1     = 0x78;		// Enable Crossbar, T0, T1, ECI
//V2.00
	XBR1     = 0xE8;		// Enable Crossbar, T1, ECI, Disable Weak Pullups

// Init the SPI Enables, etc.
	SDEN0=SDEN2=SDEN3=1;
	SDEN1=0;
//	ME0=ME1=MDRXN=0;
	GPO0=GPO1=GPO2=GPO3=0;
	USBRXLED=USBTXLED=LEDOFF;
}


//-------------------------
// SPI0 Initialization
//-------------------------
void SPI0_Init(void)
{  
	SPI0CKR = 0x0B;		//Rate=SYSCLK/(2*(SPI0CKR+1))=12MHz/(2*(SPI0CKR+1))=500kHz
	SPI0CFG = 0x40;		//Master Mode, CKPOL=CKPHA=0;
	SPI0CN  = 0x01;		//3 Wire Mode, SPI enabled
}



//-------------------------
// Timer initialization
//-------------------------
//
void Timer_Init(void)
{
// Init Timer P = PCA
//	PCA0MD = 0x07;			// Disable Watchdog, ECI input, int
	PCA0MD = 0x06;			// Disable Watchdog, ECI input, no int
	PCA0CN = 0x40;			// Start TimerP


// Init Timer 0/1
//V1.00!!	TMOD = 0x55;				// T0,1 are 16-bit counters (mode 1), with ext clk pins

//V2.00 Timer 0 is now used for the MSTEP rate generator!!
	TMOD = 0x51;			// T0,1 are 16-bit counters (mode 1), T1 = extclk, T0 = sysclk/48
	ET0 = 1;				// Enable Timer0 interrupts
	PT0 = 1;				// Enable Timer0 high priority

	TR0 = 1;				// Start Timer0
	TR1 = 1;				// Start Timer1



// Init Timer 2
	TMR2RLH = 0xFE;			// 65536 - (.002S * 2MHz/8) = 0xFE0C
	TMR2RLL = 0x0C;
	TMR2H   = 0xffff;		// Set to reload immediately on first pass

	OSCXCN = 0x20;			// Enable the External CMOS oscillator input
	TMR2CN = 0x01;			// Stop Timer2; Clear TF2, T2CLK=EXT;
//	CKCON &= 0x0F;			// Timer2 clocked based on T2XCLK;
	ET2 = 1;						// Enable Timer2 interrupts
	TR2 = 1;						// Start Timer2
}

/*
//-------------------------
// ADC initialization
//-------------------------
//
void ADC_Init(void)
{
	AMX0P = 0x09;					// Port 2 pin 1
	AMX0N = 0xFF;					// GND
}

// Comparator initialization
//-------------------------
//
void Comparator_Init(void)
{
	CPT0CN = 0x83;					// CPT0 enabled, pos hyst=0, neg hyst=20mV
	CPT0MX = 0x22;					// Mux = P21-P22
//	CPT0MD = 
	
}
*/

//-------------------------
// Check for power-down and enter STOP Mode
//-------------------------
//
void Check_Power(void)
{ 
	if(PDWN==0)		// PDWN = P21 was ME1
	{
	// Enter STOP Mode
		OSCXCN = 0x00;			// Disable the External CMOS oscillator input
		XBR0 = XBR1 = 0;		// Disable the crossbar
		TR0 = TR1 = TR2 =0;	// Disable the timers
		ET0 = ET1 = ET2 =0;	// Disable the timer interrupts
		PCA0CN = 0x00;			// Disable TimerP
	 	USB0XCN = 0x00;     // Disable USB transceiver
		EA=0;								// Disable global interrupts

		PCA0MD &= ~0x40;   // WDTE = 0 (disable watchdog timer)
		VDM0CN = 0x00;     // Disable VDD monitor
		RSTSRC = 0x00;			// Disable all reset sources 
		REG0CN = 0x80;     // Disable vreg
		P0MDIN = 0x00;			// Set Port I/O as analog inputs
		P1MDIN = 0x00;
		P2MDIN = 0x00;
		P3MDIN = 0x00;
		PCON   = 0x02;			// Enter stop mode
	}

}
