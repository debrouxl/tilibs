/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#include "defsxx.h"
#include "calc_ext.h"
#include "calc_err.h"
#include "calc_int.h"

#include "defs83p.h"
#include "defs92p.h"
#include "defs89.h"

/* 
   PC: 08 6D 00 00		PC request a screen dump
   TI: 98 56 00 00		TI reply OK 
*/

static int tiXX_isOK(byte *d);

/* 
   This function try to detect the calculator type. It supposes that the
   communication port is correctly initialized and your calc is on
*/
/*
  Beware: the call sequence is very important: 89, 92+, 92, 86, 85, 83, 82 !!!
*/
TIEXPORT int TICALL detect_calc(int *calc_type)
{
  int err;
  byte data;
  //int old_timeout;
  
  //old_timeout=ticable_get_timeout();
  ////ticable_set_timeout(5); // 0.5 seconds
  DISPLAY("Probing calculator...\n");

  /* Test for a TI 89 or a TI92+ */
  DISPLAY("Trying TI89/TI92+... ");
  TRY(cable->open());
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  err=tiXX_isOK(&data);
  DISPLAY("<%02X/%02X> ", PC_TI89, data);
  TRY(cable->close());
  if( !err && (data == TI89_PC) )
    {
      DISPLAY("OK (TI89) !\n");
      *calc_type=CALC_TI89;
      //ticable_set_timeout(old_timeout);
      return 0;
    }
  else
    if( !err && (data == TI92p_PC) )
      {
	DISPLAY("OK (TI92+) !\n");
	*calc_type=CALC_TI92P;
	//ticable_set_timeout(old_timeout);
	return 0;
      }
    else
      {
	DISPLAY("NOK.\n");
      }
  
  /* Test for a TI92 */
  DISPLAY("Trying TI92... ");
  TRY(cable->open());
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  err=tiXX_isOK(&data);
  DISPLAY("<%02X/%02X> ", PC_TI92, data);
  TRY(cable->close())
  if( !err && (data == TI92_PC) )
    {
      DISPLAY("OK !\n");
      *calc_type=CALC_TI92;
      //ticable_set_timeout(old_timeout);
      return 0;
    }
  else
    {
      DISPLAY("NOK.\n");
    }
  
  /* Test for a TI86 before a TI85 */
  DISPLAY("Trying TI86... ");
  TRY(cable->open());
  TRY(cable->put(PC_TI86));
  TRY(cable->put(CMD86_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  err=tiXX_isOK(&data);
  DISPLAY("<%02X/%02X> ", PC_TI86, data);
  TRY(cable->close());
  if( !err && (data == TI86_PC) )
    {
      DISPLAY("OK !\n");
      *calc_type=CALC_TI86;
      //ticable_set_timeout(old_timeout);
      return 0;
    }
  else
    {
      DISPLAY("NOK.\n");
    }
  
  /* Test for a TI85 */
  DISPLAY("Trying TI85... ");
  TRY(cable->open());
  TRY(cable->put(PC_TI85));
  TRY(cable->put(CMD85_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  err=tiXX_isOK(&data);
  DISPLAY("<%02X/%02X> ", PC_TI85, data);
  TRY(cable->close());
  if( !err && (data == TI85_PC) )
    {
      DISPLAY("OK !\n");
      *calc_type=CALC_TI85;
      //ticable_set_timeout(old_timeout);
      return 0;
    }
  else
    {
      DISPLAY("NOK.\n");
    }
  
  /* Test for a TI83 before a TI82 */
  DISPLAY("Trying TI83... ");
  TRY(cable->open());
  TRY(cable->put(PC_TI83));
  TRY(cable->put(CMD83_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  err=tiXX_isOK(&data);
  DISPLAY("<%02X/%02X> ", PC_TI82, data);
  TRY(cable->close());
  if( !err && (data == TI83_PC) )
    {
      DISPLAY("OK !\n");
      *calc_type=CALC_TI83;
      //ticable_set_timeout(old_timeout);
      return 0;
    }
  else
    {
      DISPLAY("NOK.\n");
    }
  
  /* Test for a TI82 */
  DISPLAY("Trying TI82... ");
  TRY(cable->open());
  TRY(cable->put(PC_TI82));
  TRY(cable->put(CMD82_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  err=tiXX_isOK(&data);
  DISPLAY("<%02X> ", data);
  TRY(cable->close());
  if( !err && (data == TI82_PC) )
    {
      DISPLAY("OK !\n");
      *calc_type=CALC_TI82;
      //ticable_set_timeout(old_timeout);
      return 0;
    }
  else
    {
      DISPLAY("NOK.\n");
    }
  /* Next calc */
  
  //ticable_set_timeout(old_timeout);
  return 0;
}

/* 
   Retrieve the first byte replied by the calc 
*/
static int tiXX_isOK(byte *d)
{
  byte data;
  //int err;

  TRY(cable->get(&data));
  *d=data;
  TRY(cable->get(&data));
  if(data != 0x56)
    {
      return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if(data != 0x00)
    {
      return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if(data != 0x00)
    {
      return ERR_NOT_READY;
    }
  //DISPLAY("The calculator reply OK.\n");

  return 0;
}

int ticalc_get_calc(void); // defined in intrface.c

/*
  Check if the calculator is ready and detect the type.
  Works only with TI89/92/92+ calculators.
  Practically, call this function first, and call tixx_isready next.
  Return 0 if successful, 0 otherwise
*/
/*
  Thanks to Sebastian Reichelt who suggested me this idea.
  Explanation of the method:
  - send a TI92_isOK: 09 68 00 00
  - received answer: 89 56 00 xx for a TI92, 86 56 xx xx for another calc
  - send a TI89_isOK: 08 68 00 00
  - received answer: 98 56 xx xx for a TI89, 88 56 xx xx for a TI92+
*/
TIEXPORT int TICALL ti89_92_92p_isready(int *calc_type)
{
  byte data;
  int ct;

  ct = ticalc_get_calc();

  if( (ct != CALC_TI89) && (ct != CALC_TI92) && (ct != CALC_TI92P) )
    return 0;
  
  TRY(cable->open());
  // Suppose that calc is a TI92
  DISPLAY("Is calculator ready (and check type) ?\n");
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_ISREADY));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  TRY(cable->get(&data));
  //DISPLAY("isOK_1: 0x%02X\n", data);
  if(data != TI92_PC)
      return ERR_NOT_REPLY;
  TRY(cable->get(&data));
  //DISPLAY("isOK_2: 0x%02X\n", data);
  if(data != CMD92_TI_OK)
    {
      if(data==CMD92_CHK_ERROR) return ERR_CHECKSUM;
      else return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  //DISPLAY("isOK_3: 0x%02X\n", data);
  if(data != 0x00)
    {
      DISPLAY("The calculator is ready but not a TI92.\n");
      DISPLAY("Check again...\n");
      TRY(cable->get(&data));
      //DISPLAY("isOK_4: 0x%02X\n", data);
      // The calc is not a TI92, is it a 89 or a 92+ ?
      // Repeat procedure
      TRY(cable->put(PC_TI89));
      TRY(cable->put(CMD89_ISREADY));
      TRY(cable->put(0x00));
      TRY(cable->put(0x00));
      TRY(cable->get(&data));
      //DISPLAY("isOK_1: 0x%02X\n", data);
      switch(data)
	{
	case TI89_PC:
	  *calc_type = CALC_TI89;
	  break;
	case TI92p_PC:
	  *calc_type = CALC_TI92P;
	  break;
	default:
	  return ERR_NOT_REPLY;
	  break;
	}
      TRY(cable->get(&data));
      //DISPLAY("isOK_2: 0x%02X\n", data);
      if(data != CMD92_TI_OK)
	{
	  if(data==CMD92_CHK_ERROR) 
	    return ERR_CHECKSUM;
	  else return ERR_NOT_REPLY;
	}
      TRY(cable->get(&data));
      //DISPLAY("isOK_3: 0x%02X\n", data);
      TRY(cable->get(&data));
      //DISPLAY("isOK_4: 0x%02X\n", data);
      if((data&1) != 0)
	return ERR_NOT_REPLY;

      DISPLAY("The calculator is ready.\n");
      DISPLAY("Calculator type: %s\n", (*calc_type==CALC_TI89)?"TI89":"TI92+");
      TRY(cable->close());
      
    }
  else
    {
      TRY(cable->get(&data));
      //DISPLAY("isOK_4: 0x%02X\n", data);
      if((data&1) != 0)
	return ERR_NOT_REPLY;
      *calc_type = CALC_TI92;
      DISPLAY("The calculator is ready.\n");
      DISPLAY("Calculator type: TI92\n");
      TRY(cable->close());
      
      return 0;
    }

  return 0;
}


/*
  Check if the calculator is ready and detect the type.
  Works only with TI83+/89/92+ calculators.
  Practically, call this function first, and call tixx_isready next.
  Return 0 if successful, 0 otherwise
*/
TIEXPORT int TICALL ticalc_83p_89_92p_isready(int *calc_type)
{
  byte data;
  int ct;

  ct = ticalc_get_calc();
  if( (ct != CALC_TI89) && (ct != CALC_TI92P) && (ct != CALC_TI83P) )
    return 0;
  
  TRY(cable->open());
  DISPLAY("Is calculator ready (and check type) ?\n");
  TRY(cable->put(0x00));
  TRY(cable->put(CMD89_ISREADY));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));

  TRY(cable->get(&data));		// 0x98: TI89, 0x88: TI92+, 0x73: TI83+
  //DISPLAY("isOK_1: 0x%02X\n", data);
  switch(data)
  {
  case TI89_PC: *calc_type = CALC_TI89; break;
  case TI92p_PC: *calc_type = CALC_TI92P; break;
  case TI83p_PC: *calc_type = CALC_TI83P; break;
  default: *calc_type = CALC_NONE; return ERR_INVALID_BYTE; break;
  }
  TRY(cable->get(&data)); 
  if(data != CMD89_TI_OK) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  if((data&1) != 0)
      return ERR_NOT_READY;

  DISPLAY("The calculator is ready.\n");
  DISPLAY("Calculator type: %s\n", (*calc_type==CALC_TI83P)?"TI83+":(*calc_type==CALC_TI89)?"TI89":"TI92+");

  return 0;
}
