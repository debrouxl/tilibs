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

#ifndef __TIMEOUT_H__
#define __TIMEOUT_H__

#if defined(__LINUX__)
# include <time.h>
  typedef clock_t			TIME;
# define  tSTART(ref)		{ (ref)=clock(); }
# define  tELAPSED(ref, max)	((clock()-(ref)) > (max/10.0*CLOCKS_PER_SEC))

#elif defined(__WIN32__)
# include <windows.h>
  typedef DWORD				TIME;
# define  tSTART(ref)		{ (ref)=GetTickCount(); }
# define  tELAPSED(ref, max)	( (int)(GetTickCount()-(ref)) > (100*max) )

#else
# include <time.h>
typedef clock_t                       TIME;
# define  tSTART(ref)           { (ref)=clock(); }
# define  tELAPSED(ref, max)    ((clock()-(ref)) > (max/10.0*CLOCKS_PER_SEC))

#endif

#endif

