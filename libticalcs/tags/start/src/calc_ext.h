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

#ifndef __CALC_EXTERN__
#define __CALC_EXTERN__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(HAVE_TILP_MACROS_H) && !defined(__MACOSX__)
#include <tilp/cabl_int.h>
#include <tilp/intl.h>
#elif defined(HAVE_TILP_MACROS_H) && defined(__MACOSX__)
#include <libticables/cabl_int.h>
#include <libticables/intl.h>
#else
#include "cabl_int.h"
#include "intl2.h"
#endif
#include "calc_def.h"

/*********************************************************/
/* Global variables: must be defined in the main program */
/*********************************************************/

//extern struct ti_calc      ti_calc;
extern struct ticalc_info_update *update; // Internally used
extern struct ticable_link  *cable; // Internally used

#endif




