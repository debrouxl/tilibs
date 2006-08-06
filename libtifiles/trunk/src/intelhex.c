/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	IntelHex format reader/writer for TI8X FLASH calculators.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdints2.h"
#include "macros.h"
#include "intelhex.h"
#include "export2.h"
#include "logging.h"

/* Constants */

#define HEX_DATA	0x00	// data packet
#define HEX_END		0x01	// end of section (1 for app, 3 for OS)
#define HEX_PAGE	0x02	// page change
#define HEX_EOF		0x03	// end of file (custom)

#define PKT_MAX		32		// 32 bytes max
#define BLK_MAX		16384	// 16KB max


/* TI8X+ FLASH files contains text data. */
static int read_byte(FILE * f, uint8_t *b)
{
	return (fscanf(f, "%02X", b) < 1) ? -1 : 0;
}

/*
	hex_packet_read:
	@f : file descriptor
	@size : size of packet (field #1)
	@addr : addr of packet (field #2)
	@type : type of packet (field #3)
	@data : data of packet (field after #3), 32 bytes max

	Read an IntelHexa block from FLASH file like these:
	- ': 10 0000 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 CR/LF'
	- ': 00 0000 01 FF'
	- ': 02 0000 02 0000 FC'

	Returns : 0 if success, a negative value otherwise:
		-1: stream error
		-2: bad size
		-3: bad checksum
		-4: semicolon not found
		-5: end of file
*/
static int hex_packet_read(FILE *f, uint8_t *size, uint16_t *addr, uint8_t *type, uint8_t *data)
{
  int c, i;
  uint8_t sum, checksum;
  uint8_t tmp;

  sum = 0;
  c = fgetc(f);
  if(c == EOF)
  {
    *type = HEX_EOF;
    return 0;
  }
  else if (c != ':')
    return -4;

  TRYC(read_byte(f, size));
  TRYC(read_byte(f, &tmp)); *addr = tmp << 8;
  TRYC(read_byte(f, &tmp)); *addr |= tmp;
  TRYC(read_byte(f, type));

  if(*size > PKT_MAX)
	  return -2;

  sum = *size + MSB(*addr) + LSB(*addr) + *type;

  for (i = 0; i < *size; i++) 
  {
    TRYC(read_byte(f, data + i));
    sum += data[i];
  }

  TRYC(read_byte(f, &checksum));
  if (LSB(sum + checksum))
    return -3;

  c = fgetc(f);
  if (c == '\r')
    c = fgetc(f);		// skip \r\n (Win32) or \n (Linux)  

  if ((c == EOF) || (c == ' ')) 
  {	
	// end of file
    *type = HEX_EOF;
    return 0;
  }

  return 0;
}

/*
	hex_block_read:
	@f : file descriptor
	@size : size of block
	@addr : address of block
	@type : a flag (0x80 or 0x00)
	@page : page of block	
	@data : the buffer where block is placed (16KB max)

	Read a data block (page or segment) from FLASH file. 
	If all args are set to NULL, this resets the parser.

	Returns : 0 if success, EOF if end of file has been reached.
*/
int hex_block_read(FILE *f, uint16_t *size, uint16_t *addr, uint8_t *type, uint8_t *data, uint16_t *page)
{
	static int flag = 0x80;
	static uint16_t flash_page;
	static uint16_t flash_addr;
	int i;
	int new_page = 0;

	// reset condition: all args set to NULL
	if(!size && !addr && !type && !data && !page)
	{
		flag = 0x80;
		flash_page = flash_addr = 0;
		return 0;
	}

	// fill-up buffer with 0xff (flash)
	memset(data, 0xff, BLK_MAX);

	// load data
	for (i = 0; i < BLK_MAX; )
	{
		int ret;
		uint8_t pkt_size, pkt_type;
		uint8_t pkt_data[PKT_MAX];
		uint16_t pkt_addr;

		// read packet
		ret = hex_packet_read(f, &pkt_size, &pkt_addr, &pkt_type, pkt_data);
		if(ret < 0)	return ret;

		// new block ? Set address
		if(new_page)
		{
			flash_addr = pkt_addr;
			new_page = 0;
		}

		// returned values
		*addr = flash_addr;
		*type = flag;
		*page = flash_page;
		
		// determine what to do
		switch(pkt_type)
		{
		case HEX_DATA:
			// copy data
			memcpy(&data[i], pkt_data, pkt_size);
			i += pkt_size;
			*size = i;
			break;

		case HEX_END: 
			// new section
			flash_addr = 0;
			flash_page = 0;
			flag ^= 0x80;
			if(i == 0)
				break;
			else
				return 0;

		case HEX_PAGE: 
			// new page
			flash_page = (pkt_data[0] << 8) | pkt_data[1];
			new_page = !0;
			break;

		case HEX_EOF: 
			// end of file
			return -5;

		default: 
			return -1;
		}
	}

	return 0;
}

