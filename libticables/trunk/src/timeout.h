/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticables2 - link cable library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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

#ifndef __CABLE_TIMEOUT__
#define __CABLE_TIMEOUT__

/*
  Platform independant time measurement (in milli-seconds) 
  and timeout management (in tenth of seconds)
  - TO_START:   retrieve time (starting point)
  - TO_CURRENT: return the number of milli-seconds elapsed since TO_START()
  - TO_ELAPSED: return TRUE if max tenth of seconds have elapsed
*/

// I don't want to include windows.h here.
//# include <windows.h>
//typedef DWORD tiTIME;
typedef unsigned long tiTIME;

#if defined(__WIN32__) && !defined(__MINGW32__)

# define  TO_START(ref)          { (ref) = GetTickCount(); }
# define  TO_CURRENT(ref)        ( GetTickCount() - (ref) )
# define  TO_ELAPSED(ref, max)   ( TO_CURRENT(ref) > (100UL*(max)) )

#else

# include <time.h>
# define  TO_START(ref)         { (ref) = ((1000*clock()) / CLOCKS_PER_SEC); }
# define  TO_CURRENT(ref)       ( (1000*clock()) / CLOCKS_PER_SEC - (ref) )
# define  TO_ELAPSED(ref, max)  ( TO_CURRENT(ref) > (100UL*(max)) )

#endif

#endif
