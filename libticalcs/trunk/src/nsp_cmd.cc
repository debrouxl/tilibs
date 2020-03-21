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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"

#include "nsp_vpkt.h"
#include "nsp_cmd.h"

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

static int put_str(uint8_t *dst, const char *src)
{
	size_t i, j;
	size_t len = strlen(src);

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

int TICALL nsp_cmd_r_login(CalcHandle *handle)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  receiving login:");

	retval = nsp_recv_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

/////////////----------------

int TICALL nsp_cmd_s_status(CalcHandle *handle, uint8_t status)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new_ex(handle, 1, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, handle->priv.nsp_dst_port, NSP_CMD_STATUS, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 1));

	ticalcs_info("  sending status (%04x):", status);

	pkt->data[0] = status;
	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_status(CalcHandle *handle, uint8_t *status)
{
	NSPVirtualPacket* pkt;
	uint8_t value;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  receiving status:");

	retval = nsp_recv_data(handle, pkt);
	if (!retval)
	{
		value = pkt->data[0];

		if (pkt->cmd != NSP_CMD_STATUS)
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
	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

/////////////----------------

int TICALL nsp_cmd_s_dev_infos(CalcHandle *handle, uint8_t cmd)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  requesting device information (cmd = %02x):", cmd);

	pkt = nsp_vtl_pkt_new_ex(handle, 0, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_DEV_INFOS, cmd, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 0));

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_dev_infos(CalcHandle *handle, uint8_t *cmd, uint32_t *size, uint8_t **data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(cmd);
	VALIDATE_NONNULL(size);
	VALIDATE_NONNULL(data);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  receiving device information:");

	retval = nsp_recv_data(handle, pkt);
	if (!retval)
	{
		*cmd = pkt->cmd;
		*data = (uint8_t *)g_malloc0(pkt->size);
		if (NULL != *data)
		{
			memcpy(*data, pkt->data, pkt->size);
		}
		else
		{
			retval = ERR_MALLOC;
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

/////////////----------------

int TICALL nsp_cmd_s_screen_rle(CalcHandle *handle, uint8_t cmd)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new_ex(handle, 0, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_SCREEN_RLE, cmd, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 0));

	ticalcs_info("  requesting RLE screenshot (cmd = %02x):", cmd);

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_screen_rle(CalcHandle *handle, uint8_t *cmd, uint32_t *size, uint8_t **data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(cmd);
	VALIDATE_NONNULL(size);
	VALIDATE_NONNULL(data);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  receiving RLE screenshot:");

	pkt->size = *size;
	retval = nsp_recv_data(handle, pkt);
	if (!retval)
	{
		*cmd = pkt->cmd;
		*size = pkt->size;
		*data = (uint8_t *)g_malloc0(pkt->size);
		if (NULL != *data)
		{
			memcpy(*data, pkt->data, pkt->size);
		}
		else
		{
			retval = ERR_MALLOC;
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

/////////////----------------

int TICALL nsp_cmd_s_dir_attributes(CalcHandle *handle, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	len = strlen(name) < 8 ? 8 : strlen(name);
	pkt = nsp_vtl_pkt_new_ex(handle, 1 + len + 1, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_ATTRIBUTES, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 1 + len + 1));

	ticalcs_info("  unknown directory list command in <%s>:", name);

	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_dir_attributes(CalcHandle *handle, uint32_t *size, uint8_t *type, uint32_t *date)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  unknown directory list command reply received:");

	retval = nsp_recv_data(handle, pkt);
	if (!retval)
	{
		if (pkt->cmd != NSP_CMD_FM_ATTRIBUTES)
		{
			retval = ERR_CALC_ERROR3 + err_code(pkt->data[0]);
			goto end;
		}

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

end:
	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_s_dir_enum_init(CalcHandle *handle, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	len = strlen(name) < 8 ? 8 : strlen(name);

	pkt = nsp_vtl_pkt_new_ex(handle, len + 1, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DIRLIST_INIT, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, len + 1));

	ticalcs_info("  initiating directory listing in <%s>:", name);

	put_str(pkt->data, name);

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_dir_enum_init(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

int TICALL nsp_cmd_s_dir_enum_next(CalcHandle *handle)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new_ex(handle, 0, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DIRLIST_NEXT, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 0));
	if (pkt != NULL)
	{
		ticalcs_info("  requesting next directory entry:");

		retval = nsp_send_data(handle, pkt);

		nsp_vtl_pkt_del(handle, pkt);
	}
	else
	{
		retval = ERR_MALLOC;
	}

	return retval;
}

int TICALL nsp_cmd_r_dir_enum_next(CalcHandle *handle, char* name, uint32_t *size, uint8_t *type)
{
	NSPVirtualPacket* pkt;
	uint8_t data_size;
	//uint32_t date;
	int o;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  next directory entry:");

	retval = nsp_recv_data(handle, pkt);
	if (!retval)
	{
		if (pkt->cmd != NSP_CMD_FM_DIRLIST_ENT)
		{
			if (pkt->data[0] == NSP_ERR_NO_MORE_TO_LIST)
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
		ticalcs_strlcpy(name, (char *)pkt->data + 2, data_size + 1);
		o = data_size - 10;

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

end:
	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_s_dir_enum_done(CalcHandle *handle)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new_ex(handle, 0, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DIRLIST_DONE, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 0));

	ticalcs_info("  closing directory listing:");

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_dir_enum_done(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

/////////////----------------

int TICALL nsp_cmd_s_put_file(CalcHandle *handle, const char *name, uint32_t size)
{
	NSPVirtualPacket* pkt;
	int o;
	size_t len;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	len = strlen(name) < 8 ? 8 : strlen(name);
	pkt = nsp_vtl_pkt_new_ex(handle, 6 + len, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_PUT_FILE, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 6 + len));

	ticalcs_info("  sending variable:");

	pkt->data[0] = 0x01;
	o = put_str(pkt->data + 1, name);
	o++;

	pkt->data[o+0] = MSB(MSW(size));
	pkt->data[o+1] = LSB(MSW(size));
	pkt->data[o+2] = MSB(LSW(size));
	pkt->data[o+3] = LSB(LSW(size));

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_put_file(CalcHandle *handle)
{
	return nsp_cmd_r_file_ok(handle);
}

int TICALL nsp_cmd_s_put_file_eot(CalcHandle *handle)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new_ex(handle, 2, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_PUT_FILE_EOT, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 2));

	ticalcs_info("  sending EOT:");

	pkt->data[0] = 0x01;

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

// No nsp_cmd_r_put_file_eot because the calculator doesn't seem to reply to CMD_FM_PUT_FILE_EOT.

int TICALL nsp_cmd_s_get_file(CalcHandle *handle, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  requesting variable:");

	pkt = nsp_vtl_pkt_new_ex(handle, 2 + len, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_GET_FILE, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 2 + len));
	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_get_file(CalcHandle *handle, uint32_t *size)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  file size:");

	retval = nsp_recv_data(handle, pkt);

	if (!retval)
	{

		if (pkt->cmd != NSP_CMD_FM_PUT_FILE)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if (size)
		{
			*size = (  (((uint32_t)pkt->data[10]) << 24)
			         | (((uint32_t)pkt->data[11]) << 16)
			         | (((uint32_t)pkt->data[12]) <<  8)
			         | (((uint32_t)pkt->data[13])      ));
		}
	}

end:
	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_s_del_file(CalcHandle *handle, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	len = strlen(name) < 8 ? 8 : strlen(name);
	pkt = nsp_vtl_pkt_new_ex(handle, 2 + len, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DEL_FILE, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 2 + len));

	ticalcs_info("  deleting variable:");

	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_del_file(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

int TICALL nsp_cmd_s_new_folder(CalcHandle *handle, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	len = strlen(name) < 8 ? 8 : strlen(name);

	pkt = nsp_vtl_pkt_new_ex(handle, 2 + len, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_NEW_FOLDER, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 2 + len));

	ticalcs_info("  creating folder:");

	pkt->data[0] = 0x03;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_new_folder(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

int TICALL nsp_cmd_s_del_folder(CalcHandle *handle, const char *name)
{
	NSPVirtualPacket* pkt;
	size_t len;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);

	len = strlen(name) < 8 ? 8 : strlen(name);

	pkt = nsp_vtl_pkt_new_ex(handle, 2 + len, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_DEL_FOLDER, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 2 + len));

	ticalcs_info("  deleting folder:");

	pkt->data[0] = 0x03;
	put_str(pkt->data + 1, name);

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_del_folder(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

int TICALL nsp_cmd_s_copy_file(CalcHandle *handle, const char *name, const char *name2)
{
	NSPVirtualPacket* pkt;
	size_t len;
	size_t len2;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);
	VALIDATE_NONNULL(name2);

	len = strlen(name) < 8 ? 8 : strlen(name);
	len2 = strlen(name2) < 8 ? 8 : strlen(name2);

	pkt = nsp_vtl_pkt_new_ex(handle, 3 + len + len2, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_COPY_FILE, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 3 + len + len2));

	ticalcs_info("  copying file:");

	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);
	put_str(pkt->data + 2 + len, name2);

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_copy_file(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

int TICALL nsp_cmd_s_rename_file(CalcHandle *handle, const char *name, const char *name2)
{
	NSPVirtualPacket* pkt;
	size_t len;
	size_t len2;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(name);
	VALIDATE_NONNULL(name2);

	len = strlen(name) < 8 ? 8 : strlen(name);
	len2 = strlen(name2) < 8 ? 8 : strlen(name2);

	ticalcs_info("  renaming file:");

	pkt = nsp_vtl_pkt_new_ex(handle, 3 + len + len2, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_RENAME_FILE, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 3 + len + len2));
	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);
	put_str(pkt->data + 2 + len, name2);

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_rename_file(CalcHandle *handle)
{
	return nsp_cmd_r_status(handle, NULL);
}

int TICALL nsp_cmd_s_file_ok(CalcHandle *handle)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new_ex(handle, 0, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_OK, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 0));

	ticalcs_info("  sending file contents:");

	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_file_ok(CalcHandle *handle)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  file status:");

	retval = nsp_recv_data(handle, pkt);
	if (!retval)
	{
		if (pkt->cmd != NSP_CMD_FM_OK)
		{
			if (pkt->cmd == NSP_CMD_STATUS)
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

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_s_file_contents(CalcHandle *handle, uint32_t size, uint8_t *data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	pkt = nsp_vtl_pkt_new_ex(handle, size, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_FILE_MGMT, NSP_CMD_FM_CONTENTS, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, size));

	ticalcs_info("  sending file contents:");

	memcpy(pkt->data, data, size);
	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_file_contents(CalcHandle *handle, uint32_t *size, uint8_t **data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(size);
	VALIDATE_NONNULL(data);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  receiving file contents:");

	pkt->size = *size;
	retval = nsp_recv_data(handle, pkt);

	if (!retval)
	{
		*size = pkt->size;
		*data = (uint8_t *)g_malloc0(pkt->size);
		memcpy(*data, pkt->data, pkt->size);
	}

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

/////////////----------------

int TICALL nsp_cmd_s_os_install(CalcHandle *handle, uint32_t size)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new_ex(handle, 4, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_OS_INSTALL, NSP_CMD_OS_INSTALL, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 4));

	ticalcs_info("  installing OS:");

	pkt->data[0] = MSB(MSW(size));
	pkt->data[1] = LSB(MSW(size));
	pkt->data[2] = MSB(LSW(size));
	pkt->data[3] = LSB(LSW(size));
	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_os_install(CalcHandle *handle)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  receiving OS installation:");

	retval = nsp_recv_data(handle, pkt);

	if (!retval)
	{
		if (pkt->cmd != NSP_CMD_OS_OK)
		{
			retval = ERR_INVALID_PACKET;
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_s_os_contents(CalcHandle *handle, uint32_t size, uint8_t *data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(data);

	pkt = nsp_vtl_pkt_new_ex(handle, size, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_OS_INSTALL, NSP_CMD_OS_CONTENTS, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, size));

	ticalcs_info("  sending OS contents:");

	memcpy(pkt->data, data, size);
	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_progress(CalcHandle *handle, uint8_t *value)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(value);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  OS installation status:");

	retval = nsp_recv_data(handle, pkt);
	if (!retval)
	{
		*value = pkt->data[0];

		switch(pkt->cmd)
		{
		case NSP_CMD_OS_PROGRESS:
			ticalcs_info("  %i/100", *value);
			break;
		case NSP_CMD_STATUS:
			retval = ERR_CALC_ERROR3 + err_code(*value);
			break;
		default:
			retval = ERR_INVALID_PACKET;
			break;
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

/////////////----------------

int TICALL nsp_cmd_s_generic_data(CalcHandle *handle, uint32_t size, uint8_t *data, uint16_t sid, uint8_t cmd)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  sending generic data of size %lu (%lX) with command %02X:", (unsigned long)size, (unsigned long)size, cmd);

	pkt = nsp_vtl_pkt_new_ex(handle, size, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, sid, cmd, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, size));

	if (data)
	{
		memcpy(pkt->data, data, size);
	}
	retval = nsp_send_data(handle, pkt);

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

int TICALL nsp_cmd_r_generic_data(CalcHandle *handle, uint32_t *size, uint8_t **data)
{
	NSPVirtualPacket* pkt;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	pkt = nsp_vtl_pkt_new(handle);

	ticalcs_info("  receiving generic data:");

	retval = nsp_recv_data(handle, pkt);
	if (size)
	{
		*size = pkt->size;
	}

	if (data)
	{
		*data = (uint8_t *)g_malloc0(pkt->size);
		if (*data)
		{
			memcpy(*data, pkt->data, pkt->size);
		}
		else
		{
			retval = ERR_MALLOC;
		}
	}

	nsp_vtl_pkt_del(handle, pkt);

	return retval;
}

/////////////----------------

int TICALL nsp_cmd_s_echo(CalcHandle *handle, uint32_t size, uint8_t *data)
{
	ticalcs_info("  sending echo:");
	return nsp_cmd_s_generic_data(handle, size, data, NSP_PORT_ECHO, 0);
}

int TICALL nsp_cmd_r_echo(CalcHandle *handle, uint32_t *size, uint8_t **data)
{
	ticalcs_info("  receiving echo:");
	return nsp_cmd_r_generic_data(handle, size, data);
}

/////////////----------------

int TICALL nsp_cmd_s_key(CalcHandle *handle, uint32_t code)
{
	NSPVirtualPacket * pkt1, * pkt2;
	int retval = 0;

	VALIDATE_HANDLE(handle);

	ticalcs_info("  sending key:");

	retval = nsp_session_open(handle, NSP_SID_KEYPRESSES);
	if (!retval)
	{
		pkt1 = nsp_vtl_pkt_new_ex(handle, 3, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_KEYPRESSES, 0x01, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 3));

		pkt1->data[2] = 0x80;
		retval = nsp_send_data(handle, pkt1);

		if (!retval)
		{
			pkt2 = nsp_vtl_pkt_new_ex(handle, 25, NSP_SRC_ADDR, handle->priv.nsp_src_port, NSP_DEV_ADDR, NSP_PORT_KEYPRESSES, 0, (uint8_t *)nsp_vtl_pkt_alloc_data(handle, 25));

			pkt2->data[3] = 0x08;
			pkt2->data[4] = 0x02;
			pkt2->data[5] = (uint8_t)(code >> 16);
			pkt2->data[7] = (uint8_t)(code >>  8);
			pkt2->data[23] = (uint8_t)(code & 0xFF);

			retval = nsp_send_data(handle, pkt2);

			nsp_vtl_pkt_del(handle, pkt2);
		}

		nsp_vtl_pkt_del(handle, pkt1);

		if (!retval)
		{
			retval = nsp_session_close(handle);
		}
	}

	return retval;
}

int TICALL nsp_cmd_s_keypress_event(CalcHandle *handle, const uint8_t keycode[3])
{
	uint32_t key;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(keycode);

	key = ((uint32_t)(keycode[0]) << 16) | ((uint32_t)(keycode[1]) << 8) | (uint32_t)(keycode[2]);
	return nsp_cmd_s_key(handle, key);
}

// There doesn't seem to be a need for cmd_r_key / cmd_r_keypress_event.
