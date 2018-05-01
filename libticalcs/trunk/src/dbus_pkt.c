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

typedef struct
{
	uint8_t     id;
	uint8_t     has_data;
	const char *name;
	const char *officialname;
	const char *description;
} DBUSCmdInfo;

static const DBUSCmdInfo cmd_types[] =
{
	{ DBUS_CMD_VAR,  1, "VAR",  "RTS",  "Variable header" },
	{ DBUS_CMD_CTS,  0, "CTS",  "RDY",  "Continue To Send" },
	{ DBUS_CMD_XDP,  1, "XDP",  "XDP",  "Data Packet" },
	{ DBUS_CMD_ELD,  0, "ELD",  "EBL",  "Enable LockDown" },
	{ DBUS_CMD_DLD,  0, "DLD",  "DBL",  "Disable LockDown" },
	{ DBUS_CMD_EKE,  0, "EKE",  "EKE",  "Enable Key Echo" },
	{ DBUS_CMD_DKE,  0, "DKE",  "DKE",  "Disable Key Echo" },
	{ DBUS_CMD_VER,  0, "VER",  "RDI",  "Request version" },
	{ DBUS_CMD_SKP,  1, "SKP",  "EOR",  "Skip/Exit" },
	{ DBUS_CMD_SID,  1, "SID",  "SIOA", "Send calc ID" },
	{ DBUS_CMD_ACK,  0, "ACK",  "ACK",  "Acknowledge" },
	{ DBUS_CMD_ERR,  1, "ERR",  "CHK",  "Checksum error" }, // ERR has data when receiving.
	{ DBUS_CMD_RDY,  0, "RDY",  "RDC",  "Test calculator ready" },
	{ DBUS_CMD_SCR,  0, "SCR",  "REQ",  "Request screenshot" },
	{ DBUS_CMD_GID,  0, "GID",  "???",  "Get calculator ID (from cert)" },
	{ DBUS_CMD_DMP,  1, "DMP",  "???",  "Request memory page dump" },
	{ DBUS_CMD_TG2,  1, "TG2",  "???",  "TestGuard 2 packet" },
	{ DBUS_CMD_RID,  0, "RID",  "RIOA", "Request calc ID" },
	{ DBUS_CMD_CNT,  0, "CNT",  "RTC",  "Continue" },
	{ DBUS_CMD_KEY,  0, "KEY",  "SKY",  "Send key" }, // KEY has special 4-byte format: no length.
	{ DBUS_CMD_DEL,  1, "DEL",  "DVL",  "Delete variable / app" },
	{ DBUS_CMD_RUN,  1, "RUN",  "EPL",  "Run variable / app" },
	{ DBUS_CMD_EOT,  0, "EOT",  "DONE", "End Of Transmission" },
	{ DBUS_CMD_REQ,  1, "REQ",  "GET",  "Request variable" },
	{ DBUS_CMD_ERR2, 1, "ERR2", "???",  "Checksum error" }, // ERR2 might have data when receiving ?
	{ DBUS_CMD_IND,  1, "IND",  "GTE",  "Request variable index" },
	{ DBUS_CMD_RTS,  1, "RTS",  "SEND", "Request To Send" },
	{ DBUS_CMD_RSE,  1, "RSE",  "SETP", "Request to Send and Execute" },
};

typedef struct
{
	uint8_t     id;
	const char *direction;
} DBUSMachineInfo;

