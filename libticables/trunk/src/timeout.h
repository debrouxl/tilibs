/*  libticables - link cable library, a part of the TiLP project
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
  - toSTART: init variable
  - toELAPSED: return TRUE if max tenth of seconds have elapsed
  - toCURRENT: return the number of elapsed seconds
*/

#if defined(__WIN32__)
# include <windows.h>
  typedef DWORD                         TIME;
# define  toSTART(ref)          { (ref)=GetTickCount(); }
# define  toELAPSED(ref, max)   ( (int)(GetTickCount()-(ref)) > (100*max) )
# define  toCURRENT(ref)        ( (float)(GetTickCount()-(ref)) / 1000 )

#elif defined(__LINUX__) || defined(__MACOSX__)
# include <time.h>
  typedef clock_t			TIME;
# define  toSTART(ref)		{ (ref)=clock(); }
# define  toELAPSED(ref, max)	((clock()-(ref)) > (max/10.0*CLOCKS_PER_SEC))
# define  toCURRENT(ref)        ( (float)(clock()-(ref))/CLOCKS_PER_SEC )
#endif

#endif

