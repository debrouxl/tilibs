/*  hex2nsp - an D-USB packet decompiler
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

//#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "ticables.h"
#include "logging.h"
#include "internal.h"

#define HEXDUMP_SIZE	12

#define ARRAY_SIZE (256)

/*
	Format (see http://hackspire.unsads.com/USB_Protocol#Service_identifiers):

	54 FD SA SA SS SS DA DA DS DS DC DC SZ AK SQ CK [data part]
*/

typedef struct
{
	uint16_t	unused;
	uint16_t	src_addr;
	uint16_t	src_id;
	uint16_t	dst_addr;
	uint16_t	dst_id;
	uint16_t	data_sum;
	uint8_t		data_size;
	uint8_t		ack;
	uint8_t		seq;
	uint8_t		hdr_sum;
} Packet;

typedef struct
{
	uint16_t		value;
	const char*		name;
} ServiceId;

typedef struct
{
	uint16_t		value;
	const char*		name;
} Address;

static const Address addrs[] = 
{
	{ 0x0000, "TI" },
	{ 0x6400, "PC" },
	{ 0x6401, "TI" },
	{ 0, nullptr },
};

static const ServiceId sids[] = 
{
	{ 0x00FE, "Reception Acknowledgment" },
	{ 0x00FF, "Reception Ack" },
	{ 0x4001, "Null" },
	{ 0x4002, "Echo" },
	{ 0x4003, "Device Address Request/Assignment" },
	{ 0x4020, "Device Information" },
	{ 0x4021, "Screen Capture" },
	{ 0x4022, "Event" },
	{ 0x4023, "Shutdown" },
	{ 0x4024, "Screen Capture w/ RLE" },
	{ 0x4041, "Activity" },
	{ 0x4042, "Keypresses" },
	{ 0x4043, "RPC" },
	{ 0x4050, "Login" },
	{ 0x4051, "Messages" },
	{ 0x4054, "Hub Connection" },
	{ 0x4060, "File Management" },
	{ 0x4080, "OS Installation" },
	{ 0x4090, "Remote Handheld Management" },
	{ 0x40DE, "Service Disconnect" },
	{ 0x5000, "Ext Echo" },

	{ 0x8003, "8003" },
	{ 0x8004, "8004" },
	{ 0x8005, "8005" },
	{ 0x8006, "8006" },
	{ 0x8007, "8007" },
	{ 0x8009, "8009" },

	{ 0, nullptr },
};

/* */

/*static int is_a_sid(uint8_t id)
{
  int i;
  
  for(i=0; sids[i].name; i++)
    if(id == sids[i].value)
      break;
  return i;
}*/

static const char* name_of_sid(uint16_t id)
{
	for (int i = 0; sids[i].name; i++)
	{
		if (id == sids[i].value)
		{
			return sids[i].name;
		}
	}

	return "???";
}

static const char* name_of_addr(uint16_t addr)
{
	for (int i = 0; addrs[i].name; i++)
	{
		if (addr == addrs[i].value)
		{
			return addrs[i].name;
		}
	}

	return "??";
}

/*static const char* ep_way(int ep)
{
	if(ep == 0x01) return "TI>PC";
	else if(ep == 0x02) return "PC>TI";
	else return "XX>XX";
}*/

/* */

static int add_sid(uint16_t* array, uint16_t id, int *count)
{
	int i;
	for (i = 0; i < *count; i++)
	{
		if (array[i] == id)
		{
			return 0;
		}
	}

	if (i < ARRAY_SIZE - 1)
	{
		array[++i] = id;
	}
	else
	{
		static int warn_add_sid;
		if (!warn_add_sid++)
		{
			ticables_warning("NSP protocol interpreter: no room left in sid_found array.");
		}
	}

	*count = i;

	return i;
}