static const DBUSMachineInfo machine_types[] =
{
	{ DBUS_MID_PC_TIXX, "PC>TI" },         // 0x00, also PC_TI80
	{ DBUS_MID_PC_TI82, "PC>TI" },         // 0x02
	{ DBUS_MID_PC_TI83, "PC>TI" },         // 0x03
	{ DBUS_MID_PC_TI85, "PC>TI" },         // 0x05
	{ DBUS_MID_PC_TI86, "PC>TI" },         // 0x06
	{ DBUS_MID_PC_TI73, "PC>TI" },         // 0x07
	{ DBUS_MID_PC_TI89, "PC>TI" },         // 0x08, also PC_TI89t / PC_TI92p / PC_V200
	{ DBUS_MID_PC_TI92, "PC>TI" },         // 0x09

	{ DBUS_MID_CBL_TI73, "CBL>TI" },       // 0x12, also CBL_TI82 / CBL_TI83 / CBL_TI83p / CBL_TI84p
	{ DBUS_MID_CBL_TI85, "CBL>TI" },       // 0x15, also CBL_TI86
	{ DBUS_MID_CBL_TI89, "CBL>TI" },       // 0x19, also CBL_TI89t / CBL_TI92 / CBL_TI92p / CBL_V200

	{ DBUS_MID_PC_TI83p, "PC>TI" },        // 0x23, also PC_TI84p

	{ DBUS_MID_TI83p_PC, "TI>PC" },        // 0x73, also TI84p_PC
	{ DBUS_MID_TI73_PC,  "TI>PC" },        // 0x74
	{ DBUS_MID_TI80_PC,  "TI>PC" },        // 0x80
	{ DBUS_MID_TI82_PC,  "TI>PC|TI_CBL" }, // 0x82, also TI82_CBL
	{ DBUS_MID_TI83_PC,  "TI>PC" },        // 0x83
	{ DBUS_MID_TI85_PC,  "TI>PC|TI>CBL" }, // 0x85, also TI85_CBL / TI86_CBL
	{ DBUS_MID_TI86_PC,  "TI>PC" },        // 0x86
	{ DBUS_MID_TI92p_PC, "TI>PC" },        // 0x88, also V200_PC
	{ DBUS_MID_TI92_PC,  "TI>PC|TI>CBL" }, // 0x89, also TI89_CBL / TI89T_CBL / TI92_CBL / TI82P_CBL / V200_CBL
	{ DBUS_MID_TI73_CBL, "TI>CBL" },       // 0x95, also TI893_CBL / TI83p_CBL / TI84p_CBL
	{ DBUS_MID_TI89_PC,  "TI>PC" },        // 0x98, also TI89t_PC

};

TIEXPORT3 const char* TICALL dbus_cmd2name(uint8_t id)
{
	unsigned int i;

	for (i = 0; i < sizeof(cmd_types) / sizeof(cmd_types[0]); i++)
	{
		if (id == cmd_types[i].id)
		{
			return cmd_types[i].name;
		}
	}

	return "";
}

TIEXPORT3 const char* TICALL dbus_cmd2officialname(uint8_t id)
{
	unsigned int i;

	for (i = 0; i < sizeof(cmd_types) / sizeof(cmd_types[0]); i++)
	{
		if (id == cmd_types[i].id)
		{
			return cmd_types[i].officialname;
		}
	}

	return "";
}

TIEXPORT3 const char* TICALL dbus_cmd2desc(uint8_t id)
{
	unsigned int i;

	for (i = 0; i < sizeof(cmd_types) / sizeof(cmd_types[0]); i++)
	{
		if (id == cmd_types[i].id)
		{
			return cmd_types[i].description;
		}
	}

	return "";
}

TIEXPORT3 const char* TICALL dbus_mid2direction(uint8_t id)
{
	unsigned int i;

	for (i = 0; i < sizeof(machine_types) / sizeof(machine_types[0]); i++)
	{
		if (id == machine_types[i].id)
		{
			return machine_types[i].direction;
		}
	}

	return "";
}


// We split packets into chunks to get control regularly and update statistics.

