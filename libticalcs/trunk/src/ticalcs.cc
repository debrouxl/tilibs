/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#define __STDC_FORMAT_MACROS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if defined(__WIN32__)
#include <windows.h>
#endif
#include <locale.h>

#include "gettext.h"
#include "ticalcs.h"
#include "internal.h"
#include "logging.h"
#include "error.h"
#include "calc_xx.h"

/*****************/
/* Internal data */
/*****************/

extern CalcUpdate default_update;

static CalcFncts const *const calcs[] =
{
	&calc_00,
#ifndef NO_TI73
	&calc_73,
#endif
#ifndef NO_TI82
	&calc_82,
#endif
#ifndef NO_TI83
	&calc_83,
#endif
#ifndef NO_TI83P
	&calc_83p,
#endif
#ifndef NO_TI84P
	&calc_84p,
#endif
#ifndef NO_TI85
	&calc_85,
#endif
#ifndef NO_TI86
	&calc_86,
#endif
#ifndef NO_TI89
	&calc_89,
#endif
#ifndef NO_TI89T
	&calc_89t,
#endif
#ifndef NO_TI92
	&calc_92,
#endif
#ifndef NO_TI92P
	&calc_92p,
#endif
#ifndef NO_V200
	&calc_v2,
#endif
#ifndef NO_TI84P_USB
	&calc_84p_usb,
#endif
#ifndef NO_TI89T_USB
	&calc_89t_usb,
#endif
#ifndef NO_NSPIRE
	&calc_nsp,
#endif
#ifndef NO_TI80
	&calc_80,
#endif
#ifndef NO_TI84PC
	&calc_84pcse,
#endif
#ifndef NO_TI84PC_USB
	&calc_84pcse_usb,
#endif
#ifndef NO_TI83PCE_USB
	&calc_83pce_usb,
#endif
#ifndef NO_TI84PCE_USB
	&calc_84pce_usb,
#endif
#ifndef NO_TI82A_USB
	&calc_82a_usb,
#endif
#ifndef NO_TI84PT_USB
	&calc_84pt_usb,
#endif
#ifndef NO_NSPIRE_CRADLE
	&calc_nsp_cradle,
#endif
#ifndef NO_NSPIRE_CLICKPAD
	&calc_nsp_clickpad,
#endif
#ifndef NO_NSPIRE_CLICKPAD_CAS
	&calc_nsp_clickpad_cas,
#endif
#ifndef NO_NSPIRE_TOUCHPAD
	&calc_nsp_touchpad,
#endif
#ifndef NO_NSPIRE_TOUCHPAD_CAS
	&calc_nsp_touchpad_cas,
#endif
#ifndef NO_NSPIRE_CX
	&calc_nsp_cx,
#endif
#ifndef NO_NSPIRE_CX_CAS
	&calc_nsp_cx_cas,
#endif
#ifndef NO_NSPIRE_CMC
	&calc_nsp_cmc,
#endif
#ifndef NO_NSPIRE_CMC_CAS
	&calc_nsp_cmc_cas,
#endif
#ifndef NO_NSPIRE_CXII
	&calc_nsp_cxii,
#endif
#ifndef NO_NSPIRE_CXII_CAS
	&calc_nsp_cxii_cas,
#endif
#ifndef NO_NSPIRE_CXIIT
	&calc_nsp_cxiit,
#endif
#ifndef NO_NSPIRE_CXIIT_CAS
	&calc_nsp_cxiit_cas,
#endif
#ifndef NO_TI82AEP_USB
	&calc_82aep_usb,
#endif
#ifndef NO_CBL
	&calc_cbl,
#endif
#ifndef NO_CBR
	&calc_cbr,
#endif
#ifndef NO_CBL2
	&calc_cbl2,
#endif
#ifndef NO_CBR2
	&calc_cbr2,
#endif
#ifndef NO_LABPRO
	&calc_labpro,
#endif
#ifndef NO_TIPRESENTER
	&calc_tipresenter,
#endif
	nullptr
};

static const uint64_t supported_calcs =
	  (UINT64_C(1) << CALC_NONE)
