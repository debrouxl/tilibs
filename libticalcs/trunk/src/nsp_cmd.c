/* Hey EMACS -*- linux-c -*- */
/* $Id: nsp_cmd.c 2077 2006-03-31 21:16:19Z roms $ */

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
	This unit handles commands and session management thru DirectLink.
*/

#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "macros.h"
#include "pause.h"

#include "nsp_vpkt.h"
#include "nsp_cmd.h"

/////////////----------------

static const uint8_t usb_errors[] = {
	0x02, 0x04, 0x07, 0x0a, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x20, 0x80
};

static int err_code(uint8_t code)
{
	int i;

	for(i = 0; i < (int)(sizeof(usb_errors) / sizeof(usb_errors[0])); i++)
		if(usb_errors[i] == code)
			return i+1;

	ticalcs_warning("Nspire error code 0x%02x not found in list. Please report it at <tilp-devel@lists.sf.net>.", (int)code);

	return 0;
}

/////////////----------------

static int put_str(uint8_t *dst, const char *src)
{
	size_t i, j;
	size_t len = strlen(src);

	for(i = 0; i < len; i++)
	{
		dst[i] = src[i];
	}
	dst[i++] = '\0';

	if(i < 9)
	{
		for(j = i; j < 9; j++)
			dst[j] = '\0';
	}
	else
	{
		j = i;
	}

	return j;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_r_login(CalcHandle *h)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving login:");

	retval = nsp_recv_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_status(CalcHandle *h, uint8_t status)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new_ex(1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, nsp_dst_port);

	ticalcs_info("  sending status (%04x):", status);

	pkt->cmd = CMD_STATUS;
	pkt->data[0] = status;
	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_status(CalcHandle *h, uint8_t *status)
{
	NSPVirtualPacket* pkt;
	uint8_t value;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving status:");

	retval = nsp_recv_data(h, pkt);
	if (!retval)
	{
		value = pkt->data[0];

		if (pkt->cmd != CMD_STATUS)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if (status)
		{
			*status = value;
		}

		if (value != 0x00)
		{
			retval = ERR_CALC_ERROR3 + err_code(value);
		}
	}

end:
	nsp_vtl_pkt_del(pkt);

	return retval;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_dev_infos(CalcHandle *h, uint8_t cmd)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  requesting device information (cmd = %02x):", cmd);

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_DEV_INFOS);

	pkt->cmd = cmd;
	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_dev_infos(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	if (cmd == NULL || size == NULL || data == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving device information:");

	retval = nsp_recv_data(h, pkt);
	if (!retval)
	{
		*cmd = pkt->cmd;
		*data = g_malloc0(pkt->size); // aborts the program if it fails.
		memcpy(*data, pkt->data, pkt->size);
	}

	nsp_vtl_pkt_del(pkt);

	return retval;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_screen_rle(CalcHandle *h, uint8_t cmd)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_SCREEN_RLE);

	ticalcs_info("  requesting RLE screenshot (cmd = %02x):", cmd);

	pkt->cmd = cmd;
	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_screen_rle(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (cmd == NULL || size == NULL || data == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving RLE screenshot:");

	pkt->size = *size;
	retval = nsp_recv_data(h, pkt);
	if (!retval)
	{
		*cmd = pkt->cmd;
		*size = pkt->size;
		*data = g_malloc0(pkt->size); // aborts the program if it fails.
		memcpy(*data, pkt->data, pkt->size);
	}

	nsp_vtl_pkt_del(pkt);

	return retval;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_dir_attributes(CalcHandle *h, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL)
	{
		ticalcs_critical("%s: name is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	len = strlen(name) < 8 ? 8 : strlen(name);
	pkt = nsp_vtl_pkt_new_ex(1 + len + 1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  unknown directory list command in <%s>:", name);

	pkt->cmd = CMD_FM_ATTRIBUTES;

	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_dir_attributes(CalcHandle *h, uint32_t *size, uint8_t *type, uint32_t *date)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  unknown directory list command reply received:");

	retval = nsp_recv_data(h, pkt);
	if (!retval)
	{
		if(pkt->cmd != CMD_FM_ATTRIBUTES)
		{
			retval = ERR_CALC_ERROR3 + err_code(pkt->data[0]);
			goto end;
		}

		if(size)
		{
			//*size = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + 0)));
			*size = (  (((uint32_t)pkt->data[0]) << 24)
			         | (((uint32_t)pkt->data[1]) << 16)
			         | (((uint32_t)pkt->data[2]) <<  8)
			         | (((uint32_t)pkt->data[3])      ));
		}
		if(date)
		{
			//*date = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + 4)));
			*date = (  (((uint32_t)pkt->data[4]) << 24)
			         | (((uint32_t)pkt->data[5]) << 16)
			         | (((uint32_t)pkt->data[6]) <<  8)
			         | (((uint32_t)pkt->data[7])      ));
		}
		if(type)
		{
			*type = *(pkt->data + 8);
		}
	}

end:
	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_init(CalcHandle *h, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL)
	{
		ticalcs_critical("%s: name is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	len = strlen(name) < 8 ? 8 : strlen(name);

	pkt = nsp_vtl_pkt_new_ex(len + 1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  initiating directory listing in <%s>:", name);

	pkt->cmd = CMD_FM_DIRLIST_INIT;
	put_str(pkt->data, name);

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_init(CalcHandle *h)
{
	return nsp_cmd_r_status(h, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_next(CalcHandle *h)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);
	if (pkt != NULL)
	{
		ticalcs_info("  requesting next directory entry:");

		pkt->cmd = CMD_FM_DIRLIST_NEXT;

		retval = nsp_send_data(h, pkt);

		nsp_vtl_pkt_del(pkt);
	}
	else
	{
		retval = ERR_MALLOC;
	}

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_next(CalcHandle *h, char* name, uint32_t *size, uint8_t *type)
{
	NSPVirtualPacket* pkt;
	uint8_t data_size;
	//uint32_t date;
	int o;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL)
	{
		ticalcs_critical("%s: name is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  next directory entry:");

	retval = nsp_recv_data(h, pkt);
	if (!retval)
	{
		if(pkt->cmd != CMD_FM_DIRLIST_ENT)
		{
			if(pkt->data[0] == ERR_NO_MORE_TO_LIST)
			{
				retval = ERR_EOT;
				goto end;
			}
			else
			{
				retval = ERR_CALC_ERROR3 + err_code(pkt->data[0]);
				goto end;
			}
		}

		data_size = pkt->data[1] + 2;
		strcpy(name, (char *)pkt->data + 2);
		o = data_size - 10;

		if(size)
		{
			// *size = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + o)));
			*size = (  (((uint32_t)pkt->data[o    ]) << 24)
			         | (((uint32_t)pkt->data[o + 1]) << 16)
			         | (((uint32_t)pkt->data[o + 2]) <<  8)
			         | (((uint32_t)pkt->data[o + 3])      ));
		}
		//date = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + o + 4)));
		if(type)
		{
			*type = pkt->data[o + 8];
		}
	}

end:
	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_done(CalcHandle *h)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  closing directory listing:");

	pkt->cmd = CMD_FM_DIRLIST_DONE;

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_done(CalcHandle *h)
{
	return nsp_cmd_r_status(h, NULL);
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_put_file(CalcHandle *h, const char *name, uint32_t size)
{
	NSPVirtualPacket* pkt;
	int o;
	size_t len;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL)
	{
		ticalcs_critical("%s: name is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	len = strlen(name) < 8 ? 8 : strlen(name);
	pkt = nsp_vtl_pkt_new_ex(6 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  sending variable:");

	pkt->cmd = CMD_FM_PUT_FILE;
	pkt->data[0] = 0x01;
	o = put_str(pkt->data + 1, name);
	o++;

	pkt->data[o+0] = MSB(MSW(size));
	pkt->data[o+1] = LSB(MSW(size));
	pkt->data[o+2] = MSB(LSW(size));
	pkt->data[o+3] = LSB(LSW(size));

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_put_file(CalcHandle *h)
{
	return nsp_cmd_r_file_ok(h);
}

TIEXPORT3 int TICALL nsp_cmd_s_put_file_eot(CalcHandle *h)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new_ex(2, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  sending EOT:");

	pkt->cmd = CMD_FM_PUT_FILE_EOT;
	pkt->data[0] = 0x01;

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

// No nsp_cmd_r_put_file_eot because the calculator doesn't seem to reply to CMD_FM_PUT_FILE_EOT.

TIEXPORT3 int TICALL nsp_cmd_s_get_file(CalcHandle *h, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL)
	{
		ticalcs_critical("%s: name is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  requesting variable:");

	pkt = nsp_vtl_pkt_new_ex(2 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_GET_FILE;
	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_get_file(CalcHandle *h, uint32_t *size)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  file size:");

	retval = nsp_recv_data(h, pkt);

	if (!retval)
	{

		if(pkt->cmd != CMD_FM_PUT_FILE)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if(size)
		{
			// *size = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + 10)));
			*size = (  (((uint32_t)pkt->data[10]) << 24)
			         | (((uint32_t)pkt->data[11]) << 16)
			         | (((uint32_t)pkt->data[12]) <<  8)
			         | (((uint32_t)pkt->data[13])      ));
		}
	}

end:
	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_s_del_file(CalcHandle *h, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL)
	{
		ticalcs_critical("%s: name is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	len = strlen(name) < 8 ? 8 : strlen(name);
	pkt = nsp_vtl_pkt_new_ex(2 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  deleting variable:");

	pkt->cmd = CMD_FM_DEL_FILE;
	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_del_file(CalcHandle *h)
{
	return nsp_cmd_r_status(h, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_new_folder(CalcHandle *h, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL)
	{
		ticalcs_critical("%s: name is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	len = strlen(name) < 8 ? 8 : strlen(name);

	pkt = nsp_vtl_pkt_new_ex(2 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  creating folder:");

	pkt->cmd = CMD_FM_NEW_FOLDER;
	pkt->data[0] = 0x03;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_new_folder(CalcHandle *h)
{
	return nsp_cmd_r_status(h, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_del_folder(CalcHandle *h, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL)
	{
		ticalcs_critical("%s: name is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	len = strlen(name) < 8 ? 8 : strlen(name);

	pkt = nsp_vtl_pkt_new_ex(2 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  deleting folder:");

	pkt->cmd = CMD_FM_DEL_FOLDER;
	pkt->data[0] = 0x03;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_del_folder(CalcHandle *h)
{
	return nsp_cmd_r_status(h, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_copy_file(CalcHandle *h, const char *name, const char *name2)
{
	NSPVirtualPacket* pkt;
	size_t len;
	size_t len2;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL || name2 == NULL)
	{
		ticalcs_critical("%s: a parameter is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	len = strlen(name) < 8 ? 8 : strlen(name);
	len2 = strlen(name2) < 8 ? 8 : strlen(name2);

	pkt = nsp_vtl_pkt_new_ex(3 + len + len2, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  copying file:");

	pkt->cmd = CMD_FM_COPY_FILE;
	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);
	put_str(pkt->data + 2 + len, name2);

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_copy_file(CalcHandle *h)
{
	return nsp_cmd_r_status(h, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_rename_file(CalcHandle *h, const char *name, const char *name2)
{
	NSPVirtualPacket* pkt;
	size_t len;
	size_t len2;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (name == NULL || name2 == NULL)
	{
		ticalcs_critical("%s: a parameter is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	len = strlen(name) < 8 ? 8 : strlen(name);
	len2 = strlen(name2) < 8 ? 8 : strlen(name2);

	ticalcs_info("  renaming file:");

	pkt = nsp_vtl_pkt_new_ex(3 + len + len2, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_RENAME_FILE;
	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);
	put_str(pkt->data + 2 + len, name2);

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_rename_file(CalcHandle *h)
{
	return nsp_cmd_r_status(h, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_file_ok(CalcHandle *h)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  sending file contents:");

	pkt->cmd = CMD_FM_OK;

	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_file_ok(CalcHandle *h)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  file status:");

	retval = nsp_recv_data(h, pkt);
	if (!retval)
	{
		if(pkt->cmd != CMD_FM_OK)
		{
			if(pkt->cmd == CMD_STATUS)
			{
				retval = ERR_CALC_ERROR3 + err_code(pkt->data[0]);
			}
			else
			{
				retval = ERR_INVALID_PACKET;
			}
		}
		else
		{
			ticalcs_info("  ok");
		}
	}

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_s_file_contents(CalcHandle *h, uint32_t size, uint8_t *data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (data == NULL)
	{
		ticalcs_critical("%s: data is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = nsp_vtl_pkt_new_ex(size, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT);

	ticalcs_info("  sending file contents:");

	pkt->cmd = CMD_FM_CONTENTS;
	memcpy(pkt->data, data, size);
	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_file_contents(CalcHandle *h, uint32_t *size, uint8_t **data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (size == NULL || data == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving file contents:");

	pkt->size = *size;
	retval = nsp_recv_data(h, pkt);

	if (!retval)
	{
		*size = pkt->size;
		*data = g_malloc0(pkt->size);
		memcpy(*data, pkt->data, pkt->size);
	}

	nsp_vtl_pkt_del(pkt);

	return retval;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_os_install(CalcHandle *h, uint32_t size)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new_ex(4, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_OS_INSTALL);

	ticalcs_info("  installing OS:");

	pkt->cmd = CMD_OS_INSTALL;
	pkt->data[0] = MSB(MSW(size));
	pkt->data[1] = LSB(MSW(size));
	pkt->data[2] = MSB(LSW(size));
	pkt->data[3] = LSB(LSW(size));
	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_os_install(CalcHandle *h)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving OS installation:");

	retval = nsp_recv_data(h, pkt);

	if (!retval)
	{
		if(pkt->cmd != CMD_OS_OK)
		{
			retval = ERR_INVALID_PACKET;
		}
	}

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_s_os_contents(CalcHandle *h, uint32_t size, uint8_t *data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (data == NULL)
	{
		ticalcs_critical("%s: data is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = nsp_vtl_pkt_new_ex(size, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_OS_INSTALL);

	ticalcs_info("  sending OS contents:");

	pkt->cmd = CMD_OS_CONTENTS;
	memcpy(pkt->data, data, size);
	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_progress(CalcHandle *h, uint8_t *value)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (value == NULL)
	{
		ticalcs_critical("%s: value is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  OS installation status:");

	retval = nsp_recv_data(h, pkt);
	if (!retval)
	{
		*value = pkt->data[0];

		switch(pkt->cmd)
		{
		case CMD_OS_PROGRESS:
			ticalcs_info("  %i/100", *value);
			break;
		case CMD_STATUS:
			nsp_vtl_pkt_del(pkt);
			retval = ERR_CALC_ERROR3 + err_code(*value);
		default:
			nsp_vtl_pkt_del(pkt);
			retval = ERR_INVALID_PACKET;
		}
	}

	return retval;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_echo(CalcHandle *h, uint32_t size, uint8_t *data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	ticalcs_info("  sending echo:");

	pkt = nsp_vtl_pkt_new_ex(size, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_ECHO);

	pkt->cmd = 0;
	if(data)
	{
		memcpy(pkt->data, data, size);
	}
	retval = nsp_send_data(h, pkt);

	nsp_vtl_pkt_del(pkt);

	return retval;
}

TIEXPORT3 int TICALL nsp_cmd_r_echo(CalcHandle *h, uint32_t *size, uint8_t **data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving echo:");

	retval = nsp_recv_data(h, pkt);
	if(size)
	{
		*size = pkt->size;
	}

	if(data)
	{
		*data = g_malloc0(pkt->size);
		if(*data)
		{
			memcpy(*data, pkt->data, pkt->size);
		}
		else
		{
			retval = ERR_MALLOC;
		}
	}

	nsp_vtl_pkt_del(pkt);

	return retval;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_keypress_event(CalcHandle *h, const uint8_t keycode[3])
{
	NSPVirtualPacket * pkt1, * pkt2;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (keycode == NULL)
	{
		ticalcs_critical("%s: keycode is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  sending keypress event:");

	retval = nsp_session_open(h, SID_KEYPRESSES);
	if (!retval)
	{
		pkt1 = nsp_vtl_pkt_new_ex(3, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_KEYPRESSES);
		pkt2 = nsp_vtl_pkt_new_ex(25, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, NSP_PORT_KEYPRESSES);

		pkt1->cmd = 0x01;
		pkt1->data[2] = 0x80;
		retval = nsp_send_data(h, pkt1);

		if (!retval)
		{
			pkt2->cmd = 0;
			pkt2->data[3] = 0x08;
			pkt2->data[4] = 0x02;
			pkt2->data[5] = keycode[0];
			pkt2->data[7] = keycode[1];
			pkt2->data[23] = keycode[2];

			retval = nsp_send_data(h, pkt2);
		}

		nsp_vtl_pkt_del(pkt2);
		nsp_vtl_pkt_del(pkt1);

		if (!retval)
		{
			retval = nsp_session_close(h);
		}
	}

	return retval;
}

// There doesn't seem to be a need for cmd_r_keypress_event.
