/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Li�vin
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

#include "ticalcs.h"

static void ticalcs_default_update_start(void)	{};
static void ticalcs_default_update_stop(void)	{};
static void ticalcs_default_update_refresh(void){};
static void ticalcs_default_update_pbar(void)	{};
static void ticalcs_default_update_label(void)	{};

CalcUpdate default_update =
{
	"", 0,
	0.0, 0, 0, 0, 0, 0, 0, (1 << 0), 0,
	ticalcs_default_update_start,
	ticalcs_default_update_stop,
	ticalcs_default_update_refresh,
	ticalcs_default_update_pbar,
	ticalcs_default_update_label,
};
