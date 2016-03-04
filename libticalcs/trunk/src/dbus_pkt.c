/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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
	This unit manages packets from/to D-BUS.
*/

#include <stdio.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "dbus_pkt.h"
#include "logging.h"
#include "error.h"

// We split packets into chunks to get control regularly and update statistics.

/*
    Send a packet from PC (host) to TI (target):
    - target [in] : a machine ID uint8_t
    - cmd [in]    : a command ID uint8_t
    - length [in] : length of buffer
    - data [in]   : data to send (or 0x00 if NULL)
    - int [out]   : an error code
*/
TIEXPORT3 int TICALL dbus_send(CalcHandle* handle, uint8_t target, uint8_t cmd, uint16_t len, const uint8_t* data)
{
	int i;
	uint16_t sum;
	uint32_t length = (len == 0x0000) ? 65536 : len;   // wrap around
	uint8_t *buf;
	int r, q;
	static int ref = 0;
	int ret;

	VALIDATE_HANDLE(handle);

	buf = (uint8_t *)handle->buffer;                    //[65536+6];
	if (buf == NULL)
	{
		ticalcs_critical("%s: handle->buffer is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticables_progress_reset(handle->cable);

	if (data == NULL)
	{
		// short packet (no data)
		buf[0] = target;
		buf[1] = cmd;
		buf[2] = 0x00;
		buf[3] = 0x00;

		// The TI-80 does not use length
		ret = ticables_cable_send(handle->cable, buf, (target == PC_TI80) ? 2 : 4);
	}
	else 
	{
		// std packet (data + checksum)
		buf[0] = target;
		buf[1] = cmd;
		buf[2] = LSB(length);
		buf[3] = MSB(length);

		// copy data
		memcpy(buf+4, data, length);

		// add checksum of packet
		sum = tifiles_checksum(data, length);
		buf[length+4+0] = LSB(sum);
		buf[length+4+1] = MSB(sum);

		// compute chunks
		handle->priv.progress_min_size = (handle->cable->model == CABLE_GRY) ? 512 : 2048;
		handle->priv.progress_blk_size = (length + 6) / 20;		// 5%
		if (handle->priv.progress_blk_size == 0)
		{
			handle->priv.progress_blk_size = length + 6;
		}
		if (handle->priv.progress_blk_size < 32)
		{
			handle->priv.progress_blk_size = 128;	// SilverLink doesn't like small block (< 32)
		}

		q = (length + 6) / handle->priv.progress_blk_size;
		r = (length + 6) % handle->priv.progress_blk_size;

		handle->updat->max1 = length + 6;
		handle->updat->cnt1 = 0;

		ret = 0;

		// send full chunks
		for (i = 0; i < q; i++)
		{
			ret = ticables_cable_send(handle->cable, &buf[i*handle->priv.progress_blk_size], handle->priv.progress_blk_size);
			if (ret)
			{
				break;
			}
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

			handle->updat->cnt1 += handle->priv.progress_blk_size;
			if (length > handle->priv.progress_min_size)
			{
				handle->updat->pbar();
			}

			if (handle->updat->cancel)
			{
				ret = ERR_ABORT;
				break;
			}
		}

		// send last chunk
		if (!ret)
		{
			ret = ticables_cable_send(handle->cable, &buf[i*handle->priv.progress_blk_size], (uint16_t)r);
			if (!ret)
			{
				ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

				handle->updat->cnt1 += 1;
				if (length > handle->priv.progress_min_size)
				{
					handle->updat->pbar();
				}

				if (handle->updat->cancel)
				{
					ret = ERR_ABORT;
				}
			}
		}
	}

	// force periodic refresh
	if (!ret && !(ref++ % 4))
	{
		handle->updat->refresh();
	}

	return ret;
}

TIEXPORT3 int TICALL dbus_recv_header(CalcHandle *handle, uint8_t* host, uint8_t* cmd, uint16_t* length)
{
	int ret;
	uint8_t buf[4];

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(host);
	VALIDATE_NONNULL(cmd);
	VALIDATE_NONNULL(length);

	// Any packet has always at least 2 bytes (MID, CID)
	ret = ticables_cable_recv(handle->cable, buf, 2);
	if (!ret)
	{
		*host = buf[0];
		*cmd = buf[1];

		// Any non-TI-80 packet has a length; TI-80 data packets also have a length
		if (*host != TI80_PC || *cmd == CMD_XDP)
		{
			ret = ticables_cable_recv(handle->cable, buf, 2);
			if (!ret)
			{
				*length = buf[0] | ((uint16_t)buf[1] << 8);
			}
		}
		else
		{
			*length = 0;
		}
	}

	return ret;
}

TIEXPORT3 int TICALL dbus_recv_data(CalcHandle *handle, uint16_t* length, uint8_t* data)
{
	int ret;
	int i;
	uint16_t chksum;
	uint8_t buf[4];
	int r, q;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(length);
	VALIDATE_NONNULL(data);

	// compute chunks
	handle->priv.progress_min_size = (handle->cable->model == CABLE_GRY) ? 512 : 2048;
	handle->priv.progress_blk_size = *length / 20;
	if (handle->priv.progress_blk_size == 0)
	{
		handle->priv.progress_blk_size = 1;
	}

	q = *length / handle->priv.progress_blk_size;
	r = *length % handle->priv.progress_blk_size;
	handle->updat->max1 = *length;
	handle->updat->cnt1 = 0;

	ret = 0;
	// recv full chunks
	for (i = 0; i < q; i++)
	{
		ret = ticables_cable_recv(handle->cable, &data[i*handle->priv.progress_blk_size], handle->priv.progress_blk_size);
		if (ret)
		{
			break;
		}
		ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

		handle->updat->cnt1 += handle->priv.progress_blk_size;
		if (*length > handle->priv.progress_min_size)
		{
			handle->updat->pbar();
		}

		if (handle->updat->cancel)
		{
			ret = ERR_ABORT;
			break;
		}
	}

	// recv last chunk
	if (!ret)
	{
		ret = ticables_cable_recv(handle->cable, &data[i*handle->priv.progress_blk_size], (uint16_t)r);
		if (!ret)
		{
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
			ret = ticables_cable_recv(handle->cable, buf, 2);
			if (!ret)
			{
				handle->updat->cnt1++;
				if (*length > handle->priv.progress_min_size)
				{
					handle->updat->pbar();
				}

				if (handle->updat->cancel)
				{
					ret = ERR_ABORT;
				}
			}
		}
	}

	if (!ret)
	{
		// verify checksum
		chksum = buf[0] | ((uint16_t)buf[1] << 8);
		if (chksum != tifiles_checksum(data, *length))
		{
			ret = ERR_CHECKSUM;
		}
	}

	return ret;
}

/*
  Receive a packet from TI (target) to PC (host):
  - host [out]	 : a machine ID uint8_t
  - cmd [out]	 : a command ID uint8_t
  - length [out] : length of buffer
  - data [out]	 : received data (depending on command)
  - int [out]	 : an error code
*/
TIEXPORT3 int TICALL dbus_recv(CalcHandle* handle, uint8_t* host, uint8_t* cmd, uint16_t* length, uint8_t* data)
{
	static int ref = 0;
	int ret;

	ret = dbus_recv_header(handle, host, cmd, length);
	if (!ret)
	{
		if (*cmd == CMD_ERR || *cmd == CMD_ERR2)
		{
			return ERR_CHECKSUM; // THIS RETURNS !
		}

		switch (*cmd)
		{
		case CMD_VAR:	// std packet ( data + checksum)
		case CMD_XDP:
		case CMD_SKP:
		case CMD_SID:
		case CMD_REQ:
		case CMD_IND:
		case CMD_RTS:
			ret = dbus_recv_data(handle, length, data);
			break;
		case CMD_CTS:	// short packet (no data)
		case CMD_ACK:
		case CMD_ERR:
		case CMD_ERR2:
		case CMD_RDY:
		case CMD_SCR:
		case CMD_RID:
		case CMD_KEY:
		case CMD_EOT:
		case CMD_CNT:
			break;
		default:
			return ERR_INVALID_CMD; // THIS RETURNS !
		}

		if (!ret && !(ref++ % 4))
		{
			// force periodic refresh
			handle->updat->refresh();
		}
	}

	return ret;
}