static int add_addr(uint16_t* array, uint16_t addr, int *count)
{
	int i;

	for (i = 0; i < *count; i++)
	{
		if (array[i] == addr)
		{
			return 0;
		}
	}

	if (i < ARRAY_SIZE - 1)
	{
		array[++i] = addr;
	}
	else
	{
		static int warn_add_addr;
		if (!warn_add_addr++)
		{
			ticables_warning("NSP protocol interpreter: no room left in addr_found array.");
		}
	}

	*count = i;

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

static uint16_t sid_found[ARRAY_SIZE] = { 0 };
static uint16_t addr_found[ARRAY_SIZE] = { 0 };
static int sif=0, af=0;

static int dusb_write(int dir, uint8_t data)
{
	static uint8_t array[20];
  	static int i = 0;
	static unsigned long state = 1;
	static uint16_t src_addr, src_id;
	static uint16_t dst_addr, dst_id;
	static uint8_t data_size, ack, sq;
	static int cnt;
	static uint8_t ascii[HEXDUMP_SIZE+1];

	if (logfile == nullptr)
	{
		return -1;
	}

	array[i++ % 16] = data;

	switch(state)	// Finite State Machine
	{
	case 1:			// unused
	case 2:
		break;

	case 3:			// source address
		break;
	case 4: 
		src_addr = (((uint16_t)(array[2])) << 8) | ((uint16_t)(array[3]));
		fprintf(logfile, "%04x:", src_addr);
		add_addr(addr_found, src_addr, &af);
		break;

	case 5:			// source service id
		break;
	case 6: 
		src_id = (((uint16_t)(array[4])) << 8) | ((uint16_t)(array[5]));
		fprintf(logfile, "%04x->", src_id);
		add_sid(sid_found, src_id, &sif);
		break;

	case 7:			// destination address
		break;
	case 8: 
		dst_addr = (((uint16_t)(array[6])) << 8) | ((uint16_t)(array[7]));
		fprintf(logfile, "%04x:", dst_addr);
		add_addr(addr_found, dst_addr, &af);
		break;

	case 9:			// destination service id
		break;
	case 10: 
		dst_id = (((uint16_t)(array[8])) << 8) | ((uint16_t)(array[9]));
		fprintf(logfile, "%04x ", dst_id);
		add_sid(sid_found, src_id, &sif);
		break;

	case 11: break;	// data checksum
	case 12: break;

	case 13:		// data size
		data_size = array[12];
		break;

	case 14:		// acknowledgment
		ack = array[13];
		fprintf(logfile, "AK=%02x ", ack);
		break;

	case 15:		// sequence number
		sq = array[14];
		fprintf(logfile, "SQ=%02x ", sq);
		break;

	case 16:		// header checksum
		fprintf(logfile, "(%3i bytes) ", data_size);
		cnt = 0;

		fprintf(logfile, "\t\t\t\t\t%s (%s) ==> %s (%s)\n", 
			name_of_addr(src_addr), name_of_sid(src_id), 
			name_of_addr(dst_addr), name_of_sid(dst_id));

		if (data_size == 0)
		{
			state = 0;
		}
		break;

	default:
		if (!cnt)
		{
			fprintf(logfile, "\t\t");
		}

		fprintf(logfile, "%02X ", data);
		ascii[cnt % HEXDUMP_SIZE] = data;

		if (!(++cnt % HEXDUMP_SIZE))
		{
			fprintf(logfile, " | ");
			for (i = 0; i < HEXDUMP_SIZE; i++)
			{
				fprintf(logfile, "%c", isalnum(ascii[i]) ? ascii[i] : '.');
			}

			fprintf(logfile, "\n\t\t");
		}

		if (--data_size == 0)
		{
			for (i = 0; i < HEXDUMP_SIZE - (cnt%HEXDUMP_SIZE); i++)
			{
				fprintf(logfile, "   ");
			}
			fprintf(logfile, " | ");
			for (i = 0; i < (cnt%HEXDUMP_SIZE); i++)
			{
				fprintf(logfile, "%c", isalnum(ascii[i]) ? ascii[i] : '.');
			}

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

int nsp_decomp(const char *filename)
{
	char src_name[1024];
	char dst_name[1024];
	char line[256];
	unsigned char data = 0;
	int i;

	snprintf(src_name, sizeof(src_name) - 1, "%s.hex", filename);
	src_name[sizeof(src_name) - 1] = 0;

	snprintf(dst_name, sizeof(dst_name) - 1, "%s.pkt", filename);
	dst_name[sizeof(dst_name) - 1] = 0;

	hex = fopen(src_name, "rt");
	if (hex == nullptr)
	{
		fprintf(stderr, "Unable to open this file: %s\n", src_name);
		return -1;
	}

	logfile = fopen(dst_name, "wt");
	if (logfile == nullptr)
	{
		fprintf(stderr, "Unable to open this file: %s\n", dst_name);
		fclose(hex);
		return -1;
	}

	fprintf(logfile, "TI packet decompiler for NSpire, version 1.0\n");

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

	fprintf(logfile, "() Service IDs found: ");
	for(i = 0; i < sif; i++)
	{
		fprintf(logfile, "%04x ", sid_found[i]);
	}
	fprintf(logfile, "\n");
	fprintf(logfile, "() Addresses found: ");
	for(i = 0; i < af; i++)
	{
		fprintf(logfile, "%04x ", addr_found[i]);
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
	if(argc < 2)
	{
		fprintf(stderr, "Usage: hex2nsp [file]\n");
		exit(0);
	}

	return nsp_decomp(argv[1]);
}
#endif