/*
    Send a packet from PC (host) to TI (target):
    - target [in] : a machine ID uint8_t
    - cmd [in]    : a command ID uint8_t
    - length [in] : length of buffer
    - data [in]   : data to send (or 0x00 if NULL)
    - int [out]   : an error code
*/
TIEXPORT3 int TICALL dbus_send(CalcHandle* handle, uint8_t target, uint8_t cmd, uint16_t len, uint8_t* data)
{
	int i;
	uint16_t sum;
	uint32_t length = (len == 0x0000) ? 65536 : len;   // wrap around
	uint8_t *buf;
	int r, q;
	static int ref = 0;
	int ret = 0;
	CalcEventData event;

	VALIDATE_HANDLE(handle);

	buf = (uint8_t *)handle->buffer;                    //[65536+6];
	if (buf == NULL)
	{
		ticalcs_critical("%s: handle->buffer is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticables_progress_reset(handle->cable);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_SEND_DBUS_PKT, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ length, /* id */ target, /* cmd */ cmd, /* data */ data);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		if (data == NULL)
		{
			// short packet (no data)
			buf[0] = target;
			buf[1] = cmd;
			buf[2] = 0x00;
			buf[3] = 0x00;

			// The TI-80 does not use length
			ret = ticables_cable_send(handle->cable, buf, (target == DBUS_MID_PC_TI80) ? 2 : 4);
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
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_SEND_DBUS_PKT, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ length, /* id */ target, /* cmd */ cmd, /* data */ data);
	ret = ticalcs_event_send(handle, &event);

	return ret;
}

TIEXPORT3 int TICALL dbus_recv_header(CalcHandle *handle, uint8_t* host, uint8_t* cmd, uint16_t* length)
{
	int ret = 0;
	uint8_t buf[4];
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(host);
	VALIDATE_NONNULL(cmd);
	VALIDATE_NONNULL(length);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_RECV_DBUS_PKT_HEADER, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ 0, /* id */ 0, /* cmd */ 0, /* data */ NULL);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
		// Any packet has always at least 2 bytes (MID, CID)
		ret = ticables_cable_recv(handle->cable, buf, 2);
		if (!ret)
		{
			*host = buf[0];
			*cmd = buf[1];

			// Any non-TI-80 packet has a length; TI-80 data packets also have a length
			if (*host != DBUS_MID_TI80_PC || *cmd == DBUS_CMD_XDP)
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
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_RECV_DBUS_PKT_HEADER, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ *length, /* id */ *host, /* cmd */ *cmd, /* data */ NULL);
	ret = ticalcs_event_send(handle, &event);

	return ret;
}

TIEXPORT3 int TICALL dbus_recv_data(CalcHandle *handle, uint16_t* length, uint8_t* data)
{
	int ret = 0;
	int i;
	uint16_t chksum;
	uint8_t buf[4];
	int r, q;
	CalcEventData event;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(length);
	VALIDATE_NONNULL(data);

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_RECV_DBUS_PKT_DATA, /* retval */ 0, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ *length, /* id */ 0, /* cmd */ 0, /* data */ data);
	ret = ticalcs_event_send(handle, &event);

	if (!ret)
	{
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
	}

	ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_RECV_DBUS_PKT_DATA, /* retval */ ret, /* operation */ CALC_FNCT_LAST);
	ticalcs_event_fill_dbus_pkt(&event, /* length */ *length, /* id */ 0, /* cmd */ 0, /* data */ data);
	ret = ticalcs_event_send(handle, &event);

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
		if (*cmd == DBUS_CMD_ERR || *cmd == DBUS_CMD_ERR2)
		{
			return ERR_CHECKSUM; // THIS RETURNS !
		}

		switch (*cmd)
		{
		case DBUS_CMD_VAR:	// std packet ( data + checksum)
		case DBUS_CMD_XDP:
		case DBUS_CMD_SKP:
		case DBUS_CMD_SID:
		case DBUS_CMD_REQ:
		case DBUS_CMD_IND:
		case DBUS_CMD_RTS:
			ret = dbus_recv_data(handle, length, data);
			break;
		case DBUS_CMD_CTS:	// short packet (no data)
		case DBUS_CMD_ACK:
		case DBUS_CMD_ERR:
		case DBUS_CMD_ERR2:
		case DBUS_CMD_RDY:
		case DBUS_CMD_SCR:
		case DBUS_CMD_RID:
		case DBUS_CMD_KEY:
		case DBUS_CMD_EOT:
		case DBUS_CMD_CNT:
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

TIEXPORT3 int TICALL dbus_dissect(CalcModel model, FILE * f, const uint8_t * data, uint32_t len)
{
	VALIDATE_NONNULL(f);
	VALIDATE_NONNULL(data);

	if (len < 2 || len > 65536U + 6)
	{
		ticalcs_critical("Length %lu (%lX) is too small or too large for a valid DBUS packet", (unsigned long)len, (unsigned long)len);
		return ERR_INVALID_PACKET;
	}

	// TODO, from libticables' hex2dbus.c.
	return ERR_INVALID_PACKET;
}
