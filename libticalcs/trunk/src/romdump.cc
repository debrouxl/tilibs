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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <glib/gstdio.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "romdump.h"

#ifdef __WIN32__
#include <io.h>
#endif

#define MAX_RETRY	3

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

// --- Packet Layer

static int send_pkt(CalcHandle* handle, uint16_t cmd, uint16_t len, uint8_t* data)
{
	int ret = 0;
	CalcEventData event;

	uint8_t * buf = (uint8_t *)handle->buffer;

	// command
	buf[0] = LSB(cmd);
	buf[1] = MSB(cmd);

	// length
	buf[2] = LSB(len);
	buf[3] = MSB(len);

	// data
	if (data)
	{
		memmove(buf+4, data, len);
	}

	// checksum
	const uint16_t sum = tifiles_checksum(buf, 4 + len);
	buf[len+4+0] = LSB(sum);
	buf[len+4+1] = MSB(sum);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_SEND_ROMDUMP_PKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_romdump_pkt(&event, /* length */ len, /* cmd */ cmd, /* data */ data);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		ret = ticables_cable_send(handle->cable, buf, len + 6);
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_SEND_ROMDUMP_PKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_romdump_pkt(&event, /* length */ len, /* cmd */ cmd, /* data */ data);
	ret = ticalcs_event_send(handle, &event);

	return ret;
}

static inline int cmd_is_valid(uint16_t cmd)
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
}

static int recv_pkt(CalcHandle* handle, uint16_t* cmd, uint16_t* len, uint8_t* data)
{
	int i;
	int ret = 0;
	uint8_t * buf = (uint8_t *)handle->buffer;
	CalcEventData event;

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_RECV_ROMDUMP_PKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_romdump_pkt(&event, /* length */ 0, /* cmd */ 0, /* data */ data);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		// Any packet has always at least 4 bytes (cmd, len)
		ret = ticables_cable_recv(handle->cable, buf, 4);
		if (!ret)
		{

			*cmd = (((uint16_t)buf[1]) << 8) | buf[0];
			*len = (((uint16_t)buf[3]) << 8) | buf[2];

			if (!cmd_is_valid(*cmd))
			{
				ret = ERR_INVALID_CMD;
				goto exit;
			}

			if (*cmd == CMD_ERROR)
			{
				ret = ERR_ROM_ERROR;
				goto exit;
			}

			// compute chunks
			handle->priv.progress_min_size = 256;
			handle->priv.progress_blk_size = *len / 20;
			if (handle->priv.progress_blk_size == 0)
			{
				handle->priv.progress_blk_size = 1;
			}

			const int q = *len / handle->priv.progress_blk_size;
			const int r = *len % handle->priv.progress_blk_size;
			handle->updat->cnt1 = 0;
			handle->updat->max1 = *len;

			// recv full chunks
			for(i = 0; i < q; i++)
			{
				ret = ticables_cable_recv(handle->cable, &buf[i*handle->priv.progress_blk_size + 4], handle->priv.progress_blk_size);
				if (ret)
				{
					goto exit;
				}
				ticables_progress_get(handle->cable, nullptr, nullptr, &handle->updat->rate);
				handle->updat->cnt1 += handle->priv.progress_blk_size;
				if (*len > handle->priv.progress_min_size)
				{
					ticalcs_update_pbar(handle);
				}
				//if (ticalcs_update_canceled(handle))
				//	return ERR_ABORT;
			}

			// recv last chunk
			{
				ret = ticables_cable_recv(handle->cable, &buf[i*handle->priv.progress_blk_size + 4], (uint16_t)(r+2));
				if (ret)
				{
					goto exit;
				}
				ticables_progress_get(handle->cable, nullptr, nullptr, &handle->updat->rate);
				handle->updat->cnt1++;
				if (*len > handle->priv.progress_min_size)
				{
					ticalcs_update_pbar(handle);
				}
				if (ticalcs_update_canceled(handle))
				{
					ret = ERR_ABORT;
					goto exit;
				}
			}

			// verify checksum
			const uint16_t chksum = ((uint16_t)buf[*len + 4 + 1] << 8) | buf[*len + 4 + 0];
			const uint16_t sum = tifiles_checksum(buf, *len + 4);
			//printf("<%04x %04x>\n", sum, chksum);

			if (chksum != sum)
			{
				ticalcs_info(" TI->PC: CHECKSUM ERROR received:%04X computed:%04X !!!", chksum, sum);
				ret = ERR_CHECKSUM;
				goto exit;
			}

			if (data)
			{
				memmove(data, buf+4, *len);
			}
		}
	}
