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
	This unit handles virtual packet types (commands) thru DirectLink.
*/

// Some functions should be renamed or re-organized...

#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "logging.h"
#include "error.h"
#include "macros.h"
#include "pause.h"

#include "dusb_vpkt.h"
#include "dusb_cmd.h"

//#define err_code(vtl)		((vtl->data[0] << 8) | vtl->data[1])
#define err_code(vtl)	(0)

// Helpers

GList *cpca_list = NULL;

CalcParam*	cp_new(uint16_t id, uint16_t size)
{
	CalcParam* cp = calloc(1, sizeof(CalcParam));

	cp->id = id;
	cp->size = size;
	cp->data = calloc(1, size);

	cpca_list = g_list_append(cpca_list, cp);
	return cp;
}

void		cp_del(CalcParam* cp)
{
	cpca_list = g_list_remove(cpca_list, cp);

	free(cp->data);
	free(cp);
}

CalcParam** cp_new_array(int size)
{
	CalcParam** array = calloc(size+1, sizeof(CalcParam *));
	return array;
}

void cp_del_array(int size, CalcParam **params)
{
	int i;

	for(i = 0; i < size && params[i]; i++)
		cp_del(params[i]);
	free(params);
}

/////////////----------------

CalcAttr*	ca_new(uint16_t id, uint16_t size)
{
	CalcAttr* cp = calloc(1, sizeof(CalcAttr));

	cp->id = id;
	cp->size = size;
	cp->data = calloc(1, size);

	cpca_list = g_list_append(cpca_list, cp);
	return cp;
}

void		ca_del(CalcAttr* cp)
{
	cpca_list = g_list_remove(cpca_list, cp);

	free(cp->data);
	free(cp);
}

CalcAttr** ca_new_array(int size)
{
	CalcAttr** array = calloc(size+1, sizeof(CalcParam *));
	return array;
}

void ca_del_array(int size, CalcAttr **attrs)
{
	int i;

	for(i = 0; i < size && attrs[i]; i++)
		ca_del(attrs[i]);
	free(attrs);
}

void cpca_purge(void)
{
	//printf("cpca_purge: %p %i\n", cpca_list, g_list_length(cpca_list));
	g_list_foreach(cpca_list, (GFunc)ca_del, NULL);
	g_list_free(cpca_list);
	cpca_list = NULL;
}

/////////////----------------

static void byteswap(uint8_t *data, uint32_t len)
{/*
	if(len == 2)
	{
		uint8_t tmp;

		tmp = data[0];
		data[0] = data[1];
		data[1] = tmp;
	}
	else if(len == 4)
	{
		uint8_t tmp;

		tmp = data[0];
		data[0] = data[3];
		data[3] = tmp;

		tmp = data[1];
		data[1] = data[2];
		data[2] = tmp;
	}
	*/
}

/////////////----------------

#define CATCH_DELAY()					\
	if(pkt->type == VPKT_DELAY_ACK)		\
	{									\
		uint32_t delay = (pkt->data[0] << 24) | (pkt->data[1] << 16) | (pkt->data[2] << 8) | (pkt->data[3] << 0);	\
		ticalcs_info("    delay = %u\n", delay);	\
										\
		PAUSE(50);						\
										\
		vtl_pkt_del(pkt);				\
		pkt = vtl_pkt_new(0, 0);		\
		TRYF(dusb_recv_data(h, pkt));	\
	}

