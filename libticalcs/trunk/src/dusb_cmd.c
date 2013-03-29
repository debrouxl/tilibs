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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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

// Helpers

GList *cpca_list = NULL;

TIEXPORT3 DUSBCalcParam* TICALL dusb_cp_new(uint16_t id, uint16_t size)
{
	DUSBCalcParam* cp = g_malloc0(sizeof(DUSBCalcParam)); // aborts the program if it fails.

	cp->id = id;
	cp->size = size;
	cp->data = g_malloc0(size); // aborts the program if it fails.

	cpca_list = g_list_append(cpca_list, cp);

	return cp;
}

TIEXPORT3 void TICALL dusb_cp_del(DUSBCalcParam* cp)
{
	if (cp != NULL)
	{
		cpca_list = g_list_remove(cpca_list, cp);

		g_free(cp->data);
		g_free(cp);
	}
	else
	{
		ticalcs_critical("%s: cp is NULL", __FUNCTION__);
	}
}

TIEXPORT3 DUSBCalcParam** TICALL dusb_cp_new_array(int size)
{
	DUSBCalcParam** array = g_malloc0((size+1) * sizeof(DUSBCalcParam *)); // aborts the program if it fails.
	return array;
}

TIEXPORT3 void TICALL dusb_cp_del_array(int size, DUSBCalcParam **params)
{
	int i;

	if (params != NULL)
	{
		for(i = 0; i < size && params[i]; i++)
		{
			dusb_cp_del(params[i]);
		}
		g_free(params);
	}
	else
	{
		ticalcs_critical("%s: params is NULL", __FUNCTION__);
	}

}

/////////////----------------

TIEXPORT3 DUSBCalcAttr* TICALL dusb_ca_new(uint16_t id, uint16_t size)
{
	DUSBCalcAttr* cp = g_malloc0(sizeof(DUSBCalcAttr)); // aborts the program if it fails.

	cp->id = id;
	cp->size = size;
	cp->data = g_malloc0(size); // aborts the program if it fails.

	cpca_list = g_list_append(cpca_list, cp);

	return cp;
}

TIEXPORT3 void TICALL dusb_ca_del(DUSBCalcAttr* cp)
{
	if (cp != NULL)
	{
		cpca_list = g_list_remove(cpca_list, cp);

		g_free(cp->data);
		g_free(cp);
	}
	else
	{
		ticalcs_critical("%s: cp is NULL", __FUNCTION__);
	}
}

TIEXPORT3 DUSBCalcAttr** TICALL dusb_ca_new_array(int size)
{
	DUSBCalcAttr** array = g_malloc0((size+1) * sizeof(DUSBCalcAttr *)); // aborts the program if it fails.
	return array;
}

TIEXPORT3 void TICALL dusb_ca_del_array(int size, DUSBCalcAttr **attrs)
{
	int i;

	if (attrs != NULL)
	{
		for(i = 0; i < size && attrs[i]; i++)
		{
			dusb_ca_del(attrs[i]);
		}
		g_free(attrs);
	}
	else
	{
		ticalcs_critical("%s: attrs is NULL", __FUNCTION__);
	}
}

