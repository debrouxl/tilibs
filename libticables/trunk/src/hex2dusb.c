/*  hex2dusb - an D-USB packet decompiler
 *  Copyright (C) 2005-2007  Romain Lievin
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "logging.h"
#include "stdints1.h"

#define ARRAY_SIZE (256)

/*
	Format:

	| packet header    | data												 |
	|				   | data header         |								 |
	| size		  | ty | size		 | code	 | data							 |
	|			  |    |			 |		 |								 |
	| 00 00 00 10 | 04 | 00 00 00 0A | 00 01 | 00 03 00 01 00 00 00 00 07 D0 |	
*/

typedef struct
{
	uint8_t		type;
	const char*	name;
	int			data_hdr;
	int			data;
} Packet;

typedef struct
{
	uint16_t		type;
	const char*		name;
} Opcode;

static const Packet packets[] = 
{
	{ 0x01, "Buffer Size Request", 0, 4 }, 
	{ 0x02, "Buffer Size Allocation", 0, 4 }, 
	{ 0x03, "Virtual Packet Data with Continuation", 1, 6 }, 
	{ 0x04, "Virtual Packet Data Final", 1, 6 }, 
	{ 0x05, "Virtual Packet Data Acknowledgement", 0, 2 }, 
	{ 0,    NULL, 0, 0 },
};

static const Opcode opcodes[] = 
{
	{ 0x0001, "Ping / Set Mode" }, 
	{ 0x0002, "Begin OS Transfer" },
	{ 0x0003, "Acknowledgement of OS Transfer" },
	{ 0x0005, "OS Data" },
	{ 0x0006, "Acknowledgement of EOT" },
	{ 0x0007, "Parameter Request"}, 
	{ 0x0008, "Parameter Data"}, 
	{ 0x0009, "Request Directory Listing" },
	{ 0x000a, "Variable Header" },
	{ 0x000b, "Request to Send" },
	{ 0x000c, "Request Variable" },
	{ 0x000D, "Variable Contents" },
	{ 0x000e, "Parameter Set"},
	{ 0x0010, "Delete Variable"}, 
	{ 0x0011, "Remote Control"}, 
	{ 0x0012, "Acknowledgement of Mode Setting"}, 
	{ 0xaa00, "Acknowledgement of Data"}, 
	{ 0xbb00, "Acknowledgement of Parameter Request"},
	{ 0xdd00, "End of Transmission"}, 
	{ 0xee00, "Error"},
	{ 0,      NULL },
};

/* */

/*static int is_a_packet(uint8_t id)
{
  int i;
  
  for(i=0; packets[i].name; i++)
    if(id == packets[i].type)
      break;
  return i;
}*/

static const char* name_of_packet(uint8_t id)
{
	int i;
  
	for(i=0; packets[i].name; i++)
		if(id == packets[i].type)
			return packets[i].name;
	return "";
}

/*static int is_a_packet_with_data_header(uint8_t id)
{
	int i;
  
  for(i=0; packets[i].name; i++)
    if(id == packets[i].type)
		if(packets[i].data_hdr)
			return 1;

  return 0;
}*/

/*static int is_a_opcode(uint16_t id)
{
  int i;

  for(i=0; opcodes[i].name; i++)
    if(id == opcodes[i].type)
      break;

  return i;
}*/

static const char* name_of_data(uint16_t id)
{
	int i;
  
	for(i=0; opcodes[i].name; i++)
		if(id == opcodes[i].type)
			return opcodes[i].name;

	return "unknown";
}

static const char* ep_way(int ep)
{
	if(ep == 0x01) return "TI>PC";
	else if(ep == 0x02) return "PC>TI";
	else return "XX>XX";
}

/* */

static int add_pkt_type(uint8_t* array, uint8_t type, int *count)
{
	int i;

	for(i = 0; i < *count; i++)
		if(array[i] == type)
			return 0;

	if (i < ARRAY_SIZE - 1)
	{
		array[++i] = type;
	}
	else
	{
		static int warn_add_pkt_type;
		if (!warn_add_pkt_type++)
		{
			ticables_warning("DUSB protocol interpreter: no room left in pkt_type_found array.");
		}
	}

	*count = i;

	return i;
}

static int add_data_code(uint16_t* array, uint16_t code, int *count)
{
	int i;

	for(i = 0; i < *count; i++)
		if(array[i] == code)
			return 0;

	if (i < ARRAY_SIZE - 1)
	{
		array[++i] = code;
	}
	else
	{
		static int warn_add_data_code;
		if (!warn_add_data_code++)
		{
			ticables_warning("DUSB protocol interpreter: no room left in data_code_found array.");
		}
	}

	*count = i;

	return i;
}

/* */

static FILE *hex = NULL;
static FILE *logfile = NULL;

static int hex_read(unsigned char *data)
{
	static int idx = 0;
	int ret;
	int data2;

	if(feof(hex))
		return -1;

	ret = fscanf(hex, "%02X", &data2);
	if(ret < 1)
		return -1;
	*data = data2 & 0xFF;
	fgetc(hex);
	idx++;

	if(idx >= 16)
	{
		int i;

		idx = 0;
		for(i = 0; (i < 67-49) && !feof(hex); i++)
			fgetc(hex);
	}
    
	return 0;
}

