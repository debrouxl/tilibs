///* Hey EMACS -*- linux-c -*- */

/*  RomDumper - an TI89/92/92+/V200PLT/Titanium ROM dumper
 *
 *  Copyright (c) 2004-2005, Romain Liévin for the TiLP and TiEmu projects
 *  Copyright (c) 2006, Romain Liévin for the Direct USB port
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
 
/*
	Note: this program could be optimized because I didn't write it 
	with embedded view in mind.
*/

#include <tigcclib.h>         // Include All Header Files
#include "romdump.h"
#include "dusb.h"

#define ROM_size ((uint32_t)(0x200000 << (V200 || ((uint32_t)ROM_base == 0x800000))))

// --- Packet Layer

#define BLK_SIZE (4096)

static uint16_t CheckSum(uint8_t* data, uint16_t len)
{
	uint16_t i;
	uint16_t sum = 0;

	for(i = 0; i < len; i++)
	{
		sum += data[i];
	}

	return sum;
}

static int SendPacket(uint8_t* buf, uint16_t cmd, uint16_t len, uint8_t* data)
{
	short ret;
	uint16_t sum;

	// command & length
	buf[0] = LSB(cmd);
	buf[1] = MSB(cmd);
	buf[2] = LSB(len);
	buf[3] = MSB(len);

	ret = USB_SendData(buf, 4, TIMEOUT);
	if(ret) return ret;

	// data
	ret = USB_SendData(data, len, TIMEOUT);
	if(ret) return ret;

	// checksum
	sum = CheckSum(buf, 4) + CheckSum(data, len);
	buf[0] = LSB(sum);
	buf[1] = MSB(sum);

	// send
	ret = USB_SendData(buf, 2, TIMEOUT);
	if(ret) return ret;

	return 0;
}

static int RecvPacket(uint8_t* buf, uint16_t* cmd, uint16_t* len, uint8_t* data)
{
	short ret;
	uint16_t sum, tmp;

	// default values
	*cmd = CMD_NONE;
	*len = 0;

	// any packet has always at least 4 bytes (cmd, len)
	ret = USB_RecvData(buf, 4, TIMEOUT);
	if(ret) return ret;

	*cmd = (buf[1] << 8) | buf[0];
	*len = (buf[3] << 8) | buf[2];
	tmp = CheckSum(buf, 4);

	// data part
	if(data)
	{
		ret = USB_RecvData(data, *len, TIMEOUT);
		if(ret) return ret;
	}

	// checksum
	ret = USB_RecvData(buf+*len, 2, TIMEOUT);
	if(ret) return ret;

	sum = (buf[*len+1] << 8) | buf[*len+0];

	if (sum != CheckSum(data, *len) + tmp)
		return -1;

	return 0;
}

// --- Command Layer

static inline int Send_OK(uint8_t* buf)
{
	return SendPacket(buf, CMD_OK, 0, NULL);
}

static inline int Send_KO(uint8_t* buf)
{
	return SendPacket(buf, CMD_KO, 0, NULL);
}

static inline int Send_SIZE(uint8_t* buf, uint32_t size)
{
	uint32_t le_size = LE_BE(size);

	return SendPacket(buf, CMD_REQ_SIZE, 4, (uint8_t *)&le_size);
}

static inline int Send_DATA(uint8_t* buf, uint16_t len, uint8_t* data)
{
	return SendPacket(buf, CMD_DATA1, len, data);
}

static inline int Send_RDATA(uint8_t* buf, uint16_t data)
{
	uint8_t tmp[4];

	tmp[0] = LSB(BLK_SIZE);
	tmp[1] = MSB(BLK_SIZE);
	tmp[2] = LSB(data);
	tmp[3] = MSB(data);

	return SendPacket(buf, CMD_DATA2, 4, tmp);
}

static inline int Send_EXIT(uint8_t* buf)
{
	return SendPacket(buf, CMD_EXIT, 0, NULL);
}

static inline int Send_ERR(uint8_t* buf)
{
	return SendPacket(buf, CMD_ERROR, 0, NULL);
}

// --- Dumper Layer

static int Dump(uint8_t* buf)
{
	int exit = 0;
	int ret;
	uint16_t cmd, len;
	uint32_t addr;
	char str[30];
	unsigned int i;
	uint8_t* ptr;

	while(!exit)
	{
		// wait for command
		ret = RecvPacket(buf, &cmd, &len, buf);
		//if(ret) continue;	// pb with ret val

		// or keypress
		if(kbhit()) 
			exit = !0;

		// and process
		switch(cmd)
		{
			case CMD_IS_READY: 
				Send_OK(buf);
			break;
			case CMD_EXIT: 
				Send_EXIT(buf);
				exit = !0; 
			break;
			case CMD_REQ_SIZE: 
				Send_SIZE(buf, ROM_size);

				sprintf(str, "Size: %lu KB", ROM_size >> 10);
				DrawStr(0, 60, str, A_REPLACE);
			break;
			case CMD_REQ_BLOCK: 
				addr  = ((uint32_t)buf[0] <<  0); addr |= ((uint32_t)buf[1] <<  8);
				addr |= ((uint32_t)buf[2] << 16); addr |= ((uint32_t)buf[3] << 24);

				if(addr > ROM_size)
					Send_ERR(buf);

#if 0
				if(addr >= 0x10000 && addr < 0x12000)	// read protected (certificate)
					addr = 0;	
				if(addr >= 0x18000 && addr < 0x1A000)	// read protected (certificate)
					addr = 0;
#endif

				//sprintf(str, "%02x %02x %02x %02x     ", buf[0], buf[1], buf[2], buf[3]);
				//DrawStr(0, 80, str, A_REPLACE);
				sprintf(str, "Done: %lu/%luKB     ", addr >> 10, ROM_size >> 10);
				DrawStr(0, 60, str, A_REPLACE);
				
				// Check for filled blocks (like 0xff)
				ptr = (uint8_t *)(ROM_base + addr);
				for(i = 0; i < BLK_SIZE; i++)
					if(ptr[i] != ptr[0])
						break;

				if(i == BLK_SIZE)
					Send_RDATA(buf, ptr[0]);
				else
					Send_DATA(buf, BLK_SIZE, (uint8_t *)(ROM_base + addr));
			break;
			case CMD_NONE:
			break;
			default:
			break;
		}
	}	

	USBLinkClose();	
	return 0;
}

// --- Main Function


void _main(void)
{
	char str[30];
	uint8_t buf[BLK_SIZE + 3*2];

	ClrScr ();
	FontSetSys (F_8x10);

	sprintf(str, "RomDumper v%s", VERSION);
	DrawStr(0, 0, str, A_NORMAL);

	sprintf(str, "Type: HW%i", HW_VERSION);
	DrawStr(0, 20, str, A_NORMAL);

	sprintf(str, "ROM base: 0x%lx", (uint32_t)ROM_base);
	DrawStr(0, 40, str, A_NORMAL);  

	sprintf(str, "by The TiLP Team");
	DrawStr(0, 80, str, A_NORMAL);  

	Dump(buf);

	return;
}

