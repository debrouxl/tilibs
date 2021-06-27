/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gettext.h"
#include "logging.h"
#include "ticables.h"
#include "error.h"
#include "internal.h"

#include "linux/detect.h"
#include "win32/detect.h"
#include "bsd/detect.h"
#include "macos/detect.h"

int TICALL ticables_probing_found(int *array)
{
	if (array != NULL)
	{
		int i;
		for (i = PORT_FIRST; i < PORT_MAX; i++)
		{
			if (array[i])
			{
				return i;
			}
		}
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}

void TICALL ticables_probing_show(int **array)
{
	if (array != NULL)
	{
		int model;

		for (model = CABLE_NUL; model < CABLE_MAX; model++)
		{
			int * arraymodel = array[model];
			if (arraymodel != NULL)
			{
				ticables_info(" %i: %i %i %i %i", model, array[model][PORT_1], array[model][PORT_2], array[model][PORT_3], array[model][PORT_4]);
			}
			else
			{
				ticables_info("%i: null", model);
			}
		}
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
	}
}

/**
 * ticables_probing_do:
 * @result: address of an array of integers to put the result.
 * @timeout: timeout to set during probing
 * @method: defines which link cables you want to search for.
 *
 * Returns cables which have been detected. All cables should be closed before calling this function !
 * The array defines a matrix of PORT_MAX columns and CABLE_MAX + 1 rows; the rows corresponding to
 * cables which cannot be probed are allocated but empty.
 * The array must be freed by #ticables_probing_finish when no longer used.
 *
 * Return value: 0 if successful, ERR_NO_CABLE if no cables found.
 **/
int TICALL ticables_probing_do(int ***result, unsigned int timeout, ProbingMethod method)
{
	int port;
	int model;
	int **array;
	int found = 0;

	if (result == NULL)
	{
		ticables_critical("%s: result is NULL", __FUNCTION__);
		return ERR_PROBE_FAILED;
	}
	*result = NULL;

	ticables_info("%s", _("Link cable probing:"));

	array = (int **)calloc(CABLE_MAX + 1, sizeof(int *));
	if (array == NULL)
	{
		return ERR_PROBE_FAILED; // THIS RETURNS !
	}

	for (model = CABLE_NUL; model <= CABLE_MAX; model++)
	{
		array[model] = (int *)calloc(PORT_MAX, sizeof(int));
		if (array[model] == NULL)
		{
			for (model = CABLE_NUL; model <= CABLE_MAX; model++)
			{
				free(array[model]);
			}
			free(array);
			return ERR_PROBE_FAILED; // THIS RETURNS !
		}
	}

	// look for USB devices (faster)
	if (method & PROBE_USB)
	{
		int *list, n, i;

		ticables_get_usb_devices(&list, &n);

		for (i = 0; i < n; i++)
		{
			port = i+1;

			if (list[i] == PID_TIGLUSB)
			{
				array[CABLE_SLV][port] = !0;
			}

			if (list[i])
			{
				array[CABLE_USB][port] = !0;
				found = !0;
			}
		}

		ticables_free_usb_devices(list);
	}

	if ((method & PROBE_FIRST) && found)
	{
		*result = array;
		return 0;
	}

	// look for DBUS devices (slower)
	if (method & PROBE_DBUS)
	{
		for (model = CABLE_GRY; model <= CABLE_PAR; model++)
		{
			for (port = PORT_1; port < PORT_MAX; port++)
			{
				CableHandle* handle;
				int err, ret;

				handle = ticables_handle_new((CableModel)model, (CablePort)port);
				if (handle != NULL)
				{
					ticables_options_set_timeout(handle, timeout);
					err = ticables_cable_probe(handle, &ret);
					array[model][port] = (ret && !err) ? 1: 0;
					if (array[model][port])
					{
						found = !0;
					}

					if (found && (method & PROBE_FIRST))
					{
						ticables_handle_del(handle);
						break;
					}
				}
				ticables_handle_del(handle);
			}
		}
	}

	*result = array;
	return found ? 0 : ERR_NO_CABLE;
}

/**
 * ticables_probing_finish:
 * @result: address of an array of integers. 
 *
 * Free the array created by #ticables_probing_do.
 *
 * Return value: always 0.
 **/
int TICALL ticables_probing_finish(int ***result)
{
	int i;

	if (result != NULL && *result != NULL)
	{
		for (i = CABLE_NUL; i <= CABLE_MAX; i++)
		{
			free((*result)[i]);
			(*result)[i] = NULL;
		}

		free(*result);
		*result = NULL;
	}
	else
	{
		ticables_critical("%s(NULL)", __FUNCTION__);
	}

	return 0;
}

/**
 * ticables_is_usb_enabled:
 *
 * Checks whether USB support is available. Can be called at any time.
 *
 * Return value: !0 if available, 0 otherwise.
 **/
int TICALL ticables_is_usb_enabled(void)
{
#if defined(__WIN32__)
	return !win32_check_libusb();
#elif defined(__MACOSX__)
	return !macosx_check_libusb();
#elif defined(__BSD__)
	return !bsd_check_libusb();
#elif defined(__LINUX__)
	return !linux_check_libusb();
#else
	return 0;
#endif
}

/**
 * ticables_get_usb_devices:
 * @list: out address of a NULL-terminated allocated array of integers (PIDs).
 * @len: out pointer to number of detected USB devices.
 *
 * Returns the list of detected USB PIDs. Note that list is in the 
 * same order as PORT#x.
 * The array must be freed when no longer used.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticables_get_usb_devices(int **list, int *len)
{
	if (list != NULL)
	{
		int ret = 0;
		const USBCableInfo *info = NULL;
		int i, n = 0;

#if defined(__WIN32__) || (defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0))
		ret = usb_probe_device_info(&info, &n);
#endif
		*list = (int *)calloc(1 + n, sizeof(int));
		for (i = 0; i < n; i++)
		{
			(*list)[i] = info[i].pid;
		}
		if (len)
		{
			*len = i;
		}

		return ret;
	}
	else
	{
		ticables_critical("%s: list is NULL", __FUNCTION__);
		return -1;
	}
}

/**
 * \brief Frees an array of USB devices
 * \param array the array previously allocated by ticables_get_usb_devices()
 * \return Always 0
 */
int TICALL ticables_free_usb_devices(int *array)
{
	free(array);
	return 0;
}

void translate_usb_device_info(CableDeviceInfo *info, const USBCableInfo *usbinfo)
{
	if (usbinfo->pid == PID_TIGLUSB)
	{
		info->family = CABLE_FAMILY_DBUS;
		info->variant = CABLE_VARIANT_TIGLUSB;
	}
	else if (usbinfo->pid == PID_TI84P)
	{
		info->family = CABLE_FAMILY_USB_TI8X;
		if (!strcmp(usbinfo->product_str, "TI-84 Plus"))
		{
			info->variant = CABLE_VARIANT_TI84P;
		}
		else if (!strcmp(usbinfo->product_str, "TI-82 Advanced"))
		{
			info->variant = CABLE_VARIANT_TI82A;
		}
		else if (usbinfo->version <= 0x0110)
		{
			info->variant = CABLE_VARIANT_TI84P;
		}
		else if (usbinfo->version == 0x0190)
		{
			info->variant = CABLE_VARIANT_TI82A;
		}
		else
		{
			info->variant = CABLE_VARIANT_UNKNOWN;
		}
	}
	else if (usbinfo->pid == PID_TI84P_SE)
	{
		info->family = CABLE_FAMILY_USB_TI8X;
		if (!strcmp(usbinfo->product_str, "TI-84 Plus Silver Edition"))
		{
			info->variant = CABLE_VARIANT_TI84PSE;
		}
		else if (!strcmp(usbinfo->product_str, "TI-84 Plus C Silver Edition"))
		{
			info->variant = CABLE_VARIANT_TI84PCSE;
		}
		else if (!strcmp(usbinfo->product_str, "TI-84 Plus CE"))
		{
			info->variant = CABLE_VARIANT_TI84PCE;
		}
		else if (!strcmp(usbinfo->product_str, "TI-83 Premium CE"))
		{
			info->variant = CABLE_VARIANT_TI83PCE;
		}
		else if (!strcmp(usbinfo->product_str, "TI-84 Plus T"))
		{
			info->variant = CABLE_VARIANT_TI84PT;
		}
		else if (!strcmp(usbinfo->product_str, "TI-82 Advanced Edition Python"))
		{
			info->variant = CABLE_VARIANT_TI82AEP;
		}
		else if (usbinfo->version <= 0x0110)
		{
			info->variant = CABLE_VARIANT_TI84PSE;
		}
		else if (usbinfo->version == 0x0120)
		{
			info->variant = CABLE_VARIANT_TI84PCSE;
		}
		else if (usbinfo->version == 0x0220)
		{
			info->variant = CABLE_VARIANT_TI84PCE;
		}
		else if (usbinfo->version == 0x0260)
		{
			info->variant = CABLE_VARIANT_TI83PCE;
		}
		else if (usbinfo->version == 0x0660)
		{
			info->variant = CABLE_VARIANT_TI82AEP;
		}
		else
		{
			info->variant = CABLE_VARIANT_UNKNOWN;
		}
	}
	else if (usbinfo->pid == PID_TI89TM)
	{
		info->family = CABLE_FAMILY_USB_TI9X;
		info->variant = CABLE_VARIANT_TI89TM;
	}
	else if (usbinfo->pid == PID_NSPIRE)
	{
		info->family = CABLE_FAMILY_USB_NSPIRE;
		if (!strcmp(usbinfo->product_str, "TI-Nspire(tm) Handheld"))
		{
			info->variant = CABLE_VARIANT_NSPIRE;
		}
		else if (!strcmp(usbinfo->product_str, "TI-Nspire(tm) CAS Handheld"))
		{
			info->variant = CABLE_VARIANT_NSPIRE_CAS;
		}
		else
		{
			info->variant = CABLE_VARIANT_UNKNOWN;
		}
	}
	else if (usbinfo->pid == PID_NSPIRE_CRADLE)
	{
		info->family = CABLE_FAMILY_USB_NSPIRE_CRADLE;
		info->variant = CABLE_VARIANT_NSPIRE_CRADLE;
	}
	else if (usbinfo->pid == PID_NSPIRE_CXII)
	{
		info->family = CABLE_FAMILY_USB_NSPIRE_CXII;
		info->variant = CABLE_VARIANT_NSPIRE_CXII;
	}
	else
	{
		ticables_critical("unknown PID %x", usbinfo->pid);
		info->family = CABLE_FAMILY_UNKNOWN;
		info->variant = CABLE_VARIANT_UNKNOWN;
	}
}

/**
 * ticables_get_usb_device_info:
 * @array: address of a newly allocated array of CableDeviceInfo structures
 * @length: number of detected USB devices.
 *
 * Returns the list of detected USB devices. Note that list is in the
 * same order as PORT#x.
 * The array must be freed when no longer used.
 *
 * Return value: 0 if successful, an error code otherwise.
 **/
int TICALL ticables_get_usb_device_info(CableDeviceInfo **list, int *len)
{
	if (list != NULL)
	{
		int ret = 0;
		const USBCableInfo *usbinfo = NULL;
		int i, n = 0;

#if defined(__WIN32__) || (defined(HAVE_LIBUSB) || defined(HAVE_LIBUSB_1_0))
		ret = usb_probe_device_info(&usbinfo, &n);
#endif
		*list = (CableDeviceInfo *)calloc(1 + n, sizeof(CableDeviceInfo));
		for (i = 0; i < n; i++)
		{
			translate_usb_device_info(&(*list)[i], &usbinfo[i]);
		}
		if (len)
		{
			*len = i;
		}

		return ret;
	}
	else
	{
		ticables_critical("%s: list is NULL", __FUNCTION__);
		return -1;
	}
}

/**
 * \brief Frees an array of USB device information
 * \param array the array previously allocated by ticables_get_usb_device_info()
 * \return Always 0
 */
int TICALL ticables_free_usb_device_info(CableDeviceInfo *list)
{
	free(list);
	return 0;
}
