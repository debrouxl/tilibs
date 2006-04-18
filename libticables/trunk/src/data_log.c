/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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

/* 
	This unit allows to trace bytes which are transferred between PC
	and TI calculator.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "stdints.h"
#include "gettext.h"
#include "logging.h"
#include "data_log.h"

#include "log_hex.h"
#include "log_dbus.h"
#include "log_dusb.h"

int log_start(void)
{
	//ticables_info(_("Logging started."));
	log_hex_start();
	log_dusb_start();
	log_dbus_start();

  	return 0;
}

int log_1(int dir, uint8_t data)
{
	printf("%02x-", data);
	log_hex_1(dir, data);
	log_dusb_1(dir, data);
	log_dbus_1(dir, data);

  	return 0;
}

int log_N(int dir, uint8_t *data, int len)
{
	int i;

	for(i = 0; i < len; i++)
	{
		
		log_hex_1(dir, data[i]);
		log_dusb_1(dir, data[i]);
		log_dbus_1(dir, data[i]);
	}
  	
  	return 0;
}

int log_stop(void)
{
	log_hex_stop();
	log_dusb_stop();
	log_dbus_stop();
	//ticables_info(_("Logging stopped."));

  	return 0;
}
