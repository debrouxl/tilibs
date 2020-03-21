/* Hey EMACS -*- linux-c -*- */

/*  libticalcs - TI Calculator library, a part of the TILP project
 *  Copyright (C) 2007-2009  Romain Liévin
 *  Copyright (C) 2009-2019  Lionel Debroux
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "nsp_vpkt.h"
#include "nsp_cmd.h"

#define SEND_DATA nsp_send_data
#define RECV_DATA nsp_recv_data

/////////////----------------

static const uint8_t usb_errors[] = {
	0x02, 0x04, 0x07, 0x0a, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x20, 0x80
};

static int err_code(uint8_t code)
{
	int i;

	for (i = 0; i < (int)(sizeof(usb_errors) / sizeof(usb_errors[0])); i++)
	{
		if (usb_errors[i] == code)
		{
			return i+1;
		}
	}

	ticalcs_warning("Nspire error code 0x%02x not found in list. Please report it at <tilp-devel@lists.sf.net>.", (int)code);

	return 0;
}

/////////////----------------

static int put_str(uint8_t * dst, const char * src, size_t len)
{
	size_t i, j;

	for (i = 0; i < len; i++)
	{
		dst[i] = src[i];
	}
	dst[i++] = '\0';

	if (i < 9)
	{
		for (j = i; j < 9; j++)
		{
			dst[j] = '\0';
		}
	}
	else
	{
		j = i;
	}

	return j;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_r_login(CalcHandle * handle)
{
	int ret;

	ticalcs_info("  receiving login:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_status(CalcHandle * handle, uint8_t status)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  sending status (%04x):", status);

	uint8_t data[1] = { status };
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, handle->priv.nsp_dst_port, NSP_CMD_STATUS, 1, data };
	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_status(CalcHandle * handle, uint8_t * status)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  receiving status:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (!ret)
	{
		uint8_t value = pkt->data[0];

		if (pkt->cmd == NSP_CMD_STATUS)
		{
			if (status)
			{
				*status = value;
			}
			if (value)
			{
				ret = ERR_CALC_ERROR3 + err_code(value);
			}
		}
		else
		{
			ret = ERR_INVALID_PACKET;
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_dev_infos(CalcHandle * handle, uint8_t cmd)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  requesting device information (cmd = %02x):", cmd);

	uint8_t data[1] { 0 };
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_DEV_INFOS, cmd, 0, data };

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_dev_infos(CalcHandle * handle, uint8_t * cmd, uint32_t * size, uint8_t ** data)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(cmd);
	VALIDATE_NONNULL(size);
	VALIDATE_NONNULL(data);

	ticalcs_info("  receiving device information:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (!ret)
	{
		*cmd = pkt->cmd;
		*size = pkt->size;
		*data = pkt->data; // Borrow data.
		pkt->data = nullptr;
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_screen_rle(CalcHandle * handle, uint8_t cmd)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  requesting RLE screenshot (cmd = %02x):", cmd);

	uint8_t data[1] { 0 };
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_SCREEN_RLE, cmd, 0, data };

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_screen_rle(CalcHandle * handle, uint8_t * cmd, uint32_t * size, uint8_t ** data)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(cmd);
	VALIDATE_NONNULL(size);
	VALIDATE_NONNULL(data);

	ticalcs_info("  receiving RLE screenshot:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (!ret)
	{
		*cmd = pkt->cmd;
		*size = pkt->size;
		*data = pkt->data; // Borrow data.
		pkt->data = nullptr;
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_dir_attributes(CalcHandle * handle, const char * name)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	uint32_t len = strlen(name);
	uint32_t lenb = (len < 8) ? 8 : len;
	if (len > FULNAME_MAX)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  unknown directory list command in <%s>:", name);

	uint8_t data[1 + FULNAME_MAX + 1];
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_ATTRIBUTES, 1 + lenb + 1, data };
	pkt.data[0] = 0x01;
	put_str(pkt.data + 1, name, len);

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_dir_attributes(CalcHandle *handle, uint32_t *size, uint8_t *type, uint32_t *date)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  unknown directory list command reply received:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (!ret)
	{
		if (pkt->cmd == NSP_CMD_FM_ATTRIBUTES)
		{
			if (size)
			{
				*size = (  (((uint32_t)pkt->data[0]) << 24)
					 | (((uint32_t)pkt->data[1]) << 16)
					 | (((uint32_t)pkt->data[2]) <<  8)
					 | (((uint32_t)pkt->data[3])      ));
			}
			if (date)
			{
				*date = (  (((uint32_t)pkt->data[4]) << 24)
					 | (((uint32_t)pkt->data[5]) << 16)
					 | (((uint32_t)pkt->data[6]) <<  8)
					 | (((uint32_t)pkt->data[7])      ));
			}
			if (type)
			{
				*type = *(pkt->data + 8);
			}
		}
		else
		{
			ret = ERR_CALC_ERROR3 + err_code(pkt->data[0]);
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_init(CalcHandle *handle, const char *name)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	uint32_t len = strlen(name);
	uint32_t lenb = (len < 8) ? 8 : len;
	if (len > FULNAME_MAX)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  initiating directory listing in <%s>:", name);

	uint8_t data[FULNAME_MAX + 1];
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DIRLIST_INIT, lenb + 1, data };
	put_str(pkt.data, name, len);

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_init(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_next(CalcHandle *handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  requesting next directory entry:");

	uint8_t data[1] { 0 };
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DIRLIST_NEXT, 0, data };
	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_next(CalcHandle *handle, char* name, uint32_t *size, uint8_t *type)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	ticalcs_info("  next directory entry:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (!ret)
	{
		if (pkt->cmd == NSP_CMD_FM_DIRLIST_ENT)
		{
			uint8_t data_size = pkt->data[1] + 2;
			ticalcs_strlcpy(name, (char *)pkt->data + 2, data_size + 1);
			int o = data_size - 10;

			if (size)
			{
				*size = (  (((uint32_t)pkt->data[o    ]) << 24)
					 | (((uint32_t)pkt->data[o + 1]) << 16)
					 | (((uint32_t)pkt->data[o + 2]) <<  8)
					 | (((uint32_t)pkt->data[o + 3])      ));
			}
			if (type)
			{
				*type = pkt->data[o + 8];
			}
		}
		else
		{
			if (pkt->data[0] == NSP_ERR_NO_MORE_TO_LIST)
			{
				ret = ERR_EOT;
			}
			else
			{
				ret = ERR_CALC_ERROR3 + err_code(pkt->data[0]);
			}
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_s_dir_enum_done(CalcHandle *handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  closing directory listing:");

	uint8_t data[1] { 0 };
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DIRLIST_DONE, 0, data };

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_dir_enum_done(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_put_file(CalcHandle *handle, const char *name, uint32_t size)
{
	int ret;
	int o;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	uint32_t len = strlen(name);
	uint32_t lenb = (len < 8) ? 8 : len;
	if (len > FULNAME_MAX)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  sending variable:");

	uint8_t data[6 + FULNAME_MAX];
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_PUT_FILE, 6 + lenb, data };
	pkt.data[0] = 0x01;
	o = put_str(pkt.data + 1, name, len);
	o++;
	pkt.data[o + 0] = MSB(MSW(size));
	pkt.data[o + 1] = LSB(MSW(size));
	pkt.data[o + 2] = MSB(LSW(size));
	pkt.data[o + 3] = LSB(LSW(size));

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_put_file(CalcHandle *handle)
{
	return nsp_cmd_r_file_ok(handle);
}

TIEXPORT3 int TICALL nsp_cmd_s_put_file_eot(CalcHandle *handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  sending EOT:");

	uint8_t data[2] { 0x01, 0 };
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_PUT_FILE_EOT, 2, data };

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

// No nsp_cmd_r_put_file_eot because the calculator doesn't seem to reply to CMD_FM_PUT_FILE_EOT.

TIEXPORT3 int TICALL nsp_cmd_s_get_file(CalcHandle *handle, const char *name)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	uint32_t len = strlen(name);
	uint32_t lenb = (len < 8) ? 8 : len;
	if (len > FULNAME_MAX)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  requesting variable:");

	uint8_t data[2 + FULNAME_MAX];
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_GET_FILE, 2 + lenb, data };
	pkt.data[0] = 0x01;
	put_str(pkt.data + 1, name, len);

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_get_file(CalcHandle *handle, uint32_t *size)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  file size:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (!ret)
	{
		if (pkt->cmd == NSP_CMD_FM_PUT_FILE)
		{
			if (size)
			{
				*size = (  (((uint32_t)pkt->data[10]) << 24)
					 | (((uint32_t)pkt->data[11]) << 16)
					 | (((uint32_t)pkt->data[12]) <<  8)
					 | (((uint32_t)pkt->data[13])      ));
			}
		}
		else
		{
			ret = ERR_INVALID_PACKET;
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_s_del_file(CalcHandle *handle, const char *name)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	uint32_t len = strlen(name);
	uint32_t lenb = (len < 8) ? 8 : len;
	if (len > FULNAME_MAX)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  deleting variable:");

	uint8_t data[2 + FULNAME_MAX];
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DEL_FILE, 2 + lenb, data };
	pkt.data[0] = 0x01;
	put_str(pkt.data + 1, name, len);

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_del_file(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_new_folder(CalcHandle *handle, const char *name)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	uint32_t len = strlen(name);
	uint32_t lenb = (len < 8) ? 8 : len;
	if (len > FULNAME_MAX)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  creating folder:");

	uint8_t data[2 + FULNAME_MAX];
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_NEW_FOLDER, 2 + lenb, data };
	pkt.data[0] = 0x03;
	put_str(pkt.data + 1, name, len);

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_new_folder(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_del_folder(CalcHandle *handle, const char *name)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	uint32_t len = strlen(name);
	uint32_t lenb = (len < 8) ? 8 : len;
	if (len > FULNAME_MAX)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  deleting folder:");

	uint8_t data[2 + FULNAME_MAX];
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DEL_FOLDER, 2 + lenb, data };
	pkt.data[0] = 0x03;
	put_str(pkt.data + 1, name, len);

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_del_folder(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_copy_file(CalcHandle *handle, const char *name, const char *name2)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);
	VALIDATE_NONNULL(name2);

	uint32_t len = strlen(name);
	uint32_t lenb = (len < 8) ? 8 : len;
	uint32_t len2 = strlen(name2);
	uint32_t len2b = (len2 < 8) ? 8 : len2;
	if (len > FULNAME_MAX || len2 > FULNAME_MAX)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  copying file:");

	uint8_t data[3 + FULNAME_MAX + FULNAME_MAX];
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_COPY_FILE, 3 + lenb + len2b, data };
	pkt.data[0] = 0x01;
	put_str(pkt.data + 1, name, lenb);
	put_str(pkt.data + 2 + lenb, name2, len2b);

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_copy_file(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_rename_file(CalcHandle *handle, const char *name, const char *name2)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);
	VALIDATE_NONNULL(name2);

	uint32_t len = strlen(name);
	uint32_t lenb = (len < 8) ? 8 : len;
	uint32_t len2 = strlen(name2);
	uint32_t len2b = (len2 < 8) ? 8 : len2;
	if (len > FULNAME_MAX || len2 > FULNAME_MAX)
	{
		return ERR_INVALID_PARAMETER;
	}

	ticalcs_info("  renaming file:");

	uint8_t data[3 + FULNAME_MAX + FULNAME_MAX];
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_RENAME_FILE, 3 + lenb + len2b, data };
	pkt.data[0] = 0x01;
	put_str(pkt.data + 1, name, len);
	put_str(pkt.data + 2 + lenb, name2, len2);

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_rename_file(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

TIEXPORT3 int TICALL nsp_cmd_s_file_ok(CalcHandle *handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  sending file contents:");

	uint8_t data[1] { 0 };
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_OK, 0, data };

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_file_ok(CalcHandle *handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  file status:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (!ret)
	{
		if (pkt->cmd != NSP_CMD_FM_OK)
		{
			if (pkt->cmd == NSP_CMD_STATUS)
			{
				ret = ERR_CALC_ERROR3 + err_code(pkt->data[0]);
			}
			else
			{
				ret = ERR_INVALID_PACKET;
			}
		}
		else
		{
			ticalcs_info("  ok");
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_s_file_contents(CalcHandle *handle, uint32_t size, uint8_t *data)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	ticalcs_info("  sending file contents:");

	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_CONTENTS, size, data };

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_file_contents(CalcHandle *handle, uint32_t *size, uint8_t **data)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(size);
	VALIDATE_NONNULL(data);

	ticalcs_info("  receiving file contents:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);

	if (!ret)
	{
		*size = pkt->size;
		*data = pkt->data; // Borrow data.
		pkt->data = nullptr;
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_os_install(CalcHandle *handle, uint32_t size)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  installing OS:");

	uint8_t data[4] { MSB(MSW(size)), LSB(MSW(size)), MSB(LSW(size)), LSB(LSW(size)) };
	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_OS_INSTALL, NSP_CMD_OS_INSTALL, 4, data };

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_os_install(CalcHandle *handle)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  receiving OS installation:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (!ret)
	{
		if (pkt->cmd != NSP_CMD_OS_OK)
		{
			ret = ERR_INVALID_PACKET;
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_s_os_contents(CalcHandle *handle, uint32_t size, uint8_t *data)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	ticalcs_info("  sending OS contents:");

	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_OS_INSTALL, NSP_CMD_OS_CONTENTS, size, data };

	ret = SEND_DATA(handle, &pkt);

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_progress(CalcHandle *handle, uint8_t *value)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(value);

	ticalcs_info("  OS installation status:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (!ret)
	{
		*value = pkt->data[0];

		switch (pkt->cmd)
		{
		case NSP_CMD_OS_PROGRESS:
			ticalcs_info("  %i/100", *value);
			break;
		case NSP_CMD_STATUS:
			ret = ERR_CALC_ERROR3 + err_code(*value);
			break;
		default:
			ret = ERR_INVALID_PACKET;
			break;
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_generic_data(CalcHandle *handle, uint32_t size, uint8_t *data, uint16_t sid, uint8_t cmd)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  sending generic data of size %lu (%lX) with command %02X:", (unsigned long)size, (unsigned long)size, cmd);

	NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, sid, cmd, size, data }; // Borrow data.
	ret = SEND_DATA(handle, &pkt);

	// Do NOT free pkt.data.

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_r_generic_data(CalcHandle *handle, uint32_t *size, uint8_t **data)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  receiving generic data:");

	NSPVirtualPacket * pkt = nsp_vtl_pkt_new_ex(handle, 0, 0, 0, 0, 0, 0, nullptr);
	RETURN_ERR_MALLOC_IF_NULLPTR(pkt);
	ret = RECV_DATA(handle, pkt);
	if (size)
	{
		*size = pkt->size;
	}

	if (data)
	{
		*data = pkt->data; // Borrow data.
		pkt->data = nullptr;
	}

	nsp_vtl_pkt_del(handle, pkt);

	return ret;
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_echo(CalcHandle *handle, uint32_t size, uint8_t *data)
{
	ticalcs_info("  sending echo:");
	return nsp_cmd_s_generic_data(handle, size, data, NSP_PORT_ECHO, 0);
}

TIEXPORT3 int TICALL nsp_cmd_r_echo(CalcHandle *handle, uint32_t *size, uint8_t **data)
{
	ticalcs_info("  receiving echo:");
	return nsp_cmd_r_generic_data(handle, size, data);
}

/////////////----------------

TIEXPORT3 int TICALL nsp_cmd_s_key(CalcHandle *handle, uint32_t code)
{
	int ret;

	VALIDATE_HANDLE(handle);

	ret = nsp_session_open(handle, NSP_SID_KEYPRESSES);
	if (!ret)
	{
		uint8_t data[25];

		ticalcs_info("  sending key:");

		NSPVirtualPacket pkt { NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_KEYPRESSES, 0x01, 3, data };
		pkt.data[0] = 0;
		pkt.data[1] = 0;
		pkt.data[2] = 0x80;

		ret = SEND_DATA(handle, &pkt);

		if (!ret)
		{
			nsp_vtl_pkt_fill(&pkt, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_KEYPRESSES, 0, 25, data);

			memset((void *)data, 0, sizeof(data));
			pkt.data[3] = 0x08;
			pkt.data[4] = 0x02;
			pkt.data[5] = (uint8_t)(code >> 16);
			pkt.data[7] = (uint8_t)(code >>  8);
			pkt.data[23] = (uint8_t)(code & 0xFF);

			ret = SEND_DATA(handle, &pkt);
		}

		if (!ret)
		{
			ret = nsp_session_close(handle);
		}
	}

	return ret;
}

TIEXPORT3 int TICALL nsp_cmd_s_keypress_event(CalcHandle *handle, const uint8_t keycode[3])
{
	uint32_t key;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(keycode);

	key = ((uint32_t)(keycode[0]) << 16) | ((uint32_t)(keycode[1]) << 8) | (uint32_t)(keycode[2]);
	return nsp_cmd_s_key(handle, key);
}

// There doesn't seem to be a need for cmd_r_key / cmd_r_keypress_event.
