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

#ifndef __CALC_DEFS__
#define __CALC_DEFS__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <time.h>
#if defined(HAVE_TILP_TYPEDEFS_H) && !defined(__MACOSX__)
#include <tilp/typedefs.h>
#elif defined(HAVE_TILP_TYPEDEFS_H) && defined(__MACOSX__)
#include <libticables/typedefs.h>
#else
#include "typedefs.h"
#endif

/********************/
/* Type definitions */
/********************/

/* 
   A structure for creating a dirlist tree 
*/
#define VARIABLE 0
#define FOLDER   1

#define VARATTR_NONE	0
#define VARATTR_LOCK	1	// Var is locked
#define VARATTR_ARCH	3	// Var is archived

struct varinfo
{
  char varname[9];        // Name of the var
  byte vartype;           // Type of the var
  byte varlocked;         // Attribute of the var (locked, archived, none)
  longword varsize;       // Real size of the var
  char translate[9];      // Real name of the var

  int is_folder;          // 1 if folder, 0 if variable

  struct varinfo *folder; // Points to the parent folder
  struct varinfo *next;   // Next variable
};
typedef struct varinfo VAR_INFO;
//typedef struct varinfo VarInfo; //conflict with TiFFEP: to fix...

/*
  A structure used for the screendump functions
*/
struct screen_coord
{
  byte width;          // real width
  byte height;         // real height
  byte clipped_width;  // clipped width (89 for instance)
  byte clipped_height; // clipped height (idem)
};
typedef struct screen_coord SCR_COORD;
typedef struct screen_coord ScrCoord;

/* 
   A structure which contains the TI scancode of a TI key and additional
   informations
*/
struct ti_key
{
  char *key_name;
  char *key1;
  word nothing;
  char *key2;
  word shift;
  char *key3;
  word second;
  char *key4;
  word diamond;
  char *key5;
  word alpha;
};
typedef struct ti_key TI_KEY;
typedef struct ti_key TiKey;

/* 
   Refresh/progress functions
   This structure allows to implement a kind of callbacks mechanism (which
   allow libticalcs to interact with user without being dependant of a GUI).
*/
struct ticalc_info_update
{
  /* Variables to update */
  int cancel;                // Abort the current transfer
  char label_text[MAXCHARS]; // A text to display (varname, ...)
  float percentage;          // Percentage of the current operation
  float main_percentage;     // Percentage of all operations
  float prev_percentage;     // Previous percentage of current operation
  float prev_main_percentage;// Previous percentage of all operations
  int count;                 // Number of bytes exchanged
  int total;                 // Number of bytes to exchange
  clock_t start_time;        // Time when transfer has begun
  clock_t current_time;      // Current time

  /* Functions for updating */
  void (*start)   (void);                   // Init internal variables
  void (*stop)    (void);                   // Release internal variables
  void (*refresh) (void);                   // Pass control to GUI for refresh
  void (*msg_box) (const char *t, char *s); // Display a message box
  void (*pbar)    (void);                   // Refresh the progress bar
  void (*label)   (void);                   // Refresh the label
  int  (*choose)  (char *cur_name, 
		   char *new_name);         // Display choice box (skip, rename
                                            // or overwrite, ... )
};
typedef struct ticalc_info_update INFO_UPDATE;
typedef struct ticalc_info_update InfoUpdate;

/* 
   Calculator functions 
*/
struct ticalc_fncts
{
  /* TI byte <-> type conversion functions, defined in the tiXX.c files */
  const char* (*byte2type) (byte data);
  byte        (*type2byte) (char *s);
  const char* (*byte2fext) (byte data);
  byte        (*fext2byte) (char *s);

  /* TI routines, defined in the tiXX.c files */
  int (*isready)         (void);

  int (*send_key)        (int key);
  int (*remote_control)  (void);
  int (*screendump)      (byte **bitmap, int mask_mode, 
			  ScrCoord *sc);
  int (*directorylist)   (struct varinfo *list, int *n_elts);
  int (*recv_backup)     (FILE *file, int mask_mode, longword *version);
  int (*send_backup)     (FILE *file, int mask_mode);
  int (*recv_var)        (FILE *file, int mask_mode, char *varname, 
			  byte vartype, byte varlock);
  int (*send_var)        (FILE *file, int mask_mode); 
  int (*send_flash)      (FILE *file, int mask_mode);
  int (*recv_flash)      (FILE *file, int mask_mode, char *appname);
  int (*dump_rom)        (FILE *file, int mask_mode);
  int (*get_rom_version) (char *version);
  int (*get_idlist)      (char *idlist);

