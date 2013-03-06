/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
 *  Copyright (C) 2006  Kevin Kofler
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ticalcs.h"
#include "error.h"
#include "logging.h"
#include "gettext.h"

/**
 * ticalcs_calc_features:
 * @handle: a previously allocated handle
 *
 * Returns the features and operations supported by the hand-held.
 *
 * Return value: a mask of features (CalcFeatures).
 **/
TIEXPORT3 CalcFeatures TICALL ticalcs_calc_features(CalcHandle* handle)
{
	const CalcFncts *calc;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return FTS_NONE;
	}
	calc = handle->calc;

	return calc->features;
}

/**
 * ticalcs_calc_isready:
 * @handle: a previously allocated handle
 *
 * Check whether calc is ready.
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT3 int TICALL ticalcs_calc_isready(CalcHandle* handle)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Checking hand-held status:"));
	handle->busy = 1;
	if(calc->is_ready)
		ret = calc->is_ready(handle);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_key:
 * @handle: a previously allocated handle
 * @key: a TI scancode
 *
 * Check whether calc is ready.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_key(CalcHandle* handle, uint16_t key)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending key %04x:"), key);
	handle->busy = 1;
	if(calc->send_key)
		ret = calc->send_key(handle, key);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_execute:
 * @handle: a previously allocated handle
 * @ve: folder and variable name with type
 * @args: argument to pass to program (in TI-charset, aka native)
 *
 * Remotely execute a program or a FLASH application.
 * Restrictions: execution of FLASH applications is supported thru D-USB only.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_execute(CalcHandle* handle, VarEntry* ve, const char* args)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (ve == NULL)
	{
		ticalcs_critical("%s: ve is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Executing %s/%s with %s:"), ve->folder, ve->name, args);
	handle->busy = 1;
	if(calc->execute)
		ret = calc->execute(handle, ve, args);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_screen:
 * @handle: a previously allocated handle
 * @sc: a structure which contains required screen format and returns screen sizes
 * @bitmap: adress of pointer for allocated bitmap. Must be freed when no longer needed.
 *
 * Request a screenshot and receive a raw B&W bitmap (1 bit per pixel).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_screen(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (sc == NULL || bitmap == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting screenshot:"));
	handle->busy = 1;
	if(calc->recv_screen)
		ret = calc->recv_screen(handle, sc, bitmap);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_get_dirlist:
 * @handle: a previously allocated handle
 * @vars: a tree of folder & variables
 * @apps: a tree of FLASH apps
 *
 * Request a directory listing.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_get_dirlist(CalcHandle* handle, GNode** vars, GNode **apps)
{
	const CalcFncts *calc;
	int ret = 0;
	TreeInfo *ti;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (vars == NULL || apps == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting folder & vars & apps listing:"));
	handle->busy = 1;
	if(calc->get_dirlist)
		ret = calc->get_dirlist(handle, vars, apps);

	ti = (*vars)->data;
	ti->mem_mask |= MEMORY_USED;
	ti->mem_used = ticalcs_dirlist_ram_used(*vars);

	ti = (*apps)->data;
	ti->mem_mask |= MEMORY_USED;
	ti->mem_used = ticalcs_dirlist_flash_used(*vars, *apps);

	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_get_memfree:
 * @handle: a previously allocated handle
 * @ram: RAM memory available
 * @flash: Flash memory available
 *
 * Request free memory. Do a dirlist to update value.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_get_memfree(CalcHandle* handle, uint32_t* ram, uint32_t *flash)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (ram == NULL || flash == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting RAM & FLASH free"));
	handle->busy = 1;
	if(calc->get_memfree)
		ret = calc->get_memfree(handle, ram, flash);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_backup:
 * @handle: a previously allocated handle
 * @content: backup content
 *
 * Request a backup and receive it.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_backup(CalcHandle* handle, BackupContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL)
	{
		ticalcs_critical("%s: content is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting backup:"));
	handle->busy = 1;
	if(calc->recv_backup)
		ret = calc->recv_backup(handle, content);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_backup:
 * @handle: a previously allocated handle
 * @content: backup content
 *
 * Send a backup.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_backup(CalcHandle* handle, BackupContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL)
	{
		ticalcs_critical("%s: content is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending backup:"));
	handle->busy = 1;
	if(calc->send_backup)
		ret = calc->send_backup(handle, content);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_var:
 * @handle: a previously allocated handle
 * @mode: to document
 * @content: file content to send
 *
 * Send one or more variables (silent mode).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_var(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL)
	{
		ticalcs_critical("%s: content is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending one or more variables:"));
	handle->busy = 1;
	if(calc->send_var)
		ret = calc->send_var(handle, mode, content);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_var:
 * @handle: a previously allocated handle
 * @mode:
 * @content: where to store variable content
 * @var: a #VarEntry structure got with dirlist
 *
 * Request receiving of _one_ variable (silent mode).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_var(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL || vr == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting variable '%s':"), vr->name);
	handle->busy = 1;
	if(calc->recv_var)
		ret = calc->recv_var(handle, mode, content, vr);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_var_ns:
 * @handle: a previously allocated handle
 * @mode:
 * @content: file content to send
 *
 * Send one or more variable (non-silent mode).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_var_ns(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL)
	{
		ticalcs_critical("%s: content is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending variable (non-silent mode):"));
	handle->busy = 1;
	if(calc->send_var_ns)
		ret = calc->send_var_ns(handle, mode, content);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_var_ns:
 * @handle: a previously allocated handle
 * @mode:
 * @content: where to store variables
 * @var: information on the received variable (if single) or NULL (if group)
 *
 * Receive one or more variable (non-silent mode).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_var_ns(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** var)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL || var == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Receiving variable (non-silent mode):"));
	handle->busy = 1;
	if(calc->recv_var_ns)
		ret = calc->recv_var_ns(handle, mode, content, var);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_app:
 * @handle: a previously allocated handle
 * @content: content to send
 *
 * Send a FLASH app.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_app(CalcHandle* handle, FlashContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL)
	{
		ticalcs_critical("%s: content is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending FLASH application:"));
	handle->busy = 1;
	if(calc->send_app)
		ret = calc->send_app(handle, content);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_app:
 * @handle: a previously allocated handle
 * @content: where to store content
 * @var: FLASH app to request
 *
 * Request receiving of a FLASH app.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_app(CalcHandle* handle, FlashContent* content, VarRequest* var)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL || var == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting receiving of FLASH application:"));
	handle->busy = 1;
	if(calc->recv_app)
		ret = calc->recv_app(handle, content, var);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_os:
 * @handle: a previously allocated handle
 * @content: content to send
 *
 * Send a FLASH os.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_os(CalcHandle* handle, FlashContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL)
	{
		ticalcs_critical("%s: content is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending FLASH os:"));
	handle->busy = 1;
	if(calc->send_app)
		ret = calc->send_os(handle, content);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_idlist:
 * @handle: a previously allocated handle
 * @idlist: static buffer (32 chars) where to store ID-LIST
 *
 * Request ID-LIST of hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_idlist(CalcHandle* handle, uint8_t* idlist)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (idlist == NULL)
	{
		ticalcs_critical("%s: idlist is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting ID-LIST:"));
	handle->busy = 1;
	if(calc->recv_idlist)
		ret = calc->recv_idlist(handle, idlist);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_dump_rom_1:
 * @handle: a previously allocated handle
 *
 * Send a ROM dumping program to hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_dump_rom_1(CalcHandle* handle)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending ROM dumper:"));
	handle->busy = 1;
	if(calc->dump_rom_1)
		ret = calc->dump_rom_1(handle);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_dump_rom_2:
 * @handle: a previously allocated handle
 * @size: optional size of dump
 * @filename: where to store the dump
 *
 * Start dumping (if possible).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_dump_rom_2(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Dumping ROM:"));
	handle->busy = 1;
	if(calc->dump_rom_2)
		ret = calc->dump_rom_2(handle, size, filename);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_set_clock:
 * @handle: a previously allocated handle
 * @clock: a #CalcClock structure
 *
 * Set date & time of hand-held (if AMS >= 2.09).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_set_clock(CalcHandle* handle, CalcClock* _clock)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (_clock == NULL)
	{
		ticalcs_critical("%s: _clock is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Setting clock:"));
	handle->busy = 1;
	if(calc->set_clock)
		ret = calc->set_clock(handle, _clock);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_get_clock:
 * @handle: a previously allocated handle
 * @clock: a #CalcClock structure
 *
 * Get date & time of hand-held (if AMS >= 2.09).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_get_clock(CalcHandle* handle, CalcClock* _clock)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (_clock == NULL)
	{
		ticalcs_critical("%s: _clock is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Getting clock:"));
	handle->busy = 1;
	if(calc->get_clock)
		ret = calc->get_clock(handle, _clock);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_cert:
 * @handle: a previously allocated handle
 * @content: content to send
 *
 * Send a certificate.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_cert(CalcHandle* handle, FlashContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL)
	{
		ticalcs_critical("%s: content is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending certificate:"));
	handle->busy = 1;
	if(calc->send_cert)
		ret = calc->send_cert(handle, content);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_cert:
 * @handle: a previously allocated handle
 * @content: where to store content
 *
 * Request receiving of a FLASH app.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_cert(CalcHandle* handle, FlashContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (content == NULL)
	{
		ticalcs_critical("%s: content is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting receiving of certificate:"));
	handle->busy = 1;
	if(calc->recv_cert)
		ret = calc->recv_cert(handle, content);
	handle->busy = 0;

	return ret;
}


// ---

/**
 * ticalcs_calc_recv_backup2:
 * @handle: a previously allocated handle
 * @filename: name of file where to store backup
 *
 * Request a backup and receive it to file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_backup2(CalcHandle* handle, const char *filename)
{
	BackupContent *content1;
	FileContent *content2;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	if(ticalcs_calc_features(handle) & FTS_BACKUP)
	{
		// true backup capability
		content1 = tifiles_content_create_backup(handle->model);
		TRYF(ticalcs_calc_recv_backup(handle, content1));
		TRYF(tifiles_file_write_backup(filename, content1));
		TRYF(tifiles_content_delete_backup(content1));
	}
	else
	{
		// pseudo-backup
		content2 = tifiles_content_create_regular(handle->model);
		TRYF(ticalcs_calc_recv_backup(handle, (BackupContent *)content2));
		TRYF(tifiles_file_write_regular(filename, content2, NULL));
		TRYF(tifiles_content_delete_regular(content2));
	}

	return 0;
}

/**
 * ticalcs_calc_send_backup2:
 * @handle: a previously allocated handle
 * @filename: name of file which contains backup to send
 *
 * Send a backup from file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_backup2(CalcHandle* handle, const char* filename)
{
	BackupContent *content1;
	FileContent *content2;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	if(ticalcs_calc_features(handle) & FTS_BACKUP)
	{
		// true backup capability
		content1 = tifiles_content_create_backup(handle->model);
		TRYF(tifiles_file_read_backup(filename, content1));
		TRYF(ticalcs_calc_send_backup(handle, content1));
		TRYF(tifiles_content_delete_backup(content1));
	}
	else
	{
		// pseudo-backup
		content2 = tifiles_content_create_regular(handle->model);
		TRYF(tifiles_file_read_regular(filename, content2));
		TRYF(ticalcs_calc_send_backup(handle, (BackupContent *)content2));
		TRYF(tifiles_content_delete_regular(content2));
	}

	return 0;
}

/**
 * ticalcs_calc_send_var2:
 * @handle: a previously allocated handle
 * @mode: to document
 * @filename: name of file
 *
 * Send one or more variables (silent mode) from file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_var2(CalcHandle* handle, CalcMode mode, const char* filename)
{
	FileContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_regular(handle->model);
	TRYF(tifiles_file_read_regular(filename, content));
	TRYF(ticalcs_calc_send_var(handle, mode, content));
	TRYF(tifiles_content_delete_regular(content));

	return 0;
}

/**
 * ticalcs_calc_recv_var2:
 * @handle: a previously allocated handle
 * @mode:
 * @content: where to store variable content
 * @var: a #VarEntry structure got with dirlist
 *
 * Request receiving of _one_ variable (silent mode).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_var2(CalcHandle* handle, CalcMode mode, const char* filename, VarRequest* vr)
{
	FileContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL || vr == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_regular(handle->model);
	TRYF(ticalcs_calc_recv_var(handle, mode, content, vr));
	TRYF(tifiles_file_write_regular(filename, content, NULL));
	TRYF(tifiles_content_delete_regular(content));

	return 0;
}

/**
 * ticalcs_calc_send_var_ns2:
 * @handle: a previously allocated handle
 * @mode:
 * @filename: name of file
 *
 * Send one or more variable (non-silent mode).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_var_ns2(CalcHandle* handle, CalcMode mode, const char* filename)
{
	FileContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_regular(handle->model);
	TRYF(tifiles_file_read_regular(filename, content));
	TRYF(ticalcs_calc_send_var_ns(handle, mode, content));
	TRYF(tifiles_content_delete_regular(content));

	return 0;
}

/**
 * ticalcs_calc_recv_var_ns2:
 * @handle: a previously allocated handle
 * @mode:
 * @filename: where to store variables
 * @var: information on the received variable (if single) or NULL (if group)
 *
 * Receive one or more variable (non-silent mode).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_var_ns2(CalcHandle* handle, CalcMode mode, const char* filename, VarEntry** vr)
{
	FileContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL || vr == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_regular(handle->model);
	TRYF(ticalcs_calc_recv_var_ns(handle, mode, content, vr));
	TRYF(tifiles_file_write_regular(filename, content, NULL));
	TRYF(tifiles_content_delete_regular(content));

	return 0;
}

/**
 * ticalcs_calc_send_app2:
 * @handle: a previously allocated handle
 * @filename: name of file
 *
 * Send a FLASH app or os.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_app2(CalcHandle* handle, const char* filename)
{
	FlashContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_flash(handle->model);
	TRYF(tifiles_file_read_flash(filename, content));
	TRYF(ticalcs_calc_send_app(handle, content));
	TRYF(tifiles_content_delete_flash(content));

	return 0;
}

/**
 * ticalcs_calc_recv_app2:
 * @handle: a previously allocated handle
 * @content: where to store content
 * @var: FLASH app to request
 *
 * Request receiving of a FLASH app.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_app2(CalcHandle* handle, const char* filename, VarRequest* vr)
{
	FlashContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL || vr == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_flash(handle->model);
	TRYF(ticalcs_calc_recv_app(handle, content, vr));
	TRYF(tifiles_file_write_flash(filename, content));
	TRYF(tifiles_content_delete_flash(content));

	return 0;
}

/**
 * ticalcs_calc_new_fld:
 * @handle: a previously allocated handle
 * @vr: name of folder to create (vr->folder)
 *
 * Request creation of a folder. Beware: %vr.name may be modified !
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_new_fld(CalcHandle* handle, VarRequest* vr)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (vr == NULL)
	{
		ticalcs_critical("%s: vr is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Creating folder '%s':"), vr->folder);
	handle->busy = 1;
	if (calc->new_fld)
		ret = calc->new_fld(handle, vr);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_del_var:
 * @handle: a previously allocated handle
 * @var: var to delete
 *
 * Request deleting of a variable (if possible).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_del_var(CalcHandle* handle, VarRequest* vr)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (vr == NULL)
	{
		ticalcs_critical("%s: vr is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Deleting variable '%s':"), vr->name);
	handle->busy = 1;
	if (calc->del_var)
		ret = calc->del_var(handle, vr);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_rename_var:
 * @handle: a previously allocated handle
 * @oldname: existing name and folder
 * @newname: new name and folder
 *
 * Remotely rename a variable, and/or move it to a different folder.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_rename_var(CalcHandle* handle, VarRequest* oldname, VarRequest *newname)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (oldname == NULL || newname == NULL)
	{
		ticalcs_critical("%s: an argument is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	if(oldname->folder && oldname->folder[0] && newname->folder && newname->folder[0])
		ticalcs_info(_("Renaming variable '%s/%s' to '%s/%s':"), oldname->folder, oldname->name, newname->folder, newname->name);
	else
		ticalcs_info(_("Renaming variable '%s' to '%s':"), oldname->name, newname->name);

	handle->busy = 1;
	if (calc->rename_var)
		ret = calc->rename_var(handle, oldname, newname);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_change_attr:
 * @handle: a previously allocated handle
 * @vr: variable to modify
 * @attr: new attributes for variable
 *
 * Remotely change a variable's archive/lock state.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_change_attr(CalcHandle* handle, VarRequest* vr, FileAttr attr)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (vr == NULL)
	{
		ticalcs_critical("%s: vr is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Setting variable '%s' to %s:"), vr->name, tifiles_attribute_to_string(attr));

	handle->busy = 1;
	if (calc->change_attr)
		ret = calc->change_attr(handle, vr, attr);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_get_version:
 * @handle: a previously allocated handle
 * @infos: where to store version information
 *
 * Request version info.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_get_version(CalcHandle* handle, CalcInfos* infos)
{
	const CalcFncts *calc;
	int ret = 0;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (infos == NULL)
	{
		ticalcs_critical("%s: infos is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}
	calc = handle->calc;

	//if(!handle->attached)
	//	return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting version infos:"));
	handle->busy = 1;
	if (calc->get_version)
		ret = calc->get_version(handle, infos);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_cert2:
 * @handle: a previously allocated handle
 * @filename: name of file
 *
 * Send a certificate.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_cert2(CalcHandle* handle, const char* filename)
{
	FlashContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_flash(handle->model);
	TRYF(tifiles_file_read_flash(filename, content));
	TRYF(ticalcs_calc_send_cert(handle, content));
	TRYF(tifiles_content_delete_flash(content));

	return 0;
}

/**
 * ticalcs_calc_recv_cert2:
 * @handle: a previously allocated handle
 * @filename: name of file
 *
 * Request certificate. Depending on extension, saves it as *.9Xq or *.cer.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_cert2(CalcHandle* handle, const char* filename)
{
	FlashContent *content;
	char *ext;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ext = tifiles_fext_get(filename);
	if(!strcmp(ext, "cer"))
	{
		// .cer format as generated by SDK
		gchar *basename = g_strdup(filename);
		FILE *f;
		gchar *e = tifiles_fext_get(basename);
		int err;

		memcpy(e, "crt", 3);

		content = tifiles_content_create_flash(handle->model);
		if ((err = ticalcs_calc_recv_cert(handle, content))) {
			g_free(basename);
			return err;
		}

		f = fopen(basename, "wb");
		g_free(basename);
		if (!f)
			return ERR_SAVE_FILE;
		if (fwrite(content->data_part, content->data_length, 1, f) < 1)
		{
			fclose(f);
			return ERR_SAVE_FILE;
		}
		if (fclose(f))
			return ERR_SAVE_FILE;
		TRYF(tifiles_content_delete_flash(content));

	}
	else
	{
		// .??q format as generated by TI
		content = tifiles_content_create_flash(handle->model);
		TRYF(ticalcs_calc_recv_cert(handle, content));
		TRYF(tifiles_file_write_flash(filename, content));
		TRYF(tifiles_content_delete_flash(content));
	}	

	return 0;
}

/**
 * ticalcs_calc_send_os2:
 * @handle: a previously allocated handle
 * @filename: name of file
 *
 * Send a FLASH app.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_os2(CalcHandle* handle, const char* filename)
{
	FlashContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_flash(handle->model);
	TRYF(tifiles_file_read_flash(filename, content));
	TRYF(ticalcs_calc_send_os(handle, content));
	TRYF(tifiles_content_delete_flash(content));

	return 0;
}

/**
 * ticalcs_calc_send_tigroup2:
 * @handle: a previously allocated handle
 * @filename: name of file
 * @mode: which vars/apps to send
 *
 * Send a TiGroup file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_send_tigroup2(CalcHandle* handle, const char* filename, TigMode mode)
{
	TigContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_tigroup(handle->model, 0);
	TRYF(tifiles_file_read_tigroup(filename, content));
	TRYF(ticalcs_calc_send_tigroup(handle, content, mode));
	TRYF(tifiles_content_delete_tigroup(content));

	return 0;
}

/**
 * ticalcs_calc_recv_tigroup2:
 * @handle: a previously allocated handle
 * @filename: name of file
 * @mode: which vars/apps to receive
 *
 * Receive a TiGroup file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT3 int TICALL ticalcs_calc_recv_tigroup2(CalcHandle* handle, const char* filename, TigMode mode)
{
	TigContent *content;

	if(handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return ERR_INVALID_HANDLE;
	}
	if (filename == NULL)
	{
		ticalcs_critical("%s: filename is NULL", __FUNCTION__);
		return ERR_INVALID_PARAMETER;
	}

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_tigroup(handle->model, 0);
	TRYF(ticalcs_calc_recv_tigroup(handle, content, mode));
	TRYF(tifiles_file_write_tigroup(filename, content));
	TRYF(tifiles_content_delete_tigroup(content));

	return 0;
}
