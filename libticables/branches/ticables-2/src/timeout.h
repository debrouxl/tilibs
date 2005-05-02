/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables - Ti Link Cable library, a part of the TiLP project
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
  Some explanations about this unit: I use different functions for measuring
  time depending on the platform type.
  In fact, it appears that the Win32 clock() function of <time.h> is 600 times 
  slower than the Win32 GetTickCount() function of <windows.h>. 
  As a consequency of this fact, the speed rate was divided by 4.

  A great thanks to Timothy Singer <tsinger@uoregon.gladstone.edu>, the
  FastLink's developer. He sent to me her routines so that I can compare with
  mine. This allowed me to find why my routines was so slow !
*/

#ifndef __TICABLE_TIMEOUT__
#define __TICABLE_TIMEOUT__

/*
  Platform independant time measurement & timeout management
  - TO_START: init variable
  - TO_ELAPSED: return TRUE if max tenth of seconds have elapsed
  - TO_CURRENT: return the number of elapsed seconds
*/

#if defined(__WIN32__) && !defined(__MINGW32__)
//# include <windows.h>
//typedef DWORD tiTIME;
// I don't want to include windows.h here:
typedef unsigned long tiTIME;
# define  TO_START(ref)          { (ref)=GetTickCount(); }
# define  TO_ELAPSED(ref, max)   ( (int)(GetTickCount()-(ref)) > (100*max) )
# define  TO_CURRENT(ref)        ( (float)(GetTickCount()-(ref)) / 1000 )

#else
# include <time.h>
typedef clock_t tiTIME;
# define  TO_START(ref)	       { (ref)=clock(); }
# define  TO_ELAPSED(ref, max)  ( (clock()-(ref)) > ((max)/10.0*CLOCKS_PER_SEC))
# define  TO_CURRENT(ref)       ( (float)(clock()-(ref))/CLOCKS_PER_SEC )
#endif

#endif