exit:

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_RECV_ROMDUMP_PKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_romdump_pkt(&event, /* length */ *len, /* cmd */ *cmd, /* data */ data);
	ret = ticalcs_event_send(handle, &event);

	return ret;
}

// --- Command Layer

static int rom_send_RDY(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: IS_READY");
	return send_pkt(handle, CMD_IS_READY, 0, nullptr);
}

static int rom_recv_RDY(CalcHandle* handle)
{
	uint16_t cmd = 0, len = 0;

	const int ret = recv_pkt(handle, &cmd, &len, nullptr);
	ticalcs_info(" TI->PC: %s", ret ? "ERROR" : (cmd ? "OK" : "KO"));

	return ret;
}

static int rom_send_EXIT(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: EXIT");
	return send_pkt(handle, CMD_EXIT, 0, nullptr);
}

static int rom_recv_EXIT(CalcHandle* handle)
{
	uint16_t cmd = 0, len = 0;

	const int ret = recv_pkt(handle, &cmd, &len, nullptr);
	ticalcs_info(" TI->PC: EXIT");

	return ret;
}

static int rom_send_SIZE(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: REQ_SIZE");
	return send_pkt(handle, CMD_REQ_SIZE, 0, nullptr);
}

static int rom_recv_SIZE(CalcHandle* handle, uint32_t* size)
{
	uint16_t cmd = 0, len = 0;

	const int ret = recv_pkt(handle, &cmd, &len, (uint8_t*)size);
	ticalcs_info(" TI->PC: SIZE (0x%08x bytes)", *size);

	return ret;
}

static int rom_send_DATA(CalcHandle* handle, uint32_t addr)
{
	ticalcs_info(" PC->TI: REQ_BLOCK at @%08x", addr);
	return send_pkt(handle, CMD_REQ_BLOCK, 4, (uint8_t *)&addr);
}

static int rom_recv_DATA(CalcHandle* handle, uint16_t* size, uint8_t* data)
{
	uint16_t cmd = 0;

	int ret = recv_pkt(handle, &cmd, size, data);
	if (!ret)
	{
		if (cmd == CMD_DATA1)
		{
			ticalcs_info(" TI->PC: BLOCK WITHOUT REPEATED DATA (0x%04x bytes)", *size);
			handle->priv.romdump_std_blk++;
		}
		else if (cmd == CMD_DATA2)
		{
			*size = (((uint16_t)data[1]) << 8) | data[0];
			const uint16_t rpt = (((uint16_t)data[3]) << 8) | data[2];
			memset(data, rpt, *size);
			ticalcs_info(" TI->PC: BLOCK WITH REPEATED DATA (0x%04x bytes)", *size);
			handle->priv.romdump_sav_blk++;
		}
		else
		{
			ticalcs_info(" TI->PC: INVALID COMMAND !!!");
			ret = ERR_INVALID_CMD;
		}
	}

	return ret;
}

/*static int rom_send_ERR(CalcHandle* handle)
{
	ticalcs_info(" PC->TI: ERROR");
	return send_pkt(handle, CMD_ERROR, 0, NULL);
}*/

// --- Dumping Layer

