/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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
#include "internal.h"
#include "gettext.h"
#include "logging.h"
#include "error.h"

/**
 * ticalcs_calc_features:
 * @handle: a previously allocated handle
 *
 * Returns the features and operations supported by the hand-held.
 *
 * Return value: a mask of features (CalcFeatures).
 **/
CalcFeatures TICALL ticalcs_calc_features(CalcHandle* handle)
{
	const CalcFncts *calc;

	if (handle == NULL)
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return FTS_NONE;
	}

	calc = handle->calc;
	if (!calc)
	{
		return FTS_NONE;
	}

	return (CalcFeatures)calc->features;
}

/**
 * ticalcs_calc_isready:
 * @handle: a previously allocated handle
 *
 * Check whether calc is ready.
 *
 * NOTE: after connecting to the calculator, you usually want to use this command once,
 * before attempting to use any other commands. This is required on some models (protocols).
 * There are exceptions to this general rule, especially when a DUSB calculator (84+ family, 89T)
 * is in the OS receive mode, where the Ready check cannot be performed.
 *
 * Return value: 0 if ready else ERR_NOT_READY.
 **/
int TICALL ticalcs_calc_isready(CalcHandle* handle)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Checking hand-held status:"));
	handle->busy = 1;
	if (calc->fncts.is_ready)
	{
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_IS_READY);
		if (!ret)
		{
			ret = calc->fncts.is_ready(handle);
		}
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_IS_READY);
	}
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
int TICALL ticalcs_calc_send_key(CalcHandle* handle, uint32_t key)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info(_("Sending key %08x:"), key);
	handle->busy = 1;
	if (calc->fncts.send_key)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_SEND_KEY);
		event.data.uintval = key;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.send_key(handle, key);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_SEND_KEY);
		event.data.uintval = key;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_execute(CalcHandle* handle, VarEntry* ve, const char* args)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_VARENTRY(ve);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info(_("Executing %s/%s with %s:"), ve->folder, ve->name, args);
	handle->busy = 1;
	if (calc->fncts.execute)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_EXECUTE);
		event.data.ptrval = (void *)ve;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.execute(handle, ve, args);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_EXECUTE);
		event.data.ptrval = (void *)ve;
		ret = ticalcs_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_screen:
 * @handle: a previously allocated handle
 * @sc: a structure which contains required screen format and returns screen sizes
 * @bitmap: adress of pointer for allocated bitmap. Must be freed when no longer needed.
 *
 * Request a screenshot and receive a raw bitmap in the calculator's native format (depending on the model, 1 bit B/W, 4-bit grayscale, 16-bit color).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_calc_recv_screen(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(sc);
	VALIDATE_NONNULL(bitmap);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Requesting screenshot:"));
	handle->busy = 1;
	if (calc->fncts.recv_screen)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_RECV_SCREEN);
		event.data.ptrval = (void *)sc;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.recv_screen(handle, sc, bitmap);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_RECV_SCREEN);
		event.data.ptrval = (void *)*bitmap;
		ret = ticalcs_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_screen_rgb888:
 * @handle: a previously allocated handle
 * @sc: a structure which contains required screen format and returns screen sizes
 * @bitmap: adress of pointer for allocated bitmap. Must be freed when no longer needed.
 *
 * Request a screenshot and receive a RGB888 bitmap (convenience function).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_calc_recv_screen_rgb888(CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap)
{
	int ret = ticalcs_calc_recv_screen(handle, sc, bitmap);

	if (!ret)
	{
		uint8_t * bitmap2 = NULL;

		if (sc->width > 320)
		{
			ticalcs_critical("%s: no calculator model known to this library has screens of width > 320 pixels", __FUNCTION__);
			ticalcs_free_screen(*bitmap);
			ret = ERR_INVALID_PARAMETER;
		}
		else if (sc->height > 240)
		{
			ticalcs_critical("%s: no calculator model known to this library has screens of height > 240 pixels", __FUNCTION__);
			ticalcs_free_screen(*bitmap);
			ret = ERR_INVALID_PARAMETER;
		}
		else
		{
			bitmap2 = (uint8_t *)ticalcs_alloc_screen(3 * sc->width * sc->height);
			ret = ticalcs_screen_convert_native_to_rgb888(sc->pixel_format, *bitmap, sc->width, sc->height, bitmap2);
			if (ret)
			{
				ticalcs_free_screen(bitmap2);
				bitmap2 = NULL;
			}
		}
		*bitmap = bitmap2;
	}

	return ret;
}