void dusb_cpca_purge(void)
{
	//printf("cpca_purge: %p %i\n", cpca_list, g_list_length(cpca_list));
	g_list_foreach(cpca_list, (GFunc)dusb_ca_del, NULL);
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

static const uint16_t usb_errors[] = {
	0x0004, 0x0006, 0x0008, 0x0009, 0x000c, 0x000d, 0x000e, 
	0x0011, 0x0012, 0x001c, 0x001d, 0x0022, 0x0027, 0x0029, 
	0x002b, 0x002e, 0x0034 };

static int err_code(DUSBVirtualPacket *pkt)
{
	int i;
	int code = (pkt->data[0] << 8) | pkt->data[1];

	for(i = 0; i < (int)(sizeof(usb_errors) / sizeof(usb_errors[0])); i++)
		if(usb_errors[i] == code)
			return i+1;

	ticalcs_warning("USB error code 0x%02x not found in list. Please report it at <tilp-devel@lists.sf.net>.", code);

	return 0;
}

/////////////----------------

extern const DUSBVtlPktName vpkt_types[];

#define CATCH_DELAY() \
	if (pkt->type == DUSB_VPKT_DELAY_ACK) \
	{ \
		uint32_t delay = (pkt->data[0] << 24) | (pkt->data[1] << 16) | (pkt->data[2] << 8) | (pkt->data[3] << 0); \
		ticalcs_info("    delay = %u", delay); \
		if (delay > 400000) \
		{ \
			delay = 400000; \
			ticalcs_info("    (absurdly high delay, clamping to a more reasonable value)"); \
		} \
\
		PAUSE(delay/1000); \
\
		dusb_vtl_pkt_del(pkt); \
		pkt = dusb_vtl_pkt_new(0, 0); \
\
		retval = dusb_recv_data(h, pkt); \
		if (retval) \
		{ \
			goto end; \
		} \
	}

// 0x0001: set mode or ping
TIEXPORT3 int TICALL dusb_cmd_s_mode_set(CalcHandle *h, DUSBModeSet mode)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	TRYF(dusb_send_buf_size_request(h, DUSB_DFL_BUF_SIZE));
	TRYF(dusb_recv_buf_size_alloc(h, NULL));

	pkt = dusb_vtl_pkt_new(sizeof(mode), DUSB_VPKT_PING);

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
	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);

	ticalcs_info("   %04x %04x %04x %04x %04x", mode.arg1, mode.arg2, mode.arg3, mode.arg4, mode.arg5);

	return retval;
}

// 0x0002: begin OS transfer
TIEXPORT3 int TICALL dusb_cmd_s_os_begin(CalcHandle *h, uint32_t size)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = dusb_vtl_pkt_new(11, DUSB_VPKT_OS_BEGIN);

	pkt->data[7] = MSB(MSW(size));
	pkt->data[8] = LSB(MSW(size));
	pkt->data[9] = MSB(LSW(size));
	pkt->data[10]= LSB(LSW(size));
	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);

	ticalcs_info("   size = %08x (%i)", size, size);

	return retval;
}

// 0x0003: acknowledgement of OS transfer
TIEXPORT3 int TICALL dusb_cmd_r_os_ack(CalcHandle *h, uint32_t *size)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = dusb_vtl_pkt_new(0, 0);

	retval = dusb_recv_data(h, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if(pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code(pkt);
			goto end;
		}
		else if(pkt->type != DUSB_VPKT_OS_ACK)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if (size != NULL)
		{
			*size = (pkt->data[0] << 24) | (pkt->data[1] << 16) | (pkt->data[2] << 8) | (pkt->data[3] << 0);
			ticalcs_info("   size = %08x (%i)", *size, *size);
		}
	}

end:
	dusb_vtl_pkt_del(pkt);

	return retval;
}

static int s_os(uint8_t type, CalcHandle *h, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data)
{
	DUSBVirtualPacket* pkt;
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

	pkt = dusb_vtl_pkt_new(4 + size, type);

	pkt->data[0] = MSB(addr);
	pkt->data[1] = LSB(addr);
	pkt->data[2] = page;
	pkt->data[3] = flag;
	memcpy(pkt->data+4, data, size);
	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   addr=%04x, page=%02x, flag=%02x, size=%04x", addr, page, flag, size);

	return retval;
}

// 0x0004: OS header
TIEXPORT3 int TICALL dusb_cmd_s_os_header(CalcHandle *h, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data)
{
	return s_os(DUSB_VPKT_OS_HEADER, h, addr, page, flag, size, data);
}

// 0x0005: OS data
TIEXPORT3 int TICALL dusb_cmd_s_os_data(CalcHandle *h, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data)
{
	return s_os(DUSB_VPKT_OS_DATA, h, addr, page, flag, size, data);
}

