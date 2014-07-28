/* Hey EMACS -*- linux-c -*- */
/* $Id: packets.c 1352 2005-07-12 07:24:18Z roms $ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
 *  Copyright (C) 2006  Kevin Kofler
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
	This unit manages common ROM dumper.
*/

#include <stdio.h>
#include <string.h>
#include <glib/gstdio.h>

#include "ticalcs.h"
#include "dbus_pkt.h"
#include "logging.h"
#include "error.h"
#include "macros.h"
#include "pause.h"

#define MAX_RETRY	3

static unsigned int BLK_SIZE = 1024;// heuristic
#define MIN_SIZE	256				// don't refresh if block is small

/* CMD | LEN | DATA | CHK */
#define	CMD_IS_READY	0xAA55
#define CMD_KO			0x0000
#define CMD_OK			0x0001
#define CMD_EXIT		0x0002
#define CMD_REQ_SIZE	0x0003
#define CMD_ERROR		0x0004	// unused !
#define CMD_REQ_BLOCK	0x0005
#define CMD_DATA1		0x0006
#define CMD_DATA2		0x0007

static uint8_t buf[65536 + 3*2];
static int std_blk = 0;
static int sav_blk = 0;

// --- Packet Layer

static int send_pkt(CalcHandle* handle, uint16_t cmd, uint16_t len, uint8_t* data)
{
	uint16_t sum;

	// command
	buf[0] = LSB(cmd);
	buf[1] = MSB(cmd);

	// length
	buf[2] = LSB(len);
	buf[3] = MSB(len);

	// data
	if(data)
		memcpy(buf+4, data, len);

	// checksum
	sum = tifiles_checksum(buf, 4 + len);
	buf[len+4+0] = LSB(sum);
	buf[len+4+1] = MSB(sum);

	return ticables_cable_send(handle->cable, buf, len+6);
}

static int cmd_is_valid(uint16_t cmd)
{
	switch(cmd)
	{
	case CMD_IS_READY:
	case CMD_OK:
	case CMD_KO:
	case CMD_EXIT:
	case CMD_REQ_SIZE:
	case CMD_ERROR:
	case CMD_REQ_BLOCK:
	case CMD_DATA1:
	case CMD_DATA2:
		return 1;
	default:
		return 0;
	}

	return 0;
}

static int recv_pkt(CalcHandle* handle, uint16_t* cmd, uint16_t* len, uint8_t* data)
{
	int i, r, q;
	uint16_t sum, chksum;
	int err;

	// Any packet has always at least 4 bytes (cmd, len)
	err = ticables_cable_recv(handle->cable, buf, 4);
	if (!err)
	{

		*cmd = (buf[1] << 8) | buf[0];
		*len = (buf[3] << 8) | buf[2];

		if (!cmd_is_valid(*cmd))
		{
			err = ERR_INVALID_CMD;
			goto exit;
		}

		if (*cmd == CMD_ERROR)
		{
			err = ERR_ROM_ERROR;
			goto exit;
		}

		// compute chunks
		BLK_SIZE = *len / 20;
		if (BLK_SIZE == 0) BLK_SIZE = 1;

		q = *len / BLK_SIZE;
		r = *len % BLK_SIZE;
		handle->updat->max1 = *len;
		handle->updat->cnt1 = 0;

		// recv full chunks
		for(i = 0; i < q; i++)
		{
			err = ticables_cable_recv(handle->cable, &buf[i*BLK_SIZE + 4], BLK_SIZE);
			if (err)
			{
				goto exit;
			}
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
			handle->updat->cnt1 += BLK_SIZE;
			if(*len > MIN_SIZE)
				handle->updat->pbar();
			//if (handle->updat->cancel) 
			//	return ERR_ABORT;
		}

		// recv last chunk
		{
			err = ticables_cable_recv(handle->cable, &buf[i*BLK_SIZE + 4], (uint16_t)(r+2));
			if (err)
			{
				goto exit;
			}
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
			handle->updat->cnt1 += 1;
			if(*len > MIN_SIZE)
				handle->updat->pbar();
			if (handle->updat->cancel)
				return ERR_ABORT;
		}

		// verify checksum
		chksum = (buf[*len+4 + 1] << 8) | buf[*len+4 + 0];
		sum = tifiles_checksum(buf, *len + 4);
		//printf("<%04x %04x>\n", sum, chksum);

		if (chksum != sum)
		{
			return ERR_CHECKSUM;
		}

		if (data)
		{
			memcpy(data, buf+4, *len);
		}
	}

exit:
	return err;
}

// --- Command Layer

static int rom_send_RDY(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: IS_READY");
	return send_pkt(handle, CMD_IS_READY, 0, NULL);
}

static int rom_recv_RDY(CalcHandle* handle)
{
	uint16_t cmd, len;
	int err;

	err = recv_pkt(handle, &cmd, &len, NULL);
	ticalcs_info(" TI->PC: %s", err ? "ERROR" : (cmd ? "OK" : "KO"));

	return err;
}

static int rom_send_EXIT(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: EXIT");
	return send_pkt(handle, CMD_EXIT, 0, NULL);
}

static int rom_recv_EXIT(CalcHandle* handle)
{
	uint16_t cmd, len;
	int err;

	err = recv_pkt(handle, &cmd, &len, NULL);
	ticalcs_info(" TI->PC: EXIT");

	return err;
}

