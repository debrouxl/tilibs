/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
	IntelHex format reader/writer for TI8X FLASH calculators.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include "macros.h"
#include "intelhex.h"
#include "export2.h"
#include "logging.h"

/* Constants */

#define HEX_DATA        0x00     // data packet
#define HEX_END         0x01     // end of section (1 for app, 3 for OS)
#define HEX_PAGE        0x02     // page change
#define HEX_EOF         0x03     // end of file (custom)

#define PKT_MAX         32       // 32 bytes max
#define BLK_MAX         16384    // 16KB max


/* TI8X+ FLASH files contains text data. */
static uint8_t read_byte(FILE * f)
{
	unsigned int b;

	if (fscanf(f, "%02X", &b) < 1)
	{
		static int warn_read_byte = 0;
		if (!warn_read_byte)
		{
			tifiles_warning("intelhex: couldn't read byte");
			warn_read_byte++;
		}
		b = 0; /* 0 is better than random garbage */
	}
	return (uint8_t)b;
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

	Returns : 0 if success, a negative value otherwise.
*/
static int hex_packet_read(FILE *f, uint8_t *size, uint16_t *addr, uint8_t *type, uint8_t *data)
{
	uint8_t sum = 0;
	int c = fgetc(f);
	if (c != ':')
	{
		printf("Unexpected char: <%c> = %02X\n", c, c);
		return -1;
	}

	*size = read_byte(f);
	uint16_t localaddr = ((uint16_t)(read_byte(f))) << 8;
	localaddr |= read_byte(f);
	*addr = localaddr;
	*type = read_byte(f);

	if(*size > PKT_MAX)
	{
		return -2;
	}

	sum = *size + MSB(*addr) + LSB(*addr) + *type;

	for (int i = 0; i < *size; i++) 
	{
		data[i] = read_byte(f);
		sum += data[i];
	}

	const uint8_t checksum = read_byte(f); // verify checksum of block
	if (LSB(sum + checksum))
	{
		return -3;
	}

	{
		// check for end of file without mangling data checksum
		long pos = ftell(f);

		if (pos < 0)
		{
			return -4;
		}

		c = fgetc(f);

		if (c == 0x0d) // CR
		{
			c = fgetc(f);
			if (c == 0x0a) // LF
			{
read_lf:
				c = fgetc(f);
				if (c == EOF) // EOF checking is set to keep compatibility with old generated FLASH files (buggy)
				{
					goto eof;
				}
			}
			else // Unconditional else: either c is EOF, or the format is invalid.
			{
				goto eof;
			}
		}
		else if (c == 0x0a) // LF
		{
			pos--;
			goto read_lf;
		}
		else
		{
eof:
			// Invalid format / end of file.
			*type = HEX_EOF;
			if (fseek(f, pos, SEEK_SET) < 0)
			{
				return -4;
			}
			return 0;
		}

		if (fseek(f, pos+2, SEEK_SET) < 0)
		{
			return -4;
		}
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
	int new_page = 0;

	// reset condition
	if (f == nullptr)
	{
		flag = 0x80;
		flash_page = flash_addr = 0;
		return 0;
	}
	if (size == nullptr || addr == nullptr || type == nullptr || data == nullptr || page == nullptr)
	{
		return -1;
	}

	// fill-up buffer with 0xff (flash)
	memset(data, 0xff, BLK_MAX);

	*addr = flash_addr;
	*type = flag;
	*page = flash_page;
	*size = 0;

	// load data
	for (int i = 0; i < BLK_MAX; )
	{
		uint8_t pkt_size, pkt_type;
		uint8_t pkt_data[PKT_MAX];
		uint16_t pkt_addr;

		// read packet
		const int ret = hex_packet_read(f, &pkt_size, &pkt_addr, &pkt_type, pkt_data);
		if(ret < 0)
		{
			return ret; // THIS RETURNS !
		}

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
			flash_page = (((uint16_t)(pkt_data[0])) << 8) | pkt_data[1];
			new_page = !0;
			break;

		case HEX_EOF: 
			// end of file
			return EOF;

		default: 
			printf("Unexpected char: <%c> = %02x\n", pkt_type, pkt_type);
			return -1;
		}
	}

	return 0;
}

static int write_byte(uint8_t b, FILE * f)
{
	fprintf(f, "%02X", b);
	return 2;
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

	Returns : number of chars written to file.
*/
static int hex_packet_write(FILE *f, uint8_t size, uint16_t addr, uint8_t type_, uint8_t *data)
{
	int num = 0;
	const uint8_t type = (type_ == HEX_EOF ? HEX_END : type_);

	fputc(':', f); num++;
	num += write_byte((uint8_t)size, f);
	num += write_byte(MSB(addr), f);
	num += write_byte(LSB(addr), f);
	num += write_byte(type, f);

	int sum = size + MSB(addr) + LSB(addr) + type;
	for (int i = 0; i < size; i++) 
	{
		num += write_byte(data[i], f);
		sum += data[i];
	}

	num += write_byte((uint8_t)(0x100 - LSB(sum)), f);

	if(type_ != HEX_EOF)
	{
		fputc(0x0D, f);	num++; // CR
		fputc(0x0A, f); num++; // LF
	}

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
	@extra_bytes : number of additional 0xff bytes to add after the end of the data

	Write a data block (page/segment) to FLASH file. 

	Returns : number of chars written to file.
*/
int hex_block_write(FILE *f, uint16_t size, uint16_t addr, uint8_t type, uint8_t *data, uint16_t page, uint16_t extra_bytes)
{
	int bytes_written = 0;
	static int old_flag = 0x80;
	uint8_t buf[PKT_MAX];
	int new_section = 0;

	// write end block
	if(!size && !addr && !type && !data && !page)
	{
		return hex_packet_write(f, 0, 0x0000, HEX_EOF, nullptr);
	}

	// new section (FLASH OS only)
	if(old_flag == 0x80 && type == 0x00)
	{
		new_section = !0;
	}

	if(old_flag != type)
	{
		old_flag = type;
		bytes_written += hex_packet_write(f, 0, 0x0000, HEX_END, nullptr);
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
		bytes_written += hex_packet_write(f, 2, 0x0000, HEX_PAGE, buf);
		new_section = 0;
	}

	// write a block (=page)
	while (size > 0 || extra_bytes > 0)
	{
		const int n = (size > PKT_MAX ? PKT_MAX : size);
		if (n > 0)
		{
			memcpy(buf, data, n);
			size -= n;
			data += n;
		}

		const int m = (extra_bytes > PKT_MAX - n ? PKT_MAX - n : extra_bytes);
		if (m > 0)
		{
			memset(buf + n, 0xff, m);
			extra_bytes -= m;
		}

		bytes_written += hex_packet_write(f, n + m, addr, HEX_DATA, buf);
		addr += n + m;
	}

	return bytes_written;
}
