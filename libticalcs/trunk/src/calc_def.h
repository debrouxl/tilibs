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

#ifndef __TICALC_DEFS__
#define __TICALC_DEFS__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <time.h>

#include "headers.h"

/********************/
/* Type definitions */
/********************/

/* 
   A structure for creating a dirlist tree 
*/
#ifndef __MACOSX__
#include <glib.h>
#else
#include <glib/glib.h>
#endif

typedef GNode TNode;


/*
  A structure used for the screendump functions
*/
typedef struct
{
  uint8_t width;          // real width
  uint8_t height;         // real height
  uint8_t clipped_width;  // clipped width (89 for instance)
  uint8_t clipped_height; // clipped height (idem)
}
TicalcScreenCoord;


/* 
   A structure which contains the TI scancode of a TI key and additional
   informations
*/
typedef struct
{
  char *key_name;	// Name of key
  char *key1;		// Normal key
  uint16_t nothing;
  char *key2;		// SHIFT'ed key		(89,92,92+)
  uint16_t shift;
  char *key3;		// 2nd key			(all)
  uint16_t second;
  char *key4;		// CTRL'ed key		(92,92+)
  uint16_t diamond;
  char *key5;		// ALPHA key		(83+,89)
  uint16_t alpha;
}
TicalcKey; 


/* 
   Refresh/progress functions
   This structure allows to implement a kind of callbacks mechanism (which
   allow libticalcs to interact with user without being dependant of a GUI).
*/
typedef struct
{
  /* Variables to update */
  int cancel;                // Abort the current transfer
  char label_text[256];      // A text to display (varname, ...)
  int count;                 // Number of uint8_ts exchanged
  int total;                 // Number of uint8_ts to exchange
  float percentage;          // Percentage of the current operation
  float prev_percentage;     // Previous percentage of current operation
  float main_percentage;     // Percentage of all operations
  float prev_main_percentage;// Previous percentage of all operations

  /* Functions for updating */
  void (*start)   (void);                   // Init internal variables
  void (*stop)    (void);                   // Release internal variables
  void (*refresh) (void);                   // Pass control to GUI for refresh
  void (*pbar)    (void);                   // Refresh the progress bar
  void (*label)   (void);                   // Refresh the label
}
TicalcInfoUpdate;


/*
  A structure used for clock management
*/
typedef struct
{
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hours;
  uint8_t  minutes;
  uint8_t  seconds;
  uint8_t  time_format; // 12 or 24
  uint8_t  date_format; // 1 to 6
}
TicalcClock;


/* 
   Calculator functions (independant of calculator model)
*/
typedef struct
{
  // Supported functions
  int (*supported_operations) (void);

  // Silent calc model
  int is_silent;
  int has_folder;
  int memory;

  // Communication functions
  int (*isready)         (void);

  int (*send_key)        (uint16_t key);
  int (*screendump)      (uint8_t **bitmap, int mode, 
			  TicalcScreenCoord *sc);
  int (*directorylist)   (TNode **tree, uint32_t *memory);
  
  int (*recv_backup)     (const char *filename, int mode);
  int (*send_backup)     (const char *filename, int mode);
  
  int (*recv_var)        (      char *filename, int mode, TiVarEntry *ve);
  int (*send_var)        (const char *filename, int mode, char **actions);
  
  int (*send_flash)      (const char *filename, int mode);
  int (*recv_flash)      (const char *filename, int mask_mode, TiVarEntry *ve);
  int (*get_idlist)      (char *idlist);

  int (*dump_rom)        (const char *filename, int mode);

  int (*set_clock)       (const TicalcClock *clock, int mode);
  int (*get_clock)       (      TicalcClock *clock, int mode);
}
TicalcFncts;


/*********************/
/* Macro definitions */
/*********************/


/*
  Screendump: full or clipped screen
*/
#define FULL_SCREEN    0
#define CLIPPED_SCREEN 1


/* 
   Path: full or local 
*/
#define FULL_PATH  0
#define LOCAL_PATH 1


/*
  Memory field
*/
#define MEMORY_NONE 0
#define MEMORY_FREE 1
#define MEMORY_USED 2


