// Header File
// Created 12/07/2005; 18:32:26

#define VERSION		"1.00"			// Version

#define TIMEOUT		2000				// in ms

/* CMD | LEN | DATA | CHK */
#define	CMD_IS_READY	0xAA55
#define CMD_KO				0x0000
#define CMD_OK				0x0001
#define CMD_EXIT			0x0002
#define CMD_REQ_SIZE	0x0003
#define CMD_ERROR			0x0004	// unused !
#define CMD_REQ_BLOCK	0x0005
#define CMD_DATA1			0x0006
#define CMD_DATA2			0x0007
#define CMD_NONE			0xffff

typedef unsigned char 			uint8_t;
typedef unsigned short int  uint16_t;
typedef unsigned long int		uint32_t;

#define MSB(v)	(v >> 8)
#define LSB(v)	(v & 0xff)

#define LE_BE(v) (((v & 0xff000000) >> 24) | ((v & 0x00ff0000) >>  8) | ((v & 0x0000ff00) <<  8) | ((v & 0x000000ff) << 24))	