#if 0
TIEXPORT int TICALL test_hex_read(void)
{
	const char *filename = "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\chembio.8Xk";
	//const char *filename = "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\TI84Plus_OS.8Xu";
	FILE *f;
	uint16_t size, addr, page;
	uint8_t type, data[BLK_MAX];
	int ret;

	f = gfopen(filename, "rb");
	if (f == NULL) 
	{
		printf("Unable to open this file: <%s>", filename);
		return -1;
	}

	fseek(f, 0x4e, SEEK_SET);

	ret = hex_block_read(f, NULL, NULL, NULL, NULL, NULL);

	do
	{
		ret = hex_block_read(f, &size, &addr, &type, data, &page);
	} 
	while(!ret);

	fclose(f);

	return 0;
}
#endif

static int write_byte(FILE * f, uint8_t b)
{
	return fprintf(f, "%02X", b);
}

/*
	hex_packet_write:
	@f : file descriptor
	@size : size of packet (field #1)
	@addr : addr of packet (field #2)
	@type : type of packet (field #3)
	@data : data of packet (field after #3), 32 bytes max

	Write an IntelHexa block from FLASH file like these:
	- ': 10 0000 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 CR/LF'
	- ': 00 0000 01 FF'
	- ': 02 0000 02 0000 FC'

	Returns : number of chars written to file or -1 if error.
*/
static int hex_packet_write(FILE *f, uint8_t size, uint16_t addr, uint8_t type, uint8_t *data)
{
  int i;
  int sum;
  int num = 0;
  int ret;

  fputc(':', f); num++;
  ret = write_byte(f, (uint8_t)size);
  if(ret < 0) return ret; num += ret;
  ret = write_byte(f, MSB(addr));
  if(ret < 0) return ret; num += ret;
  ret = write_byte(f, LSB(addr));
  if(ret < 0) return ret; num += ret;
  ret = write_byte(f, type);
  if(ret < 0) return ret; num += ret;

  sum = size + MSB(addr) + LSB(addr) + type;
  for (i = 0; i < size; i++) 
  {
	  ret = write_byte(f, data[i]);
	  if(ret < 0) return ret; num += ret;
      sum += data[i];
  }

  ret = write_byte(f, (uint8_t)(0x100 - LSB(sum)));
  if(ret < 0) return ret; num += ret;

  fputc(0x0D, f); num++;	// CR
  fputc(0x0A, f); num++;	// LF

  return num;
}

/*
	hex_block_write:
	@f : file descriptor
	@size : size of block
	@addr : address of block
	@type : a flag (0x80 or 0x00)
	@page : page of block	
	@data : the buffer where block is placed (16KB max)

	Write a data block (page/segment) to FLASH file. 

	Returns : number of chars written to file or -1 if error
*/
int hex_block_write(FILE *f, uint16_t size, uint16_t addr, uint8_t type, uint8_t *data, uint16_t page)
{
	int i, bytes_written = 0;
	static int old_flag = 0x80;
	int n = size / PKT_MAX;
	int r = size % PKT_MAX;
	uint8_t buf[3];
	int  new_section = 0;
	int ret;

	// write end block
	if(!size && !addr && !type && !data && !page)
		return hex_packet_write(f, 0, 0x0000, HEX_END, NULL);

	// new section (FLASH OS only)
	if(old_flag == 0x80 && type == 0x00)
		new_section = !0;

	if(old_flag != type)
	{
		old_flag = type;
		ret = hex_packet_write(f, 0, 0x0000, HEX_END, NULL);
		if(ret < 0) return ret;	bytes_written += ret;
	}

	// write page
	if(!addr && !page && !new_section)
	{
		// no packet if default address (0) & page (0)
	}
	else
	{
		buf[0] = page >> 8;
		buf[1] = page & 0xff;
		ret = hex_packet_write(f, 2, 0x0000, HEX_PAGE, buf);
		if(ret < 0) return ret;	bytes_written += ret;
		new_section = 0;
	}

	// write a block (=page)
	for(i = 0; i < n * PKT_MAX; i += PKT_MAX)
	{
		ret = hex_packet_write(f, 
		PKT_MAX, 
		(uint16_t)(addr + i), 
		HEX_DATA, 
		data + i);
		if(ret < 0) return ret;	bytes_written += ret;
	}
	if(r > 0)
	{
		ret = hex_packet_write(f, (uint8_t)r, (uint16_t)(addr + i), HEX_DATA, data + i);
		if(ret < 0) return ret;	bytes_written += ret;
	}
	
	return bytes_written;
}
