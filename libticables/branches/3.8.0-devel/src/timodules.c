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
#include <fcntl.h>
#if defined(__LINUX__) || defined(__BSD__) || defined(__MACOSX__)
# include <unistd.h>		// access
#else
# include <io.h>
#endif
#include <errno.h>
#include "intl.h"

/* Devices nodes for tipar/tiser/tiusb kernel modules */
/* Implemented for compatibility and automatic node use */
const char *tipar_nodes[][32] = {
  {"/dev/tiP0", "/dev/tipar0", "/dev/ticables/par/0", NULL},
  {"/dev/tiP1", "/dev/tipar1", "/dev/ticables/par/1", NULL},
  {"/dev/tiP2", "/dev/tipar2", "/dev/ticables/par/2", NULL},
  {0}
};

const char *tiser_nodes[][32] = {
  {"/dev/tiS0", "/dev/tiser0", "/dev/ticables/ser/0", NULL},
  {"/dev/tiS1", "/dev/tiser1", "/dev/ticables/ser/1", NULL},
  {"/dev/tiS2", "/dev/tiser2", "/dev/ticables/ser/2", NULL},
  {"/dev/tiS3", "/dev/tiser3", "/dev/ticables/ser/3", NULL},
  {0}
};

const char *tiusb_nodes[][32] = {
  {"/dev/tigl0", "/dev/tiusb0", "/dev/ticables/usb/0", NULL},
  {"/dev/tigl1", "/dev/tiusb1", "/dev/ticables/usb/1", NULL},
  {"/dev/tigl2", "/dev/tiusb2", "/dev/ticables/usb/2", NULL},
  {"/dev/tigl3", "/dev/tiusb3", "/dev/ticables/usb/3", NULL},
  {0}
};

#ifdef __WIN32__
# define F_OK 0
#endif

const char *search_for_tiusb_node(int minor)
{
  int i;
  const char *node;
  int ret = 0;

  for (i = 0; tiusb_nodes[minor][i] != NULL; i++) {
    node = tiusb_nodes[minor][i];
    ret = access(node, F_OK);
    //printf("nodes = <%s> %i %i\n", node, ret, errno);
    if (ret != -1)
      return node;
  }

  return _("not found");
}

const char *search_for_tipar_node(int minor)
{
  int i;
  const char *node;
  int ret = 0;

  for (i = 0; tipar_nodes[minor][i] != NULL; i++) {
    node = tipar_nodes[minor][i];
    ret = access(node, F_OK);
    //printf("nodes = <%s> %i %i\n", node, ret, errno);
    if (ret != -1)
      return node;
  }

  return _("not found");
}

const char *search_for_tiser_node(int minor)
{
  int i;
  const char *node;
  int ret = 0;

  for (i = 0; tiser_nodes[minor][i] != NULL; i++) {
    node = tiser_nodes[minor][i];
    ret = access(node, F_OK);
    //printf("nodes = <%s> %i %i\n", node, ret, errno);
    if (ret != -1)
      return node;
  }

  return _("not found");
}
