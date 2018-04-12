//-----------------------------------------------------------------------------
// USB_DESCRIPTOR.c
//-----------------------------------------------------------------------------
// Copyright 2006 Optronic Laboratories, Inc.
//
// Description: USB descriptor data.
// AUTH: Emmett Bradford
// DATE: 3-17-06
//
//
// Target: C8051F32x
// Tool chain: KEIL C51 6.03 / KEIL EVAL C51
//
// REVISIONS:
//	1.00	10/22/11	Initial Release, Ch9 and HID compliant
//
//

#include "USB_REGISTER.h"
#include "USB_MAIN.h"
#include "USB_DESCRIPTOR.h"

//---------------------------
// Descriptor Declarations
//---------------------------
const device_descriptor DeviceDesc = 
{
   18,                  // bLength
   0x01,                // bDescriptorType
//   0x1001,              // bcdUSB (from SiLAbs)
   0x0001,              // bcdUSB
   0x00,                // bDeviceClass
   0x00,                // bDeviceSubClass
   0x00,                // bDeviceProtocol
   EP0_PACKET_SIZE,     // bMaxPacketSize0
   0x740C,              // idVendor  (0C74 = OLI)
//   0x0700,              // idProduct (=7 for 400)
   0x1200,              // idProduct (=18 for 400 w/ control flash xfer)
   0x0001,              // bcdDevice (=1.00)
   0x01,                // iManufacturer
   0x02,                // iProduct     
   0x00,                // iSerialNumber
   0x01                 // bNumConfigurations
}; //end of DeviceDesc

// From "USB Device Class Definition for Human Interface Devices (HID)".
// Section 7.1:
// "When a Get_Descriptor(Configuration) request is issued, 
// it returns the Configuration descriptor, all Interface descriptors,
// all Endpoint descriptors, and the HID descriptor for each interface."
const hid_configuration_descriptor HidConfigDesc =
{

{ // configuration_descriptor hid_configuration_descriptor
   0x09,                // Length
   0x02,                // Type
   0x2900,              // Totallength (= 9+9+9+7+7)
   0x01,                // NumInterfaces
   0x01,                // bConfigurationValue
   0x00,                // iConfiguration
   0xC0,                // bmAttributes = Self-powered, no remote wakeup
   0x00                 // MaxPower (in 2mA units)
},

{ // interface_descriptor hid_interface_descriptor
   0x09,                // bLength
   0x04,                // bDescriptorType
   0x00,                // bInterfaceNumber
   0x00,                // bAlternateSetting
   0x02,                // bNumEndpoints
   0x03,                // bInterfaceClass (3 = HID)
   0x00,                // bInterfaceSubClass
   0x00,                // bInterfaceProcotol
   0x00                 // iInterface
},

{ // class_descriptor hid_descriptor
	0x09,	// bLength
	0x21,	// bDescriptorType
//	0x0101,	// bcdHID
	0x0001,	// bcdHID
	0x00,	// bCountryCode
	0x01,	// bNumDescriptors
	0x22,	// bDescriptorType
	HID_REPORT_DESCRIPTOR_SIZE_LE // wItemLength (total length of report descriptor)
},

// IN endpoint (mandatory for HID)
{ // endpoint_descriptor hid_endpoint_in_descriptor
   0x07,                // bLength
   0x05,                // bDescriptorType	(= endpoint)
   0x81,                // bEndpointAddress	(= IN 1)
   0x03,                // bmAttributes			(= interrupt)
   EP1_PACKET_SIZE_LE,  // MaxPacketSize (LITTLE ENDIAN)
//   0x64                 // bInterval
   0x0A                 // bInterval
},

// OUT endpoint (optional for HID)
{ // endpoint_descriptor hid_endpoint_out_descriptor
   0x07,                // bLength
   0x05,                // bDescriptorType	(= endpoint)
   0x02,                // bEndpointAddress	(= OUT 2)
   0x03,                // bmAttributes			(= interrupt)
   EP2_PACKET_SIZE_LE,  // MaxPacketSize (LITTLE ENDIAN)
//   0x64                 // bInterval
   0x0A                 // bInterval
}

};


