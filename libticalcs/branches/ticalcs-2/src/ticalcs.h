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

#ifndef __TICALCS__
#define __TICALCS__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ticables.h>
#include <tifiles.h>

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
#ifndef __TIFILES_H__
typedef enum 
{
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, CALC_TI83, CALC_TI83P, CALC_TI84P, CALC_TI85, CALC_TI86,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200,
} CalcModel;
#endif

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

// To clean-up !
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
} CalcDumpSize;

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
  FTS_NONE = 0,

  OPS_ISREADY	= (1 << 0),
  OPS_KEYS		= (1 << 1),
  OPS_SCREEN	= (1 << 2),
  OPS_DIRLIST	= (1 << 3),
  OPS_BACKUP	= (1 << 4),
  OPS_VARS		= (1 << 5),
  OPS_FLASH		= (1 << 6),
  OPS_IDLIST	= (1 << 7),
  OPS_CLOCK		= (1 << 8),
  OPS_ROMDUMP	= (1 << 9),

  FTS_SILENT	= (1 << 15),
  FTS_FOLDER	= (1 << 16),
  FTS_MEMFREE	= (1 << 17),
  FTS_FLASH		= (1 << 18),

} CalcFeatures;

/**
 * CalcScreenCoord:
 * @format: returns full or clipped image (#CalcScreenFormat)
 * @width: real width
 * @height: real height
 * @clipped_width: clipped width (89 for instance)
 * @clipped_height: clipped height (89 for instance)
 *
 * A structure used for storing screen size.
 **/
typedef struct 
{
	int		format;

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
	char*	key_name;	// Name of key

	KeyPair	normal;		// normal key
	KeyPair	shift;		// SHIFT key (TI9x)
	KeyPair	second;		// 2ND key (all)
	KeyPair diamond;	// CTRL key (TI92)
	KeyPair alpha;		// ALPHA key (83+,89)

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
 * @rate: data rate of cable
 * @cnt1: current counter for local operation
 * @max1: max value of this counter
 * @cnt2: current counter for global operation
 * @max2: max value of this counter
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
	char	text[256];
	int		cancel;

	float	rate;
    int		cnt1;
	int		max1;
	int		cnt2;
	int		max2;

	void	(*start)	(void);
	void	(*stop)		(void);
	void	(*refresh)	(void);
	void	(*pbar)		(void);
	void	(*label)	(void);
} CalcUpdate;

typedef struct _CalcFncts	CalcFncts;
typedef struct _CalcHandle	CalcHandle;

// convenient wrappers: will be removed later !
#define update				(handle->updat)
#define update_start()		handle->updat->start()
#define update_pbar()       handle->updat->pbar()
#define update_label()      handle->updat->label()
#define update_refresh()    handle->updat->refresh()
#define update_stop()       handle->updat->stop()

typedef VarEntry	VarRequest;	// alias

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
 * @is_ready: check whether calculator is ready
 * @send_key: send key value
 * @recv_screen: request a screendump
 * @get_dirlist: request a listing of variables, folders (if any) and apps (if any)
 * @recv_backup: request a backup
 * @send_backup: send a backup
 * @send_var: send a variable (silent mode)
 * @recv_var: request a variable silent mode)
 * @send_var_ns: send a variable (non-silent mode)
 * @recv_var_ns: receive a variable (non-silent mode)
 * @send_flash: send a FLASH app/os
 * @recv_flash: request a FLASH app/os
 * @recv_idlist: request hand-held IDLIST
 * @dump_rom: dump the hand-held ROM
 * @set_clock: set date/time
 * @get_clock: get date/time
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
	const int		features;

	const int		(*is_ready)		(CalcHandle*);

	const int		(*send_key)		(CalcHandle*, uint16_t);

	const int		(*recv_screen)	(CalcHandle*, CalcScreenCoord*, uint8_t**);

	const int		(*get_dirlist)	(CalcHandle*, TNode** vars, TNode** apps);
	const int		(*get_memfree)	(CalcHandle*, uint32_t*);

	const int		(*send_backup)	(CalcHandle*, BackupContent*);
	const int		(*recv_backup)	(CalcHandle*, BackupContent*);

	const int		(*send_var)		(CalcHandle*, CalcMode, FileContent*);
	const int		(*recv_var)		(CalcHandle*, CalcMode, FileContent*, VarRequest*);
	const int		(*del_var)		(CalcHandle*, VarRequest*);

	const int		(*send_var_ns)	(CalcHandle*, CalcMode, FileContent*);
	const int		(*recv_var_ns)	(CalcHandle*, CalcMode, FileContent*, VarEntry*);

	const int		(*send_flash)	(CalcHandle*, FlashContent*);
	const int		(*recv_flash)	(CalcHandle*, FlashContent*, VarRequest*);
	const int		(*recv_idlist)	(CalcHandle*, uint8_t*);

	const int		(*dump_rom)		(CalcHandle*, CalcDumpSize, const char *filename);

	const int		(*set_clock)	(CalcHandle*, CalcClock* clock);
	const int		(*get_clock)	(CalcHandle*, CalcClock* clock);
};

