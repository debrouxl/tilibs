/*
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

#ifndef __UPDATE__
#define __UPDATE__

/* Functions for updating */
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
#define update_start()      if(update->start)   update->start()
#define update_msgbox(t, s) if(update->msgbox)  update->msg_box(t, s)
#define update_pbar()       if(update->pbar)    update->pbar()
#define update_label()      if(update->label)   update->label()
#define update_refresh()    if(update->refresh) update->refresh()
//#define update_choose(c, n) if(update->choose)  update->choose(c, n)
#define update_choose(c, n)                     update->choose(c, n)
#define update_stop()       if(update->stop)    update->stop()

#endif
