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

#ifndef __TICALCS_UPDATE__
#define __TICALCS_UPDATE__

#ifndef TRYV
# define TRYV(x) { int aaaa_; if((aaaa_ = (x))) return aaaa_; }   //new !
#endif

/*
 * Implement a kind of mutex: prevent that someone tries to initiate
 * a transfer while another is in progress
 */
extern int lock;
#define LOCK_TRANSFER   TRYV(lock);  lock = ERR_PENDING_TRANSFER;
#define UNLOCK_TRANSFER lock = 0;

/* 
 * Functions for updating 
 */

/*
  void (*start)   (void);                   // Init internal variables
  void (*msg_box) (const char *t, char *s); // Display a message box
  void (*pbar)    (void);                   // Refresh the progress bar
  void (*label)   (void);                   // Refresh the label
  void (*refresh) (void);                   // Pass control to GUI for refresh
  int  (*choose)  (char *cur_name, 
  char *new_name);         // Display choice box (skip, rename, overwrite, ... )
  void (*stop)    (void);
*/

/* 
   Macros: they check that function pointer is good and then
   call the pointer itself 
*/
#ifdef CHECK_UPDATE
# define update_start()      if(update && update->start)   update->start()
# define update_msgbox(t, s) if(update && update->msgbox) update->msg_box(t, s)
# define update_pbar()       if(update && update->pbar)    update->pbar()
# define update_label()      if(update && update->label)   update->label()
# define update_refresh()    if(update && update->refresh) update->refresh()
# define update_stop()       if(update && update->stop)    update->stop()
/*
# ifndef UPDATE_INLINE
#  define UPDATE_INLINE
int inline update_choose(char* c, char* n)
{ if(update && update->choose) return update->choose(c, n); else return 0; }
# endif
*/
# define update_choose(c, n) update->choose(c, n)
#else
# define update_start()      update->start()
# define update_msgbox(t, s) update->msg_box(t, s)
# define update_pbar()       update->pbar()
# define update_label()      update->label()
# define update_refresh()    update->refresh()
# define update_stop()       update->stop()
# define update_choose(c, n) update->choose(c, n)
#endif

#endif





