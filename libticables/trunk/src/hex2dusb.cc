/*  hex2dusb - an D-USB packet decompiler
 *  Copyright (C) 2005-2007  Romain Liévin
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

#include "ticables.h"
#include "logging.h"
#include "internal.h"

#define ARRAY_SIZE (256)

/*
	Format:

	| packet header    | data                                                |
	|                  | data header         |                               |
	| size        | ty | size        | code  | data                          |
	|             |    |             |       |                               |
	| 00 00 00 10 | 04 | 00 00 00 0A | 00 01 | 00 03 00 01 00 00 00 00 07 D0 |
*/

typedef struct
{
	uint8_t     type;
	uint8_t     data_hdr;
	uint8_t     data;
	const char* name;
} Packet;

static const Packet packets[] = 
{
	{ 0x01, 0, 4, "Buffer Size Request" },
	{ 0x02, 0, 4, "Buffer Size Allocation" },
	{ 0x03, 1, 6, "Virtual Packet Data with Continuation" },
	{ 0x04, 1, 6, "Virtual Packet Data Final" },
	{ 0x05, 0, 2, "Virtual Packet Data Acknowledgement" },
	{ 0x00, 0, 0, nullptr }
};

typedef struct
{
	uint16_t    type;
	const char* name;
} Opcode;

static const Opcode opcodes[] = 
{
	{ 0x0001, "Ping / Set Mode" },
	{ 0x0002, "Begin OS Transfer" },
	{ 0x0003, "Acknowledgement of OS Transfer" },
	{ 0x0004, "OS Header" },
	{ 0x0005, "OS Data" },
	{ 0x0006, "Acknowledgement of EOT" },
	{ 0x0007, "Parameter Request"},
	{ 0x0008, "Parameter Data"},
	{ 0x0009, "Request Directory Listing" },
	{ 0x000A, "Variable Header" },
	{ 0x000B, "Request to Send" },
	{ 0x000C, "Request Variable" },
	{ 0x000D, "Variable Contents" },
	{ 0x000E, "Parameter Set"},
	{ 0x0010, "Modify Variable"},
	{ 0x0011, "Remote Control"},
	{ 0x0012, "Acknowledgement of Mode Setting"},
	{ 0xAA00, "Acknowledgement of Data"},
	{ 0xBB00, "Acknowledgement of Parameter Request"},
	{ 0xDD00, "End of Transmission"},
	{ 0xEE00, "Error"},
	{ 0, nullptr }
};

/* */

/*static int is_a_packet(uint8_t id)
{
	int i;

	for (i=0; packets[i].name; i++)
	{
		if (id == packets[i].type)
		{
			break;
		}
	}

	return i;
}*/

static const char* name_of_packet(uint8_t id)
{
	for (int i = 0; packets[i].name; i++)
	{
		if (id == packets[i].type)
		{
			return packets[i].name;
		}
	}

	return "";
}

/*static int is_a_packet_with_data_header(uint8_t id)
{
	int i;

	for (i=0; packets[i].name; i++)
	{
		if (id == packets[i].type)
		{
			if (packets[i].data_hdr)
			{
				return 1;
			}
		}
	}

	return 0;
}*/

/*static int is_a_opcode(uint16_t id)
{
	int i;

	for (i=0; opcodes[i].name; i++)
	{
		if (id == opcodes[i].type)
		{
			break;
		}
	}

	return i;
}*/

static const char* name_of_data(uint16_t id)
{
	for (unsigned int i = 0; opcodes[i].name != nullptr; i++)
	{
		if (id == opcodes[i].type)
		{
			return opcodes[i].name;
		}
	}

	return "unknown";
}

static const char* ep_way(int ep)
{
	if (ep == 0x01)
	{
		return "TI>PC";
	}
	else if (ep == 0x02)
	{
		return "PC>TI";
	}
	else
	{
		return "XX>XX";
	}
}

/* */

static uint8_t pkt_type_found[ARRAY_SIZE] = { 0 };
static uint16_t data_code_found[ARRAY_SIZE] = { 0 };
static unsigned int ptf=0, dcf=0;


static int add_pkt_type(uint8_t type)
{
	unsigned int i;

	for (i = 0; i < ptf; i++)
	{
		if (pkt_type_found[i] == type)
		{
			return 0;
		}
	}

	if (i < (sizeof(pkt_type_found)/sizeof(pkt_type_found[0])) - 1)
	{
		pkt_type_found[++i] = type;
	}
	else
	{
		static int warn_add_pkt_type = 0;
		if (!warn_add_pkt_type)
		{
			ticables_warning("DUSB protocol interpreter: no room left in pkt_type_found array.");
			warn_add_pkt_type++;
		}
	}

	ptf = i;

	return i;
}

static int add_data_code(uint16_t code)
{
	unsigned int i;

	for (i = 0; i < dcf; i++)
	{
		if (data_code_found[i] == code)
		{
			return 0;
		}
	}

	if (i < (sizeof(data_code_found)/sizeof(data_code_found[0])) - 1)
	{
		data_code_found[++i] = code;
	}
	else
	{
		static int warn_add_data_code = 0;
		if (!warn_add_data_code)
		{
			ticables_warning("DUSB protocol interpreter: no room left in data_code_found array.");
			warn_add_data_code++;
		}
	}

	dcf = i;

	return i;
}

/* */

static FILE *hex = nullptr;
static FILE *logfile = nullptr;