#ifndef NO_TI73
	| (UINT64_C(1) << CALC_TI73)
#endif
#ifndef NO_TI82
	| (UINT64_C(1) << CALC_TI82)
#endif
#ifndef NO_TI83
	| (UINT64_C(1) << CALC_TI83)
#endif
#ifndef NO_TI83P
	| (UINT64_C(1) << CALC_TI83P)
#endif
#ifndef NO_TI84P
	| (UINT64_C(1) << CALC_TI84P)
#endif
#ifndef NO_TI85
	| (UINT64_C(1) << CALC_TI85)
#endif
#ifndef NO_TI86
	| (UINT64_C(1) << CALC_TI86)
#endif
#ifndef NO_TI89
	| (UINT64_C(1) << CALC_TI89)
#endif
#ifndef NO_TI89T
	| (UINT64_C(1) << CALC_TI89T)
#endif
#ifndef NO_TI92
	| (UINT64_C(1) << CALC_TI92)
#endif
#ifndef NO_TI92P
	| (UINT64_C(1) << CALC_TI92P)
#endif
#ifndef NO_V200
	| (UINT64_C(1) << CALC_V200)
#endif
#ifndef NO_TI84P_USB
	| (UINT64_C(1) << CALC_TI84P_USB)
#endif
#ifndef NO_TI89T_USB
	| (UINT64_C(1) << CALC_TI89T_USB)
#endif
#ifndef NO_NSPIRE
	| (UINT64_C(1) << CALC_NSPIRE)
#endif
#ifndef NO_TI80
	| (UINT64_C(1) << CALC_TI80)
#endif
#ifndef NO_TI84PC
	| (UINT64_C(1) << CALC_TI84PC)
#endif
#ifndef NO_TI84PC_USB
	| (UINT64_C(1) << CALC_TI84PC_USB)
#endif
#ifndef NO_TI83PCE_USB
	| (UINT64_C(1) << CALC_TI83PCE_USB)
#endif
#ifndef NO_TI84PCE_USB
	| (UINT64_C(1) << CALC_TI84PCE_USB)
#endif
#ifndef NO_TI82A_USB
	| (UINT64_C(1) << CALC_TI82A_USB)
#endif
#ifndef NO_TI84PT_USB
	| (UINT64_C(1) << CALC_TI84PT_USB)
#endif
#ifndef NO_NSPIRE_CRADLE
	| (UINT64_C(1) << CALC_NSPIRE_CRADLE)
#endif
#ifndef NO_NSPIRE_CLICKPAD
	| (UINT64_C(1) << CALC_NSPIRE_CLICKPAD)
#endif
#ifndef NO_NSPIRE_CLICKPAD_CAS
	| (UINT64_C(1) << CALC_NSPIRE_CLICKPAD_CAS)
#endif
#ifndef NO_NSPIRE_TOUCHPAD
	| (UINT64_C(1) << CALC_NSPIRE_TOUCHPAD)
#endif
#ifndef NO_NSPIRE_TOUCHPAD_CAS
	| (UINT64_C(1) << CALC_NSPIRE_TOUCHPAD_CAS)
#endif
#ifndef NO_NSPIRE_CX
	| (UINT64_C(1) << CALC_NSPIRE_CX)
#endif
#ifndef NO_NSPIRE_CX_CAS
	| (UINT64_C(1) << CALC_NSPIRE_CX_CAS)
#endif
#ifndef NO_NSPIRE_CMC
	| (UINT64_C(1) << CALC_NSPIRE_CMC)
#endif
#ifndef NO_NSPIRE_CMC_CAS
	| (UINT64_C(1) << CALC_NSPIRE_CMC_CAS)
#endif
#ifndef NO_NSPIRE_CXII
	| (UINT64_C(1) << CALC_NSPIRE_CXII)
#endif
#ifndef NO_NSPIRE_CXII_CAS
	| (UINT64_C(1) << CALC_NSPIRE_CXII_CAS)
#endif
#ifndef NO_NSPIRE_CXIIT
	| (UINT64_C(1) << CALC_NSPIRE_CXIIT)
