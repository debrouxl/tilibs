/* Hey EMACS -*- linux-c -*- */
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

#ifndef __CALCS_UPDATE__
#define __CALCS_UPDATE__

/*
 * Implement a kind of mutex: prevent that someone tries to initiate
 * a transfer while another is in progress
 */
extern int lock;
#define UNLOCK_TRANSFER() lock = 0;

#ifndef TRYF
# define TRYF(x) { int aaaa_; if((aaaa_ = (x))) { UNLOCK_TRANSFER() return aaaa_; } }	//new !
#endif

#define LOCK_TRANSFER()   { TRYF(lock);  lock = ERR_PENDING_TRANSFER; }

/* 
   Macros: they check that function pointer is good and then
   call the pointer itself.
   Default behaviour: check is not done !
*/
#ifdef CHECK_UPDATE
# define update_start()      if(update && update->start)   update->start()
# define update_pbar()       if(update && update->pbar)    update->pbar()
# define update_label()      if(update && update->label)   update->label()
# define update_refresh()    if(update && update->refresh) update->refresh()
# define update_stop()       if(update && update->stop)    update->stop()
#else
# define update_start()      update->start()
# define update_pbar()       update->pbar()
# define update_label()      update->label()
# define update_refresh()    update->refresh()
# define update_stop()       update->stop()
#endif

#endif