static int hex_read(unsigned char *data)
{
	static int idx = 0;
	int data2;

	if (feof(hex))
	{
		return -1;
	}

	const int ret = fscanf(hex, "%02X", &data2);
	if (ret < 1)
	{
		return -1;
	}
	*data = data2 & 0xFF;
	if (fgetc(hex) < 0)
	{
		return -1;
	}
	idx++;

	if (idx >= 16)
	{
		idx = 0;
		for (int i = 0; (i < 67-49) && !feof(hex); i++)
		{
			if (fgetc(hex) < 0)
			{
				return -1;
			}
		}
	}

	return 0;
}

static int dusb_write(int dir, uint8_t data)
{
	static uint8_t array[20];
  	static int i = 0;
	static unsigned long state = 1;
	static uint32_t raw_size;
	static uint8_t raw_type;
	static uint32_t vtl_size;
	static uint16_t vtl_type;
	static int cnt;
	static int first = 1;

	if (logfile == nullptr)
	{
		return -1;
	}

	//printf("<%i %i> ", i, state);
	array[i++ % 16] = data;

	switch(state)	// Finite State Machine
	{
	case 1: break;
	case 2: break;
	case 3: break;
	case 4:
		raw_size = (((uint32_t)(array[0])) << 24) | (((uint32_t)(array[1])) << 16) | (((uint32_t)(array[2])) << 8) | ((uint32_t)(array[3]));
		fprintf(logfile, "%08x ", (unsigned int)raw_size);
		break;
	case 5: 
		raw_type = array[4];
		fprintf(logfile, "(%02X) ", (unsigned int)raw_type);

		fprintf(logfile, "\t\t\t\t\t\t\t");
		fprintf(logfile, "| %s: %s\n", ep_way(dir), name_of_packet(raw_type));
		add_pkt_type(raw_type);

		break;
	case 6: break;
	case 7:
		if (raw_type == 5)
		{
			const uint16_t tmp = (((uint32_t)(array[5])) << 8) | ((uint32_t)(array[6]));
			fprintf(logfile, "\t[%04x]\n", tmp);
			state = 0;
		}
		break;
	case 8: break;
	case 9:
		if (raw_type == 1 || raw_type == 2)
		{
			const uint32_t tmp = (((uint32_t)(array[5])) << 24) | (((uint32_t)(array[6])) << 16) | (((uint32_t)(array[7])) << 8) | ((uint32_t)(array[8]));
			fprintf(logfile, "\t[%08x]\n", (unsigned int)tmp);
			state = 0;
		}
		else if (first && ((raw_type == 3) || (raw_type == 4)))
		{
			vtl_size = (((uint32_t)(array[5])) << 24) | (((uint32_t)(array[6])) << 16) | (((uint32_t)(array[7])) << 8) | ((uint32_t)(array[8]));
			fprintf(logfile, "\t%08x ", (unsigned int)vtl_size);
			cnt = 0;
			first = (raw_type == 3) ? 0 : 1;
			raw_size -= 6;
		}
		else if (!first && ((raw_type == 3) || (raw_type == 4)))
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
		vtl_type = (((uint32_t)(array[9])) << 8) | ((uint32_t)(array[10]));
		fprintf(logfile, "{%04x}", vtl_type);

		fprintf(logfile, "\t\t\t\t\t\t");
		fprintf(logfile, "| %s: %s\n\t\t", "CMD", name_of_data(vtl_type));
		add_data_code(vtl_type);

		if (!vtl_size)
		{
			fprintf(logfile, "\n");
			state = 0;
		}
		break;
	default: push:
		fprintf(logfile, "%02X ", data);

		if (!(++cnt % 12))
		{
			fprintf(logfile, "\n\t\t");
		}

		if (--raw_size == 0)
		{
			fprintf(logfile, "\n");
			state = 0;
		}
		break;
	}

	if (state == 0)
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
	unsigned char data = 0;
	unsigned int i;

	snprintf(src_name, sizeof(src_name) - 1, "%s.hex", filename);
	src_name[sizeof(src_name) - 1] = 0;

	snprintf(dst_name, sizeof(dst_name) - 1, "%s.pkt", filename);
	dst_name[sizeof(dst_name) - 1] = 0;

	hex = fopen(src_name, "rt");
	if (hex == nullptr)
	{
		fprintf(stderr, "Unable to open input file: %s\n", src_name);
		return -1;
	}

	logfile = fopen(dst_name, "wt");
	if (logfile == nullptr)
	{
		fprintf(stderr, "Unable to open output file: %s\n", dst_name);
		fclose(hex);
		return -1;
	}

	fprintf(logfile, "TI packet decompiler for D-USB, version 1.0\n");

	// skip comments
	if (   fgets(line, sizeof(line), hex) == nullptr
	       || fgets(line, sizeof(line), hex) == nullptr
	       || fgets(line, sizeof(line), hex) == nullptr)
	{
		goto exit;
	}

	while (hex_read(&data) != -1)
	{
		dusb_write(0, data);
	}

	fprintf(logfile, "() Packet types found: ");
	for (i = 0; i < ptf; i++)
	{
		fprintf(logfile, "%02x ", pkt_type_found[i]);
	}
	fprintf(logfile, "\n");
	fprintf(logfile, "{} Data codes found: ");
	for (i = 0; i < dcf; i++)
	{
		fprintf(logfile, "%04x ", data_code_found[i]);
	}
	fprintf(logfile, "\n");

exit:
	fclose(logfile);
	fclose(hex);

	return 0;
}

#if 0
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: hex2dusb [file]\n");
		exit(0);
	}

	return dusb_decomp(argv[1]);
}
#endif