// 0x0001: set mode or ping
int cmd_s_mode_set(CalcHandle *h, ModeSet mode)
{
	VirtualPacket* pkt;

	TRYF(dusb_send_buf_size_request(h, DUSB_DFL_BUF_SIZE));
	TRYF(dusb_recv_buf_size_alloc(h, NULL));

	pkt = vtl_pkt_new(sizeof(mode), VPKT_PING);
	pkt->data[0] = MSB(mode.arg1);
	pkt->data[1] = LSB(mode.arg1);
	pkt->data[2] = MSB(mode.arg2);
	pkt->data[3] = LSB(mode.arg2);
	pkt->data[4] = MSB(mode.arg3);
	pkt->data[5] = LSB(mode.arg3);
	pkt->data[6] = MSB(mode.arg4);
	pkt->data[7] = LSB(mode.arg4);
	pkt->data[8] = MSB(mode.arg5);
	pkt->data[9] = LSB(mode.arg5);
	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0x0002: begin OS transfer
int cmd_s_os_begin(CalcHandle *h, uint32_t size)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(11, VPKT_OS_BEGIN);

	pkt->data[7] = MSB(MSW(size));
	pkt->data[8] = LSB(MSW(size));
	pkt->data[9] = MSB(LSW(size));
	pkt->data[10]= LSB(LSW(size));
	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0x0003: acknowledgement of OS transfer
int cmd_r_os_ack(CalcHandle *h, uint32_t *size)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(0, 0);
	TRYF(dusb_recv_data(h, pkt));

	CATCH_DELAY();

	if(pkt->type == VPKT_ERROR)
		return ERR_CALC_ERROR + err_code(pkt);
	else if(pkt->type != VPKT_OS_ACK)
		return ERR_INVALID_PACKET;
	
	*size = (pkt->data[0] << 24) | (pkt->data[1] << 16) | (pkt->data[2] << 8) | (pkt->data[3] << 0);

	vtl_pkt_del(pkt);
	return 0;
}

static int s_os(uint8_t type, CalcHandle *h, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(4 + size, type);

	pkt->data[0] = MSB(addr);
	pkt->data[1] = LSB(addr);
	pkt->data[2] = page;
	pkt->data[3] = flag;
	memcpy(pkt->data+4, data, size);
	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0x0004: OS header
int cmd_s_os_header(CalcHandle *h, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data)
{
	return s_os(VPKT_OS_HEADER, h, addr, page, flag, size, data);	
}

// 0x0005: OS data
int cmd_s_os_data(CalcHandle *h, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data)
{
	return s_os(VPKT_OS_DATA, h, addr, page, flag, size, data);	
}

// 0x0006: acknowledgement of EOT
int cmd_r_eot_ack(CalcHandle *h)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(0, 0);
	TRYF(dusb_recv_data(h, pkt));

	CATCH_DELAY();

	if(pkt->type == VPKT_ERROR)
		return ERR_CALC_ERROR + err_code(pkt);
	else if(pkt->type != VPKT_EOT_ACK)
		return ERR_INVALID_PACKET;
	
	vtl_pkt_del(pkt);
	return 0;
}

