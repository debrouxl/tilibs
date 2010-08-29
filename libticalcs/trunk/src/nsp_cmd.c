/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd84p.c 2077 2006-03-31 21:16:19Z roms $ */

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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	This unit handles commands and session management thru DirectLink.
*/

#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "logging.h"
#include "error.h"
#include "macros.h"
#include "pause.h"

#include "nsp_vpkt.h"
#include "nsp_cmd.h"

/////////////----------------

static uint8_t usb_errors[] = { 
	0x04, 0x07, 0x0a, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16
};

static int err_code(uint8_t code)
{
	int i;

	for(i = 0; i < (int)(sizeof(usb_errors) / sizeof(usb_errors[0])); i++)
		if(usb_errors[i] == code)
			return i+1;

	ticalcs_warning("NSpire error code 0x%02x not found in list. Please report it at <tilp-devel@lists.sf.net>.", (int)code);

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

int cmd_r_login(CalcHandle *h)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving login:");

	TRYF(nsp_recv_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

/////////////----------------

int cmd_s_status(CalcHandle *h, uint8_t status)
{
	VirtualPacket* pkt;

	ticalcs_info("  sending status (%04x):", status);

	pkt = nsp_vtl_pkt_new_ex(1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, nsp_dst_port);
	pkt->cmd = CMD_STATUS;
	pkt->data[0] = status;
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);

	return 0;
}

int cmd_r_status(CalcHandle *h, uint8_t *status)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();
	uint8_t value;

	ticalcs_info("  receiving status:");

	TRYF(nsp_recv_data(h, pkt));
	value = pkt->data[0];

	if(pkt->cmd != CMD_STATUS)
		return ERR_INVALID_PACKET;

	if(status)
		*status = value;

	if(value != 0x00)
		return ERR_CALC_ERROR3 + err_code(value);

	nsp_vtl_pkt_del(pkt);
	return 0;
}

/////////////----------------

int cmd_s_dev_infos(CalcHandle *h, uint8_t cmd)
{
	VirtualPacket* pkt;

	ticalcs_info("  requesting device information (cmd = %02x):", cmd);

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_DEV_INFOS);
	pkt->cmd = cmd;
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_dev_infos(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving device information:");

	TRYF(nsp_recv_data(h, pkt));

	*cmd = pkt->cmd;
	*data = g_malloc0(pkt->size);
	memcpy(*data, pkt->data, pkt->size);

	nsp_vtl_pkt_del(pkt);
	return 0;
}

/////////////----------------

int cmd_s_screen_rle(CalcHandle *h, uint8_t cmd)
{
	VirtualPacket* pkt;

	ticalcs_info("  requesting RLE screenshot (cmd = %02x):", cmd);

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_SCREEN_RLE);
	pkt->cmd = cmd;
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_screen_rle(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving RLE screenshot:");

	pkt->size = *size;
	TRYF(nsp_recv_data(h, pkt));

	*cmd = pkt->cmd;
	*size = pkt->size;
	*data = g_malloc0(pkt->size);
	memcpy(*data, pkt->data, pkt->size);

	nsp_vtl_pkt_del(pkt);
	return 0;
}

/////////////----------------

