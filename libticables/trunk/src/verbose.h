/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#ifndef __VERBOSE1_H__
#define __VERBOSE1_H__

#include <stdio.h>
#include "export.h"

#define DSP_OFF   0
#define DSP_ON    1
#define DSP_CLOSE 2

DLLEXPORT int DLLEXPORT2 DISPLAY(const char *format, ...);
DLLEXPORT int DLLEXPORT2 ticable_DISPLAY_settings(int op);

#endif