#endif
#ifndef NO_NSPIRE_CXIIT_CAS
	| (UINT64_C(1) << CALC_NSPIRE_CXIIT_CAS)
#endif
#ifndef NO_TI82AEP_USB
	| (UINT64_C(1) << CALC_TI82AEP_USB)
#endif
#ifndef NO_CBL
	| (UINT64_C(1) << CALC_CBL)
#endif
#ifndef NO_CBR
	| (UINT64_C(1) << CALC_CBR)
#endif
#ifndef NO_CBL2
	| (UINT64_C(1) << CALC_CBL2)
#endif
#ifndef NO_CBR2
	| (UINT64_C(1) << CALC_CBR2)
#endif
#ifndef NO_LABPRO
	| (UINT64_C(1) << CALC_LABPRO)
#endif
#ifndef NO_TIPRESENTER
	| (UINT64_C(1) << CALC_TIPRESENTER)
#endif
;

static const uint32_t supported_protocols =
	  CALC_COMM_PROTO_DBUS
	| CALC_COMM_PROTO_CARS
	| CALC_COMM_PROTO_NAVNET
	| CALC_COMM_PROTO_TICALCS_ROMDUMP
;

/****************/
/* Entry points */
/****************/

// not static, must be shared between instances
int ticalcs_instance = 0;	// counts # of instances

/**
 * ticalcs_library_init:
 *
 * This function must be the first one to call. It inits library internals.
 *
 * Return value: the instance count.
 **/
int TICALL ticalcs_library_init(void)
{
	char locale_dir[65536 + 20];

#ifdef __WIN32__
	HMODULE hDll;
	int i;

	hDll = GetModuleHandle("libticalcs2-13.dll");
	GetModuleFileName(hDll, locale_dir, sizeof(locale_dir) - 21);

	for (i = strlen(locale_dir); i >= 0; i--)
	{
		if (locale_dir[i] == '\\')
		{
			break;
		}
	}
	locale_dir[i] = '\0';

#ifdef __MINGW32__
	strcat(locale_dir, "\\..\\share\\locale");
#else
	strcat(locale_dir, "\\locale");
#endif
#else
	strncpy(locale_dir, LOCALEDIR, sizeof(locale_dir) - 21);
	locale_dir[sizeof(locale_dir) - 21] = 0;
#endif

	if (ticalcs_instance)
	{
		return (++ticalcs_instance);
	}
	ticalcs_info(_("ticalcs library version %s"), LIBCALCS_VERSION);
	errno = 0;

#if defined(ENABLE_NLS)
	ticalcs_info("setlocale: %s", setlocale(LC_ALL, ""));
	ticalcs_info("bindtextdomain: %s", bindtextdomain(PACKAGE, locale_dir));
	bind_textdomain_codeset(PACKAGE, "UTF-8"/*"ISO-8859-15"*/);
	ticalcs_info("textdomain: %s", textdomain(NULL));
#endif

	return (++ticalcs_instance);
}


/**
 * ticalcs_library_exit:
 *
 * This function must be the last one to call. Used to release internal resources.
 *
 * Return value: the instance count.
 **/
int TICALL ticalcs_library_exit(void)
{
	return (--ticalcs_instance);
}

/***********/
/* Methods */
/***********/

/**
 * ticalcs_version_get:
 *
 * This function returns the library version like "X.Y.Z".
 *
 * Return value: a string.
 **/
const char *TICALL ticalcs_version_get(void)
{
	return LIBCALCS_VERSION;
}

/**
 * ticalcs_supported_calcs:
 *
 * This function returns the calcs built into the current binary.
 *
 * Return value: an integer containing a binary OR of (1 << CALC_*) values,
 * where CALC_* values are defined in enum CalcModel.
 **/
uint64_t TICALL ticalcs_supported_calcs (void)
{
	return supported_calcs;
}

/**
 * ticalcs_supported_protocols:
 *
 * This function returns the protocols built into the current binary.
 *
 * Return value: an integer containing a binary OR of (1 << CALC_COMM_PROTO_*) values,
 * where CALC_COMM_PROTO_* values are defined in enum CalcCommunicationProtocols.
 **/
