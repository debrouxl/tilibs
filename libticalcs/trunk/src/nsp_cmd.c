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

static uint16_t usb_errors[] = { 
	0xff0a, 0xff0f, 0xff10, 0xff11
};

static int err_code(VirtualPacket *pkt)
{
	int i;
	int code = (pkt->data[0] << 8) | pkt->data[1];

	for(i = 0; i < sizeof(usb_errors) / sizeof(uint16_t); i++)
		if(usb_errors[i] == code)
			return i+1;

	ticalcs_warning("NSpire error code not found in list. Please report it at <tilp-devel@lists.sf.net>.");
	
	return 0;
}

/////////////----------------

int cmd_r_status(CalcHandle *h, uint16_t *status)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();
	uint16_t value;

	ticalcs_info("  receiving status:");

	TRYF(nsp_recv_data(h, pkt));

	value = (pkt->data[0] << 8) | pkt->data[1];
	//ticalcs_info("  %04x", value);

	nsp_vtl_pkt_del(pkt);

	if(status)
		*status = value;

	if(value != 0xff00)
		return ERR_CALC_ERROR3 + err_code(pkt);

	return 0;
}

int cmd_s_dev_infos(CalcHandle *h, uint8_t cmd)
{
	VirtualPacket* pkt;

	ticalcs_info("  requesting device information (cmd = %02x):", cmd);

	pkt = nsp_vtl_pkt_new_ex(1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_DEV_INFOS);
	pkt->data[0] = cmd;
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);

	return 0;
}

int cmd_r_dev_infos(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving device information:");

	TRYF(nsp_recv_data(h, pkt));

	*cmd = pkt->data[0];
	*data = g_malloc0(pkt->size);
	memcpy(*data, pkt->data + 1, pkt->size - 1);

	nsp_vtl_pkt_del(pkt);

	return 0;
}

int cmd_s_screen_rle(CalcHandle *h, uint8_t cmd)
{
	VirtualPacket* pkt;

	ticalcs_info("  requesting RLE screenshot (cmd = %02x):", cmd);

	pkt = nsp_vtl_pkt_new_ex(1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_SCREEN_RLE);
	pkt->data[0] = cmd;
	TRYF(nsp_send_data(h, pkt));

	nsp_vtl_pkt_del(pkt);

	return 0;
}

int cmd_r_screen_rle(CalcHandle *h, uint8_t *cmd, uint32_t *size, uint8_t **data)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();

	ticalcs_info("  receiving RLE screenshot:");

	TRYF(nsp_recv_data(h, pkt));

	*cmd = pkt->data[0];
	*size = pkt->size - 1;
	*data = g_malloc0(pkt->size);
	memcpy(*data, pkt->data + 1, pkt->size - 1);

	nsp_vtl_pkt_del(pkt);

	return 0;
}

int cmd_s_dir_enum_init(CalcHandle *h, const char *name)
{
	VirtualPacket* pkt;
	int i;
	uint8_t len = strlen(name) < 8 ? 8 : strlen(name);

	ticalcs_info("  initiating directory listing in <%s>:", name);

	pkt = nsp_vtl_pkt_new_ex(2 + len, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->data[0] = DL_INIT;
	strcpy(pkt->data + 1, name);
	if(strlen(name) < 8)
		for(i = strlen(name); i < 8; i++)
			pkt->data[i+1] = '\0';

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

	pkt = nsp_vtl_pkt_new_ex(1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->data[0] = DL_NEXT;

	TRYF(nsp_send_data(h, pkt));
	nsp_vtl_pkt_del(pkt);

	return 0;
}

int cmd_r_dir_enum_next(CalcHandle *h, char* name, uint32_t *size, uint8_t *type)
{
	VirtualPacket* pkt = nsp_vtl_pkt_new();
	uint16_t answer;
	uint8_t data_size;
	uint32_t date;
	int o;

	ticalcs_info("  next directory entry:");

	TRYF(nsp_recv_data(h, pkt));

	answer = (pkt->data[0] << 8) | pkt->data[1];
	if(answer == 0xff11)
	{
		nsp_vtl_pkt_del(pkt);
		return ERR_EOT;
	}
	else if((answer & 0xff) == 0xff)
		return ERR_CALC_ERROR3 + err_code(pkt);
	else if(answer != 0x1000)
		return ERR_INVALID_PACKET;

	data_size = pkt->data[2] + 3;
	strcpy(name, pkt->data + 3);
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

	pkt = nsp_vtl_pkt_new_ex(1, NSP_SRC_ADDR, nsp_src_port, NSP_DEV_ADDR, PORT_FILE_MGMT);
	pkt->data[0] = DL_DONE;

	TRYF(nsp_send_data(h, pkt));
	nsp_vtl_pkt_del(pkt);

	return 0;
}

int cmd_r_dir_enum_done(CalcHandle *h)
{
	return cmd_r_status(h, NULL);
}