static uint8_t pkt_type_found[ARRAY_SIZE] = { 0 };
static uint16_t data_code_found[ARRAY_SIZE] = { 0 };
static int ptf=0, dcf=0;

static int dusb_write(int dir, uint8_t data)
{
	static int array[20];
  	static int i = 0;
	static unsigned long state = 1;
	static uint32_t raw_size;
	static uint8_t raw_type;
	static uint32_t vtl_size;
	static uint16_t vtl_type;
	static int cnt;
	static int first = 1;

  	if (logfile == NULL)
    		return -1;

	//printf("<%i %i> ", i, state);
	array[i++ % 16] = data;

	switch(state)	// Finite State Machine
	{
	case 1: break;
	case 2: break;
	case 3: break;
	case 4: 
		raw_size = (array[0] << 24) | (array[1] << 16) | (array[2] << 8) | (array[3] << 0);
		fprintf(logfile, "%08x ", (unsigned int)raw_size);
		break;
	case 5: 
		raw_type = array[4];
		fprintf(logfile, "(%02X) ", (unsigned int)raw_type);

		fprintf(logfile, "\t\t\t\t\t\t\t");
		fprintf(logfile, "| %s: %s\n", ep_way(dir), name_of_packet(raw_type));
		add_pkt_type(pkt_type_found, raw_type, &ptf);

		break;
	case 6: break;
	case 7:
		if(raw_type == 5)
		{
			uint16_t tmp = (array[5] << 8) | (array[6] << 0);
			fprintf(logfile, "\t[%04x]\n", tmp);
			state = 0;
		}
		break;
	case 8: break;
	case 9:
		if(raw_type == 1 || raw_type == 2)
		{
			uint32_t tmp = (array[5] << 24) | (array[6] << 16) | (array[7] << 8) | (array[8] << 0);
			fprintf(logfile, "\t[%08x]\n", (unsigned int)tmp);
			state = 0;
		}
		else if(first && ((raw_type == 3) || (raw_type == 4)))
		{
			vtl_size = (array[5] << 24) | (array[6] << 16) | (array[7] << 8) | (array[8] << 0);
			fprintf(logfile, "\t%08x ", (unsigned int)vtl_size);
			cnt = 0;
			first = (raw_type == 3) ? 0 : 1;
			raw_size -= 6;
		}
		else if(!first && ((raw_type == 3) || (raw_type == 4)))
		{
			fprintf(logfile, "\t");
			fprintf(logfile, "%02X %02X %02X ", array[5], array[6], array[7]);
			cnt = 3;
			raw_size -= 3;
			first = (raw_type == 3) ? 0 : 1;

			state = 12;
			goto push;
		}			
		break;
	case 10: break;
	case 11:
		vtl_type = (array[9] << 8) | (array[10] << 0);
		fprintf(logfile, "{%04x}", vtl_type);
		
		fprintf(logfile, "\t\t\t\t\t\t");
		fprintf(logfile, "| %s: %s\n\t\t", "CMD", name_of_data(vtl_type));
		add_data_code(data_code_found, vtl_type, &dcf);

		if(!vtl_size)
		{
			fprintf(logfile, "\n");
			state = 0;
		}
		break;
	default: push:
		fprintf(logfile, "%02X ", data);

		if(!(++cnt % 12))
			fprintf(logfile, "\n\t\t");
		
		if(--raw_size == 0)
		{
			fprintf(logfile, "\n");
			state = 0;
		}
		break;
	}

	if(state == 0)
	{
		fprintf(logfile, "\n");
		i = 0;
	}
	state++;

	return 0;
}

int dusb_decomp(const char *filename)
{
	char src_name[1024];
	char dst_name[1024];
	char line[256];
	unsigned char data;
	int i;

	strcpy(src_name, filename);
	strcat(src_name, ".hex");

	strcpy(dst_name, filename);
	strcat(dst_name, ".pkt");

	hex = fopen(src_name, "rt");
	if(hex == NULL)
	{
		fprintf(stderr, "Unable to open input file: %s\n", src_name);
		return -1;
	}

	logfile = fopen(dst_name, "wt");
	if(logfile == NULL)
	{
		fprintf(stderr, "Unable to open output file: %s\n", dst_name);
		fclose(hex);
		return -1;
	}

	if (fgets(line, sizeof(line), hex) == NULL) goto exit;
	if (fgets(line, sizeof(line), hex) == NULL) goto exit;
	if (fgets(line, sizeof(line), hex) == NULL) goto exit;

	fprintf(logfile, "TI packet decompiler for D-USB, version 1.0\n");

	while(hex_read(&data) != -1)
	{
		dusb_write(0, data);
	}

	fprintf(logfile, "() Packet types found: ");
	for(i = 0; i < ptf; i++) fprintf(logfile, "%02x ", pkt_type_found[i]);
	fprintf(logfile, "\n");
	fprintf(logfile, "{} Data codes found: ");
	for(i = 0; i < dcf; i++) fprintf(logfile, "%04x ", data_code_found[i]);
	fprintf(logfile, "\n");

exit:
	fclose(logfile);
	fclose(hex);

	return 0;
}

#if 0
int main(int argc, char **argv)
{


	if(argc < 2)
	{
		fprintf(stderr, "Usage: hex2dusb [file]\n");
		exit(0);
	}

	return dusb_decomp(argv[1]);
}
#endif