int cmd_s_dir_attributes(CalcHandle *h, const char *name)
{
	VirtualPacket* pkt;
	size_t len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  unknown directory list command in <%s>:", name);

	pkt = nsp_vtl_pkt_new_ex(1 + len + 1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_ATTRIBUTES;

	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);
	
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_dir_attributes(CalcHandle *h, uint32_t *size, uint8_t *type, uint32_t *date)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  unknown directory list command reply received:");

	TRYF(nsp_recv_data(h, pkt));

	if(pkt->cmd != CMD_FM_ATTRIBUTES)
		return ERR_CALC_ERROR3 + err_code(pkt->data[0]);

	if(size)
		*size = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + 0)));
	if(date)
		*date = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + 4)));
	if(type)
		*type = *(pkt->data + 8);

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_s_dir_enum_init(CalcHandle *h, const char *name)
{
	VirtualPacket* pkt;
	size_t len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  initiating directory listing in <%s>:", name);

	pkt = nsp_vtl_pkt_new_ex(len + 1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_DIRLIST_INIT;
	put_str(pkt->data, name);
	
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_dir_enum_init(CalcHandle *h)
{
	return cmd_r_status(h, NULL);
}

int cmd_s_dir_enum_next(CalcHandle *h)
{
	VirtualPacket* pkt;

	ticalcs_info("  requesting next directory entry:");

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_DIRLIST_NEXT;

	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_dir_enum_next(CalcHandle *h, char* name, uint32_t *size, uint8_t *type)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();
	uint8_t data_size;
	uint32_t date;
	int o;

	ticalcs_info("  next directory entry:");

	TRYF(nsp_recv_data(h, pkt));

	if(pkt->cmd != CMD_FM_DIRLIST_ENT)
	{
		if(pkt->data[0] == ERR_NO_MORE_TO_LIST)
		{
			nsp_vtl_pkt_del(pkt);
			return ERR_EOT;
		}
		else
			return ERR_CALC_ERROR3 + err_code(pkt->data[0]);
	}

	data_size = pkt->data[1] + 2;
	strcpy(name, (char *)pkt->data + 2);
	o = data_size - 10;
	
	if(size)
		*size = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + o)));
	date = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + o + 4)));
	if(type)
		*type = pkt->data[o + 8];

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_s_dir_enum_done(CalcHandle *h)
{
	VirtualPacket* pkt;

	ticalcs_info("  closing directory listing:");

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_DIRLIST_DONE;

	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_dir_enum_done(CalcHandle *h)
{
	return cmd_r_status(h, NULL);
}

/////////////----------------

int cmd_s_put_file(CalcHandle *h, const char *name, uint32_t size)
{
	VirtualPacket* pkt;
	int o;
	size_t len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  sending variable:");

	pkt = nsp_vtl_pkt_new_ex(6 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_PUT_FILE;
	pkt->data[0] = 0x01;
	o = put_str(pkt->data + 1, name);
	o++;

	pkt->data[o+0] = MSB(MSW(size));
	pkt->data[o+1] = LSB(MSW(size));
	pkt->data[o+2] = MSB(LSW(size));
	pkt->data[o+3] = LSB(LSW(size));

	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_put_file(CalcHandle *h)
{
	return cmd_r_file_ok(h);
}

int cmd_s_get_file(CalcHandle *h, const char *name)
{
	VirtualPacket* pkt;
	size_t len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  requesting variable:");

	pkt = nsp_vtl_pkt_new_ex(2 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_GET_FILE;
	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);

	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_get_file(CalcHandle *h, uint32_t *size)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  file size:");

	TRYF(nsp_recv_data(h, pkt));

	if(pkt->cmd != CMD_FM_PUT_FILE)
	{
		nsp_vtl_pkt_del(pkt);
		return ERR_INVALID_PACKET;
	}
	
	if(size)
		*size = GUINT32_FROM_BE(*((uint32_t *)(pkt->data + 10)));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_s_del_file(CalcHandle *h, const char *name)
{
	VirtualPacket* pkt;
	size_t len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  deleting variable:");

	pkt = nsp_vtl_pkt_new_ex(2 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_DEL_FILE;
	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);

	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_del_file(CalcHandle *h)
{
	return cmd_r_status(h, NULL);
}

int cmd_s_new_folder(CalcHandle *h, const char *name)
{
	VirtualPacket* pkt;
	size_t len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  creating folder:");

	pkt = nsp_vtl_pkt_new_ex(2 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_NEW_FOLDER;
	pkt->data[0] = 0x03;
	put_str(pkt->data + 1, name);

	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_new_folder(CalcHandle *h)
{
	return cmd_r_status(h, NULL);
}

int cmd_s_del_folder(CalcHandle *h, const char *name)
{
	VirtualPacket* pkt;
	size_t len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  deleting folder:");

	pkt = nsp_vtl_pkt_new_ex(2 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_DEL_FOLDER;
	pkt->data[0] = 0x03;
	put_str(pkt->data + 1, name);

	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_del_folder(CalcHandle *h)
{
	return cmd_r_status(h, NULL);
}

int cmd_s_copy_file(CalcHandle *h, const char *name, const char *name2)
{
	VirtualPacket* pkt;
	size_t len = strlen(name) < 8 ? 8 : strlen(name);
	size_t len2 = strlen(name2) < 8 ? 8 : strlen(name2);

	ticalcs_info("  copying file:");

	pkt = nsp_vtl_pkt_new_ex(3 + len + len2, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_COPY_FILE;
	pkt->data[0] = 0x01;
	put_str(pkt->data + 1, name);
	put_str(pkt->data + 2 + len, name2);

	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_copy_file(CalcHandle *h)
{
	return cmd_r_status(h, NULL);
}

int cmd_s_file_ok(CalcHandle *h)
{
	VirtualPacket* pkt;

	ticalcs_info("  sending file contents:");

	pkt = nsp_vtl_pkt_new_ex(0, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_OK;
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_file_ok(CalcHandle *h)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  file status:");

	TRYF(nsp_recv_data(h, pkt));

	if(pkt->cmd != CMD_FM_OK)
	{
		if(pkt->cmd == CMD_STATUS)
		{
			uint8_t value = pkt->data[0];

			nsp_vtl_pkt_del(pkt);
			return ERR_CALC_ERROR3 + err_code(value);
		}
		else
		{
			nsp_vtl_pkt_del(pkt);
			return ERR_INVALID_PACKET;
		}
	}
	else
		ticalcs_info("  ok");

	return 0;
}

int cmd_s_file_contents(CalcHandle *h, uint32_t  size, uint8_t  *data)
{
	VirtualPacket* pkt;

	ticalcs_info("  sending file contents:");

	pkt = nsp_vtl_pkt_new_ex(size, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->cmd = CMD_FM_CONTENTS;
	memcpy(pkt->data, data, size);
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_file_contents(CalcHandle *h, uint32_t *size, uint8_t **data)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving file contents:");

	pkt->size = *size;
	TRYF(nsp_recv_data(h, pkt));

	*size = pkt->size;
	*data = g_malloc0(pkt->size);
	memcpy(*data, pkt->data, pkt->size);

	nsp_vtl_pkt_del(pkt);
	return 0;
}

/////////////----------------

int cmd_s_os_install(CalcHandle *h, uint32_t size)
{
	VirtualPacket* pkt;

	ticalcs_info("  installing OS:");

	pkt = nsp_vtl_pkt_new_ex(4, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_OS_INSTALL);
	pkt->cmd = CMD_OS_INSTALL;
	pkt->data[0] = MSB(MSW(size));
	pkt->data[1] = LSB(MSW(size));
	pkt->data[2] = MSB(LSW(size));
	pkt->data[3] = LSB(LSW(size));
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_os_install(CalcHandle *h)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving OS installation:");

	TRYF(nsp_recv_data(h, pkt));

	if(pkt->cmd != CMD_OS_OK)
		return ERR_INVALID_PACKET;

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_s_os_contents(CalcHandle *h, uint32_t size, uint8_t *data)

{
	VirtualPacket* pkt;

	ticalcs_info("  sending OS contents:");

	pkt = nsp_vtl_pkt_new_ex(size, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_OS_INSTALL);
	pkt->cmd = CMD_OS_CONTENTS;
	memcpy(pkt->data, data, size);
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_progress(CalcHandle *h, uint8_t *value)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  OS installation status:");

	TRYF(nsp_recv_data(h, pkt));
	*value = pkt->data[0];

	switch(pkt->cmd)
	{
	case CMD_OS_PROGRESS:
		ticalcs_info("  %i/100", *value);
		break;
	case CMD_STATUS:
		nsp_vtl_pkt_del(pkt);
		return ERR_CALC_ERROR3 + err_code(*value);
	default:
		nsp_vtl_pkt_del(pkt);
		return ERR_INVALID_PACKET;
	}

	return 0;
}

/////////////----------------

int cmd_s_echo(CalcHandle *h, uint32_t size, uint8_t *data)
{
	VirtualPacket* pkt;

	ticalcs_info("  sending echo:");

	pkt = nsp_vtl_pkt_new_ex(size, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_ECHO);
	pkt->cmd = 0;
	if(data) memcpy(pkt->data, data, size);
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);
	return 0;
}

int cmd_r_echo(CalcHandle *h, uint32_t *size, uint8_t **data)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving echo:");

	TRYF(nsp_recv_data(h, pkt));
	if(size) *size = pkt->size;
	if(data) *data = g_malloc0(pkt->size);
	if(size && data) memcpy(*data, pkt->data, pkt->size);

	nsp_vtl_pkt_del(pkt);
	return 0;
}