  /* General purpose routines, calc independant */
  char* (*translate_varname) (char *varname, char *translate, byte vartype);
  const struct ti_key (*ascii2ti_key) (unsigned char ascii_code);
  void  (*generate_single_file_header) (FILE *file, 
					int mask_mode,
					const char *id, 
					struct varinfo *v);
  void  (*generate_group_file_header) (FILE *file,
				       int mask_mode,
				       const char *id,
				       struct varinfo *list,
				       int calc_type);

  /* General purpose routines, calc dependant */
  const char* (*group_file_ext) (int calc_type);
  const char* (*backup_file_ext) (int calc_type);
  const int   (*tixx_dir) (int calc_type);
  const char* (*pak_name) (int calc_type);
  const char* (*flash_app_file_ext) (int calc_type);
  const char* (*flash_os_file_ext) (int calc_type);
  const int   (*tixx_flash) (int calc_type);
};
typedef struct ticalc_fncts TICALC_FNCTS;
typedef struct ticalc_fncts TicalcFncts;

/* 
   Calculator functions: check whether a function is supported by a calc
   Not used yet.
*/
struct ticalc_supp_fncts
{
  int (*isready)        (void);
  int (*send_key)       (void);
  int (*remote_control) (void);
  int (*screendump)     (void);
  int (*recv_backup) (void);
  int (*send_backup)    (void);
  int (*directorylist)  (void);
  int (*recv_var)    (void);
  int (*send_var)       (void);
  int (*dump_rom)       (void);
  int (*get_rom_version)(void);
  int (*send_flash)     (void);
};
typedef struct ticalc_supp_fncts TICALC_SUPP_FNCTS;
typedef struct ticalc_supp_fncts TicalcSuppFncts;


/*********************/
/* Macro definitions */
/*********************/

/*
  Screendump: full or clipped screen
*/
#define FULL_SCREEN 0
#define CLIPPED_SCREEN 1

/* 
   Path: full or local 
*/
#define FULL_PATH  0
#define LOCAL_PATH 1

/* 
   Use normal file format (don't keep archive attribute) or 
   extended file format (keep archive attribute in the file) 
*/
#define NORMAL_FORMAT   0
#define EXTENDED_FORMAT 1

/* 
   The different calculator types 
*/
#define CALC_NONE  0
#define CALC_TI92P 1
#define CALC_TI92  2
#define CALC_TI89  3
#define CALC_TI86  4
#define CALC_TI85  5
#define CALC_TI83P 6
#define CALC_TI83  7
#define CALC_TI82  8
#define CALC_TI73  9

/* 
   Some masks for the send/receive functions (mask_mode) 
*/
// No mask
#define MODE_NORMAL              0 // No mode
// For receiving vars
#define MODE_RECEIVE_SINGLE_VAR  1 // Receive a single var
#define MODE_RECEIVE_FIRST_VAR   2 // Recv the first var of a group file
#define MODE_RECEIVE_VARS        4 // Recv var of a group file
#define MODE_RECEIVE_LAST_VAR    8 // Recv the last var of a group file
// For sending vars
#define MODE_SEND_ONE_VAR       16 // Send a single var or the first var (grp)
#define MODE_SEND_LAST_VAR      32 // Send the last var of a group file
#define MODE_SEND_VARS          64 // Send var of a group file
// Miscellaneous
#define MODE_LOCAL_PATH        128 // Local path (full by default)
#define MODE_KEEP_ARCH_ATTRIB  256 // Keep archive attribute (89/92+)
#define MODE_USE_2ND_HEADER    512 // Use 0xC9 instead of 0x06
// For requesting an IDlist thru the recv_var function
#define MODE_IDLIST           1024 // Get the IDlist (89/92+)
// For sending FLASH (apps/AMS)
#define MODE_APPS	      2048 // Send a (free) FLASH application
#define MODE_AMS	      4096 // Send an Operating System (AMS)
// For file checking
#define MODE_FILE_CHK_NONE    8192 // Do no file checking (dangerous !)
#define MODE_FILE_CHK_MID    16384 // Do a simple file checking
#define MODE_FILE_CHK_ALL    32768 // Do a restrictive file checking
#define MODE_DIRLIST         65536 // Do a dirlist before sending vars

// ROM size for the ROM dump function
#define ROM_1MB      1 // 1 MegaBytes
#define ROM_2MB      2 // 2 MegaBytes

// Shell to use with ROM dumping
#define SHELL_NONE   4
#define SHELL_USGARD 5
#define SHELL_ZSHELL 6

// Code returned by the update.choose() function
#define ACTION_NONE      0
#define ACTION_SKIP      1
#define ACTION_OVERWRITE 2
#define ACTION_RENAME    4

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
#define CALC_CR      0x0A
#define CTRL_J  0x0A
#define CTRL_K  0x0B
#define CTRL_L  0x0C
#define CALC_LF      0x0D
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




