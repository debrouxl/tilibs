#ifndef USB_DEFS_H
#define USB_DEFS_H

#include <windows.h>

//
// Filename for accessing the USB device
//
#define USB_DEVICE_FILENAME		"\\\\.\\Thermometer_0"
#define FNCT_NUMBER				4

//
// Device command values
//
#define DC_READ_THERM	0x0b	// Read temperature
/*
	lIn:  0x0b
	lOut: button, sign, temp, status
*/

#define DC_SET_LED		0x0e	// Set LED brightness
/*
	lIn:  0x0e, brightness (0..15)
	lOut: status
*/
#define DC_READ_PORT	0x14	// Read port0/1 of the µC
/*
	lIn:  0x14, port (0 or 1)
	lOut: status, value
*/
#define DC_WRITE_PORT	0x15	// Write port0/1 of the µC
/*
	lIn:  0x15, port( 0 or 1), value
	lOut: status
*/
#define DC_READ_RAM		0x16	// Read the RAM
/*
	lIn:  0x16, address (0..255)
	lOut: status, value
*/
#define DC_WRITE_RAM	0x17	// Write the RAM
/*
	lIn:  0x17, address (0..255), value (0..255)
	lOut: statui
*/
#define DC_READ_ROM		0x18	// Read the ROM content
/*
	lIn:  0x18, address (0..255), index
	lOut: status, value
*/

//
// Port 0 & 1 are mirrored in RAM at addresses 0x2e & 0x2f
//
#define PORT0_IN_RAM	0x2e
#define PORT1_IN_RAM	0x2f

//
// Buffer for the Device I/O functions
//
struct _lIn {
  BYTE bFunction;
  BYTE bValue1;
  BYTE bValue2;
  BYTE bValue3;
};

struct _lOut {
  BYTE bAck;
  BYTE bValue1;
  BYTE bValue2;
  BYTE bValue3;
};

#endif