// 0x0004: OS header
TIEXPORT3 int TICALL dusb_cmd_s_os_header_89(CalcHandle *h, uint32_t size, uint8_t *data)
{
	DUSBVirtualPacket* pkt;
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

	pkt = dusb_vtl_pkt_new(size, DUSB_VPKT_OS_HEADER);

	memcpy(pkt->data, data, size);
	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   size = %08x (%i)", size, size);

	return retval;
}

// 0x0005: OS data
TIEXPORT3 int TICALL dusb_cmd_s_os_data_89(CalcHandle *h, uint32_t size, uint8_t *data)
{
	DUSBVirtualPacket* pkt;
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

	pkt = dusb_vtl_pkt_new(size, DUSB_VPKT_OS_DATA);

	memcpy(pkt->data, data, size);
	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   size = %08x (%i)", size, size);

	return retval;
}

// 0x0006: acknowledgement of EOT
TIEXPORT3 int TICALL dusb_cmd_r_eot_ack(CalcHandle *h)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = dusb_vtl_pkt_new(0, 0);

	retval = dusb_recv_data(h, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if(pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code(pkt);
		}
		else if(pkt->type != DUSB_VPKT_EOT_ACK)
		{
			retval = ERR_INVALID_PACKET;
		}
	}

end:
	dusb_vtl_pkt_del(pkt);

	return retval;
}