uint32_t TICALL ticalcs_supported_protocols (void)
{
	return supported_protocols;
}

/**
 * ticalcs_max_calc_function_idx:
 *
 * This function returns the maximum index of supported calc functions (mostly for internal usage).
 *
 * Return value: the CALC_FNCT_LAST value against which the library was built.
 **/
uint32_t TICALL ticalcs_max_calc_function_idx(void)
{
	return CALC_FNCT_LAST;
}

int ticalcs_default_event_hook(CalcHandle * handle, uint32_t event_count, const CalcEventData * event, void * user_pointer)
{
	(void)user_pointer;
	const char * calcstr = ticalcs_model_to_string(ticalcs_get_model(handle));
	CableHandle * cable = ticalcs_cable_get(handle);
	const char * cablestr = ticables_model_to_string(ticables_get_model(cable));
	const char * portstr = ticables_port_to_string(ticables_get_port(cable));
	const int pkt_debug = (getenv("TICALCS_PKT_DEBUG") != nullptr);
	if (getenv("TICALCS_EVENT_DEBUG") != nullptr)
	{
		ticalcs_info("Event #%u %d for calc %s connected through cable %s port %s", event_count, event->type, calcstr, cablestr, portstr);
	}
	switch (event->type)
	{
		case CALC_EVENT_TYPE_BEFORE_CABLE_ATTACH: break;
		case CALC_EVENT_TYPE_AFTER_CABLE_ATTACH:
		{
			ticalcs_info("Cable %s port %s attached", cablestr, portstr);
			break;
		}

		case CALC_EVENT_TYPE_BEFORE_CABLE_DETACH: break;
		case CALC_EVENT_TYPE_AFTER_CABLE_DETACH:
		{
			ticalcs_info("Cable %s port %s detached, retval %d", cablestr, portstr, event->retval);
			break;
		}

		case CALC_EVENT_TYPE_BEFORE_SEND_DBUS_PKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("Before PC->TI DBUS PKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
				ticalcs_info("DBUS PKT: target %u, cmd %u, length %u", event->data.dbus_pkt.id, event->data.dbus_pkt.cmd, event->data.dbus_pkt.length);
			}
			break;
		}

		case CALC_EVENT_TYPE_AFTER_RECV_DBUS_PKT_HEADER:
		case CALC_EVENT_TYPE_AFTER_RECV_DBUS_PKT_DATA:
		{
			if (pkt_debug)
			{
				ticalcs_info("After TI->PC DBUS PKT operation %d for calc %s cable %s port %s, retval %d", event->type, calcstr, cablestr, portstr, event->retval);
				ticalcs_info("DBUS PKT: target %u, cmd %u, length %u", event->data.dbus_pkt.id, event->data.dbus_pkt.cmd, event->data.dbus_pkt.length);
				tifiles_hexdump(event->data.dbus_pkt.data, event->data.dbus_pkt.length);
			}
			break;
		}

		case CALC_EVENT_TYPE_BEFORE_SEND_DUSB_RPKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("Before PC->TI DUSB RPKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}

		case CALC_EVENT_TYPE_AFTER_RECV_DUSB_RPKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("After TI->PC DUSB RPKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}

		case CALC_EVENT_TYPE_BEFORE_SEND_DUSB_VPKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("Before PC->TI DUSB VPKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}

		case CALC_EVENT_TYPE_AFTER_RECV_DUSB_VPKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("After TI->PC DUSB VPKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}

		case CALC_EVENT_TYPE_BEFORE_SEND_NSP_RPKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("Before PC->TI NSP RPKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}

		case CALC_EVENT_TYPE_AFTER_RECV_NSP_RPKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("After TI->PC NSP RPKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}


		case CALC_EVENT_TYPE_BEFORE_SEND_NSP_VPKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("Before PC->TI NSP VPKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}

		case CALC_EVENT_TYPE_AFTER_RECV_NSP_VPKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("After TI->PC NSP VPKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}

		case CALC_EVENT_TYPE_BEFORE_SEND_ROMDUMP_PKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("Before PC->TI ROM dump PKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}

		case CALC_EVENT_TYPE_AFTER_RECV_ROMDUMP_PKT:
		{
			if (pkt_debug)
			{
				ticalcs_info("After TI->PC ROM dump PKT operation for calc %s cable %s port %s", calcstr, cablestr, portstr);
			}
			break;
		}

		default: break;
	}
	return event->retval;
}

/**
 * ticalcs_handle_new:
 * @model: a hand-held model
 *
 * Create a new handle associated with the given cable on the given port.
 * Must be freed with ticalcs_handle_del when no longer needed.
 * Note: the handle is a pointer on an opaque structure and should not be modified.
 *
 * Return value: NULL if error, an handle otherwise.
 **/
CalcHandle* TICALL ticalcs_handle_new(CalcModel model)
{
	CalcHandle *handle = nullptr;
	CalcFncts * calc = nullptr;

	for (unsigned int i = 0; i < sizeof(calcs) / sizeof(calcs[0]) - 1; i++) // - 1 for the terminating NULL.
	{
		if (calcs[i]->model == (int)model)
		{
			calc = (CalcFncts *)calcs[i];
			break;
		}
	}

	if (calc != nullptr)
	{
		handle = (CalcHandle *)g_malloc0(sizeof(CalcHandle));
		if (handle != nullptr)
		{
			handle->model = model;
			handle->calc = calc;
			handle->updat = (CalcUpdate *)&default_update;

			handle->priv.nsp_src_port = 0x8001;
			handle->priv.nsp_dst_port = 0x4003; // NSP_PORT_ADDR_REQUEST

			handle->event_hook = ticalcs_default_event_hook;
			//handle->event_count = 0;

			handle->buffer = (uint8_t *)g_malloc(65536 + 6);
			if (handle->buffer == nullptr)
			{
				g_free(handle);
				handle = nullptr;
			}
			handle->buffer2 = (uint8_t *)g_malloc(65536 + 6);
			if (handle->buffer2 == nullptr)
			{
				g_free(handle->buffer);
				g_free(handle);
				handle = nullptr;
			}
		}
	}

	return handle;
}

/**
 * ticalcs_handle_del:
 * @handle: the handle
 *
 * Release the handle and free the associated resources.
 * If cable has not been detached with #ticalcs_cable_detach,
 * it will be detached.
 *
 * Return value: always 0.
 **/
int TICALL ticalcs_handle_del(CalcHandle* handle)
{
	VALIDATE_HANDLE(handle);

	if (handle->attached)
	{
		ticalcs_cable_detach(handle);
	}

	if (handle->buffer2)
	{
		g_free(handle->buffer2);
	}

	if (handle->buffer)
	{
		g_free(handle->buffer);
	}

	memset((void *)handle, 0, sizeof(*handle));
	g_free(handle);

	return 0;
}

/**
 * ticalcs_handle_show:
 * @handle: the handle
 *
 * Show information stored in the handle.
 *
 * Return value: always 0.
 **/
int TICALL ticalcs_handle_show(CalcHandle* handle)
{
	VALIDATE_HANDLE(handle);

	ticalcs_info("%s", _("Link calc handle details:"));
	ticalcs_info(_("  model   : %s"), ticalcs_model_to_string(handle->model));

	return 0;
}

/**
 * ticalcs_get_model:
 * @handle: the handle
 *
 * Return the handle's calculator model.
 *
 * Return value: an integer containing the calc handle's calculator model.
 **/
CalcModel TICALL ticalcs_get_model(CalcHandle *handle)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is invalid", __FUNCTION__);
		return CALC_NONE;
	}

	return handle->model;
}

/**
 * ticalcs_cable_attach:
 * @handle: the handle
 * @cable: a cable to use
 *
 * Attach and open the given cable for use with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_cable_attach(CalcHandle* handle, CableHandle* cable)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	ret = ticalcs_event_send_simple(handle, /* type */ CALC_EVENT_TYPE_BEFORE_CABLE_ATTACH, /* retval */ 0);

	if (!ret)
	{
		handle->cable = cable;
		handle->attached = !0;

		ret = ticables_cable_open(cable);
		if (!ret)
		{
			handle->open = !0;
		}
	}

	ret = ticalcs_event_send_simple(handle, /* type */ CALC_EVENT_TYPE_AFTER_CABLE_ATTACH, /* retval */ ret);

	return ret;
}

/**
 * ticalcs_cable_detach:
 * @handle: the handle
 *
 * Close and detach the cable associated with the hand-held.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_cable_detach(CalcHandle* handle)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	ret = ticalcs_event_send_simple(handle, /* type */ CALC_EVENT_TYPE_BEFORE_CABLE_DETACH, /* retval */ 0);

	if (!ret)
	{
		ret = ticables_cable_close(handle->cable);
		if (!ret)
		{
			handle->open = 0;

			handle->attached = 0;
			handle->cable = nullptr;
		}
	}

	ret = ticalcs_event_send_simple(handle, /* type */ CALC_EVENT_TYPE_AFTER_CABLE_DETACH, /* retval */ ret);

	return ret;
}

/**
 * ticalcs_cable_get:
 * @handle: the handle
 *
 * Returns the cable associated with the hand-held.
 *
 * Return value: the cable handle or NULL.
 **/
CableHandle* TICALL ticalcs_cable_get(CalcHandle *handle)
{
	if (ticalcs_validate_handle(handle))
	{
		return handle->cable;
	}

	return nullptr;
}

/**
 * ticalcs_update_set:
 * @handle: the handle
 * @update: the callbacks to use
 *
 * Set the callbacks to use for the given handle.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticalcs_update_set(CalcHandle* handle, CalcUpdate* upd)
{
	VALIDATE_HANDLE(handle);
	VALIDATE_CALCUPDATE(upd);

	handle->updat = upd;
	return 0;
}

/**
 * ticalcs_update_get:
 * @handle: the handle
 *
 * Get the update callbacks used for the given handle.
 *
 * Return value: the update struct pointer or NULL.
 **/
CalcUpdate* TICALL ticalcs_update_get(CalcHandle *handle)
{
	if (ticalcs_validate_handle(handle))
	{
		return handle->updat;
	}

	return nullptr;
}

/**
 * ticalcs_model_supports_dbus:
 * @model: a calculator model taken in #CalcModel.
 *
 * Returns whether the given calculator model supports the protocol dubbed DBUS by libticalcs.
 * That is, the standard protocol used by TI-Z80 and TI-68k calculators over the legacy I/O.
 *
 * Return value: nonzero if the calculator supports the DBUS protocol, zero if it doesn't.
 */
int TICALL ticalcs_model_supports_dbus(CalcModel model)
{
	return ticonv_model_has_legacy_ioport(model);
}

/**
 * ticalcs_model_supports_dusb:
 * @model: a calculator model taken in #CalcModel.
 *
 * Returns whether the given calculator model supports the CARS protocol dubbed DUSB by libticalcs.
 * That is, the standard protocol used by TI-Z80, TI-eZ80 and TI-68k calculators over the USB port.
 *
 * Return value: nonzero if the calculator supports the DUSB protocol, zero if it doesn't.
 */
int TICALL ticalcs_model_supports_dusb(CalcModel model)
{
	return (   /*model <  CALC_MAX
	        &&*/ ( model == CALC_TI84P_USB
	            || model == CALC_TI89T_USB
	            || model == CALC_TI84PC_USB
	            || model == CALC_TI83PCE_USB
	            || model == CALC_TI84PCE_USB
	            || model == CALC_TI82A_USB
	            || model == CALC_TI84PT_USB
	            || model == CALC_TI82AEP_USB));
}

/**
 * ticalcs_model_supports_nsp:
 * @model: a calculator model taken in #CalcModel.
 *
 * Returns whether the given calculator model supports the NavNet protocol dubbed NSP by libticalcs.
 * That is, the standard protocol used by TI-Nspire calculators over the USB port.
 *
 * Return value: nonzero if the calculator supports the NSP protocol, zero if it doesn't.
 */
int TICALL ticalcs_model_supports_nsp(CalcModel model)
{
	return ticonv_model_is_tinspire(model) /*&& model < CALC_NSPIRE_CXII*/; // FIXME is that right for all CX II OS versions ?
}

/**
 * ticalcs_model_supports_installing_flashapps:
 * @model: a calculator model taken in #CalcModel.
 *
 * Returns whether the given calculator model supports transferring additional FlashApps to the calculator.
 * Some models support FlashApps only as an integral part of the OS upgrade image, arbitrary FlashApps can't be transferred.
 *
 * Return value: nonzero if the calculator supports installing additional FlashApps onto the calculator, zero if it doesn't.
 */
int TICALL ticalcs_model_supports_installing_flashapps(CalcModel model)
{
	return ticonv_model_has_flash_memory(model) && !(   model == CALC_TI82A_USB
	                                                 || model == CALC_TI84PT_USB
	                                                 || model == CALC_TI82AEP_USB
	                                                 || ticonv_model_is_tinspire(model)
	                                                 || model == CALC_CBL2
	                                                 || model == CALC_CBR2
	                                                 || model == CALC_LABPRO
	                                                 || model == CALC_TIPRESENTER);
}

/**
 * ticalcs_calc_get_event_hook:
 *
 * Get the current event hook function pointer.
 *
 * Return value: a function pointer.
 */
ticalcs_event_hook_type TICALL ticalcs_calc_get_event_hook(CalcHandle *handle)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return nullptr;
	}

	return handle->event_hook;
}