/**
 * CalcHandle:
 * @model: cable model
 * @calc: calculator functions
 * @update: callbacks for GUI interaction
 * @priv: opaque data for internal/private use (static)
 * @priv2: idem (allocated)
 * @priv3: idem (static)
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
	CalcUpdate*	updat;

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
	TIEXPORT CalcFeatures TICALL ticalcs_calc_features(CalcHandle*);

	TIEXPORT int TICALL ticalcs_calc_isready(CalcHandle*);
	TIEXPORT int TICALL ticalcs_calc_send_key(CalcHandle*, uint16_t);
	TIEXPORT int TICALL ticalcs_calc_recv_screen(CalcHandle *, CalcScreenCoord* sc,
												 uint8_t** bitmap);

	TIEXPORT int TICALL ticalcs_calc_recv_backup(CalcHandle*, BackupContent*);
	TIEXPORT int TICALL ticalcs_calc_send_backup(CalcHandle*, BackupContent*);

	TIEXPORT int TICALL ticalcs_calc_send_var(CalcHandle*, CalcMode, FileContent*);
	TIEXPORT int TICALL ticalcs_calc_recv_var(CalcHandle*, CalcMode, FileContent*, VarRequest*);
	TIEXPORT int TICALL ticalcs_calc_del_var(CalcHandle*, VarRequest*);

	TIEXPORT int TICALL ticalcs_calc_send_var_ns(CalcHandle*, CalcMode, FileContent*);
	TIEXPORT int TICALL ticalcs_calc_recv_var_ns(CalcHandle*, CalcMode, FileContent*, VarEntry*);

	TIEXPORT int TICALL ticalcs_calc_send_flash(CalcHandle*, FlashContent*);
	TIEXPORT int TICALL ticalcs_calc_recv_flash(CalcHandle*, FlashContent*, VarRequest*);
	TIEXPORT int TICALL ticalcs_calc_recv_idlist(CalcHandle*, uint8_t*);

	TIEXPORT int TICALL ticalcs_calc_dump_rom(CalcHandle*, CalcDumpSize, const char *filename);

	TIEXPORT int TICALL ticalcs_calc_set_clock(CalcHandle*, CalcClock* clock);
	TIEXPORT int TICALL ticalcs_calc_get_clock(CalcHandle*, CalcClock* clock);

	// dirlist.c
	TIEXPORT void TICALL ticalc_dirlist_destroy(TNode** tree);
	TIEXPORT void TICALL ticalc_dirlist_display(TNode*  tree);

	TIEXPORT int TICALL ticalc_dirlist_num_vars(TNode* tree);
	TIEXPORT int TICALL ticalc_dirlist_mem_used(TNode* tree);

	TIEXPORT VarEntry *TICALL ticalc_dirlist_var_exist(TNode* tree, char* varname);
	TIEXPORT VarEntry *TICALL ticalc_dirlist_app_exist(TNode* tree, char* appname);

	// type2str.c
	TIEXPORT const char*  TICALL ticalcs_model_to_string(CalcModel model);
	TIEXPORT CalcModel    TICALL ticalcs_string_to_model (const char *str);

	TIEXPORT const char*      TICALL ticalcs_scrfmt_to_string(CalcScreenFormat format);
	TIEXPORT CalcScreenFormat TICALL ticalc_string_to_scrfmt(const char *str);

	TIEXPORT const char*  TICALL ticalcs_pathtype_to_string(CalcPathType type);
	TIEXPORT CalcPathType TICALL ticalcs_string_to_pathtype(const char *str);

	TIEXPORT const char*  TICALL ticalcs_memtype_to_string(CalcMemType type);
	TIEXPORT CalcMemType  TICALL ticalcs_string_to_memtype(const char *str);

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