// 0x0007: parameter request
TIEXPORT3 int TICALL dusb_cmd_s_param_request(CalcHandle *h, int npids, uint16_t *pids)
{
	DUSBVirtualPacket* pkt;
	int i;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (npids != 0 && pids == NULL)
	{
		ticalcs_critical("%s: pids is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = dusb_vtl_pkt_new(2 + npids * sizeof(uint16_t), DUSB_VPKT_PARM_REQ);

	pkt->data[0] = MSB(npids);
	pkt->data[1] = LSB(npids);

	for(i = 0; i < npids; i++)
	{
		pkt->data[2*(i+1) + 0] = MSB(pids[i]);
		pkt->data[2*(i+1) + 1] = LSB(pids[i]);
	}

	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   npids=%i", npids);

	return retval;
}

// 0x0008: parameter data
TIEXPORT3 int TICALL dusb_cmd_r_param_data(CalcHandle *h, int nparams, DUSBCalcParam **params)
{
	DUSBVirtualPacket* pkt;
	int i, j;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (params == NULL)
	{
		ticalcs_critical("%s: params is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = dusb_vtl_pkt_new(0, 0);

	retval = dusb_recv_data(h, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if(pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code(pkt);
			goto end;
		}
		else if(pkt->type != DUSB_VPKT_PARM_DATA)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if(((pkt->data[0] << 8) | pkt->data[1]) != nparams)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		for(i = 0, j = 2; i < nparams; i++)
		{
			DUSBCalcParam *s = params[i] = dusb_cp_new(0, 0);

			s->id = pkt->data[j++] << 8; s->id |= pkt->data[j++];
			s->ok = !pkt->data[j++];
			if(s->ok)
			{
				s->size = pkt->data[j++] << 8; s->size |= pkt->data[j++];
				s->data = (uint8_t *)g_malloc0(s->size);
				memcpy(s->data, &pkt->data[j], s->size);
				j += s->size;
			}
		}
	}

end:
	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   nparams=%i", nparams);

	return retval;
}

// 0x0009: request directory listing
TIEXPORT3 int TICALL dusb_cmd_s_dirlist_request(CalcHandle *h, int naids, uint16_t *aids)
{
	DUSBVirtualPacket* pkt;
	int i;
	int j = 0;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (naids != 0 && aids == NULL)
	{
		ticalcs_critical("%s: aids is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = dusb_vtl_pkt_new(4 + 2*naids + 7, DUSB_VPKT_DIR_REQ);

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

	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   naids=%i", naids);

	return retval;
}

// 0x000A: variable header (name is utf-8)
// beware: attr array contents is allocated by function
TIEXPORT3 int TICALL dusb_cmd_r_var_header(CalcHandle *h, char *folder, char *name, DUSBCalcAttr **attr)
{
	DUSBVirtualPacket* pkt;
	uint8_t fld_len;
	uint8_t var_len;
	int nattr;
	int i, j;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (folder == NULL || name == NULL || attr == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = dusb_vtl_pkt_new(0, 0);

	retval = dusb_recv_data(h, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if(pkt->type == DUSB_VPKT_EOT)
		{
			retval = ERR_EOT;
			goto end;
		}
		else if(pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code(pkt);
			goto end;
		}
		else if(pkt->type != DUSB_VPKT_VAR_HDR)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

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
			DUSBCalcAttr *s = attr[i] = dusb_ca_new(0, 0);

			s->id = pkt->data[j++] << 8; s->id |= pkt->data[j++];
			s->ok = !pkt->data[j++];
			if(s->ok)
			{
				s->size = pkt->data[j++] << 8; s->size |= pkt->data[j++];
				s->data = (uint8_t *)g_malloc0(s->size);
				memcpy(s->data, &pkt->data[j], s->size);
				j += s->size;
			}
		}
	}

end:
	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   folder=%s, name=%s", folder, name);

	return retval;
}

// 0x000B: request to send
TIEXPORT3 int TICALL dusb_cmd_s_rts(CalcHandle *h, const char *folder, const char *name, uint32_t size, int nattrs, const DUSBCalcAttr **attrs)
{
	DUSBVirtualPacket* pkt;
	int pks;
	int i;
	int j = 0;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (folder == NULL || name == NULL || (nattrs != 0 && attrs == NULL))
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pks = 2 + strlen(name)+1 + 5 + 2;
	if(strlen(folder))
		pks += strlen(folder)+1;
	for(i = 0; i < nattrs; i++) pks += 4 + attrs[i]->size;

	pkt = dusb_vtl_pkt_new(pks, DUSB_VPKT_RTS);

	if(strlen(folder))
	{
		pkt->data[j++] = strlen(folder);
		memcpy(pkt->data + j, folder, strlen(folder)+1);
		j += strlen(folder)+1;
	}
	else
	{
		pkt->data[j++] = 0;
	}

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

	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   folder=%s, name=%s, size=%i, nattrs=%i", folder, name, size, nattrs);

	return retval;
}

// 0x000C: variable request
TIEXPORT3 int TICALL dusb_cmd_s_var_request(CalcHandle *h, const char *folder, const char *name, int naids, uint16_t *aids, int nattrs, const DUSBCalcAttr **attrs)
{
	DUSBVirtualPacket* pkt;
	int pks;
	int i;
	int j = 0;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (folder == NULL || name == NULL || (naids != 0 && aids == NULL) || (nattrs != 0 && attrs == NULL))
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pks = 2 + strlen(name)+1 + 5 + 2 + 2*naids + 2;
	if(strlen(folder)) pks += strlen(folder)+1;
	for(i = 0; i < nattrs; i++) pks += 4 + attrs[i]->size;
	pks += 2;

	pkt = dusb_vtl_pkt_new(pks, DUSB_VPKT_VAR_REQ);

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

	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   folder=%s, name=%s, naids=%i, nattrs=%i", folder, name, naids, nattrs);

	return retval;
}

// 0x000D: variable contents (recv)
TIEXPORT3 int TICALL dusb_cmd_r_var_content(CalcHandle *h, uint32_t *size, uint8_t **data)
{
	DUSBVirtualPacket* pkt;
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

	pkt = dusb_vtl_pkt_new(0, 0);

	retval = dusb_recv_data(h, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if(pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code(pkt);
			goto end;
		}
		else if(pkt->type != DUSB_VPKT_VAR_CNTS)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}

		if(size != NULL)
		{
			*size = pkt->size;
		}

		*data = g_malloc0(pkt->size);
		if (*data != NULL)
		{
			memcpy(*data, pkt->data, pkt->size);
		}
		else
		{
			retval = ERR_MALLOC;
		}
		ticalcs_info("   size=%i", pkt->size);
	}

end:
	dusb_vtl_pkt_del(pkt);

	return retval;
}

// 0x000D: variable contents (send)
TIEXPORT3 int TICALL dusb_cmd_s_var_content(CalcHandle *h, uint32_t size, uint8_t *data)
{
	DUSBVirtualPacket* pkt;
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

	pkt = dusb_vtl_pkt_new(size, DUSB_VPKT_VAR_CNTS);

	memcpy(pkt->data, data, size);
	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   size=%i", size);

	return retval;
}

// 0x000E: parameter set
TIEXPORT3 int TICALL dusb_cmd_s_param_set(CalcHandle *h, const DUSBCalcParam *param)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (param == NULL)
	{
		ticalcs_critical("%s: param is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pkt = dusb_vtl_pkt_new(2 + 2 + param->size, DUSB_VPKT_PARM_SET);

	pkt->data[0] = MSB(param->id);
	pkt->data[1] = LSB(param->id);
	pkt->data[2] = MSB(param->size);
	pkt->data[3] = LSB(param->size);
	memcpy(pkt->data + 4, param->data, param->size);

	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   pid=%04x, size=%04x", param->id, param->size);

	return retval;
}

// 0x0010: modify/rename/delete variable
TIEXPORT3 int TICALL dusb_cmd_s_var_modify(CalcHandle *h,
                     const char *src_folder, const char *src_name,
                     int n_src_attrs, const DUSBCalcAttr **src_attrs,
                     const char *dst_folder, const char *dst_name,
                     int n_dst_attrs, const DUSBCalcAttr **dst_attrs)
{
	DUSBVirtualPacket* pkt;
	int i;
	int j = 0;
	int pks;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (src_folder == NULL || src_name == NULL || src_attrs == NULL || dst_folder == NULL || dst_name == NULL || (n_dst_attrs != 0 && dst_attrs == NULL))
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	pks = 2 + strlen(src_name)+1 + 2;
	if(strlen(src_folder))
		pks += strlen(src_folder)+1;
	for(i = 0; i < n_src_attrs; i++)
		pks += 4 + src_attrs[i]->size;

	pks += 5;

	if(strlen(dst_folder))
		pks += strlen(dst_folder)+1;
	if(strlen(dst_name))
		pks += strlen(dst_name)+1;
	for (i = 0; i < n_dst_attrs; i++)
		pks += 4 + dst_attrs[i]->size;

	pkt = dusb_vtl_pkt_new(pks, DUSB_VPKT_DEL_VAR);

	if(strlen(src_folder))
	{
		pkt->data[j++] = strlen(src_folder);
		memcpy(pkt->data + j, src_folder, strlen(src_folder)+1);
		j += strlen(src_folder)+1;
	}
	else
	{
		pkt->data[j++] = 0;
	}

	pkt->data[j++] = strlen(src_name);
	memcpy(pkt->data + j, src_name, strlen(src_name)+1);
	j += strlen(src_name)+1;

	pkt->data[j++] = MSB(n_src_attrs);
	pkt->data[j++] = LSB(n_src_attrs);
	for(i = 0; i < n_src_attrs; i++)
	{
		pkt->data[j++] = MSB(src_attrs[i]->id);
		pkt->data[j++] = LSB(src_attrs[i]->id);
		pkt->data[j++] = MSB(src_attrs[i]->size);
		pkt->data[j++] = LSB(src_attrs[i]->size);
		memcpy(pkt->data + j, src_attrs[i]->data, src_attrs[i]->size);
		j += src_attrs[i]->size;
	}

	pkt->data[j++] = 0x01; /* ??? */

	if(strlen(dst_folder))
	{
		pkt->data[j++] = strlen(dst_folder);
		memcpy(pkt->data + j, dst_folder, strlen(dst_folder)+1);
		j += strlen(dst_folder)+1;
	}
	else
		pkt->data[j++] = 0;

	if(strlen(dst_name))
	{
		pkt->data[j++] = strlen(dst_name);
		memcpy(pkt->data + j, dst_name, strlen(dst_name)+1);
		j += strlen(dst_name)+1;
	}
	else
		pkt->data[j++] = 0;

	pkt->data[j++] = MSB(n_dst_attrs);
	pkt->data[j++] = LSB(n_dst_attrs);
	for(i = 0; i < n_dst_attrs; i++)
	{
		pkt->data[j++] = MSB(dst_attrs[i]->id);
		pkt->data[j++] = LSB(dst_attrs[i]->id);
		pkt->data[j++] = MSB(dst_attrs[i]->size);
		pkt->data[j++] = LSB(dst_attrs[i]->size);
		memcpy(pkt->data + j, dst_attrs[i]->data, dst_attrs[i]->size);
		j += dst_attrs[i]->size;
	}

	g_assert(j == pks);

	retval = dusb_send_data(h, pkt);

	ticalcs_info("   src_folder=%s, name=%s, nattrs=%i", src_folder, src_name, n_src_attrs);
	ticalcs_info("   dst_folder=%s, name=%s, nattrs=%i", dst_folder, dst_name, n_dst_attrs);

	dusb_vtl_pkt_del(pkt);
	return retval;
}

TIEXPORT3 int TICALL dusb_cmd_s_var_delete(CalcHandle *h, const char *folder, const char *name, int nattrs, const DUSBCalcAttr **attrs)
{
	const DUSBCalcAttr * dummy;
	return dusb_cmd_s_var_modify(h, folder, name, nattrs, attrs, "", "", 0, &dummy);
}

// 0x0011: remote control
TIEXPORT3 int TICALL dusb_cmd_s_execute(CalcHandle *h, const char *folder, const char *name, uint8_t action, const char *args, uint16_t code)
{
	DUSBVirtualPacket* pkt = NULL;
	int pks;
	int j = 0;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (folder == NULL || name == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(h->model == CALC_TI89T_USB)
	{
		pks = 3;
		if(args) pks += strlen(args); else pks += 2;
		if(strlen(folder)) pks += strlen(folder)+1;
		if(strlen(name)) pks += strlen(name)+1;
		pkt = dusb_vtl_pkt_new(pks, DUSB_VPKT_EXECUTE);

		pkt->data[j++] = strlen(folder);
		if(strlen(folder))
		{
			memcpy(pkt->data + j, folder, strlen(folder)+1);
			j += strlen(folder)+1;
		}

		pkt->data[j++] = strlen(name);
		if(strlen(name))
		{
			memcpy(pkt->data + j, name, strlen(name)+1);
			j += strlen(name)+1;
		}
		
		pkt->data[j++] = action;

		if(action != EID_KEY && args != NULL)
		{
			memcpy(pkt->data + j, args, strlen(args));
		}
		else if(action == EID_KEY || args == NULL)
		{
			pkt->data[j++] = MSB(code);
			pkt->data[j++] = LSB(code);
		}
	}
	else if(h->model == CALC_TI84P_USB)
	{
		pks = 3;
		if(args) pks += strlen(args); else pks += 2;
		if(strlen(name)) pks += strlen(name);
		pkt = dusb_vtl_pkt_new(pks, DUSB_VPKT_EXECUTE);

		// MSB first (like other commands anyway).
		// Otherwise, a "EE 00 00 0D" packet is returned.
		pkt->data[j++] = MSB(strlen(name));
		pkt->data[j++] = LSB(strlen(name));
		if(strlen(name))
		{
			memcpy(pkt->data + j, name, strlen(name));
			j += strlen(name);
		}
		
		pkt->data[j++] = action;

		if(action != EID_KEY && args != NULL)
		{
			memcpy(pkt->data + j, args, strlen(args));
		}
		else if(action == EID_KEY || args == NULL)
		{
			pkt->data[j++] = LSB(code);
			pkt->data[j++] = MSB(code);
		}
	}

	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	if (action == EID_KEY)
	{
		ticalcs_info("   action=%i, keycode=%04x", action, code);
	}
	else
	{
		ticalcs_info("   action=%i, folder=%s, name=%s, args=%s", action, folder ? folder : "NULL", name ? name : "NULL", args ? args : "NULL");
	}

	return retval;
}

// 0x0012: acknowledgement of mode setting
TIEXPORT3 int TICALL dusb_cmd_r_mode_ack(CalcHandle *h)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = dusb_vtl_pkt_new(0, 0);

	retval = dusb_recv_data(h, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if(pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code(pkt);
		}
		else if(pkt->type != DUSB_VPKT_MODE_SET)
		{
			retval = ERR_INVALID_PACKET;
		}
	}

end:
	dusb_vtl_pkt_del(pkt);

	return retval;
}

// 0xAA00: acknowledgement of data
TIEXPORT3 int TICALL dusb_cmd_r_data_ack(CalcHandle *h)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = dusb_vtl_pkt_new(0, 0);

	retval = dusb_recv_data(h, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if(pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code(pkt);
		}
		else if(pkt->type != DUSB_VPKT_DATA_ACK)
		{
			ticalcs_info("cmd_r_data_ack: expected type 0x%4X, received type 0x%4X", DUSB_VPKT_DATA_ACK, pkt->type);
			retval = ERR_INVALID_PACKET;
		}
	}

end:
	dusb_vtl_pkt_del(pkt);

	return retval;
}

// 0xBB00: delay acknowledgement
TIEXPORT3 int TICALL dusb_cmd_r_delay_ack(CalcHandle *h)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = dusb_vtl_pkt_new(0, 0);

	retval = dusb_recv_data(h, pkt);

	if (!retval)
	{
		if(pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code(pkt);
		}
		else if(pkt->type != DUSB_VPKT_DELAY_ACK)
		{
			ticalcs_info("cmd_r_data_ack: expected type 0x%4X, received type 0x%4X", DUSB_VPKT_DELAY_ACK, pkt->type);
			retval = ERR_INVALID_PACKET;
		}
	}

	PAUSE(100);

	dusb_vtl_pkt_del(pkt);

	return retval;
}

// 0xDD00: end of transmission (send)
TIEXPORT3 int TICALL dusb_cmd_s_eot(CalcHandle *h)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = dusb_vtl_pkt_new(0, DUSB_VPKT_EOT);

	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);

	return retval;
}

// 0xDD00: end of transmission (recv)
TIEXPORT3 int TICALL dusb_cmd_r_eot(CalcHandle *h)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = dusb_vtl_pkt_new(0, 0);

	retval = dusb_recv_data(h, pkt);

	if (!retval)
	{
		CATCH_DELAY();

		if(pkt->type == DUSB_VPKT_ERROR)
		{
			retval = ERR_CALC_ERROR2 + err_code(pkt);
			goto end;
		}
		else if(pkt->type != DUSB_VPKT_EOT)
		{
			retval = ERR_INVALID_PACKET;
			goto end;
		}
	}

end:
	dusb_vtl_pkt_del(pkt);

	return retval;
}

// 0xEE00: error
TIEXPORT3 int TICALL dusb_cmd_s_error(CalcHandle *h, uint16_t code)
{
	DUSBVirtualPacket* pkt;
	int retval = 0;

	if (h == NULL)
	{
		ticalcs_critical("%s: h is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}

	pkt = dusb_vtl_pkt_new(2, DUSB_VPKT_ERROR);

	pkt->data[0] = MSB(code);
	pkt->data[1] = LSB(code);
	retval = dusb_send_data(h, pkt);

	dusb_vtl_pkt_del(pkt);
	ticalcs_info("   code = %04x", code);

	return retval;
}