//const hid_report_descriptor HidReportDesc =
BYTE const code HidReportDesc[] =
{
    0x06, 0x00, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
//    0x95, 0x0a,                    //   REPORT_COUNT (10)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0xc0                           //   END_COLLECTION
}; //SIZE = 27 = 0x1B


/*
BYTE const code HidReportDesc[] =
{
    0x06, 0x00, 0xff,				// USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,							// USAGE (Vendor Usage 1)
    0xA1, 0x01,             // COLLECTION (Application)
    0x06, 0x00, 0xff,       // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,             // USAGE (Vendor Usage 1)
    0xA1, 0x00,             // COLLECTION (Linked Physical)
		0x15, 0x80,							// Logical Minimum = -128
		0x25, 0x7F,							// Logical Maximum = +127
		0x35, 0x00,							// Physical Minimum = 0
		0x45, 0x7F,							// Physical Minimum = +127

// Report #1 - Read Signal
		0x85, 0x01,							// Report 1
//Input Report #1
		0x09, 0x02,							// Usage
//		0x09, 0x03,							// Usage
		0x75, 0x08,							// Report Size (bits)
		0x95, 0x08,							// Report Count (bytes w/o ID)
		0x81, 0x02,							// Input type = NoNull|Pref|NoWrap|Abs|Var|Data
//Ouput Report #1
		0x09, 0x03,							// Usage
//		0x09, 0x04,							// Usage
		0x75, 0x08,							// Report Size (bits)
		0x95, 0x08,							// Report Count (bytes w/o ID)
		0x91, 0x02,							// Output type = NoNull|Pref|NoWrap|Abs|Var|Data

// Report #2 - Read Flash
		0x85, 0x02,							// Report 2
//Input Report #2
		0x09, 0x04,							// Usage
//		0x09, 0x06,							// Usage
		0x75, 0x08,							// Report Size (bits)
		0x95, 0x08,							// Report Count (bytes w/o ID)
		0x81, 0x02,							// Input type = NoNull|Pref|NoWrap|Abs|Var|Data
//Ouput Report #2
		0x09, 0x05,							// Usage
//		0x09, 0x08,							// Usage
		0x75, 0x08,							// Report Size (bits)
		0x95, 0x08,							// Report Count (bytes w/o ID)
		0x91, 0x02,							// Output type = NoNull|Pref|NoWrap|Abs|Var|Data

// Report #3 - Write Flash
		0x85, 0x03,							// Report 3
//Input Report #3
		0x09, 0x06,							// Usage
//		0x09, 0x0A,							// Usage
		0x75, 0x08,							// Report Size (bits)
		0x95, 0x08,							// Report Count (bytes w/o ID)
		0x81, 0x02,							// Input type = NoNull|Pref|NoWrap|Abs|Var|Data
//Ouput Report #3
		0x09, 0x07,							// Usage
//		0x09, 0x0C,							// Usage
		0x75, 0x08,							// Report Size (bits)
		0x95, 0x08,							// Report Count (bytes w/o ID)
		0x91, 0x02,							// Output type = NoNull|Pref|NoWrap|Abs|Var|Data

		0xC0,										// End Collection
		0xC0										// End Collection
}; // SIZE = 78 = 0x4E
*/

#define STR0LEN 4

code const BYTE String0Desc[STR0LEN] =
{
   STR0LEN, 0x03, 0x09, 0x04
}; //end of String0Desc

#define STR1LEN sizeof("Gooch and Housego")*2

#define STR1LEN sizeof("Gooch and Housego")*2

code const BYTE String1Desc[STR1LEN] =
{
   STR1LEN, 0x03,
   'G', 0,
   'o', 0,
   'o', 0,
   'c', 0,
   'h', 0,
   ' ', 0,
   'a', 0,
   'n', 0,
   'd', 0,
   ' ', 0,
   'H', 0,
   'o', 0,
   'u', 0,
   's', 0,
   'e', 0,
   'g', 0,
   'o', 0

}; //end of String1Desc

#define STR2LEN sizeof("OL 400")*2

code const BYTE String2Desc[STR2LEN] =
{
   STR2LEN, 0x03,
   'O', 0,
   'L', 0,
   ' ', 0,
   '4', 0,
   '0', 0,
   '0', 0

}; //end of String2Desc

BYTE* const code StringDescTable[] = 
{
   String0Desc,
   String1Desc,
   String2Desc
};