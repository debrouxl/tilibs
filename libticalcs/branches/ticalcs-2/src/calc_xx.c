/* Hey EMACS -*- linux-c -*- */
/* $Id: link_xxx.c 1059 2005-05-14 09:45:42Z roms $ */

/*  libCables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

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
TIEXPORT CalcFeatures TICALL ticalcs_calc_features(CalcHandle* handle)
{
	const CalcFncts *calc = handle->calc;

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
TIEXPORT int TICALL ticalcs_calc_isready(CalcHandle* handle)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Checking hand-held status:"));
	handle->busy = 1;
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
TIEXPORT int TICALL ticalcs_calc_send_key(CalcHandle* handle, uint16_t key)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending key %04x:"));
	handle->busy = 1;
	ret = calc->send_key(handle, key);
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
TIEXPORT int TICALL ticalcs_calc_recv_screen(CalcHandle* handle, CalcScreenCoord* sc,
												 uint8_t** bitmap)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting screenshot:"));
	handle->busy = 1;
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
TIEXPORT int TICALL ticalcs_calc_get_dirlist(CalcHandle* handle, 
											 TNode** vars, TNode **apps)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting folder & vars & apps listing:"));
	handle->busy = 1;
	ret = calc->get_dirlist(handle, vars, apps);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_get_dirlist:
 * @handle: a previously allocated handle
 * @memory: memory available
 *
 * Request free memory. Do a dirlist to update value.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
TIEXPORT int TICALL ticalcs_calc_get_memfree(CalcHandle* handle, uint32_t* memory)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting folder & vars & apps listing:"));
	handle->busy = 1;
	ret = calc->get_memfree(handle, memory);
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
TIEXPORT int TICALL ticalcs_calc_recv_backup(CalcHandle* handle, BackupContent* content)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting backup:"));
	handle->busy = 1;
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
TIEXPORT int TICALL ticalcs_calc_send_backup(CalcHandle* handle, BackupContent* content)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending backup:"));
	handle->busy = 1;
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
TIEXPORT int TICALL ticalcs_calc_send_var(CalcHandle* handle, CalcMode mode, 
										  FileContent* content)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending one or more variables:"));
	handle->busy = 1;
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
TIEXPORT int TICALL ticalcs_calc_recv_var(CalcHandle* handle, CalcMode mode, 
											FileContent* content, VarRequest* vr)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting variable '%s':"), vr->name);
	handle->busy = 1;
	ret = calc->recv_var(handle, mode, content, vr);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_del_var:
 * @handle: a previously allocated handle
 * @var: var to delete
 *
 * Request deleting of a variable (if possible ??).
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_del_var(CalcHandle* handle, VarRequest* vr)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting deletion of variable '%s':"), vr->name);
	handle->busy = 1;
	ret = calc->del_var(handle, vr);
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
TIEXPORT int TICALL ticalcs_calc_send_var_ns(CalcHandle* handle, CalcMode mode, 
											 FileContent* content)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending variable (non-silent mode):"));
	handle->busy = 1;
	ret = calc->send_var_ns(handle, mode, content);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_var_ns:
 * @handle: a previously allocated handle
 * @mode:
 * @content: where to store variables
 * @var: informations on the received variable
 *
 * Receive one or more variable (non-silent mode).
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_recv_var_ns(CalcHandle* handle, CalcMode mode, 
											 FileContent* content, VarEntry* var)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Receiving variable (non-silent mode):"));
	handle->busy = 1;
	ret = calc->recv_var_ns(handle, mode, content, var);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_flash:
 * @handle: a previously allocated handle
 * @content: content to send
 *
 * Send a FLASH app or os.
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_send_flash(CalcHandle* handle, FlashContent* content)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Sending FLASH app/os:"));
	handle->busy = 1;
	ret = calc->send_flash(handle, content);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_flash:
 * @handle: a previously allocated handle
 * @content: where to store content
 * @var: FLASH app to request
 *
 * Request receiving of a FLASH app.
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_recv_flash(CalcHandle* handle, FlashContent* content, 
											VarRequest* var)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting receiving of FLASH app:"));
	handle->busy = 1;
	ret = calc->recv_flash(handle, content, var);
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
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_recv_idlist(CalcHandle* handle, uint8_t* idlist)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Requesting ID-LIST:"));
	handle->busy = 1;
	ret = calc->recv_idlist(handle, idlist);
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_dump_rom:
 * @handle: a previously allocated handle
 * @size: optional size of dump
 * @filename: where to store the dump
 *
 * Send a ROM dumping program to hand-held and start dumping (if possible).
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_dump_rom(CalcHandle* handle, CalcDumpSize size, 
										  const char *filename)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Dumping ROM:"));
	handle->busy = 1;
	ret = calc->dump_rom(handle, size, filename);
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
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_set_clock(CalcHandle* handle, CalcClock* clock)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Setting clock:"));
	handle->busy = 1;
	ret = calc->set_clock(handle, clock);
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
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_get_clock(CalcHandle* handle, CalcClock* clock)
{
	const CalcFncts *calc = handle->calc;
	int ret;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	ticalcs_info(_("Getting clock:"));
	handle->busy = 1;
	ret = calc->get_clock(handle, clock);
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
TIEXPORT int TICALL ticalcs_calc_recv_backup2(CalcHandle* handle, const char *filename)
{
	const CalcFncts *calc = handle->calc;
	BackupContent *content1;
	FileContent *content2;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	switch(handle->model)
	{
	case CALC_TI73:
	case CALC_TI82:
	case CALC_TI83:
	case CALC_TI83P:
	case CALC_TI84P:
	case CALC_TI86:
	case CALC_TI92:
		// true backup capability
		content1 = tifiles_content_create_backup();
		TRYF(ticalcs_calc_recv_backup(handle, content1));
		TRYF(tifiles_file_write_backup(filename, content1));
		TRYF(tifiles_content_free_backup(content1));
		break;
	default:
		// pseudo-backup
		content2 = tifiles_content_create_regular();
		TRYF(ticalcs_calc_recv_backup(handle, (BackupContent *)content2));
		TRYF(tifiles_file_write_regular(filename, content2, NULL));
		TRYF(tifiles_content_free_regular(content2));
		break;
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
TIEXPORT int TICALL ticalcs_calc_send_backup2(CalcHandle* handle, const char* filename)
{
	const CalcFncts *calc = handle->calc;
	BackupContent content1;
	FileContent content2;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	switch(handle->model)
	{
	case CALC_TI73:
	case CALC_TI83:
	case CALC_TI86:
	case CALC_TI92:
		// true backup capability
		TRYF(tifiles_file_read_backup(filename, &content1));
		TRYF(ticalcs_calc_send_backup(handle, &content1));
		TRYF(tifiles_content_free_backup(&content1));
		break;
	default:
		// pseudo-backup
		TRYF(tifiles_file_read_regular(filename, &content2));
		TRYF(ticalcs_calc_send_backup(handle, (BackupContent *)&content2));
		TRYF(tifiles_content_free_regular(&content2));
		break;
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
TIEXPORT int TICALL ticalcs_calc_send_var2(CalcHandle* handle, CalcMode mode, 
										   const char* filename)
{
	const CalcFncts *calc = handle->calc;
	FileContent content;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	TRYF(tifiles_file_read_regular(filename, &content));
	TRYF(ticalcs_calc_send_var(handle, mode, &content));
	TRYF(tifiles_content_free_regular(&content));

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
TIEXPORT int TICALL ticalcs_calc_recv_var2(CalcHandle* handle, CalcMode mode, 
											const char* filename, VarRequest* vr)
{
	const CalcFncts *calc = handle->calc;
	FileContent* content;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_regular();
	TRYF(ticalcs_calc_recv_var(handle, mode, content, vr));
	TRYF(tifiles_file_write_regular(filename, content, NULL));
	TRYF(tifiles_content_free_regular(content));

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
TIEXPORT int TICALL ticalcs_calc_send_var_ns2(CalcHandle* handle, CalcMode mode, 
											 const char* filename)
{
	const CalcFncts *calc = handle->calc;
	FileContent content;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	TRYF(tifiles_file_read_regular(filename, &content));
	TRYF(ticalcs_calc_send_var_ns(handle, mode, &content));
	TRYF(tifiles_content_free_regular(&content));

	return 0;
}

/**
 * ticalcs_calc_recv_var_ns2:
 * @handle: a previously allocated handle
 * @mode:
 * @content: where to store variables
 * @var: informations on the received variable
 *
 * Receive one or more variable (non-silent mode).
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_recv_var_ns2(CalcHandle* handle, CalcMode mode, 
											 const char* filename, VarEntry* vr)
{
	const CalcFncts *calc = handle->calc;
	FileContent *content;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_regular();
	TRYF(ticalcs_calc_recv_var_ns(handle, mode, content, vr));
	TRYF(tifiles_file_write_regular(filename, content, NULL));
	TRYF(tifiles_content_free_regular(content));

	return 0;
}

/**
 * ticalcs_calc_send_flash2:
 * @handle: a previously allocated handle
 * @filename: name of file
 *
 * Send a FLASH app or os.
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_send_flash2(CalcHandle* handle, const char* filename)
{
	const CalcFncts *calc = handle->calc;
	FlashContent content;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	TRYF(tifiles_file_read_flash(filename, &content));
	TRYF(ticalcs_calc_send_flash(handle, &content));
	TRYF(tifiles_content_free_flash(&content));

	return 0;
}

/**
 * ticalcs_calc_recv_flash2:
 * @handle: a previously allocated handle
 * @content: where to store content
 * @var: FLASH app to request
 *
 * Request receiving of a FLASH app.
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
TIEXPORT int TICALL ticalcs_calc_recv_flash2(CalcHandle* handle, const char* filename, 
											VarRequest* vr)
{
	const CalcFncts *calc = handle->calc;
	FlashContent *content;

	if(!handle->attached)
		return ERR_NO_CABLE;

	if(!handle->open)
		return ERR_NO_CABLE;

	if(handle->busy)
		return ERR_BUSY;

	content = tifiles_content_create_flash();
	TRYF(ticalcs_calc_recv_flash(handle, content, vr));
	TRYF(tifiles_file_write_flash(filename, content));
	TRYF(tifiles_content_free_flash(content));

	return 0;
}