static int rom_send_SIZE(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: REQ_SIZE");
	return send_pkt(handle, CMD_REQ_SIZE, 0, NULL);
}

static int rom_recv_SIZE(CalcHandle* handle, uint32_t* size)
{
	uint16_t cmd, len;
	int err;

	err = recv_pkt(handle, &cmd, &len, (uint8_t *)size);
	ticalcs_info(" TI->PC: SIZE (0x%08x bytes)", *size);

	return err;
}

static int rom_send_DATA(CalcHandle* handle, uint32_t addr)
{
	ticalcs_info(" PC->TI: REQ_BLOCK at @%08x", addr);
	return send_pkt(handle, CMD_REQ_BLOCK, 4, (uint8_t *)&addr);
}

static int rom_recv_DATA(CalcHandle* handle, uint16_t* size, uint8_t* data)
{
	uint16_t cmd;
	uint16_t rpt;
	int err;

	err = recv_pkt(handle, &cmd, size, data);
	if (!err)
	{
		if(cmd == CMD_DATA1)
		{
			ticalcs_info(" TI->PC: BLOCK (0x%04x bytes)", *size);
			std_blk++;
		}
		else if(cmd == CMD_DATA2)
		{
			*size = (data[1] << 8) | data[0];
			rpt = (data[3] << 8) | data[2];
			memset(data, rpt, *size);
			ticalcs_info(" TI->PC: BLOCK (0x%04x bytes)", *size);
			sav_blk++;
		}
		else
		{
			err = ERR_INVALID_CMD;
		}
	}

	return err;
}

static int rom_send_ERR(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: ERROR");
	return send_pkt(handle, CMD_ERROR, 0, NULL);
}

// --- Dumping Layer

int rd_dump(CalcHandle* handle, const char *filename)
{
	FILE *f;
	int err = 0;
	uint32_t size;
	uint32_t addr;
	uint16_t length;
	uint32_t i;
	uint8_t data[65536];

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	f = fopen(filename, "wb");
	if (f == NULL)
		return ERR_OPEN_FILE;

	sprintf(update_->text, "Receiving data...");
	update_label();

	// check if ready
	for(i = 0; i < 3; i++)
	{
		err = rom_send_RDY(handle);
		if (rom_recv_RDY(handle))
		{
			goto exit; // Bail out.
		}
		if(!err)
		{
			break; // Proceed further.
		}
	}

	// request ROM size
	if (rom_send_SIZE(handle))
	{
		goto exit;
	}
	if (rom_recv_SIZE(handle, &size))
	{
		goto exit;
	}

	// get packets
	std_blk = sav_blk = 0;
	for(addr = 0x0000; addr < size; )
	{
		if(err == ERR_ABORT)
			goto exit;

		// resync if error
		if(err)
		{
			PAUSE(500);

			for(i = 0; i < MAX_RETRY; i++)
			{
				err = rom_send_RDY(handle);
				if(err) continue;
				err = rom_recv_RDY(handle);
				if(err) continue;
			}
			if(i == MAX_RETRY && err)
				goto exit;
			err = 0;
		}

		if(tifiles_calc_is_ti9x(handle->model) && addr >= 0x10000 && addr < 0x12000)
		{
			// certificate is read protected: skip
			memset(data, 0xff, length);
			if (fwrite(data, length, 1, f) < 1)
				return ERR_SAVE_FILE;
			addr += length;
			continue;
		}

		// receive data
		err = rom_send_DATA(handle, addr);
		if(err) continue;
		err = rom_recv_DATA(handle, &length, data);
		if(err) continue;

		if (fwrite(data, length, 1, f) < 1)
			return ERR_SAVE_FILE;
		addr += length;

		update_->cnt2 = addr;
		update_->max2 = size;
		update_->pbar();
	}

	ticalcs_info("Saved %i blocks on %i blocks\n", sav_blk, sav_blk + std_blk);

	// finished
exit:
	fclose(f);

	if (!err)
	{
		PAUSE(200);
		err = rom_send_EXIT(handle);
		if (!err)
		{
			err = rom_recv_EXIT(handle);
		}
		PAUSE(1000);
	}

	return err;
}

int rd_is_ready(CalcHandle* handle)
{
	int err;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	err = rom_send_RDY(handle);
	if (!err)
	{
		err = rom_recv_RDY(handle);
	}
	return err;
}

int rd_send(CalcHandle *handle, const char *prgname, uint16_t size, uint8_t *data)
{
	char *template, *tempfname;
	int fd, ret;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	/* Write ROM dumper to a temporary file (note that the file must have
	   the correct suffix or tifiles_file_read_regular will be
	   confused) */

	template = g_strconcat("rdXXXXXX", strrchr(prgname, '.'), NULL);
	fd = g_file_open_tmp(template, &tempfname, NULL);
	g_free(template);
	if (fd == -1)
	{
		return ERR_FILE_OPEN;
	}

	ret = write(fd, data, size);
	close(fd);
	if (ret != size)
	{
		return ERR_FATAL_ERROR;
	}

	// Transfer program to calc
	handle->busy = 0;
	ret = ticalcs_calc_send_var2(handle, MODE_SEND_EXEC_ASM, tempfname);
	g_unlink(tempfname);
	g_free(tempfname);

	return ret;
}
