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

#ifndef __CALCS__
#define __CALCS__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ticables.h>

#include "stdints.h"
#include "export.h"
#include "tnode.h"	// to replace by glib !

#ifdef __cplusplus
extern "C" {
#endif

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBCALCS_VERSION "0.0.1"
#else
# define LIBCALCS_VERSION VERSION
#endif

#define LIBCALCS_REQUIRES_LIBFILES_VERSION  "0.0.1"	// useless with pkg-config
#define LIBCALCS_REQUIRES_LIBCABLES_VERSION "0.0.1"	// useless with pkg-config

/* Types */

// Name of the root node for 'Variables' & 'Applications' tree
#define VAR_NODE_NAME "Variables"
#define APP_NODE_NAME "Applications"

/**
 * CalcModel:
 *
 * An enumeration which contains the following calculator types:
 **/
typedef enum 
{
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, CALC_TI83, CALC_TI83P, CALC_TI84P, CALC_TI85, CALC_TI86,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200,
} CalcModel;

/**
 * CalcScreenFormat:
 *
 * An enumeration which contains the following calculator types:
 **/
typedef enum 
{
	SCREEN_FULL, 
	SCREEN_CLIPPED
} CalcScreenFormat;

/**
 * CalcPathType:
 *
 * An enumeration which contains the path type:
 **/
typedef enum 
{
	PATH_FULL, 
	PATH_LOCAL
} CalcPathType;

/**
 * CalcMemType:
 *
 * An enumeration which contains the different memory sizes:
 **/
typedef enum 
{
	MEMORY_NONE, 
	MEMORY_FREE, 
	MEMORY_USED
} CalcMemType;

/**
 * CalcMode:
 *
 * An enumeration which contains different mask modes:
 **/
typedef enum {
  MODE_NORMAL = 0,
  // For receiving vars
  MODE_RECEIVE_SINGLE_VAR = (1 << 0),	// Receive a single var
  MODE_RECEIVE_FIRST_VAR = (1 << 1),	// Recv first var of group file
  MODE_RECEIVE_LAST_VAR = (1 << 3),		// Recv last var of group file
  // For sending vars
  MODE_SEND_ONE_VAR = (1 << 4),			// Send single var or first var (grp)
  MODE_SEND_LAST_VAR = (1 << 5),		// Send last var of group file
  MODE_SEND_VARS = (1 << 6),			// Send var of group file
  MODE_SEND_TO_FLASH = (1 << 2),		// Send var to FLASH archive
  // Miscellaneous
  MODE_LOCAL_PATH = (1 << 7),			// Local path (full by default)
  MODE_BACKUP = (1 << 8),				// Keep archive attribute
  // For sending FLASH (apps/AMS)
  MODE_APPS = (1 << 11),				// Send a (free) FLASH application
  MODE_AMS = (1 << 12),					// Send an Operating System (AMS)
} CalcMode;

/**
 * CalcRomDumpSize:
 *
 * An enumeration which contains the following ROM dump sizes:
 **/
typedef enum 
{
	ROM_1MB = 1, 
	ROM_2MB, 
	ROM_SE,
} CalcRomDumpSize;

/**
 * CalcShellType:
 *
 * An enumeration which contains the shell to use with ROM dumping:
 **/
typedef enum 
{
	SHELL_NONE = 4, 
	SHELL_USGARD, 
	SHELL_ZSHELL
} CalcShellType;

/**
 * CalcOperations:
 *
 * An enumeration which contains the different supported operations:
 **/
typedef enum 
{
  OPS_NONE = 0,
  OPS_ISREADY = (1 << 0),
  OPS_SEND_KEY = (1 << 1),
  OPS_RECV_KEY = (1 << 2),
  OPS_REMOTE = (1 << 3),	//disabled
  OPS_SCREENDUMP = (1 << 4),
  OPS_DIRLIST = (1 << 5),
  OPS_RECV_BACKUP = (1 << 6),
  OPS_SEND_BACKUP = (1 << 7),
  OPS_RECV_VARS = (1 << 8),
  OPS_SEND_VARS = (1 << 9),
  OPS_SEND_FLASH = (1 << 10),
  OPS_RECV_FLASH = (1 << 11),
  OPS_IDLIST = (1 << 12),
  OPS_ROMDUMP = (1 << 13),
  OPS_ROMVERSION = (1 << 14),
  OPS_CLOCK = (1 << 15),
} CalcOperations;

/**
 * CalcAction:
 *
 * An enumeration which contains a possible action:
 **/
typedef enum 
{
	ACTION_SKIP, 
	ACTION_OVER
} CalcAction;

/**
 * CalcScreenCoord:
 * @width: real width
 * @height: real height
 * @clipped_width: clipped width (89 for instance)
 * @clipped_height: clipped height (89 for instance)
 *
 * A structure used for storing screen size.
 **/
typedef struct 
{
	uint8_t width;
	uint8_t height;

	uint8_t clipped_width;
	uint8_t clipped_height;
} CalcScreenCoord;

/**
 * KeyPair:
 * @name: name of key (like "ESC")
 * @value: value of key (like 264)
 *
 * A structure which contains a TI scancode.
 **/
typedef struct
{
	char*		name;
	uint16_t	value;
} KeyPair;

/**
 * CalcKey:
 * @key_name: name of key
 *
 * A structure which contains a TI scancode with key modifiers.
 * FIXME: use KeyPair structure to pack infos !
 **/
typedef struct 
{
  char*		key_name;	// Name of key

  char*		key1;		// Normal key
  uint16_t	nothing;

  char*		key2;		// SHIFT'ed key (89,92,92+)
  uint16_t	shift;

  char*		key3;		// 2nd key      (all)
  uint16_t	second;

  char*		key4;		// CTRL'ed key  (92,92+)
  uint16_t	diamond;

  char*		key5;		// ALPHA key    (83+,89)
  uint16_t	alpha;
} CalcKey;

/**
 * CalcClock:
 * @year: year
 * @month:
 * @day:
 * @hours:
 * @minutes:
 * @seconds:
 * @time_format: 12 or 24
 * @date_format: 1 or 6
 *
 * A structure used for clock management.
 **/
typedef struct 
{
  uint16_t	year;
  uint8_t	month;
  uint8_t	day;

  uint8_t	hours;
  uint8_t	minutes;
  uint8_t	seconds;

  uint8_t	time_format;
  uint8_t	date_format;
} CalcClock;

/**
 * CalcUpdate:
 * @cancel: set to 1 if transfer have to be cancelled
 * @count: number of bytes currently transferred
 * @total: number of bytes to transfer
 * @percentage: percentage of the current operation
 * @prev_percentage: Previous percentage of current operation
 * @main_percentage: Percentage of all operations
 * @prev_main_percentage: Previous percentage of all operations
 * @info: a text to display about the current operation
 * @start: init internal vars
 * @stop: release internal vars
 * @refresh: pass control to GUI for refresh
 * @pbar: refresh pprogress bar
 * @label: refresh label
 *
 * Refresh/progress functions
 * This structure allows to implement a kind of callbacks mechanism (which
 * allow libCalcs to interact with user without being dependant of a GUI).
 **/
typedef struct 
{
  int	cancel;
  
  int	count;
  int	total;

  float percentage;
  float prev_percentage;

  float main_percentage;
  float prev_main_percentage;

  char	info[256];

  void	(*start)	(void);
  void	(*stop)		(void);
  void	(*refresh)	(void);
  void	(*pbar)		(void);
  void	(*label)	(void);
} CalcUpdate;

typedef struct _CalcFncts	CalcFncts;
typedef struct _CalcHandle	CalcHandle;

/**
 * TiCable:
 * @model: link cable model (CalcModel).
 * @name: name of hand-held like "TI89"
 * @fullname: complete name of hand-held like "TI-89"
 * @description: description of hand-held like "TI89 calculator"
 * @operations: supported operations (CalcOperations)
 * @silent: TRUE if hand-held has silent technology, 0 otherwise
 * @folder: TRUE if hand-held can store folders
 * @memory: memory used/free on hand-held (CalcMemType)
 * @flash: calculator has FLASH ROM
 *
 * A structure used for handling a hand-held.
 * !!! This structure is for private use !!!
 **/
struct _CalcFncts
{
	const int		model;			
	const char*		name;			
	const char*		fullname;		
	const char*		description;
	
	const int		operations;

	const int		silent;
	const int		folder;
	const int		memory;
	const int		flash;

	const int		(*is_ready)		(CalcHandle *);

	const int		(*send_key)		(CalcHandle *, uint16_t key);

	const int		(*recv_screen)	(CalcHandle *, 
									 CalcScreenFormat format, 
									 CalcScreenCoord  sc,
									 uint8_t **bitmap
									 );

	const int		(*get_dirlist)	(CalcHandle *, 
									TNode ** vars, TNode ** apps, 
									uint32_t * memory);

	const int		(*recv_backup)	(CalcHandle*);
	const int		(*send_backup)	(CalcHandle*);

	const int		(*send_var)		(CalcHandle*);
	const int		(*recv_var)		(CalcHandle*);
	const int		(*recv_var2)	(CalcHandle*);

	const int		(*send_flash)	(CalcHandle*);
	const int		(*recv_flash)	(CalcHandle*);
	const int		(*recv_idlist)	(CalcHandle*);

	const int		(*dump_rom)		(CalcHandle*);

	const int		(*set_clock)	(CalcHandle*);
	const int		(*get_clock)	(CalcHandle*);

/*

  // Communication functions

  int (*recv_backup) (const char *filename, int mode);
  int (*send_backup) (const char *filename, int mode);

  int (*recv_var) (char *filename, int mode, TiVarEntry * ve);
  int (*send_var) (const char *filename, int mode, char **actions);

  int (*send_flash) (const char *filename, int mode);
  int (*recv_flash) (const char *filename, int mask_mode, TiVarEntry * ve);
  int (*get_idlist) (char *idlist);

  int (*dump_rom) (const char *filename, int mode);

  int (*set_clock) (const CalcClock * clock, int mode);
  int (*get_clock) (CalcClock * clock, int mode);

  int (*recv_var_2) (char *filename, int mode, TiVarEntry * ve);
  */
};

/**
 * CalcHandle:
 * @model: cable model
 * @calc: calculator functions
 * @update: callbacks for GUI interaction
 * @priv: holding data
 * @open: device has been opened
 * @busy: transfer is in progress
 * @cable: handle on cable used with this model
 * @attached: set if a cable has been attached
 *
 * A structure used to store informations as an handle.
 * !!! This structure is for private use !!!
 **/
struct _CalcHandle
{
	CalcModel	model;	
	CalcFncts*	calc;
	CalcUpdate*	update;

	void*		priv;	
	void*		priv2;	
	void*		priv3;	

	int			open;
	int			busy;
	
	CableHandle* cable;
	int			attached;
};

// namespace scheme: library_class_function like ticalcs_fext_get

	/****************/
	/* Entry points */
	/****************/
  
	TIEXPORT int TICALL ticalcs_library_init(void);
	TIEXPORT int TICALL ticalcs_library_exit(void);

	/*********************/
	/* General functions */
	/*********************/

	// error.c
	TIEXPORT int         TICALL ticalcs_error_get (int number, char **message);

	// ticalcs.c
	TIEXPORT const char* TICALL ticalcs_version_get (void);

	TIEXPORT CalcHandle* TICALL ticalcs_handle_new(CalcModel);
	TIEXPORT int         TICALL ticalcs_handle_del(CalcHandle*);
	TIEXPORT int         TICALL ticalcs_handle_show(CalcHandle*);

	TIEXPORT int TICALL ticalcs_cable_attach(CalcHandle*, CableHandle*);
	TIEXPORT int TICALL ticalcs_cable_detach(CalcHandle*);

	TIEXPORT int TICALL ticalcs_update_set(CalcHandle*, CalcUpdate*);

	// calc.c
	//...

	// type2str.c
	TIEXPORT const char*  TICALL ticalcs_model_to_string(CalcModel model);
	TIEXPORT CalcModel    TICALL ticalcs_string_to_model (const char *str);

	TIEXPORT const char*      TICALL ticalcs_scrfmt_to_string(CalcScreenFormat format);
	TIEXPORT CalcScreenFormat TICALL ticalc_string_to_scrfmt(const char *str);

	TIEXPORT const char*  TICALL ticalcs_pathtype_to_string(CalcPathType type);
	TIEXPORT CalcPathType TICALL ticalcs_string_to_pathtype(const char *str);

	TIEXPORT const char*  TICALL ticalcs_memtype_to_string(CalcMemType type);
	TIEXPORT CalcMemType  TICALL ticalcs_string_to_memtype(const char *str);

	TIEXPORT const char*  TICALL ticalcs_action_to_string(CalcAction action);
	TIEXPORT CalcAction   TICALL ticalcs_string_to_action(const char *str);

	// clock.c
	TIEXPORT const char* TICALL ticalcs_clock_format2date(int value);
	TIEXPORT int		 TICALL ticalcs_clock_date2format(const char *format);

	// tikeys.c
	TIEXPORT const CalcKey TICALL ticalcs_keys_73 (uint8_t ascii_code);
	TIEXPORT const CalcKey TICALL ticalcs_keys_83p(uint8_t ascii_code);
	TIEXPORT const CalcKey TICALL ticalcs_keys_89 (uint8_t ascii_code);
	TIEXPORT const CalcKey TICALL ticalcs_keys_92p(uint8_t ascii_code);
	
  
  /************************/
  /* Deprecated functions */
  /************************/

#ifdef __cplusplus
}
#endif

#endif