/**
 * ticalcs_free_screen:
 * @bitmap: a previously allocated bitmap
 *
 * Frees a bitmap previously allocated by ticalcs_calc_recv_screen() or ticalcs_calc_recv_screen_rgb888().
 */
void TICALL ticalcs_free_screen(uint8_t * bitmap)
{
	g_free((void *)bitmap);
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
int TICALL ticalcs_calc_get_dirlist(CalcHandle* handle, GNode** vars, GNode **apps)
{
	const CalcFncts *calc;
	int ret = 0;
	TreeInfo *ti;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(vars);
	VALIDATE_NONNULL(apps);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	*vars = NULL;
	*apps = NULL;

	ticalcs_info("%s", _("Requesting folder & vars & apps listing:"));
	handle->busy = 1;
	if (calc->fncts.get_dirlist)
	{
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_GET_DIRLIST);
		if (!ret)
		{
			ret = calc->fncts.get_dirlist(handle, vars, apps);
		}
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_GET_DIRLIST);
	}
	handle->busy = 0;

	if (!ret)
	{
		if (*vars != NULL)
		{
			ti = (TreeInfo *)((*vars)->data);
			ti->mem_mask |= MEMORY_USED;
			ti->mem_used = ticalcs_dirlist_ram_used(*vars);
		}

		if (*apps != NULL)
		{
			ti = (TreeInfo *)((*apps)->data);
			ti->mem_mask |= MEMORY_USED;
			ti->mem_used = ticalcs_dirlist_flash_used(*vars, *apps);
		}
	}

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
int TICALL ticalcs_calc_get_memfree(CalcHandle* handle, uint32_t* ram, uint32_t *flash)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(ram);
	VALIDATE_NONNULL(flash);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Requesting RAM & FLASH free"));
	handle->busy = 1;
	if (calc->fncts.get_memfree)
	{
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_GET_MEMFREE);
		if (!ret)
		{
			ret = calc->fncts.get_memfree(handle, ram, flash);
		}
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_GET_MEMFREE);
	}
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
int TICALL ticalcs_calc_send_backup(CalcHandle* handle, BackupContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_BACKUPCONTENT(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Sending backup:"));
	handle->busy = 1;
	if (calc->fncts.send_backup)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_SEND_BACKUP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.send_backup(handle, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_SEND_BACKUP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_recv_backup(CalcHandle* handle, BackupContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_BACKUPCONTENT(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Requesting backup:"));
	handle->busy = 1;
	if (calc->fncts.recv_backup)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_RECV_BACKUP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.recv_backup(handle, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_RECV_BACKUP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

// ticalcs_calc_send_var_: core of ticalcs_calc_send_var, the take_busy argument enables avoiding playing games with handle->busy in rd_send_dumper() / rd_send_dumper2().
static int ticalcs_calc_send_var_(CalcHandle* handle, CalcMode mode, FileContent* content, int take_busy)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FILECONTENT(content);
	VALIDATE_FILECONTENT_ENTRIES(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);

	if (take_busy)
	{
		RETURN_IF_HANDLE_BUSY(handle);
		handle->busy = 1;
	}

	ticalcs_info("%s", _("Sending one or more variables:"));

	if (calc->fncts.send_var)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_SEND_VAR);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.send_var(handle, mode, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_SEND_VAR);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}

	if (take_busy)
	{
		handle->busy = 0;
	}

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
int TICALL ticalcs_calc_send_var(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	return ticalcs_calc_send_var_(handle, mode, content, 1);
}

/**
 * ticalcs_calc_recv_var:
 * @handle: a previously allocated handle
 * @mode:
 * @content: where to store variable content
 * @var: a #VarRequest structure got with dirlist
 *
 * Request receiving of _one_ variable (silent mode).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_calc_recv_var(CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FILECONTENT(content);
	VALIDATE_VARREQUEST(vr);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info(_("Requesting variable '%s':"), vr->name);
	handle->busy = 1;
	if (calc->fncts.recv_var)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_RECV_VAR);
		event.data.ptrval = (void *)vr;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.recv_var(handle, mode, content, vr);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_RECV_VAR);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_send_var_ns(CalcHandle* handle, CalcMode mode, FileContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FILECONTENT(content);
	VALIDATE_FILECONTENT_ENTRIES(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Sending variable (non-silent mode):"));
	handle->busy = 1;
	if (calc->fncts.send_var_ns)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_SEND_VAR_NS);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.send_var_ns(handle, mode, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_SEND_VAR_NS);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_recv_var_ns(CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** var)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FILECONTENT(content);
	VALIDATE_NONNULL(var);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Receiving variable (non-silent mode):"));
	handle->busy = 1;
	if (calc->fncts.recv_var_ns)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_RECV_VAR_NS);
		event.data.ptrval = (void *)var;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.recv_var_ns(handle, mode, content, var);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_RECV_VAR_NS);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_send_app(CalcHandle* handle, FlashContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FLASHCONTENT(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Sending FLASH application:"));
	handle->busy = 1;
	if (calc->fncts.send_app)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_SEND_APP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.send_app(handle, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_SEND_APP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_recv_app(CalcHandle* handle, FlashContent* content, VarRequest* vr)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FLASHCONTENT(content);
	VALIDATE_VARREQUEST(vr);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Requesting receiving of FLASH application:"));
	handle->busy = 1;
	if (calc->fncts.recv_app)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_RECV_APP);
		event.data.ptrval = (void *)vr;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.recv_app(handle, content, vr);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_RECV_APP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_send_os(CalcHandle* handle, FlashContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FLASHCONTENT(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Sending FLASH os:"));
	handle->busy = 1;
	if (calc->fncts.send_os)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_SEND_OS);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.send_os(handle, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_SEND_OS);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_recv_idlist(CalcHandle* handle, uint8_t* idlist)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(idlist);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Requesting ID-LIST:"));
	handle->busy = 1;
	if (calc->fncts.recv_idlist)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_RECV_IDLIST);
		memset((void *)&event.data, 0, sizeof(event.data));
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.recv_idlist(handle, idlist);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_RECV_IDLIST);
		event.data.ptrval = (void *)idlist;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_dump_rom_1(CalcHandle* handle)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Sending ROM dumper:"));
	handle->busy = 1;
	if (calc->fncts.dump_rom_1)
	{
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_DUMP_ROM1);
		if (!ret)
		{
			ret = calc->fncts.dump_rom_1(handle);
		}
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_DUMP_ROM1);
	}
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
int TICALL ticalcs_calc_dump_rom_2(CalcHandle* handle, CalcDumpSize size, const char *filename)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Dumping ROM:"));
	handle->busy = 1;
	if (calc->fncts.dump_rom_2)
	{
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_DUMP_ROM2);
		if (!ret)
		{
			ret = calc->fncts.dump_rom_2(handle, size, filename);
		}
		ret = ticalcs_event_send_simple_generic(handle, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_DUMP_ROM2);
	}
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
int TICALL ticalcs_calc_set_clock(CalcHandle* handle, CalcClock* _clock)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(_clock);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Setting clock:"));
	handle->busy = 1;
	if (calc->fncts.set_clock)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_SET_CLOCK);
		event.data.ptrval = (void *)_clock;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.set_clock(handle, _clock);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_SET_CLOCK);
		event.data.ptrval = (void *)_clock;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_get_clock(CalcHandle* handle, CalcClock* _clock)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(_clock);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Getting clock:"));
	handle->busy = 1;
	if (calc->fncts.get_clock)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_GET_CLOCK);
		memset((void *)&event.data, 0, sizeof(event.data));
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.get_clock(handle, _clock);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_GET_CLOCK);
		event.data.ptrval = (void *)_clock;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_del_var(CalcHandle* handle, VarRequest* vr)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_VARREQUEST(vr);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info(_("Deleting variable '%s':"), vr->name);
	handle->busy = 1;
	if (calc->fncts.del_var)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_DEL_VAR);
		event.data.ptrval = (void *)vr;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.del_var(handle, vr);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_DEL_VAR);
		event.data.ptrval = (void *)vr;
		ret = ticalcs_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
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
int TICALL ticalcs_calc_new_fld(CalcHandle* handle, VarRequest* vr)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_VARREQUEST(vr);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info(_("Creating folder '%s':"), vr->folder);
	handle->busy = 1;
	if (calc->fncts.new_fld)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_NEW_FOLDER);
		event.data.ptrval = (void *)vr;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.new_fld(handle, vr);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_NEW_FOLDER);
		event.data.ptrval = (void *)vr;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_get_version(CalcHandle* handle, CalcInfos* infos)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(infos);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	//RETURN_IF_HANDLE_NOT_ATTACHED(handle); // Disabled after '2005 probing changes.
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Requesting version info:"));
	handle->busy = 1;
	if (calc->fncts.get_version)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_GET_VERSION);
		memset((void *)&event.data, 0, sizeof(event.data));
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.get_version(handle, infos);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_GET_VERSION);
		event.data.ptrval = (void *)infos;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_send_cert(CalcHandle* handle, FlashContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FLASHCONTENT(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Sending certificate:"));
	handle->busy = 1;
	if (calc->fncts.send_cert)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_SEND_CERT);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.send_cert(handle, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_SEND_CERT);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_cert:
 * @handle: a previously allocated handle
 * @content: where to store content
 *
 * Request receiving of a certificate.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_calc_recv_cert(CalcHandle* handle, FlashContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FLASHCONTENT(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Requesting receiving of certificate:"));
	handle->busy = 1;
	if (calc->fncts.recv_cert)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_RECV_CERT);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.recv_cert(handle, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_RECV_CERT);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_rename_var(CalcHandle* handle, VarRequest* oldname, VarRequest *newname)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_VARREQUEST(oldname);
	VALIDATE_VARREQUEST(newname);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	if (oldname->folder[0] && newname->folder[0])
	{
		ticalcs_info(_("Renaming variable '%s/%s' to '%s/%s':"), oldname->folder, oldname->name, newname->folder, newname->name);
	}
	else
	{
		ticalcs_info(_("Renaming variable '%s' to '%s':"), oldname->name, newname->name);
	}

	handle->busy = 1;
	if (calc->fncts.rename_var)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_RENAME);
		event.data.ptrval = (void *)oldname;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.rename_var(handle, oldname, newname);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_RENAME);
		event.data.ptrval = (void *)newname;
		ret = ticalcs_event_send(handle, &event);
	}
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
int TICALL ticalcs_calc_change_attr(CalcHandle* handle, VarRequest* vr, FileAttr attr)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_VARREQUEST(vr);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info(_("Setting variable '%s' to %s:"), vr->name, tifiles_attribute_to_string(attr));

	handle->busy = 1;
	if (calc->fncts.change_attr)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_CHATTR);
		event.data.ptrval = (void *)vr;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.change_attr(handle, vr, attr);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_CHATTR);
		event.data.ptrval = (void *)vr;
		ret = ticalcs_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_send_all_vars_backup:
 * @handle: a previously allocated handle
 * @content: backup content
 *
 * Send a fake backup, for the models which support it.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_calc_send_all_vars_backup(CalcHandle* handle, FileContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FILECONTENT(content);
	VALIDATE_FILECONTENT_ENTRIES(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Sending fake (all vars) backup:"));
	handle->busy = 1;
	if (calc->fncts.send_all_vars_backup)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_SEND_ALL_VARS_BACKUP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.send_all_vars_backup(handle, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_SEND_ALL_VARS_BACKUP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}

/**
 * ticalcs_calc_recv_all_vars_backup:
 * @handle: a previously allocated handle
 * @content: backup content
 *
 * Request a fake backup (list of vars and apps) and receive it.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_calc_recv_all_vars_backup(CalcHandle* handle, FileContent* content)
{
	const CalcFncts *calc;
	int ret = 0;

	VALIDATE_HANDLE(handle);
	VALIDATE_FILECONTENT(content);

	calc = handle->calc;
	VALIDATE_CALCFNCTS(calc);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	ticalcs_info("%s", _("Requesting fake (all vars) backup:"));
	handle->busy = 1;
	if (calc->fncts.recv_all_vars_backup)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION, /* retval */ 0, /* operation */ FNCT_RECV_ALL_VARS_BACKUP);
		memset((void *)&event.data, 0, sizeof(event.data));
		ret = ticalcs_event_send(handle, &event);
		if (!ret)
		{
			ret = calc->fncts.recv_all_vars_backup(handle, content);
		}
		ticalcs_event_fill_header(handle, &event, /* type */ CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION, /* retval */ ret, /* operation */ FNCT_RECV_ALL_VARS_BACKUP);
		event.data.ptrval = (void *)content;
		ret = ticalcs_event_send(handle, &event);
	}
	handle->busy = 0;

	return ret;
}


// ---


/**
 * ticalcs_calc_send_backup2:
 * @handle: a previously allocated handle
 * @filename: name of file which contains backup to send
 *
 * Send a backup from file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_calc_send_backup2(CalcHandle* handle, const char* filename)
{
	int ret = ERR_FILE_OPEN;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	if (ticalcs_calc_features(handle) & FTS_BACKUP)
	{
		// true backup capability
		BackupContent * content = tifiles_content_create_backup(handle->model);
		ret = tifiles_file_read_backup(filename, content);
		if (!ret)
		{
			ret = ticalcs_calc_send_backup(handle, content);
			tifiles_content_delete_backup(content);
		}
		// content is destroyed by the functions behind tifiles_file_read_backup() if an error occurs.
	}
	else
	{
		// pseudo-backup
		FileContent * content = tifiles_content_create_regular(handle->model);
		ret = tifiles_file_read_regular(filename, content);
		if (!ret)
		{
			ret = ticalcs_calc_send_all_vars_backup(handle, content);
			tifiles_content_delete_regular(content);
		}
		// content is destroyed by the functions behind tifiles_file_read_regular() if an error occurs.
	}

	return ret;
}

/**
 * ticalcs_calc_recv_backup2:
 * @handle: a previously allocated handle
 * @filename: name of file where to store backup
 *
 * Request a backup and receive it to file.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_calc_recv_backup2(CalcHandle* handle, const char *filename)
{
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	if (ticalcs_calc_features(handle) & FTS_BACKUP)
	{
		// true backup capability
		BackupContent * content = tifiles_content_create_backup(handle->model);
		ret = ticalcs_calc_recv_backup(handle, content);
		if (!ret)
		{
			ret = tifiles_file_write_backup(filename, content);
		}
		// content is not destroyed by the functions behind ticalcs_calc_recv_backup() if an error occurs.
		tifiles_content_delete_backup(content);
	}
	else
	{
		// pseudo-backup
		FileContent * content = tifiles_content_create_regular(handle->model);
		ret = ticalcs_calc_recv_all_vars_backup(handle, content);
		if (!ret)
		{
			ret = tifiles_file_write_regular(filename, content, NULL);
		}
		// content is not destroyed by the functions behind ticalcs_calc_recv_all_vars_backup() if an error occurs.
		tifiles_content_delete_regular(content);
	}

	return ret;
}

// ticalcs_calc_send_var2_: core of ticalcs_calc_send_var2, the take_busy argument enables avoiding playing games with handle->busy in rd_send_dumper() / rd_send_dumper2().
int ticalcs_calc_send_var2_(CalcHandle* handle, CalcMode mode, const char* filename, int take_busy)
{
	FileContent *content;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	if (take_busy)
	{
		RETURN_IF_HANDLE_BUSY(handle);
	}

	content = tifiles_content_create_regular(handle->model);
	ret = tifiles_file_read_regular(filename, content);
	if (!ret)
	{
		ret = ticalcs_calc_send_var_(handle, mode, content, take_busy);
		tifiles_content_delete_regular(content);
	}
	// content is destroyed by the functions behind tifiles_file_read_regular() if an error occurs.

	return ret;
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
int TICALL ticalcs_calc_send_var2(CalcHandle* handle, CalcMode mode, const char* filename)
{
	return ticalcs_calc_send_var2_(handle, mode, filename, 1);
}

/**
 * ticalcs_calc_recv_var2:
 * @handle: a previously allocated handle
 * @mode:
 * @content: where to store variable content
 * @var: a #VarRequest structure got with dirlist
 *
 * Request receiving of _one_ variable (silent mode).
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_calc_recv_var2(CalcHandle* handle, CalcMode mode, const char* filename, VarRequest* vr)
{
	FileContent *content;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);
	VALIDATE_VARREQUEST(vr);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	content = tifiles_content_create_regular(handle->model);
	ret = ticalcs_calc_recv_var(handle, mode, content, vr);
	if (!ret)
	{
		ret = tifiles_file_write_regular(filename, content, NULL);
	}
	// content is not destroyed by the functions behind ticalcs_calc_recv_var() if an error occurs.
	tifiles_content_delete_regular(content);

	return ret;
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
int TICALL ticalcs_calc_send_var_ns2(CalcHandle* handle, CalcMode mode, const char* filename)
{
	FileContent *content;
	int ret = ERR_FILE_OPEN;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	content = tifiles_content_create_regular(handle->model);
	ret = tifiles_file_read_regular(filename, content);
	if (!ret)
	{
		ret = ticalcs_calc_send_var_ns(handle, mode, content);
		tifiles_content_delete_regular(content);
	}
	// content is destroyed by the functions behind tifiles_file_read_regular() if an error occurs.

	return ret;
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
int TICALL ticalcs_calc_recv_var_ns2(CalcHandle* handle, CalcMode mode, const char* filename, VarEntry** vr)
{
	FileContent *content;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);
	VALIDATE_NONNULL(vr);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	content = tifiles_content_create_regular(handle->model);
	ret = ticalcs_calc_recv_var_ns(handle, mode, content, vr);
	if (!ret)
	{
		ret = tifiles_file_write_regular(filename, content, NULL);
	}
	// content is not destroyed by the functions behind ticalcs_calc_recv_var_ns() if an error occurs.
	tifiles_content_delete_regular(content);

	return ret;
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
int TICALL ticalcs_calc_send_app2(CalcHandle* handle, const char* filename)
{
	FlashContent *content;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	content = tifiles_content_create_flash(handle->model);
	ret = tifiles_file_read_flash(filename, content);
	if (!ret)
	{
		ret = ticalcs_calc_send_app(handle, content);
		tifiles_content_delete_flash(content);
	}
	// content is destroyed by the functions behind tifiles_file_read_flash() if an error occurs.

	return ret;
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
int TICALL ticalcs_calc_recv_app2(CalcHandle* handle, const char* filename, VarRequest* vr)
{
	FlashContent *content;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);
	VALIDATE_VARREQUEST(vr);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	content = tifiles_content_create_flash(handle->model);
	ret = ticalcs_calc_recv_app(handle, content, vr);
	if (!ret)
	{
		ret = tifiles_file_write_flash(filename, content);
	}
	// content is not destroyed by the functions behind ticalcs_calc_recv_app() if an error occurs.
	tifiles_content_delete_flash(content);

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
int TICALL ticalcs_calc_send_cert2(CalcHandle* handle, const char* filename)
{
	FlashContent *content;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	content = tifiles_content_create_flash(handle->model);
	ret = tifiles_file_read_flash(filename, content);
	if (!ret)
	{
		ret = ticalcs_calc_send_cert(handle, content);
		tifiles_content_delete_flash(content);
	}
	// content is destroyed by the functions behind tifiles_file_read_flash() if an error occurs.

	return ret;
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
int TICALL ticalcs_calc_send_os2(CalcHandle* handle, const char* filename)
{
	FlashContent *content;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	content = tifiles_content_create_flash(handle->model);
	ret = tifiles_file_read_flash(filename, content);
	if (!ret)
	{
		ret = ticalcs_calc_send_os(handle, content);
		tifiles_content_delete_flash(content);
	}
	// content is destroyed by the functions behind tifiles_file_read_flash() if an error occurs.

	return ret;
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
int TICALL ticalcs_calc_recv_cert2(CalcHandle* handle, const char* filename)
{
	FlashContent *content = NULL;
	char *ext;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	do {
		ext = tifiles_fext_get(filename);
		if (!strcmp(ext, "cer"))
		{
			// .cer format as generated by SDK
			gchar *basename = strdup(filename);
			FILE *f;
			gchar *e;

			if (basename == NULL)
			{
				ret = ERR_MALLOC;
				break;
			}

			e = tifiles_fext_get(basename);

			memcpy(e, "crt", 3);

			content = tifiles_content_create_flash(handle->model);
			ret = ticalcs_calc_recv_cert(handle, content);
			if (ret)
			{
				free(basename);
				break;
			}

			f = fopen(basename, "wb");
			free(basename);
			if (!f)
			{
				ret = ERR_SAVE_FILE;
				break;
			}
			if (fwrite(content->data_part, content->data_length, 1, f) < 1)
			{
				fclose(f);
				ret = ERR_SAVE_FILE;
				break;
			}
			if (fclose(f))
			{
				ret = ERR_SAVE_FILE;
				break;
			}
		}
		else
		{
			// .??q format as generated by TI
			content = tifiles_content_create_flash(handle->model);
			ret = ticalcs_calc_recv_cert(handle, content);
			if (!ret)
			{
				ret = tifiles_file_write_flash(filename, content);
			}
		}
	} while (0);

	// content is not destroyed by the functions behind ticalcs_calc_recv_cert() if an error occurs.
	tifiles_content_delete_flash(content);

	return ret;
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
int TICALL ticalcs_calc_send_tigroup2(CalcHandle* handle, const char* filename, TigMode mode)
{
	TigContent *content;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	content = tifiles_content_create_tigroup(handle->model, 0);
	ret = tifiles_file_read_tigroup(filename, content);
	if (!ret)
	{
		ret = ticalcs_calc_send_tigroup(handle, content, mode);
	}
	// content is not destroyed by the functions behind tifiles_file_read_tigroup() if an error occurs.
	tifiles_content_delete_tigroup(content);

	return ret;
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
int TICALL ticalcs_calc_recv_tigroup2(CalcHandle* handle, const char* filename, TigMode mode)
{
	TigContent *content;
	int ret;

	VALIDATE_HANDLE(handle);
	VALIDATE_NONNULL(filename);

	RETURN_IF_HANDLE_NOT_ATTACHED(handle);
	RETURN_IF_HANDLE_NOT_OPEN(handle);
	RETURN_IF_HANDLE_BUSY(handle);

	content = tifiles_content_create_tigroup(handle->model, 0);
	ret = ticalcs_calc_recv_tigroup(handle, content, mode);
	if (!ret)
	{
		ret = tifiles_file_write_tigroup(filename, content);
	}
	// content is not destroyed by the functions behind ticalcs_calc_recv_tigroup() if an error occurs.
	tifiles_content_delete_tigroup(content);

	return ret;
}