/**
 * ticalcs_calc_set_post_recv_hook:
 * @hook: new post recv hook
 *
 * Set the current event hook function pointer.
 *
 * Return value: the previous post recv hook, so that the caller can use it to chain hooks.
 */
ticalcs_event_hook_type TICALL ticalcs_calc_set_event_hook(CalcHandle *handle, ticalcs_event_hook_type hook)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return nullptr;
	}

	const ticalcs_event_hook_type old_hook = handle->event_hook;
	handle->event_hook = hook;

	return old_hook;
}

/**
 * ticalcs_calc_get_event_user_pointer:
 *
 * Set the current post recv hook function pointer.
 *
 * Return value: the previous post recv hook, so that the caller can use it to chain hooks.
 */
void * ticalcs_calc_get_event_user_pointer(CalcHandle *handle)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return nullptr;
	}

	return handle->user_pointer;
}

/**
 * ticalcs_calc_set_event_user_pointer:
 * @user_pointer: new user pointer
 *
 * Set the current post recv hook function pointer.
 *
 * Return value: the previous post recv hook, so that the caller can use it to chain hooks.
 */
void * ticalcs_calc_set_event_user_pointer(CalcHandle *handle, void * user_pointer)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return nullptr;
	}

	void* old_pointer = handle->user_pointer;
	handle->user_pointer = user_pointer;

	return old_pointer;
}

