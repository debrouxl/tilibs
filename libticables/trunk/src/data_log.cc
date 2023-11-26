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

/* 
	This unit allows to trace bytes which are transferred between PC
	and TI calculator.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <stdint.h>
#include "gettext.h"
#include "logging.h"
#include "data_log.h"

#include "log_hex.h"
#include "log_dbus.h"
#include "log_dusb.h"
#include "log_nsp.h"

int log_start(CableHandle *h)
{
	int ret;

	gchar* tmp = g_strconcat(g_get_home_dir(), G_DIR_SEPARATOR_S, LOG_DIR, NULL);
	if (!g_mkdir_with_parents(tmp, 0750))
	{
		ret = log_hex_start();
		if (!ret)
		{
			if (h->model == CABLE_USB)
			{
				ret = log_dusb_start();
				if (!ret)
				{
					ret = log_nsp_start();
				}
			}
			else
			{
				ret = log_dbus_start();
			}
		}
	}
	else
	{
		ticables_critical("Failed to create folder for logs");
		ret = 1;
	}
	g_free(tmp);

	return ret;
}

int log_N(CableHandle *h, int dir, const uint8_t *data, uint32_t len)
{
	log_hex_N(dir, data, len);

	if (h->model == CABLE_USB)
	{
		log_dusb_N(dir, data, len);
		log_nsp_N(dir, data, len);
	}
	else
	{
		log_dbus_N(dir, data, len);
	}

	return 0;
}

int log_stop(CableHandle *h)
{
	log_hex_stop();

	if (h->model == CABLE_USB)
	{
		log_dusb_stop();
		log_nsp_stop();
	}
	else
	{
		log_dbus_stop();
	}

	return 0;
}