// 0x0007: parameter request
int cmd_s_param_request(CalcHandle *h, int npids, uint16_t *pids)
{
	VirtualPacket* pkt;
	int i;

	pkt = vtl_pkt_new((npids + 1) * sizeof(uint16_t), VPKT_PARM_REQ);

	pkt->data[0] = MSB(npids);
	pkt->data[1] = LSB(npids);

	for(i = 0; i < npids; i++)
	{
		pkt->data[2*(i+1) + 0] = MSB(pids[i]);
		pkt->data[2*(i+1) + 1] = LSB(pids[i]);
	}

	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0x0008: parameter data
int cmd_r_param_data(CalcHandle *h, int nparams, CalcParam **params)
{
	VirtualPacket* pkt;
	int i, j;

	pkt = vtl_pkt_new(0, 0);
	TRYF(dusb_recv_data(h, pkt));

	CATCH_DELAY();

	if(pkt->type == VPKT_ERROR)
		return ERR_CALC_ERROR + err_code(pkt);
	else if(pkt->type != VPKT_PARM_DATA)
		return ERR_INVALID_PACKET;

	if(((pkt->data[j=0] << 8) | pkt->data[j=1]) != nparams)
		return ERR_INVALID_PACKET;

	for(i = 0, j = 2; i < nparams; i++)
	{
		CalcParam *s = params[i] = cp_new(0, 0);
		
		s->id = pkt->data[j++] << 8; s->id |= pkt->data[j++];
		s->ok = !pkt->data[j++];
		if(s->ok)
		{
			s->size = pkt->data[j++] << 8; s->size |= pkt->data[j++];
			s->data = (uint8_t *)calloc(1, s->size);
			memcpy(s->data, &pkt->data[j], s->size);
			j += s->size;
		}
	}
	
	vtl_pkt_del(pkt);
	return 0;
}

// 0x0009: request directory listing
int cmd_s_dirlist_request(CalcHandle *h, int naids, uint16_t *aids)
{
	VirtualPacket* pkt;
	int i;
	int j = 0;

	pkt = vtl_pkt_new(4 + 2*naids + 7, VPKT_DIR_REQ);

	pkt->data[j++] = MSB(MSW(naids));
	pkt->data[j++] = LSB(MSW(naids));
	pkt->data[j++] = MSB(LSW(naids));
	pkt->data[j++] = LSB(LSW(naids));

	for(i = 0; i < naids; i++)
	{
		pkt->data[j++] = MSB(aids[i]);
		pkt->data[j++] = LSB(aids[i]);
	}

	pkt->data[j++] = 0x00; pkt->data[j++] = 0x01;
	pkt->data[j++] = 0x00; pkt->data[j++] = 0x01;
	pkt->data[j++] = 0x00; pkt->data[j++] = 0x01;
	pkt->data[j++] = 0x01;

	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0x000A: variable header (name is utf-8)
// beware: attr array is allocated by function
int cmd_r_var_header(CalcHandle *h, char *folder, char *name, CalcAttr **attr)
{
	VirtualPacket* pkt;
	uint8_t fld_len;
	uint8_t var_len;
	int nattr;
	int i, j;

	pkt = vtl_pkt_new(0, 0);
	TRYF(dusb_recv_data(h, pkt));

	CATCH_DELAY();

	if(pkt->type == VPKT_EOT)
	{
		vtl_pkt_del(pkt);
		return ERR_EOT;
	}
	else if(pkt->type == VPKT_ERROR)
		return ERR_CALC_ERROR + err_code(pkt);
	else if(pkt->type != VPKT_VAR_HDR)
		return ERR_INVALID_PACKET;

	j = 0;
	fld_len = pkt->data[j++];
	strcpy(folder, "");
	if(fld_len)
	{
		memcpy(folder, &pkt->data[j], fld_len+1);
		j += fld_len+1;
	}
	var_len = pkt->data[j++];
	strcpy(name, "");
	if(var_len)
	{
		memcpy(name, &pkt->data[j], var_len+1);
		j += var_len+1;
	}

	nattr = (pkt->data[j+0] << 8) | pkt->data[j+1];
	j += 2;
	
	for(i = 0; i < nattr; i++)
	{
		CalcAttr *s = attr[i] = ca_new(0, 0);

		s->id = pkt->data[j++] << 8; s->id |= pkt->data[j++];
		s->ok = !pkt->data[j++];
		if(s->ok)
		{
			s->size = pkt->data[j++] << 8; s->size |= pkt->data[j++];
			s->data = (uint8_t *)calloc(1, s->size);
			memcpy(s->data, &pkt->data[j], s->size);
			j += s->size;
		}
	}
	
	vtl_pkt_del(pkt);
	return 0;
}

// 0x000B: request to send
int cmd_s_rts(CalcHandle *h, const char *folder, const char *name, uint32_t size, int nattrs, const CalcAttr **attrs)
{
	VirtualPacket* pkt;
	int pks;
	int i;
	int j = 0;

	pks = 2 + strlen(name)+1 + 5 + 2;
	if(strlen(folder))
		pks += strlen(folder)+1;
	for(i = 0; i < nattrs; i++) pks += 4 + attrs[i]->size;
	pkt = vtl_pkt_new(pks, VPKT_RTS);

	if(strlen(folder))
	{
		pkt->data[j++] = strlen(folder);
		memcpy(pkt->data + j, folder, strlen(folder)+1);
		j += strlen(folder)+1;
	}
	else
		pkt->data[j++] = 0;

	pkt->data[j++] = strlen(name);
	memcpy(pkt->data + j, name, strlen(name)+1);
	j += strlen(name)+1;
	
	pkt->data[j++] = MSB(MSW(size));
	pkt->data[j++] = LSB(MSW(size));
	pkt->data[j++] = MSB(LSW(size));
	pkt->data[j++] = LSB(LSW(size));
	pkt->data[j++] = 0x01;

	pkt->data[j++] = MSB(nattrs);
	pkt->data[j++] = LSB(nattrs);
	for(i = 0; i < nattrs; i++)
	{
		pkt->data[j++] = MSB(attrs[i]->id);
		pkt->data[j++] = LSB(attrs[i]->id);
		pkt->data[j++] = MSB(attrs[i]->size);
		pkt->data[j++] = LSB(attrs[i]->size);
		memcpy(pkt->data + j, attrs[i]->data, attrs[i]->size);
		byteswap(pkt->data + j, attrs[i]->size);
		j += attrs[i]->size;
	}

	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0x000C: variable request
int cmd_s_var_request(CalcHandle *h, const char *folder, const char *name, 
						int naids, uint16_t *aids, 
						int nattrs, const CalcAttr **attrs)
{
	VirtualPacket* pkt;
	int pks;
	int i;
	int j = 0;

	pks = 2 + strlen(name)+1 + 5 + 2 + 2*naids + 2;
	if(strlen(folder)) pks += strlen(folder)+1;
	for(i = 0; i < nattrs; i++) pks += 4 + attrs[i]->size;
	pks += 2;
	pkt = vtl_pkt_new(pks, VPKT_VAR_REQ);

	if(strlen(folder))
	{
		pkt->data[j++] = strlen(folder);
		memcpy(pkt->data + j, folder, strlen(folder)+1);
		j += strlen(folder)+1;
	}
	else
		pkt->data[j++] = 0;

	pkt->data[j++] = strlen(name);
	memcpy(pkt->data + j, name, strlen(name)+1);
	j += strlen(name)+1;
	
	pkt->data[j++] = 0x01; 
	pkt->data[j++] = 0xFF; pkt->data[j++] = 0xFF;
	pkt->data[j++] = 0xFF; pkt->data[j++] = 0xFF;

	pkt->data[j++] = MSB(naids);
	pkt->data[j++] = LSB(naids);
	for(i = 0; i < naids; i++)
	{
		pkt->data[j++] = MSB(aids[i]);
		pkt->data[j++] = LSB(aids[i]);
	}

	pkt->data[j++] = MSB(nattrs);
	pkt->data[j++] = LSB(nattrs);
	for(i = 0; i < nattrs; i++)
	{
		pkt->data[j++] = MSB(attrs[i]->id);
		pkt->data[j++] = LSB(attrs[i]->id);
		pkt->data[j++] = MSB(attrs[i]->size);
		pkt->data[j++] = LSB(attrs[i]->size);
		memcpy(pkt->data + j, attrs[i]->data, attrs[i]->size);
		byteswap(pkt->data + j, attrs[i]->size);
		j += attrs[i]->size;
	}
	pkt->data[j++] = 0x00; pkt->data[j++] = 0x00;

	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0x000D: variable contents (recv)
int cmd_r_var_content(CalcHandle *h, uint32_t *size, uint8_t **data)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(0, 0);
	TRYF(dusb_recv_data(h, pkt));

	CATCH_DELAY();

	if(pkt->type == VPKT_ERROR)
		return ERR_CALC_ERROR + err_code(pkt);
	else if(pkt->type != VPKT_VAR_CNTS)
		return ERR_INVALID_PACKET;

	if(size != NULL)
		*size = pkt->size;

	*data = calloc(pkt->size, 1);
	memcpy(*data, pkt->data, pkt->size);
	
	vtl_pkt_del(pkt);
	return 0;
}

// 0x000D: variable contents (send)
int cmd_s_var_content(CalcHandle *h, uint32_t size, uint8_t *data)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(size, VPKT_VAR_CNTS);

	memcpy(pkt->data, data, size);
	TRYF(dusb_send_data(h, pkt));
	
	vtl_pkt_del(pkt);
	return 0;
}

// 0x000E: parameter set
int cmd_s_param_set(CalcHandle *h, const CalcParam *param)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(2 + 2 + param->size, VPKT_PARM_SET);

	pkt->data[0] = MSB(param->id);
	pkt->data[1] = LSB(param->id);
	pkt->data[2] = MSB(param->size);
	pkt->data[3] = LSB(param->size);
	memcpy(pkt->data + 4, param->data, param->size);

	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0x0010: variable delete
int cmd_s_var_delete(CalcHandle *h, const char *folder, const char *name, int nattrs, const CalcAttr **attrs)
{
	VirtualPacket* pkt;
	int i;
	int j = 0;
	int pks;

	pks = 2 + strlen(name)+1 + 2;
	if(strlen(folder))
		pks += strlen(folder)+1;
	for(i = 0; i < nattrs; i++) pks += 4 + attrs[i]->size;
	pks += 5;
	pkt = vtl_pkt_new(pks, VPKT_DEL_VAR);

	if(strlen(folder))
	{
		pkt->data[j++] = strlen(folder);
		memcpy(pkt->data + j, folder, strlen(folder)+1);
		j += strlen(folder)+1;
	}
	else
		pkt->data[j++] = 0;

	pkt->data[j++] = strlen(name);
	memcpy(pkt->data + j, name, strlen(name)+1);
	j += strlen(name)+1;

	pkt->data[j++] = MSB(nattrs);
	pkt->data[j++] = LSB(nattrs);
	for(i = 0; i < nattrs; i++)
	{
		pkt->data[j++] = MSB(attrs[i]->id);
		pkt->data[j++] = LSB(attrs[i]->id);
		pkt->data[j++] = MSB(attrs[i]->size);
		pkt->data[j++] = LSB(attrs[i]->size);
		memcpy(pkt->data + j, attrs[i]->data, attrs[i]->size);
		byteswap(pkt->data + j, attrs[i]->size);
		j += attrs[i]->size;
	}

	pkt->data[j++] = 0x01; pkt->data[j++] = 0x00;
	pkt->data[j++] = 0x00; pkt->data[j++] = 0x00;
	pkt->data[j++] = 0x00;

	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0x0012: acknowledgement of mode setting
int cmd_r_mode_ack(CalcHandle *h)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(0, 0);
	TRYF(dusb_recv_data(h, pkt));

	CATCH_DELAY();

	if(pkt->type == VPKT_ERROR)
		return ERR_CALC_ERROR + err_code(pkt);
	else if(pkt->type != VPKT_MODE_SET)
		return ERR_INVALID_PACKET;

	vtl_pkt_del(pkt);
	return 0;
}

// 0xAA00: acknowledgement of data
int cmd_r_data_ack(CalcHandle *h)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(0, 0);
	TRYF(dusb_recv_data(h, pkt));

	CATCH_DELAY();

	if(pkt->type == VPKT_ERROR)
		return ERR_CALC_ERROR + err_code(pkt);
	else if(pkt->type != VPKT_DATA_ACK)
		return ERR_INVALID_PACKET;

	vtl_pkt_del(pkt);
	return 0;
}

// 0xBB00: delay acknowledgement
int cmd_r_delay_ack(CalcHandle *h)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(0, 0);
	TRYF(dusb_recv_data(h, pkt));

	if(pkt->type == VPKT_ERROR)
		return ERR_CALC_ERROR + err_code(pkt);
	else if(pkt->type != VPKT_DELAY_ACK)
		return ERR_INVALID_PACKET;

	PAUSE(100);

	vtl_pkt_del(pkt);
	return 0;
}

// 0xDD00: end of transmission (send)
int cmd_s_eot(CalcHandle *h)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(0, VPKT_EOT);
	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}

// 0xDD00: end of transmission (recv)
int cmd_r_eot(CalcHandle *h)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(0, 0);
	TRYF(dusb_recv_data(h, pkt));

	CATCH_DELAY();

	if(pkt->type == VPKT_ERROR)
		return ERR_CALC_ERROR + err_code(pkt);
	else if(pkt->type != VPKT_EOT)
		return ERR_INVALID_PACKET;

	vtl_pkt_del(pkt);
	return 0;
}

// 0xEE00: error
int cmd_s_error(CalcHandle *h, uint16_t code)
{
	VirtualPacket* pkt;

	pkt = vtl_pkt_new(2, VPKT_ERROR);

	pkt->data[0] = MSB(code);
	pkt->data[1] = LSB(code);
	TRYF(dusb_send_data(h, pkt));

	vtl_pkt_del(pkt);
	return 0;
}