/**
 * ticalcs_calc_get_event_count:
 *
 * Get the current event count since the handle was initialized.
 *
 * Return value: an unsigned integer.
 */
uint32_t TICALL ticalcs_calc_get_event_count(CalcHandle *handle)
{
	if (!ticalcs_validate_handle(handle))
	{
		ticalcs_critical("%s: handle is NULL", __FUNCTION__);
		return 0;
	}

	return handle->event_count;
}

/**
 * ticalcs_calc_fire_user_event:
 * @handle: a previously allocated handle.
 * @type: event type.
 * @user_data: user-specified data.
 * @user_len: user-specified length.
 *
 * Fire a user-specified event to the registered event hook function, if any.
 *
 * Return value: 0 if successful, an error code otherwise.
 */
int TICALL ticalcs_calc_fire_user_event(CalcHandle *handle, CalcEventType type, int retval, void * user_data, uint32_t user_len)
{
	int ret = 0;

	VALIDATE_HANDLE(handle);

	if (handle->event_hook && type >= CALC_EVENT_TYPE_USER)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, /* type */ type, /* retval */ retval, /* operation */ CALC_FNCT_LAST);
		event.data.user_data.data = user_data;
		event.data.user_data.len = user_len;
		handle->event_count++;
		ret = handle->event_hook(handle, handle->event_count, &event, handle->user_pointer);
	}

	return ret;
}