/* 
   Some masks for the send/receive functions (mode) 
*/
// No mask
#define MODE_NORMAL                 0  // No mode
// For receiving vars
#define MODE_RECEIVE_SINGLE_VAR (1<<0) // Receive a single var
#define MODE_RECEIVE_FIRST_VAR  (1<<1) // Recv first var of group file
#define MODE_RECEIVE_LAST_VAR   (1<<3) // Recv last var of group file
// For sending vars
#define MODE_SEND_ONE_VAR       (1<<4) // Send single var or first var (grp)
#define MODE_SEND_LAST_VAR      (1<<5) // Send last var of group file
#define MODE_SEND_VARS          (1<<6) // Send var of group file
// Miscellaneous
#define MODE_LOCAL_PATH         (1<<7) // Local path (full by default)
#define MODE_BACKUP             (1<<8) // Keep archive attribute
// For sending FLASH (apps/AMS)
#define MODE_APPS	       (1<<11) // Send a (free) FLASH application
#define MODE_AMS	       (1<<12) // Send an Operating System (AMS)

// ROM size for the ROM dump function
#define ROM_1MB      1 // 1 MegaBytes
#define ROM_2MB      2 // 2 MegaBytes

// Shell to use with ROM dumping
#define SHELL_NONE   4
#define SHELL_USGARD 5
#define SHELL_ZSHELL 6


/*
  Mask returned by the 'supported_operations' function
*/
#define OPS_NONE        0
#define OPS_ISREADY     (1<<0)
#define OPS_SEND_KEY    (1<<1)
#define OPS_RECV_KEY    (1<<2)
#define OPS_REMOTE      (1<<3)	//disabled
#define OPS_SCREENDUMP  (1<<4)
#define OPS_DIRLIST     (1<<5)
#define OPS_RECV_BACKUP (1<<6)
#define OPS_SEND_BACKUP (1<<7)
#define OPS_RECV_VARS   (1<<8)
#define OPS_SEND_VARS   (1<<9)
#define OPS_SEND_FLASH  (1<<10)
#define OPS_RECV_FLASH  (1<<11)
#define OPS_IDLIST      (1<<12)
#define OPS_ROMDUMP     (1<<13)
#define OPS_ROMVERSION  (1<<14)
#define OPS_CLOCK       (1<<15)


/*
  Actions for send_var
*/
#define ACT_SKIP   0
#define ACT_OVER (!0)


/* 
   Key codes of PC's keyboard 
*/
#define CTRL_A  0x01
#define CTRL_B  0x02
#define CTRL_C  0x03  // Non useable (BREAK)
#define CTRL_D  0x04
#define CTRL_E  0x05
#define CTRL_F  0x06
#define CTRL_G  0x07
#define CTRL_H  0x08
#define CTRL_I  0x09
#define TAB     0x09
#define CALC_CR 0x0A
#define CTRL_J  0x0A
#define CTRL_K  0x0B
#define CTRL_L  0x0C
#define CALC_LF 0x0D
#define CTRL_M  0x0D
#define CTRL_N  0x0E
#define CTRL_O  0x0F
#define CTRL_P  0x010
#define CTRL_Q  0x011  // Non useable (XON)
#define CTRL_R  0x012
#define CTRL_S  0x013  // Non useable (XOFF)
#define CTRL_T  0x014
#define CTRL_U  0x015
#define CTRL_V  0x016
#define CTRL_W  0x017
#define CTRL_X  0x018
#define CTRL_Y  0x019
#define CTRL_Z  0x01A  // Non useable (STOP)
#define ESC     0x01B
// 0x20 to 0xFF: ASCII codes
#define DOWN    0x0102
#define UP      0x0103
#define LEFT    0x0104
#define RIGHT   0x0105
#define HOME    0x0106
#define BS      0x0107
#define F1      0x0109
#define F2      0x010A
#define F3      0x010B
#define F4      0x010C
#define F5      0x010D
#define F6      0x010E
#define F7      0x010F
#define F8      0x0110
#define F9      0x0111
#define F10     0x0112
#define F11     0x0113
#define F12     0x0114
#define SUPPR   0x014A
#define INSERT  0x014B
#define PAGEDW  0x0152
#define PAGEUP  0x0153
#define END     0x0168

#endif




