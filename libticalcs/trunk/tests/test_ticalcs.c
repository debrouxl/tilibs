/* $Id$ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
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

/*
 * This program shows how to use the libticalcs library. You can consider this
 * as an authoritative example. 
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WIN32__
#include <windows.h>
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../src/ticalcs.h"
#include "../src/calc_def.h"

#undef VERSION
#define VERSION "Test program"

/* Cable & Calc structures */
TicableLinkParam lp;
TicableLinkCable lc;
TicalcFncts      tc; // Functions which drive a calculator
TicalcInfoUpdate iu; // Functions to do the refresh of progress bar & label

/* Update functions */
void ilp_start(void) { }
void ilp_stop(void) { }
void ilp_refresh(void) { }
void ilp_pbar(void) { }
void ilp_label(void) { }

/* Error function */
void print_lc_error(int errnum)
{
  char msg[256] = "No error -> bug !\n";

  ticable_get_error(errnum, msg);
  fprintf(stderr, "Link cable error: %i: %s\n", errnum, msg);
}

int main(int argc, char **argv)
{
  int err;

  ticable_init();
  ticalc_init();

  /* 
     Initialize the libTIcable library 
  */
  ticable_get_default_param(&lp);
  lp.delay   = DFLT_DELAY;
  lp.timeout = DFLT_TIMEOUT;
  lp.port    = SERIAL_PORT_2;
  lp.method  = IOM_AUTO;
  ticable_set_param(&lp);
  if((err=ticable_set_cable(LINK_SER, &lc))) 
  {
		print_lc_error(err);
                return -1;
  }

  /* 
     Initialize the libTIcalc library 
  */
  ticalc_set_update(&iu, ilp_start, ilp_stop, ilp_refresh, 
		    ilp_pbar, ilp_label);

  ticalc_set_cable(&lc);
  ticalc_set_calc(CALC_TI89, &tc);
  
  // Init port
  fprintf(stdout, "Init calc\n");
  if( (err=lc.init()) )
    {
      print_lc_error(err);
      return -1;
    }
  if( (err=lc.open()) )
    {
      print_lc_error(err);
      return -1;
    }

  DISPLAY("Wait 1 second...\n");
#if defined(__WIN32__)
	Sleep(1000);
#else
	sleep(1);
#endif

  // Check ready
#if 1
  fprintf(stdout, "Test if calc is ready... ");
  err = tc.isready();
  fprintf(stdout, "%s (%i)\n", err ? "KO" : "OK", err);

  //fprintf(stdout, "Test screendump...\n");
  //err = tc.screendump(&bitmap, FULL_SCREEN, &sc);
#else
  // Check non-silent recv var
  {
      char fn[1024];
      TiVarEntry ve;
      
      err = tc.recv_var_2(fn, 0, &ve);
      fprintf(stdout, "<%s> %i\n", fn, err);
  }  
#endif
  fprintf(stdout, "\n");

  // Close port
  if( (err=lc.close()) ) print_lc_error(err);
  if( (err=lc.exit()) ) print_lc_error(err);

  return 0;
}