int TICALL rd_read_dump(CalcHandle* handle, const char *filename)
{
	int ret = 0;
	uint32_t size;
	uint16_t length;
	uint32_t i;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	uint8_t* data = (uint8_t*)handle->buffer;

	FILE* f = fopen(filename, "wb");
	if (f == nullptr)
	{
		return ERR_OPEN_FILE;
	}

	SET_HANDLE_BUSY_IF_NECESSARY(handle);

	ticalcs_strlcpy(handle->updat->text, "Receiving data...", sizeof(handle->updat->text));
	ticalcs_update_label(handle);

	// check if ready
	for(i = 0; i < 3; i++)
	{
		ret = rom_send_RDY(handle);
		if (rom_recv_RDY(handle))
		{
			goto exit; // Bail out.
		}
		if (!ret)
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
	handle->priv.romdump_std_blk = handle->priv.romdump_sav_blk = 0;
	for (uint32_t addr = 0x0000; addr < size; )
	{
		if (ret == ERR_ABORT)
		{
			goto exit;
		}

		// resync if error
		if (ret)
		{
			PAUSE(500);

			for(i = 0; i < MAX_RETRY; i++)
			{
				ret = rom_send_RDY(handle);
				if (ret)
				{
					continue;
				}
				ret = rom_recv_RDY(handle);
				if (ret)
				{
					continue;
				}
			}
			if (i == MAX_RETRY && ret)
			{
				goto exit;
			}
			ret = 0;
		}

		if (tifiles_calc_is_ti9x(handle->model) && handle->model != CALC_TI92 && addr >= 0x10000 && addr < 0x12000)
		{
			// certificate is read protected, with inconsistent reads: skip
			memset(data, 0xff, length);
			if (fwrite(data, length, 1, f) < 1)
			{
				ret = ERR_SAVE_FILE;
				goto exit;
			}
			addr += length;
			continue;
		}

		// receive data
		ret = rom_send_DATA(handle, addr);
		if (ret)
		{
			continue;
		}
		ret = rom_recv_DATA(handle, &length, data);
		if (ret)
		{
			ticalcs_info(" TI->PC: recv_DATA returned %d", ret);
			continue;
		}

		if (fwrite(data, length, 1, f) < 1)
		{
			ret = ERR_SAVE_FILE;
			goto exit;
		}
		addr += length;

		handle->updat->cnt2 = addr;
		handle->updat->max2 = size;
		ticalcs_update_pbar(handle);
	}

	ticalcs_info("Saved %i blocks on %i blocks\n", handle->priv.romdump_sav_blk, handle->priv.romdump_sav_blk + handle->priv.romdump_std_blk);

	// finished
exit:
	fclose(f);

	if (!ret)
	{
		PAUSE(200);
		ret = rom_send_EXIT(handle);
		if (!ret)
		{
			ret = rom_recv_EXIT(handle);
		}
		PAUSE(1000);
	}

	CLEAR_HANDLE_BUSY_IF_NECESSARY(handle);

	return ret;
}

int TICALL rd_is_ready(CalcHandle* handle)
{
	VALIDATE_HANDLE(handle);

	SET_HANDLE_BUSY_IF_NECESSARY(handle);

	int ret = rom_send_RDY(handle);
	if (!ret)
	{
		ret = rom_recv_RDY(handle);
	}

	CLEAR_HANDLE_BUSY_IF_NECESSARY(handle);

	return ret;
}

int TICALL rd_send_dumper(CalcHandle *handle, const char *prgname, uint16_t size, uint8_t *data)
{
	char*tempfname;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(prgname);
	VALIDATE_NONNULL(data);

	if (size < 64)
	{
		// File too short.
		return ERR_INVALID_PARAMETER;
	}

	// busy will be taken adequately by the subroutines.

	/* Write ROM dumper to a temporary file (note that the file must have
	   the correct suffix or tifiles_file_read_regular will be
	   confused) */

	char* templatename = g_strconcat("rdXXXXXX", strrchr(prgname, '.'), NULL);
	const int fd = g_file_open_tmp(templatename, &tempfname, nullptr);
	g_free(templatename);
	if (fd == -1)
	{
		ret = ERR_FILE_OPEN;
		goto end;
	}

	ret = write(fd, data, size);
	close(fd);
	if (ret == size)
	{
		// Transfer program to calc, using special internal API, taking busy if it's not taken.
		ret = ticalcs_calc_send_var2_(handle, MODE_SEND_EXEC_ASM, tempfname, !handle->busy);
	}
	else
	{
		ret = ERR_FATAL_ERROR;
	}

	g_unlink(tempfname);
	g_free(tempfname);

end:
	return ret;
}

int TICALL rd_send_dumper2(CalcHandle *handle, const char *filename)
{
	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	// busy will be taken adequately by the subroutines.

	// Transfer program to calc, using special internal API, taking busy if it's not taken.
	const int ret = ticalcs_calc_send_var2_(handle, MODE_SEND_EXEC_ASM, filename, !handle->busy);

	return ret;
}
