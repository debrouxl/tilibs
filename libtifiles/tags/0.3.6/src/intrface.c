/*  libtifiles - TI File Format library
 *  Copyright (C) 2002  Romain Lievin
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
  This unit contains the interface of the libtifiles library.
*/

#include <stdio.h>
#include <stdlib.h>

#include "export.h"
#include "file_ver.h"
#include "file_int.h"
#include "file_def.h"
#include "typesxx.h"
#include "trans.h"
#include "grp_ops.h"

/*****************/
/* Internal data */
/*****************/
int tifiles_instance = 0;          // counts # of instances
int tifiles_calc_type = CALC_NONE; // current calc type (context)

TIFILES_PRINTF tifiles_printf = printf;

/****************/
/* Entry points */
/****************/

/*
  This function should be the first one to call.
  It tries to list available I/O functions (I/O resources).
 */
TIEXPORT int TICALL tifiles_init()
{
  if(tifiles_instance == 0)
    {
      fprintf(stdout, "Libtifiles: version %s\n",
	      LIBTIFILES_VERSION);
    }

  return (++tifiles_instance);
}

/*
  This function should be called when the libticables library is
  no longer used.
 */
TIEXPORT int TICALL tifiles_exit()
{
  return (--tifiles_instance);
}


/***********/
/* Methods */
/***********/

TIFILES_MSGBOX tifiles_msgbox = NULL;
TIFILES_CHOOSE tifiles_choose = NULL;

TIEXPORT const char* TICALL tifiles_get_version()
{
  return LIBTIFILES_VERSION;
}

static void print_informations();

TIEXPORT
void TICALL tifiles_set_calc(int type)
{
  tifiles_calc_type = type;

  print_informations();

  switch(type)
    {
    case CALC_TI73:
      break;
    case CALC_TI82:
      break;
    case CALC_TI83:
      break;
    case CALC_TI83P:
      break;
    case CALC_TI85:
      break;
    case CALC_TI86:
      break;
    case CALC_TI89:
      break;
    case CALC_TI92:
      break;
    case CALC_TI92P:
      break;
    case CALC_V200:
      break;
    default:
      fprintf(stderr, "Function not implemented. This is a bug. Please report it.");
      fprintf(stderr, "Informations:\n");
      fprintf(stderr, "Calc: %i\n", type);
      fprintf(stderr, "Program halted before crashing...\n");
      abort();
      break;
    }
}

TIEXPORT
int TICALL tifiles_get_calc(void)
{
  return tifiles_calc_type;
}

static void print_informations(void)
{
  fprintf(stdout, "Libtifiles settings...\n");

  switch(tifiles_calc_type)
    {
    case CALC_V200:
      fprintf(stdout, "  Calc type: V200\n");
      break;
    case CALC_TI92P:
      fprintf(stdout, "  Calc type: TI92+\n");
      break;
    case CALC_TI92:
      fprintf(stdout, "  Calc type: TI92\n");
      break;
    case CALC_TI89:
      fprintf(stdout, "  Calc type: TI89\n");
      break;
    case CALC_TI86:
      fprintf(stdout, "  Calc type: TI86\n");
      break;
    case CALC_TI85:
      fprintf(stdout, "  Calc type: TI85\n");
      break;
    case CALC_TI83P:
      fprintf(stdout, "  Calc type: TI83+\n");
      break;
    case CALC_TI83:
      fprintf(stdout, "  Calc type: TI83\n");
      break;
    case CALC_TI82:
      fprintf(stdout, "  Calc type: TI82\n");
      break;
    case CALC_TI73:
      fprintf(stdout, "  Calc type: TI73\n");
      break;
    default: // error !
      fprintf(stdout, "Oops, there is a bug. Unknown calculator.\n");
      break;
    }
}


TIEXPORT TIFILES_PRINTF tifiles_set_printf(TIFILES_PRINTF new_printf)
{
	TIFILES_PRINTF old_printf = tifiles_printf;

	fprintf(stderr, "printf = %p\n", printf);
	fprintf(stderr, "old_printf = %p\n", old_printf);
	fprintf(stderr, "new_printf = %p\n", new_printf);

	tifiles_printf = new_printf;

	return old_printf;
}
