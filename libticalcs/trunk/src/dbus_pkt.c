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

#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "dbus_pkt.h"
#include "logging.h"
#include "error.h"
#include "macros.h"

// We split packets into chucks to get control regularly and update statistics.
static unsigned int BLK_SIZE; // refresh pbars every 5%
static unsigned int MIN_SIZE; // don't refresh at all if packet is < 512 bytes

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

	buf = (uint8_t *)handle->priv2;                    //[65536+6];
	if (buf == NULL)
	{
		ticalcs_critical("%s: handle->priv2 is NULL", __FUNCTION__);
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
		MIN_SIZE = (handle->cable->model == CABLE_GRY) ? 512 : 2048;
		BLK_SIZE = (length + 6) / 20;		// 5%
		if (BLK_SIZE == 0)
		{
			BLK_SIZE = length + 6;
		}
		if (BLK_SIZE < 32)
		{
			BLK_SIZE = 128;	// SilverLink doesn't like small block (< 32)
		}

		q = (length + 6) / BLK_SIZE;
		r = (length + 6) % BLK_SIZE;

		handle->updat->max1 = length + 6;
		handle->updat->cnt1 = 0;

		ret = 0;

		// send full chunks
		for (i = 0; i < q; i++)
		{
			ret = ticables_cable_send(handle->cable, &buf[i*BLK_SIZE], BLK_SIZE);
			if (ret)
			{
				break;
			}
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

			handle->updat->cnt1 += BLK_SIZE;
			if (length > MIN_SIZE)
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
			ret = ticables_cable_send(handle->cable, &buf[i*BLK_SIZE], (uint16_t)r);
			if (!ret)
			{
				ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

				handle->updat->cnt1 += 1;
				if (length > MIN_SIZE)
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

#if 0
static uint8_t host_ids(CalcHandle *handle)
{
	switch (handle->model)
	{
	case CALC_TI73:
		return TI73_PC;
	case CALC_TI80:
		return TI80_PC;
	case CALC_TI82:
		return TI82_PC;
	case CALC_TI83:
		return TI83_PC;
	case CALC_TI83P:
	case CALC_TI84P:
		return TI83p_PC;
	case CALC_TI85:
		return TI85_PC;
	case CALC_TI86:
		return TI86_PC;
	case CALC_TI89:
	case CALC_TI89T:
		return TI89_PC;
	case CALC_TI92:
		return TI92_PC;
	case CALC_TI92P:
		return TI92p_PC;
	case CALC_V200:
		return V200_PC;
	default:
		return 0x00;
	}
	return 0x00;
}
#endif

static int dbus_recv_(CalcHandle* handle, uint8_t* host, uint8_t* cmd, uint16_t* length, uint8_t* data, int host_check)
{
	int i;
	uint16_t chksum;
	uint8_t buf[4];
	int r, q;
	static int ref = 0;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(host);
	VALIDATE_NONNULL(cmd);
	VALIDATE_NONNULL(length);

	// Any packet has always at least 2 bytes (MID, CID)
	ret = ticables_cable_recv(handle->cable, buf, 2);
	if (ret)
	{
		return ret;
	}

	*host = buf[0];
	*cmd = buf[1];

	// Any non-TI-80 packet has a length; TI-80 data packets also have a length
	if (*host != TI80_PC || *cmd == CMD_XDP)
	{
		ret = ticables_cable_recv(handle->cable, buf, 2);
		if (ret)
		{
			return ret; // THIS RETURNS !
		}

		*length = buf[0] | ((uint16_t)buf[1] << 8);
	}
	else
	{
		*length = 0;
	}

	//removed for probing (pb here !)
	//if(host_check && (*host != host_ids(handle))) 
	//	return ERR_INVALID_HOST;

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
		if (data == NULL)
		{
			ticalcs_critical("%s: data is NULL", __FUNCTION__);
			ret = ERR_INVALID_PARAMETER;
			break;
		}

		// compute chunks*
		MIN_SIZE = (handle->cable->model == CABLE_GRY) ? 512 : 2048;
		BLK_SIZE = *length / 20;
		if(BLK_SIZE == 0) BLK_SIZE = 1;

		q = *length / BLK_SIZE;
		r = *length % BLK_SIZE;
		handle->updat->max1 = *length;
		handle->updat->cnt1 = 0;

		ret = 0;
		// recv full chunks
		for (i = 0; i < q; i++)
		{
			ret = ticables_cable_recv(handle->cable, &data[i*BLK_SIZE], BLK_SIZE);
			if (ret)
			{
				break;
			}
			ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);

			handle->updat->cnt1 += BLK_SIZE;
			if (*length > MIN_SIZE)
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
			ret = ticables_cable_recv(handle->cable, &data[i*BLK_SIZE], (uint16_t)r);
			if (!ret)
			{
				ticables_progress_get(handle->cable, NULL, NULL, &handle->updat->rate);
				ret = ticables_cable_recv(handle->cable, buf, 2);
				if (!ret)
				{
					handle->updat->cnt1++;
					if (*length > MIN_SIZE)
					{
						handle->updat->pbar();
					}

					if (handle->updat->cancel)
					{
						ret = ERR_ABORT;
						break;
					}
				}
			}
		}

		if (ret)
		{
			break;
		}

		// verify checksum
		chksum = buf[0] | ((uint16_t)buf[1] << 8);
		if (chksum != tifiles_checksum(data, *length))
		{
			ret = ERR_CHECKSUM;
		}

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
	return dbus_recv_(handle, host, cmd, length, data, !0);
}

// used only by probe.c
int dbus_recv_2(CalcHandle* handle, uint8_t* host, uint8_t* cmd, uint16_t* length, uint8_t* data)
{
	return dbus_recv_(handle, host, cmd, length, data, 0);
}
