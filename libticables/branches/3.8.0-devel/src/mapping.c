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

/* Resources mapping wrapper */

/*
  This unit compiles cable type and resources to determine the best way to use
  the link cable:
  - I/O method
  - cable access.  
  It also permforms link cable (un)registering.
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#if defined(__LINUX__)
#include "linux_mapping.h"
#elif defined(__MACOSX__)
#include "macos_mapping.h"
#elif defined(__BSD__)
#include "bsd_mapping.h"
#elif defined(__WIN32__)
#include "win32_mapping.h"
#else
#include "none.h"
#endif

/*
	This function attempts to determine the best way to use a given link
	cable by providing one or more I/O methods from detected resources.
*/
TicableMethod mapping_get_methods(TicableType type, int resources)
{
	int ret;
	
#if defined(__LINUX__)
	ret = linux_get_methods(type, resources);
#elif defined(__BSD__)
	ret = bsd_get_methods(type, resources);
#elif defined(__WIN32__)
	ret = win32_get_methods(type, resources);
#elif defined(__MACOSX__)
	ret = macos_get_methods(type, resources);
#else
	ret = 0;
#endif

  	return ret;
}


int mapping_register_cable(TicableType type, LinkCable *lc)
{
	int ret;
	
#if defined(__LINUX__)
	ret = linux_register_cable(type, lc);
#elif defined(__BSD__)
	ret = bsd_register_cable(type, lc);
#elif defined(__WIN32__)
	ret = win32_register_cable(type, lc);
#elif defined(__MACOSX__)
	ret = macos_register_cable(type, lc);
#else
	ret = 0;
#endif
	
  	return ret;
}


void mapping_unregister_cable(LinkCable *lc)
{
	dfl_register_cable(lc);	
}