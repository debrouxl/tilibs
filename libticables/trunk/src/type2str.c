/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
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

#include <stdio.h>
#include <string.h>

#include "intl.h"
#include "cabl_def.h"
#include "verbose.h"

TIEXPORT const char *TICALL ticable_cabletype_to_string(TicableType type)
{
  switch (type) {
  case LINK_NONE:
    return "none";
  case LINK_TGL:
    return "Gray TIGraphLink";
  case LINK_SER:
    return "Black TIGraphLink";
  case LINK_PAR:
    return "home-made parallel";
  case LINK_AVR:
    return "AVRlink";
  case LINK_VTL:
    return "unused";
  case LINK_TIE:
    return "TiEmu";
  case LINK_VTI:
    return "VTi";
  case LINK_TPU:
    return "unused";
  case LINK_SLV:
    return "SilverLink";
  default:
    DISPLAY_ERROR(_("libticables error: unknown cable type !\n"));
    return "unknown";
  }
}


TIEXPORT const char *TICALL ticable_baudrate_to_string(TicableBaudRate br)
{
  switch (br) {
  case BR9600:
    return "9600 bauds";
  case BR19200:
    return "19200 bauds";
  case BR38400:
    return "38400 bauds";
  case BR57600:
    return "57600 bauds";
  default:
    DISPLAY_ERROR(_("libticables error: unknown baud rate !\n"));
    return "unknown";
  }
}


TIEXPORT const char *TICALL ticable_hfc_to_string(TicableHfc hfc)
{
  switch (hfc) {
  case HFC_OFF:
    return "off";
  case HFC_ON:
    return "on";
  default:
    DISPLAY_ERROR(_("libticables error: unknown flow type !\n"));
    return "unknown";
  }
}


TIEXPORT const char *TICALL ticable_port_to_string(TicablePort port)
{
  switch (port) {
  case USER_PORT:
    return "user defined";
  case PARALLEL_PORT_1:
    return "parallel port #1";
  case PARALLEL_PORT_2:
    return "parallel port #2";
  case PARALLEL_PORT_3:
    return "parallel port #3";
  case SERIAL_PORT_1:
    return "serial port #1";
  case SERIAL_PORT_2:
    return "serial port #2";
  case SERIAL_PORT_3:
    return "serial port #3";
  case SERIAL_PORT_4:
    return "serial port #4";
  case VIRTUAL_PORT_1:
    return "virtual port #1";
  case VIRTUAL_PORT_2:
    return "virtual port #2";
  case USB_PORT_1:
    return "USB port #1";
  case USB_PORT_2:
    return "USB port #2";
  case USB_PORT_3:
    return "USB port #3";
  case USB_PORT_4:
    return "USB port #4";
  case OSX_SERIAL_PORT:
    return "serial port";
  case OSX_USB_PORT:
    return "USB port";
  default:
    DISPLAY_ERROR(_("libticables error: unknown port !\n"));
    return "unknown";
  }
}


TIEXPORT const char *TICALL ticable_method_to_string(TicableMethod method)
{
  char *p1 = "";
  char *p2 = "internal";
  char *p3 = "";
  static char buffer[64];

  strcpy(buffer, "");

  if (method & IOM_AUTO)
    p1 = "automatic";
  if (method & IOM_ASM)
    p2 = "direct access with ASM";
  if (method & IOM_API)
    p2 = "direct access thru API";
#if defined(__LINUX__)
  if (method & IOM_DRV)
    p2 = "kernel module";
#elif defined(__WIN32__)
  if (method & IOM_DRV)
    p2 = "PortTalk/TiglUsb driver";
#endif

  strcat(buffer, p1);
  strcat(buffer, " (");
  strcat(buffer, p2);
  if (strcmp(p2, "") && strcmp(p3, ""))
    strcat(buffer, " + ");
  strcat(buffer, p3);
  strcat(buffer, ")");

  return buffer;
}
