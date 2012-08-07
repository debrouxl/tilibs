/*  hex2dbus - an D-BUS packet decompiler
 *  Copyright (C) 2002-2007  Romain Liévin
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

static const unsigned char machine_id[] =
{
  0x00, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x08, 0x09, 0x23,
  0x00, 0x82, 0x83, 0x85, 0x86, 0x74, 0x98, 0x88, 0x89, 0x73,
  0xff
};

static const char* machine_way[] = 
{
	"PC>TI", "PC>TI", "PC>TI", "PC>TI", "PC>TI", "PC>TI", "PC>TI", "PC>TI", "PC>TI", "PC>TI",
	"TI>PC", "TI>PC", "TI>PC", "TI>PC", "TI>PC", "TI>PC", "TI>PC", "TI>PC", "TI>PC", "TI>PC",
	"",
};

static const unsigned char command_id[] =
{
	0x06, 0x09, 0x15, 0x2d, 0x36, 0x47, 0x56, 0x5A, 
	0x68, 0x6D, 0x74, 0x78, 0x87, 0x88, 0x92, 0xA2, 
	0xB7, 0xC9, 
	0xff
};

static const char command_name[][8] =
{
	"VAR", "CTS", "XDP", "VER", "SKP", "SID", "ACK", "ERR", 
	"RDY", "SCR", "RID", "CNT", "KEY", "DEL", "EOT", "REQ", 
	"IND", "RTS", 
	""
};

static const int cmd_with_data[] =
{
	!0, 0, !0, 0, !0, !0, 0, !0, 
	0, 0, 0, 0, 0, !0, 0, !0,
	!0, !0, -1
};

int is_a_machine_id(unsigned char id)
{
  int i;
  
  for(i=0; machine_id[i] != 0xff; i++)
    if(id == machine_id[i])
      break;
  return i;
}

int is_a_command_id(unsigned char id)
{
  int i;

  for(i=0; command_id[i] != 0xff; i++)
    if(id == command_id[i])
      break;

  return i;
}

#define WIDTH	12

int fill_buf(FILE *f, char data, int flush)
{
	static char buf[WIDTH];
	static unsigned int cnt = 0;
	unsigned int i, j;

	if(!flush)
		buf[cnt++] = data;

	if((cnt >= WIDTH) || flush)
	{
		//printf(".");
		fprintf(f, "    ");
		for(i = 0; i < cnt; i++)
			fprintf(f, "%02X ", 0xff & buf[i]);

		if(flush)
			for(j = i; j < WIDTH; j++)
				fprintf(f, "   ");

		fprintf(f, "| ");
		for(i = 0; i < cnt; i++)
			fprintf(f, "%c", isalnum(buf[i]) ? buf[i] : '.');

		fprintf(f, "\n");
		cnt = 0;
	}

	return 0;
}

/*
  Format of data: 8 hexadecimal numbers with spaces
*/
int dbus_decomp(const char *filename, int resync)
{
	char src_name[1024];
	char dst_name[1024];
	FILE *fi = NULL, *fo = NULL;
	long file_size;
	struct stat st;
	unsigned char *buffer;
	int i;
	unsigned int j;
	int num_bytes;
	char str[256];
	unsigned char mid, cid;
	unsigned int length;
	int idx;
	int ret = 0;

	// build filenames
	strcpy(src_name, filename);
	strcat(src_name, ".hex");

	strcpy(dst_name, filename);
	strcat(dst_name, ".pkt");

	stat(src_name, &st);
	file_size = st.st_size;

	// allocate buffer
	buffer = (unsigned char*)calloc(file_size/2 < 65536 ? 65526 : file_size >> 1, 1);
	memset(buffer, 0xff, file_size/2);
	if(buffer == NULL)
	{
		fprintf(stderr, "calloc error.\n");
		return -1;
	}

	// open files
	fi = fopen(src_name, "rt");
	if(fi == NULL)
	{
		fprintf(stderr, "Unable to open input file: %s\n", src_name);
		free(buffer);
		return -1;
	}

	fo = fopen(dst_name, "wt");
	if(fo == NULL)
	{
		fprintf(stderr, "Unable to open output file: %s\n", dst_name);
		fclose(fi);
		free(buffer);
		return -1;
	}

	fprintf(fo, "TI packet decompiler for D-BUS, version 1.2\n");

	// skip comments
	if (fgets(str, sizeof(str), fi) == NULL) goto exit;
	if (fgets(str, sizeof(str), fi) == NULL) goto exit;
	if (fgets(str, sizeof(str), fi) == NULL) goto exit;

	// read source file
	for(i = 0; !feof(fi);)
	{
		for(j = 0; j < 16 && !feof(fi); j++)
		{
			if (fscanf(fi, "%02X", (unsigned int *)&(buffer[i+j])) < 1)
			{
				ret = -1;
				goto exit;
			}
			fgetc(fi);
		}
		i += j;

		for(j=0; j<18 && !feof(fi); j++)
			fgetc(fi);
	}
	num_bytes = i-1; // -1 due to EOF char
	printf("%i bytes read.\n", num_bytes);

	// process data
	for(i = 0; i < num_bytes;)
	{
restart:
		mid = buffer[i+0];
		cid = buffer[i+1];
		length = buffer[i+2];
		length |= buffer[i+3] << 8;

		// check for valid packet
		if(is_a_machine_id(mid) == -1)
		{
			ret = -1;
			goto exit;
		}

		// check for valid packet
		idx = is_a_command_id(cid);
		if(idx == -1)
		{
			ret = -2;
			goto exit;
		}

		fprintf(fo, "%02X %02X %02X %02X", mid, cid, length >> 8, length & 0xff);
		for(j = 4; j <= WIDTH; j++)
			fprintf(fo, "   ");
		fprintf(fo, "  | ");
		fprintf(fo, "%s: %s\n", machine_way[is_a_machine_id(mid)], command_name[is_a_command_id(cid)]);

		i += 4;

		// get data & checksum
		if(cmd_with_data[idx] && length > 0)
		{
			// data
			for(j = 0; j < length; j++, i++)
			{
				if(resync && buffer[i] == 0x98 && (buffer[i+1] == 0x15 ||  buffer[i+1] == 0x56))
				{
					printf("Warning: there is packets in data !\n");
					fprintf(fo, "Beware : length of previous packet is wrong !\n");
					goto restart;
				}

				fill_buf(fo, buffer[i], 0);
			}

			fill_buf(fo, 0, !0);
			//fprintf(fo, "\n");

			// write checksum
			fprintf(fo, "    ");
			fprintf(fo, "%02X ", buffer[i++]);
			fprintf(fo, "%02X ", buffer[i++]);
			fprintf(fo, "\n");
		}
	}

exit:
	if(ret < 0)
		printf("Error %i\n", -ret);

	fclose(fi);
	fclose(fo);
	free(buffer);

	return ret;
}

#if 0
int main(int argc, char **argv)
{
	int resync = 0;

	if(argc < 2)
    {
		fprintf(stderr, "Usage: hex2dbus [file]\n");
		exit(0);
    }

	if(argc > 2)
		resync = !0;
  
	return dbus_decomp(argv[1], resync);
}
#endif
