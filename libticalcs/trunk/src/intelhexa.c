/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#include <stdio.h>
#include <string.h>

#include "calc_ext.h"
#include "calc_def.h"

static byte read_byte(FILE *f)
{
  int b;
  
  fscanf(f, "%02X", &b);
  return b;
}

/* 
   Read an IntelHexa block from FLASH file
   Format: ': 10 0000 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 CR/LF'
   Format: ': 00 0000 01 FF'
   Format: ': 02 0000 02 0000 FC'
   Return 0 if success, an error code otherwise.
   Type: 
	- 0x00: data block (with page address)
	- 0x01: end block (without end of file)
	- 0x02: TI block (page number)
	- 0x03: end block (with end of file)
*/
static int read_intel_packet(FILE *f, int *n, word *addr, 
			     byte *type, byte *data)
{
  int c, i;
  byte sum, checksum;
  
  sum = 0;
  c = fgetc(f);
  if(c != ':') return -1;
  *n = read_byte(f);
  *addr = read_byte(f) << 8;
  *addr |= read_byte(f);
  *type = read_byte(f);
  sum = *n + MSB(*addr) + LSB(*addr) + *type;
  for(i=0; i<*n; i++)
    {
      data[i] = read_byte(f);
      sum += data[i];
    }
  checksum = read_byte(f);			// verify checksum of block
  if(LSB(sum+checksum)) return -2;
  c = fgetc(f);						// skip \r\n (Win32) or \n (Linux)
  if(c == '\r') c = fgetc(f);		
  if( (c == EOF) || (c == ' ')) 
    {
      DISPLAY("End of file detected\n");
      *type = 3;
      return 0;						// End of File
    }
    
  return 0;
}

/*
	Read a data block from FLASH file
	- addr: the address of block
	- data: the buffer where data are placed, eventually completed with 0x00
	- mode: App or Os. A null value reset internal values.
	Return a negative value if error, a positive value (the same as read_intel_packet).
*/
int read_data_block(FILE *f, word *page_address, word *page_number, 
		    byte *data, int mode)
{
	static word offset = 0x0000;
	int ret = 0;
	int i, k;
	int n;
	int bytes_to_read=0x80;	//number of bytes to read (usually 0x80 or 0x100)

	if(mode & MODE_APPS)
		bytes_to_read = 0x80;
	else if(mode & MODE_AMS)
		bytes_to_read = 0x100;

	if(!((mode & MODE_APPS) || (mode & MODE_AMS)))
	{	// reset page_offset & index
		offset = 0x0000;
		*page_address = 0x0000;
		*page_number = 0x0000;
		return 0;
	}

	for(i=0; i<bytes_to_read; i+=n)
	{
		word addr;
		byte type;
		byte buf[32];

		ret = read_intel_packet(f, &n, &addr, &type, buf);
		if(ret < 0) return ret;

		if(type == 2)											// special block
		{
			offset = 0x4000;
			*page_number = (buf[0] << 8) | buf[1];
			ret = read_intel_packet(f, &n, &addr, &type, buf);	// get a data block
		}

		if( (type == 1) || (type == 3) )						// final block
		{	// fill up to end
			offset = 0x0000;
			*page_number = 0x0000;
			if(i == 0)			// no need to complete block
			{ 
				if(type == 3)
					break; 
			}					
			else
			{
				n = bytes_to_read - i;
				DISPLAY("-> filling block: %i %i\n", i, n);
				for(k=i; k<bytes_to_read; k++) data[k] = 0x00;
				return type;
			}
		}
		else
		{	// copy data
			for(k=0; k<n; k++) data[i+k] = buf[k];

			if(i==0)			// first loop: compute address of block
			{	
				if(mode & MODE_APPS)
					*page_address = addr;
				else
					*page_address = (addr % 0x4000) + offset;

				DISPLAY("offset = %04X, page_address = %04X, page_number = %04X\n", 
					offset, *page_address, *page_number);
			} 
		}
	}
		
	return ret;
}

static int write_byte(byte b, FILE *f)
{
  return fprintf(f, "%02X", b);
}

/* 
   Write an IntelHexa block from FLASH file
   Format: ': 10 0000 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 CR/LF'
   Format: ': 00 0000 01 FF'
   Format: ': 02 0000 02 0000 FC'
   Return 0 if success, an error code otherwise.
*/
static int write_intel_packet(FILE *f, int n, word addr, 
			      byte type, byte *data)
{
  int i;
  int sum;
  
  fputc(':', f);
  write_byte(n, f);
  write_byte(MSB(addr), f);
  write_byte(LSB(addr), f);
  write_byte(type, f);
  sum = n + MSB(addr) + LSB(addr) + type;
  for(i=0; i<n; i++)
    {
      write_byte(data[i], f);
      sum += data[i];
    }
  write_byte(0x100 - LSB(sum), f);
  if(type != 0x01)
  {
	fputc(0x0D, f);	// CR
	fputc(0x0A, f);	// LF
  }
    
  return 0;
}

/*
	Write a data block to FLASH file
	- page_address: the address of the FLASH page
	- page_number: the index of the FLASH page
	- data: the buffer where data are placed
	- mode: used for telling end of file
	Return a negative value if error, 0 otherwise.
*/
int write_data_block(FILE *f, word page_address, word page_number, 
		     byte *data, int mode)
{
	static word pn = 0xffff;
	int ret = 0;
	int i;
	int bytes_to_write = 0x80;	//number of bytes to write (usually 0x80)
	byte buf[2];

	// Write end of block
	if(mode)
		return write_intel_packet(f, 0, 0x0000, 0x01, data);

	// Write page number
	if(pn != page_number)
	{
		pn = page_number;
		buf[0] = MSB(pn);
		buf[1] = LSB(pn);
		write_intel_packet(f, 2, 0x0000, 0x02, buf);
	}

	// Write data block
	for(i=0; i<bytes_to_write; i+=32)
	{
	  write_intel_packet(f, 32, page_address+i, 0x00, data+i);
	}
		
	return ret;
}
