/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libticalcs2 - hand-helds support library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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

#ifndef __TICALCS__
#define __TICALCS__

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ticables.h>
#include <tifiles.h>

#include "export3.h"
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

	/***********************/
	/* Types & Definitions */
	/***********************/

/* Versioning */

#ifdef __WIN32__
# define LIBCALCS_VERSION "1.1.10"
#else
# define LIBCALCS_VERSION VERSION
#endif

#define LIBCALCS_REQUIRES_LIBTICONV_VERSION	"1.1.5"	// useless with pkg-config
#define LIBCALCS_REQUIRES_LIBTIFILES_VERSION	"1.1.0"	// useless with pkg-config
#define LIBCALCS_REQUIRES_LIBTICABLES_VERSION	"1.3.5"	// useless with pkg-config

/* Types */

// Name of the root node for 'Variables' & 'Applications' tree
#define VAR_NODE_NAME "Variables"
#define APP_NODE_NAME "Applications"

#define ERROR_ABORT (256) /* fixed in error.h */
#define ERROR_EOT   (262) /* fixed in error.h */

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
 * CalcPixelFormat:
 *
 * An enumeration which defines the format of screenshot images:
 */
typedef enum
{
	CALC_PIXFMT_MONO = 1,         // Monochrome (1 bpp)
	CALC_PIXFMT_GRAY_4 = 2,       // Grayscale (4 bpp - Nspire)
	CALC_PIXFMT_RGB_565_LE = 3,   // RGB (16 bpp little-endian - Nspire CX / 84+CSE / 83PCE / 84+CE)
	CALC_PIXFMT_RGB_5_6_5 = 3     // Ditto
} CalcPixelFormat;

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
	MEMORY_NONE = 0,
	MEMORY_FREE = (1<<0),
	MEMORY_USED = (1<<1),
} CalcMemType;

// To clean-up !
/**
 * CalcMode:
 *
 * An enumeration which contains different mask modes:
 **/
typedef enum
{
	MODE_NORMAL = 0,

	// For sending vars
	MODE_SEND_ONE_VAR  = (1 << 1),   // Send single var or first var of group (TI82/85 only)
	MODE_SEND_LAST_VAR = (1 << 2),   // Send last var of group file (TI82/85 only)

	MODE_SEND_EXEC_ASM = (1 << 3),   // Send and execute assembly (TI82/85 only; dangerous!)

	// Miscellaneous
	MODE_LOCAL_PATH    = (1 << 4),   // Local path (full by default)
	MODE_BACKUP        = (1 << 5),   // Keep archive attribute
} CalcMode;

/**
 * CalcRomDumpSize:
 *
 * An enumeration which contains the following ROM dump sizes:
 **/
typedef enum
{
	ROMSIZE_AUTO  = 0,
	ROMSIZE_48KB  = 48,     /* TI-80 */
	ROMSIZE_128KB = 128,    /* TI-82, TI-85 */
	ROMSIZE_256KB = 256,    /* TI-83, TI-86 */
	ROMSIZE_512KB = 512,    /* TI-83+ */
	ROMSIZE_1MB   = 1024,   /* TI-84+, TI-92, TI-82A */
	ROMSIZE_2MB   = 2048,   /* TI-83+SE, TI-84+SE, TI-89, TI-92 II, TI-92+, TI-84+T */
	ROMSIZE_4MB   = 4096,   /* TI-84+CSE, TI-89T, V200, TI-83PCE, TI-84+CE, TI-82AEP */
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

	OPS_ISREADY           = (1 << 0),
	OPS_KEYS              = (1 << 1),
	OPS_SCREEN            = (1 << 2),
	OPS_DIRLIST           = (1 << 3),
	OPS_BACKUP            = (1 << 4),
	OPS_VARS              = (1 << 5),
	OPS_FLASH             = (1 << 6),
	OPS_IDLIST            = (1 << 7),
	OPS_CLOCK             = (1 << 8),
	OPS_ROMDUMP           = (1 << 9),
	OPS_VERSION           = (1 << 10),
	OPS_NEWFLD            = (1 << 11),
	OPS_DELVAR            = (1 << 12),
	OPS_OS                = (1 << 13),
	OPS_RENAME            = (1 << 14),
	OPS_CHATTR            = (1 << 21),
	OPS_LABEQUIPMENTDATA  = (1 << 23),

	FTS_SILENT            = (1 << 15),
	FTS_FOLDER            = (1 << 16),
	FTS_MEMFREE           = (1 << 17),
	FTS_FLASH             = (1 << 18),
	FTS_CERT              = (1 << 19),
	FTS_BACKUP            = (1 << 20),
	FTS_NONSILENT         = (1 << 22)
} CalcFeatures;

/**
 * CalcCommunicationProtocols:
 *
 * An enumeration which contains the different supported communication protocols:
 **/
typedef enum
{
	CALC_COMM_PROTO_NONE = 0,

	CALC_COMM_PROTO_DBUS            = (1 << 0),
	CALC_COMM_PROTO_CARS            = (1 << 1),             ///< Official name
	CALC_COMM_PROTO_DUSB            = CALC_COMM_PROTO_CARS,
	CALC_COMM_PROTO_NAVNET          = (1 << 2),             ///< Official name
	CALC_COMM_PROTO_NSP             = CALC_COMM_PROTO_NAVNET,

	CALC_COMM_PROTO_TICALCS_ROMDUMP = (1 << 30)
	// CALC_COMM_PROTO_MORE_PROTOS  = (1 << 31)
} CalcCommunicationProtocols;

/**
 * CalcAction:
 *
 * An enumeration which contains the action taken on a variable
 **/
typedef enum
{
	ACT_NONE = 0,
	ACT_RENAME, ACT_OVER, ACT_SKIP,
} CalcAction;

/**
 * InfosMask:
 *
 * An enumeration which contains the different flags supported by CalcInfos:
 **/
typedef enum
{
	INFOS_PRODUCT_NUMBER    = (1 << 0), /* obsolete (never used) */
	INFOS_PRODUCT_NAME      = (1 << 1),
	INFOS_MAIN_CALC_ID      = (1 << 2), /* obsolete, replaced by INFOS_PRODUCT_ID */
	INFOS_HW_VERSION        = (1 << 3),
	INFOS_LANG_ID           = (1 << 4),
	INFOS_SUB_LANG_ID       = (1 << 5),
	INFOS_DEVICE_TYPE       = (1 << 6),
	INFOS_BOOT_VERSION      = (1 << 7),
	INFOS_OS_VERSION        = (1 << 8),
	INFOS_RAM_PHYS          = (1 << 9),
	INFOS_RAM_USER          = (1 << 10),
	INFOS_RAM_FREE          = (1 << 11),
	INFOS_FLASH_PHYS        = (1 << 12),
	INFOS_FLASH_USER        = (1 << 13),
	INFOS_FLASH_FREE        = (1 << 14),
	INFOS_LCD_WIDTH         = (1 << 15),
	INFOS_LCD_HEIGHT        = (1 << 16),
	INFOS_BATTERY_ENOUGH    = (1 << 17),
	INFOS_BATTERY           = INFOS_BATTERY_ENOUGH, /* For compatibility */
	INFOS_BOOT2_VERSION     = (1 << 18),
	INFOS_RUN_LEVEL         = (1 << 19),
	INFOS_BPP               = (1 << 20),
	INFOS_CLOCK_SPEED       = (1 << 21),
	INFOS_PRODUCT_ID        = (1 << 22),
	INFOS_MATH_CAPABILITIES = (1 << 23),
	INFOS_EXACT_MATH        = INFOS_MATH_CAPABILITIES, /* For compatibility */
	INFOS_CLOCK_SUPPORT     = (1 << 24),
	INFOS_COLOR_SCREEN      = (1 << 25),
	INFOS_PYTHON_ON_BOARD   = (1 << 26),
	INFOS_USER_DEFINED_ID   = (1 << 27),
	// TODO INFOS_PTT_MODE, based on DUSB_PID_PTT_MODE_STATE for the TI-eZ80 series, and whichever NavNet equivalent, if any.

	// INFOS_MORE_INFOS     = (1 << 30), /* Some day ? Reserved value for signaling more bits are available elsewhere */
	INFOS_CALC_MODEL     = 0x80000000
} InfosMask;

/**
 * CalcFnctsIdx:
 *
 * Index of function in the #CalcFncts structure:
 **/
typedef enum
{
	// TODO for next gen CALC_ prefix.
	FNCT_IS_READY=0,
	FNCT_SEND_KEY,
	FNCT_EXECUTE,
	FNCT_RECV_SCREEN,
	FNCT_GET_DIRLIST,
	FNCT_GET_MEMFREE,
	FNCT_SEND_BACKUP,
	FNCT_RECV_BACKUP,
	FNCT_SEND_VAR,
	FNCT_RECV_VAR,
	FNCT_SEND_VAR_NS,
	FNCT_RECV_VAR_NS,
	FNCT_SEND_APP,
	FNCT_RECV_APP,
	FNCT_SEND_OS,
	FNCT_RECV_IDLIST,
	FNCT_DUMP_ROM1,
	FNCT_DUMP_ROM2,
	FNCT_SET_CLOCK,
	FNCT_GET_CLOCK,
	FNCT_DEL_VAR,
	FNCT_NEW_FOLDER,
	FNCT_GET_VERSION,
	FNCT_SEND_CERT,
	FNCT_RECV_CERT,
	FNCT_RENAME,
	FNCT_CHATTR,
	FNCT_SEND_ALL_VARS_BACKUP,
	FNCT_RECV_ALL_VARS_BACKUP,
	FNCT_SEND_LAB_EQUIPMENT_DATA,
	FNCT_GET_LAB_EQUIPMENT_DATA,
	FNCT_DEL_FOLDER,
	CALC_FNCT_LAST // Keep this one last
} CalcFnctsIdx;

#define FNCT_DUMP_ROM FNCT_DUMP_ROM2

/**
 * TigMode:
 *
 * An enumeration which contains the data to save in tigroup:
 **/
typedef enum
{
	TIG_NONE    = 0,
	TIG_RAM     = (1 << 0),
	TIG_ARCHIVE = (1 << 1),
	TIG_FLASH   = (1 << 2),
	TIG_BACKUP  = (1 << 3),
	TIG_ALL     = 7,
} TigMode;

/**
 * DBUSPacket:
 *
 * Packet for the DBUS (old calculators) protocol.
 **/
typedef struct
{
	uint16_t length;     ///< data length
	uint8_t  id;         ///< target (send) or host (recv) model
	uint8_t  cmd;        ///< protocol command

	uint8_t *data;       ///< packet data
} DBUSPacket;

//! Size of the header of a \a DUSBRawPacket
#define DUSB_HEADER_SIZE (4+1)
//! Size of the data contained in \a DUSBRawPacket
#define DUSB_DATA_SIZE   (1023)

/**
 * DUSBRawPacket:
 *
 * Raw packet for the DUSB / CARS (84+(SE), 89T) protocol, version with pre-allocated data.
 **/
typedef struct
{
	uint32_t size;                 ///< raw packet size
	uint8_t  type;                 ///< raw packet type

	uint8_t  data[DUSB_DATA_SIZE]; ///< raw packet data
} DUSBRawPacket;

/**
 * DUSBRawPacketA:
 *
 * Raw packet for the DUSB / CARS (84+(SE), 89T) protocol, version with externally allocated packet data.
 **/
typedef struct
{
	uint32_t size;       ///< raw packet size
	uint8_t  type;       ///< raw packet type

	uint8_t *data;       ///< raw packet data
} DUSBRawPacketA;

/**
 * DUSBVirtualPacket:
 *
 * Virtual packet for the DUSB / CARS (84+(SE), 89T) protocol.
 **/
typedef struct
{
	uint32_t size;       ///< virtual packet size
	uint16_t type;       ///< virtual packet type

	uint8_t *data;       ///< virtual packet data
} DUSBVirtualPacket;

//! Size of the header of a \a NSPRawPacket
#define NSP_HEADER_SIZE (16)
//! Size of the data contained in \a NSPRawPacket
#define NSP_DATA_SIZE   (254)

/**
 * NSPRawPacket:
 *
 * Raw packet for the Nspire NavNet protocol, version with pre-allocated packet data.
 **/
typedef struct
{
	uint16_t  unused;
	uint16_t  src_addr;
	uint16_t  src_port;
	uint16_t  dst_addr;
	uint16_t  dst_port;
	uint16_t  data_sum;
	uint8_t   data_size;
	uint8_t   ack;
	uint8_t   seq;
	uint8_t   hdr_sum;

	uint8_t   data[NSP_DATA_SIZE];
} NSPRawPacket;

/**
 * NSPRawPacketA:
 *
 * Raw packet for the Nspire NavNet protocol, version with externally allocated packet data.
 **/
typedef struct
{
	uint16_t  unused;
	uint16_t  src_addr;
	uint16_t  src_port;
	uint16_t  dst_addr;
	uint16_t  dst_port;
	uint16_t  data_sum;
	uint8_t   data_size;
	uint8_t   ack;
	uint8_t   seq;
	uint8_t   hdr_sum;

	uint8_t * data;
} NSPRawPacketA;

/**
 * NSPVirtualPacket:
 *
 * Virtual packet for the Nspire NavNet protocol.
 **/
typedef struct
{
	uint16_t  src_addr;
	uint16_t  src_port;
	uint16_t  dst_addr;
	uint16_t  dst_port;

	uint8_t   cmd;

	uint32_t  size;
	uint8_t  *data;
} NSPVirtualPacket;

/**
 * ROMDumpRawPacket:
 *
 * Packet for libticalcs' ROM dump protocol.
 **/
typedef struct
{
	uint16_t length;     ///< data length
	uint16_t cmd;        ///< protocol command

	uint8_t *data;       ///< packet data
} ROMDumpPacket;

/**
 * CalcScreenCoord:
 * @format: returns full or clipped image (#CalcScreenFormat)
 * @width: real width
 * @height: real height
 * @clipped_width: clipped width (89 for instance)
 * @clipped_height: clipped height (89 for instance)
 * @pixel_format: format of pixel data (#CalcPixelFormat)
 *
 * A structure used for storing screen size.
 **/
typedef struct
{
	int		format;

	unsigned int width;
	unsigned int height;

	unsigned int clipped_width;
	unsigned int clipped_height;

	CalcPixelFormat pixel_format;
} CalcScreenCoord;

/**
 * TreeInfo:
 * @model: hand-held model
 * @type: var or app list (VAR_NODE_NAME or APP_NODE_NAME)
 * @mem_used: memory used (depends on hand-held model)
 * @mem_free: memory free (depends on hand-held model)
 *
 * A structure used for storing information about a directory list tree.
 **/
typedef struct
{
	CalcModel	model;
	const char*	type;

	int			mem_mask;	// tells which field is filled
	uint32_t	mem_used;
	uint32_t	mem_free;
} TreeInfo;

/**
 * KeyPair:
 * @name: name of key (like "ESC")
 * @value: value of key (like 264)
 *
 * A structure which contains a TI scancode.
 **/
typedef struct
{
	const char* name;
	uint32_t value;
} KeyPair;

/**
 * CalcKey:
 * @key_name: name of key
 * @normal: information for key when pressed without modifiers
 * @shift: information for key when pressed with SHIFT modifier (TI-68k series)
 * @second: information for key when pressed with 2nd modifier (TI-Z80 & TI-68k series)
 * @diamond: information for key when pressed with DIAMOND modifier (TI-68k series)
 * @alpha: information for key when pressed with ALPHA modifier (TI-Z80 & TI-68k series)
 *
 * A structure which contains a TI scancode with key modifiers.
 **/
typedef struct
{
	const char* key_name;

	KeyPair normal;
	KeyPair shift;
	KeyPair second;
	KeyPair diamond;
	KeyPair alpha;
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

	int		state;
} CalcClock;

/**
 * CalcLabEquipmentDataType:
 *
 * An enumeration which contains the following data types:
 */
typedef enum
{
	CALC_LAB_EQUIPMENT_DATA_TYPE_NONE = 0,
	CALC_LAB_EQUIPMENT_DATA_TYPE_STRING = 1,
	CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST = 2,
	CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST = 3,
	CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST = 3,
} CalcLabEquipmentDataType;

/**
 * CalcLabEquipmentData:
 * @size: size of data to be sent
 * @data: pointer to data to be sent
 */
typedef struct
{
	CalcLabEquipmentDataType type;
	uint16_t size;
	uint16_t items;
	const uint8_t * data;
	uint16_t index;
	uint16_t unknown;
	uint8_t vartype;
} CalcLabEquipmentData;

/**
 * CalcUpdate:
 * @text: a text to display about the current operation (locale used is those defined by tifiles_transcoding_set)
 * @cancel: set to 1 if transfer have to be cancelled
 * @rate: data rate of cable
 * @cnt1: current counter for link transfer operations (ticalcs2 only)
 * @max1: max value of this counter
 * @cnt2: current count for intermediate operations (ticalcs2 only)
 * @max2: max value of this counter
 * @cnt3: current counter for global operations (used by ticalcs2 or tilp)
 * @max3: max value of this counter
 * @mask: which cntX is/are used (unused)
 * @type: pbar type (unused)
 * @start: init internal vars
 * @stop: release internal vars
 * @refresh: pass control to GUI for refresh
 * @pbar: refresh progress bar
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

	int		cnt3;
	int		max3;

	int		mask;
	int		type;

	void	(*start)	(void);
	void	(*stop)		(void);
	void	(*refresh)	(void);
	void	(*pbar)		(void);
	void	(*label)	(void);
} CalcUpdate;

typedef struct _CalcFncts	CalcFncts;
typedef struct _CalcHandle	CalcHandle;

typedef VarEntry	VarRequest;	// alias

/**
 * CalcInfos:
 * @os: OS version like "3.01"
 * @bios: BIOS (boot) version like 2.01
 *
 * A structure used to pass arguments.
 **/
typedef struct
{
	CalcModel	model;
	InfosMask	mask;

	char		product_name[64];
	char		product_id[32];
	uint32_t	product_number;		// obsolete, replaced by product_id
	char		main_calc_id[32];	// obsolete, replaced by product_id
	uint16_t	hw_version;			// hand-held-dependent
	uint8_t		language_id;
	uint8_t		sub_lang_id;
	uint16_t	device_type;		// hand-held-dependent
	char		boot_version[32];
	char		boot2_version[32];
	char		os_version[32];
	uint64_t	ram_phys;
	uint64_t	ram_user;
	uint64_t	ram_free;
	uint64_t	flash_phys;
	uint64_t	flash_user;
	uint64_t	flash_free;
	uint16_t	lcd_width;
	uint16_t	lcd_height;
	uint8_t		battery;			// 0 = low, 1 = good
	uint8_t		run_level;			// 1 = boot, 2 = OS
	uint16_t	bits_per_pixel;		// 1, 4 or 16
	uint16_t	clock_speed;
	uint8_t		exact_math; // should be named math_capabilities
	uint8_t		clock_support;
	uint8_t		color_screen;
	uint8_t		python_on_board;
	char		user_defined_id[32];
} CalcInfos;

/**
 * CalcFnctPtrs:
 * @is_ready: check whether calculator is ready
 * @send_key: send key value
 * @execute: remotely execute a program or application
 * @recv_screen: request a screendump
 * @get_dirlist: request a listing of variables, folders (if any) and apps (if any)
 * @send_backup: send a backup
 * @recv_backup: request a backup
 * @send_var: send a variable (silent mode)
 * @recv_var: request a variable silent mode)
 * @send_var_ns: send a variable (non-silent mode)
 * @recv_var_ns: receive a variable (non-silent mode)
 * @send_flash: send a FLASH app/os
 * @recv_flash: request a FLASH app/os
 * @recv_idlist: request hand-held IDLIST
 * @dump_rom_1: dump the hand-held ROM: send dumper (if any)
 * @dump_rom_2: dump the hand-held ROM: launch dumper
 * @set_clock: set date/time
 * @get_clock: get date/time
 * @del_var: delete variable
 * @new_fld: create new folder (if supported)
 * @get_version: returns Boot code & OS version
 * @send_cert: send certificate stuff
 * @recv_cert: receive certificate stuff
 * @rename_var: rename a variable
 * @change_attr: change attributes of a variable
 * @send_all_vars_backup: send a fake backup (set of files and FlashApps)
 * @recv_all_vars_backup: request a fake backup (set of files and FlashApps)
 * @send_lab_equipment_data: send data in a format suitable for lab equipment
 * @get_lab_equipment_data: get data from a piece of lab equipment
 * @del_fld: delete folder (if supported)
 *
 * A structure containing pointers to functions implementing the various operations (potentially) supported by a hand-held.
 * !!! This structure is for private use !!!
 **/
struct _CalcFnctPtrs
{
	int		(*is_ready)		(CalcHandle*);

	int		(*send_key)		(CalcHandle*, uint32_t);
	int		(*execute)		(CalcHandle*, VarEntry*, const char*);

	int		(*recv_screen)	(CalcHandle*, CalcScreenCoord*, uint8_t**);

	int		(*get_dirlist)	(CalcHandle*, GNode** vars, GNode** apps);
	int		(*get_memfree)	(CalcHandle*, uint32_t* ram, uint32_t* flash);

	int		(*send_backup)	(CalcHandle*, BackupContent*);
	int		(*recv_backup)	(CalcHandle*, BackupContent*);

	int		(*send_var)		(CalcHandle*, CalcMode, FileContent*);
	int		(*recv_var)		(CalcHandle*, CalcMode, FileContent*, VarRequest*);

	int		(*send_var_ns)	(CalcHandle*, CalcMode, FileContent*);
	int		(*recv_var_ns)	(CalcHandle*, CalcMode, FileContent*, VarEntry**);

	int		(*send_app)		(CalcHandle*, FlashContent*);
	int		(*recv_app)		(CalcHandle*, FlashContent*, VarRequest*);

	int		(*send_os)		(CalcHandle*, FlashContent*);
	int		(*recv_idlist)	(CalcHandle*, uint8_t*);

	int		(*dump_rom_1)	(CalcHandle*);
	int		(*dump_rom_2)	(CalcHandle*, CalcDumpSize, const char *filename);

	int		(*set_clock)	(CalcHandle*, CalcClock* clock);
	int		(*get_clock)	(CalcHandle*, CalcClock* clock);

	int		(*del_var)		(CalcHandle*, VarRequest*);
	int		(*new_fld)		(CalcHandle*, VarRequest*);

	int		(*get_version)	(CalcHandle*, CalcInfos*);

	int		(*send_cert)	(CalcHandle*, FlashContent*);
	int		(*recv_cert)	(CalcHandle*, FlashContent*);

	int		(*rename_var)	(CalcHandle*, VarRequest*, VarRequest*);
	int		(*change_attr)	(CalcHandle*, VarRequest*, FileAttr);

	int		(*send_all_vars_backup)	(CalcHandle*, FileContent*);
	int		(*recv_all_vars_backup)	(CalcHandle*, FileContent*);

	int		(*send_lab_equipment_data)	(CalcHandle*, CalcModel, CalcLabEquipmentData *);
	int		(*get_lab_equipment_data)	(CalcHandle*, CalcModel, CalcLabEquipmentData *);

	int		(*del_fld)		(CalcHandle*, VarRequest*);
};

/**
 * CalcFncts:
 * @model: link cable model (CalcModel).
 * @name: name of hand-held like "TI89"
 * @fullname: complete name of hand-held like "TI-89"
 * @description: description of hand-held like "TI89 calculator"
 * @features: supported operations (CalcOperations)
 * @counters: defines which CalcUpdate counters have to be refreshed (indexed by CalcFnctsIdx)
 * @fncts: function pointers for calculator operations
 *
 * A structure used for handling a hand-held.
 * !!! This structure is for private use !!!
 **/
struct _CalcFncts
{
	const int			model;
	const char*			name;
	const char*			fullname;
	const char*			description;
	const int			features;
	const CalcProductIDs		product_id;
	const char*			counters[CALC_FNCT_LAST];

	const struct _CalcFnctPtrs	fncts;
};

/**
 * CalcEventType:
 *
 * Defines the various events fired by libticalcs into a registered event hook, if any.
 */
typedef enum
{
	CALC_EVENT_TYPE_UNKNOWN = 0,
	CALC_EVENT_TYPE_BEFORE_CABLE_ATTACH,
	CALC_EVENT_TYPE_AFTER_CABLE_ATTACH,
	CALC_EVENT_TYPE_BEFORE_CABLE_DETACH,
	CALC_EVENT_TYPE_AFTER_CABLE_DETACH,

	CALC_EVENT_TYPE_BEFORE_SEND_DBUS_PKT = 16384,
	CALC_EVENT_TYPE_AFTER_SEND_DBUS_PKT,
	CALC_EVENT_TYPE_BEFORE_RECV_DBUS_PKT_HEADER,
	CALC_EVENT_TYPE_AFTER_RECV_DBUS_PKT_HEADER,
	CALC_EVENT_TYPE_BEFORE_RECV_DBUS_PKT_DATA,
	CALC_EVENT_TYPE_AFTER_RECV_DBUS_PKT_DATA,

	CALC_EVENT_TYPE_BEFORE_SEND_DUSB_RPKT,
	CALC_EVENT_TYPE_AFTER_SEND_DUSB_RPKT,
	CALC_EVENT_TYPE_BEFORE_RECV_DUSB_RPKT,
	CALC_EVENT_TYPE_AFTER_RECV_DUSB_RPKT,

	CALC_EVENT_TYPE_BEFORE_SEND_DUSB_VPKT,
	CALC_EVENT_TYPE_AFTER_SEND_DUSB_VPKT,
	CALC_EVENT_TYPE_BEFORE_RECV_DUSB_VPKT,
	CALC_EVENT_TYPE_AFTER_RECV_DUSB_VPKT,

	CALC_EVENT_TYPE_BEFORE_SEND_NSP_RPKT,
	CALC_EVENT_TYPE_AFTER_SEND_NSP_RPKT,
	CALC_EVENT_TYPE_BEFORE_RECV_NSP_RPKT,
	CALC_EVENT_TYPE_AFTER_RECV_NSP_RPKT,

	CALC_EVENT_TYPE_BEFORE_SEND_NSP_VPKT,
	CALC_EVENT_TYPE_AFTER_SEND_NSP_VPKT,
	CALC_EVENT_TYPE_BEFORE_RECV_NSP_VPKT,
	CALC_EVENT_TYPE_AFTER_RECV_NSP_VPKT,

	CALC_EVENT_TYPE_BEFORE_SEND_ROMDUMP_PKT,
	CALC_EVENT_TYPE_AFTER_SEND_ROMDUMP_PKT,
	CALC_EVENT_TYPE_BEFORE_RECV_ROMDUMP_PKT,
	CALC_EVENT_TYPE_AFTER_RECV_ROMDUMP_PKT,

	CALC_EVENT_TYPE_BEFORE_GENERIC_OPERATION = 32768,
	CALC_EVENT_TYPE_AFTER_GENERIC_OPERATION,
	CALC_EVENT_TYPE_USER = 49152
} CalcEventType;

/**
 * CalcEventData:
 * @version: event protocol version.
 * @type: event type.
 * @retval: return value of the operation, for "after" events.
 * @attached: whether the cable is attached.
 * @operation: index (CalcFnctsIdx) of the generic operation invoked by the library user, if any.
 * @data: packet data to be sent / received, if any - or user-specified data.
 *
 * Information returned for every event fired by the libticalcs library; only a subset of the fields is valid for some event types.
 */
typedef struct
{
	unsigned int version;
	CalcEventType type;
	int retval;
	int attached;
	int open;
	CalcFnctsIdx operation;
	CalcModel model;
	union
	{
		int intval;
		uint32_t uintval;
		void * ptrval;
		const void * cptrval;
		DBUSPacket dbus_pkt;
		DUSBRawPacketA dusb_rpkt;
		DUSBVirtualPacket dusb_vpkt;
		NSPRawPacketA nsp_rpkt;
		NSPVirtualPacket nsp_vpkt;
		ROMDumpPacket romdump_pkt;
		CalcLabEquipmentData labeq_data;
		struct
		{
			void * data;
			uint32_t len;
		} user_data;
	} data;
} CalcEventData;

typedef int (*ticalcs_event_hook_type)(CalcHandle * handle, uint32_t event_count, const CalcEventData * event, void * user_pointer);

/**
 * CalcHandle:
 * @model: cable model
 * @calc: calculator functions
 * @update: callbacks for GUI interaction
 * @unused1: unused member kept for API compatibility purposes
 * @buffer: allocated data buffer for internal use
 * @buffer2: allocated data buffer for internal use
 * @open: device has been opened
 * @busy: transfer is in progress
 * @cable: handle on cable used with this model
 * @attached: set if a cable has been attached
 * @priv: private per-handle data
 * @event_hook: callback fired upon various events (replaces and expands on the deprecated callbacks).
 * @user_pointer: user-set pointer passed to the event callbacks.
 * @event_count: number of events sent since this handle was created.
 *
 * A structure used to store information as a handle.
 * !!! This structure is for private use !!!
 **/
struct _CalcHandle
{
	CalcModel	model;
	CalcFncts*	calc;
	CalcUpdate*	updat;

	void*		unused1;
	void*		buffer;
	void*		buffer2;

	int			open;
	int			busy;

	CableHandle* cable;
	int			attached;

	ticalcs_event_hook_type event_hook;
	void * user_pointer;
	uint32_t event_count;

	struct {
		uint32_t dusb_rpkt_maxlen; // max length of data in raw packet
		unsigned int progress_blk_size; // refresh pbars every once in a while.
		unsigned int progress_min_size; // don't refresh if packet is smaller than some amount.
		unsigned int romdump_std_blk; // number of full-size blocks
		unsigned int romdump_sav_blk; // number of compressed blocks
		//void * dusb_vtl_pkt_list;
		//void * dusb_cpca_list;
		//void * nsp_vtl_pkt_list;
		uint8_t nsp_seq_pc;
		uint8_t nsp_seq;
		uint16_t nsp_src_port;
		uint16_t nsp_dst_port;
	} priv;
};

/**
 * DeviceOptions:
 * @cable_model: model
 * @cable_port: port
 * @cable_timeout: timeout in tenth of seconds
 * @cable_delay: inter-bit delay in µs
 * @calc_model: calculator model
 *
 * A convenient structure free of use by the user.
 **/
typedef struct
{
	CableModel cable_model;
	CablePort  cable_port;
	int        cable_timeout;
	int        cable_delay;
	CalcModel  calc_model;
} DeviceOptions;

// namespace scheme: library_class_function like ticalcs_fext_get

	/****************/
	/* Entry points */
	/****************/

	TIEXPORT3 int TICALL ticalcs_library_init(void);
	TIEXPORT3 int TICALL ticalcs_library_exit(void);

	/*********************/
	/* General functions */
	/*********************/

	// ticalcs.c
	TIEXPORT3 const char* TICALL ticalcs_version_get(void);
	TIEXPORT3 uint64_t    TICALL ticalcs_supported_calcs(void);
	TIEXPORT3 uint32_t    TICALL ticalcs_supported_protocols(void);
	TIEXPORT3 uint32_t    TICALL ticalcs_max_calc_function_idx(void);

	TIEXPORT3 CalcHandle* TICALL ticalcs_handle_new(CalcModel model);
	TIEXPORT3 int         TICALL ticalcs_handle_del(CalcHandle *handle);
	TIEXPORT3 int         TICALL ticalcs_handle_show(CalcHandle *handle);

	TIEXPORT3 CalcModel   TICALL ticalcs_get_model(CalcHandle *handle);

	TIEXPORT3 int TICALL ticalcs_cable_attach(CalcHandle *handle, CableHandle*);
	TIEXPORT3 int TICALL ticalcs_cable_detach(CalcHandle *handle);
	TIEXPORT3 CableHandle* TICALL ticalcs_cable_get(CalcHandle *handle);

	TIEXPORT3 int TICALL ticalcs_update_set(CalcHandle *handle, CalcUpdate*);
	TIEXPORT3 CalcUpdate* TICALL ticalcs_update_get(CalcHandle *handle);

	TIEXPORT3 int TICALL ticalcs_model_supports_dbus(CalcModel model);
	TIEXPORT3 int TICALL ticalcs_model_supports_dusb(CalcModel model);
	TIEXPORT3 int TICALL ticalcs_model_supports_nsp(CalcModel model);
	TIEXPORT3 int TICALL ticalcs_model_supports_installing_flashapps(CalcModel model);

	TIEXPORT3 ticalcs_event_hook_type TICALL ticalcs_calc_get_event_hook(CalcHandle *handle);
	TIEXPORT3 ticalcs_event_hook_type TICALL ticalcs_calc_set_event_hook(CalcHandle *handle, ticalcs_event_hook_type hook);
	TIEXPORT3 void * ticalcs_calc_get_event_user_pointer(CalcHandle *handle);
	TIEXPORT3 void * ticalcs_calc_set_event_user_pointer(CalcHandle *handle, void * user_pointer);
	TIEXPORT3 uint32_t TICALL ticalcs_calc_get_event_count(CalcHandle *handle);
	TIEXPORT3 int TICALL ticalcs_calc_fire_user_event(CalcHandle *handle, CalcEventType type, int retval, void * user_data, uint32_t user_len);

	// error.c
	TIEXPORT3 int         TICALL ticalcs_error_get (int number, char **message);
	TIEXPORT3 int         TICALL ticalcs_error_free (char *message);

	// calc_xx.c
	TIEXPORT3 CalcFeatures TICALL ticalcs_calc_features(CalcHandle *handle);

	TIEXPORT3 int TICALL ticalcs_calc_isready(CalcHandle *handle);

	TIEXPORT3 int TICALL ticalcs_calc_send_key(CalcHandle *handle, uint32_t);
	TIEXPORT3 int TICALL ticalcs_calc_execute(CalcHandle *handle, VarEntry*, const char*);

	TIEXPORT3 int TICALL ticalcs_calc_recv_screen(CalcHandle *handle, CalcScreenCoord* sc, uint8_t** bitmap);
	TIEXPORT3 int TICALL ticalcs_calc_recv_screen_rgb888(CalcHandle *handle, CalcScreenCoord* sc, uint8_t** bitmap);
	TIEXPORT3 void TICALL ticalcs_free_screen(uint8_t * bitmap);

	TIEXPORT3 int TICALL ticalcs_calc_get_dirlist(CalcHandle *handle, GNode** vars, GNode **apps);
	TIEXPORT3 int TICALL ticalcs_calc_get_memfree(CalcHandle *handle, uint32_t* ram, uint32_t *flash);

	TIEXPORT3 int TICALL ticalcs_calc_send_backup(CalcHandle *handle, BackupContent*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_backup(CalcHandle *handle, BackupContent*);

	TIEXPORT3 int TICALL ticalcs_calc_send_var(CalcHandle *handle, CalcMode, FileContent*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_var(CalcHandle *handle, CalcMode, FileContent*, VarRequest*);

	TIEXPORT3 int TICALL ticalcs_calc_send_var_ns(CalcHandle *handle, CalcMode, FileContent*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_var_ns(CalcHandle *handle, CalcMode, FileContent*, VarEntry**);

	TIEXPORT3 int TICALL ticalcs_calc_send_app(CalcHandle *handle, FlashContent*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_app(CalcHandle *handle, FlashContent*, VarRequest*);

	TIEXPORT3 int TICALL ticalcs_calc_send_os(CalcHandle *handle, FlashContent*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_idlist(CalcHandle *handle, uint8_t*);

	TIEXPORT3 int TICALL ticalcs_calc_dump_rom_1(CalcHandle *handle);
	TIEXPORT3 int TICALL ticalcs_calc_dump_rom_2(CalcHandle *handle, CalcDumpSize, const char *filename);

	TIEXPORT3 int TICALL ticalcs_calc_set_clock(CalcHandle *handle, CalcClock* clock);
	TIEXPORT3 int TICALL ticalcs_calc_get_clock(CalcHandle *handle, CalcClock* clock);

	TIEXPORT3 int TICALL ticalcs_calc_new_fld(CalcHandle *handle, VarRequest*);
	TIEXPORT3 int TICALL ticalcs_calc_del_var(CalcHandle *handle, VarRequest*);

	TIEXPORT3 int TICALL ticalcs_calc_get_version(CalcHandle *handle, CalcInfos*);

	TIEXPORT3 int TICALL ticalcs_calc_send_cert(CalcHandle *handle, FlashContent*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_cert(CalcHandle *handle, FlashContent*);

	TIEXPORT3 int TICALL ticalcs_calc_rename_var(CalcHandle *handle, VarRequest*, VarRequest*);
	TIEXPORT3 int TICALL ticalcs_calc_change_attr(CalcHandle *handle, VarRequest*, FileAttr);

	TIEXPORT3 int TICALL ticalcs_calc_send_all_vars_backup(CalcHandle *handle, FileContent*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_all_vars_backup(CalcHandle *handle, FileContent*);

	TIEXPORT3 int TICALL ticalcs_calc_send_lab_equipment_data(CalcHandle *handle, CalcModel, CalcLabEquipmentData *);
	TIEXPORT3 int TICALL ticalcs_calc_get_lab_equipment_data(CalcHandle *handle, CalcModel, CalcLabEquipmentData *);

	TIEXPORT3 int TICALL ticalcs_calc_del_fld(CalcHandle *handle, VarRequest*);

	TIEXPORT3 int TICALL ticalcs_calc_send_tigroup(CalcHandle *handle, TigContent*, TigMode);
	TIEXPORT3 int TICALL ticalcs_calc_recv_tigroup(CalcHandle *handle, TigContent*, TigMode);

	// calc_xx.c: convenient functions
	TIEXPORT3 int TICALL ticalcs_calc_send_backup2(CalcHandle *handle, const char*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_backup2(CalcHandle *handle, const char*);

	TIEXPORT3 int TICALL ticalcs_calc_send_var2(CalcHandle *handle, CalcMode, const char*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_var2(CalcHandle *handle, CalcMode, const char*, VarRequest*);

	TIEXPORT3 int TICALL ticalcs_calc_send_var_ns2(CalcHandle *handle, CalcMode, const char*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_var_ns2(CalcHandle *handle, CalcMode, const char*, VarEntry**);

	TIEXPORT3 int TICALL ticalcs_calc_send_app2(CalcHandle *handle, const char*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_app2(CalcHandle *handle, const char*, VarRequest*);

	TIEXPORT3 int TICALL ticalcs_calc_send_cert2(CalcHandle *handle, const char*);
	TIEXPORT3 int TICALL ticalcs_calc_recv_cert2(CalcHandle *handle, const char*);

	TIEXPORT3 int TICALL ticalcs_calc_send_os2(CalcHandle *handle, const char*);

	TIEXPORT3 int TICALL ticalcs_calc_send_lab_equipment_data2(CalcHandle *handle, CalcModel, uint8_t, const char *);
	TIEXPORT3 int TICALL ticalcs_calc_get_lab_equipment_data2(CalcHandle *handle, CalcModel, uint8_t, const char **);
	TIEXPORT3 void TICALL ticalcs_free_lab_equipment_data2(char * data);

	TIEXPORT3 int TICALL ticalcs_calc_send_tigroup2(CalcHandle *handle, const char*, TigMode);
	TIEXPORT3 int TICALL ticalcs_calc_recv_tigroup2(CalcHandle *handle, const char*, TigMode);

	// dirlist.c
	TIEXPORT3 void TICALL ticalcs_dirlist_destroy(GNode** tree);
	TIEXPORT3 void TICALL ticalcs_dirlist_display(GNode*  tree);

	TIEXPORT3 int TICALL ticalcs_dirlist_ram_used(GNode* tree);
	TIEXPORT3 int TICALL ticalcs_dirlist_flash_used(GNode* vars, GNode* apps);

	TIEXPORT3 unsigned int TICALL ticalcs_dirlist_ve_count(GNode* tree);
	TIEXPORT3 VarEntry *TICALL ticalcs_dirlist_ve_exist(GNode* tree, VarEntry *entry);
	TIEXPORT3 void TICALL ticalcs_dirlist_ve_add(GNode* tree, VarEntry *entry);
	TIEXPORT3 void TICALL ticalcs_dirlist_ve_del(GNode* tree, VarEntry *entry);

	// type2str.c
	TIEXPORT3 const char*  TICALL ticalcs_model_to_string(CalcModel model);
	TIEXPORT3 CalcModel    TICALL ticalcs_string_to_model (const char *str);

	TIEXPORT3 const char*      TICALL ticalcs_scrfmt_to_string(CalcScreenFormat format);
	TIEXPORT3 CalcScreenFormat TICALL ticalcs_string_to_scrfmt(const char *str);

	TIEXPORT3 const char*  TICALL ticalcs_pathtype_to_string(CalcPathType type);
	TIEXPORT3 CalcPathType TICALL ticalcs_string_to_pathtype(const char *str);

	TIEXPORT3 const char*  TICALL ticalcs_memtype_to_string(CalcMemType type);
	TIEXPORT3 CalcMemType  TICALL ticalcs_string_to_memtype(const char *str);

	TIEXPORT3 int TICALL ticalcs_infos_to_string(CalcInfos *infos, char *str, uint32_t maxlen);

	// clock.c
	TIEXPORT3 const char*  TICALL ticalcs_clock_format2date(CalcModel model, int value);
	TIEXPORT3 int          TICALL ticalcs_clock_date2format(CalcModel model, const char *format);
	TIEXPORT3 int          TICALL ticalcs_clock_show(CalcModel model, CalcClock* s);

	// screen.c
	TIEXPORT3 int TICALL ticalcs_screen_convert_bw_to_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst);
	TIEXPORT3 int TICALL ticalcs_screen_convert_bw_to_blurry_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst);
	TIEXPORT3 int TICALL ticalcs_screen_convert_gs4_to_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst);
	TIEXPORT3 int TICALL ticalcs_screen_convert_rgb565le_to_rgb888(const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst);
	TIEXPORT3 int TICALL ticalcs_screen_convert_native_to_rgb888(CalcPixelFormat format, const uint8_t * src, unsigned int width, unsigned int height, uint8_t * dst);
	TIEXPORT3 int TICALL ticalcs_screen_nspire_rle_uncompress(CalcPixelFormat format, const uint8_t * src, uint32_t input_size, uint8_t * dst, uint32_t max_output_size);
	TIEXPORT3 int TICALL ticalcs_screen_84pcse_rle_uncompress(const uint8_t * src, uint32_t src_length, uint8_t * dst, uint32_t dst_length);

	// tikeys.c
	TIEXPORT3 const CalcKey* TICALL ticalcs_keys_73 (uint8_t ascii_code);
	TIEXPORT3 const CalcKey* TICALL ticalcs_keys_83 (uint8_t ascii_code);
	TIEXPORT3 const CalcKey* TICALL ticalcs_keys_83p(uint8_t ascii_code);
	TIEXPORT3 const CalcKey* TICALL ticalcs_keys_86 (uint8_t ascii_code);
	TIEXPORT3 const CalcKey* TICALL ticalcs_keys_89 (uint8_t ascii_code);
	TIEXPORT3 const CalcKey* TICALL ticalcs_keys_92p(uint8_t ascii_code);

	// probe.c
	TIEXPORT3 int TICALL ticalcs_probe_calc  (CableHandle* cable, CalcModel* model);
	TIEXPORT3 int TICALL ticalcs_probe_usb_calc(CableHandle* cable, CalcModel* model);

	TIEXPORT3 int TICALL ticalcs_probe(CableModel c_model, CablePort c_port, CalcModel* model, int all);

	TIEXPORT3 CalcModel TICALL ticalcs_device_info_to_model(const CableDeviceInfo *info);
	TIEXPORT3 CalcModel TICALL ticalcs_remap_model_from_usb(CableModel cable, CalcModel calc);
	TIEXPORT3 CalcModel TICALL ticalcs_remap_model_to_usb(CableModel cable, CalcModel calc);

	// dbus_pkt.c
	TIEXPORT3 int TICALL dbus_send(CalcHandle *handle, uint8_t target, uint8_t cmd, uint16_t length, uint8_t* data);
	TIEXPORT3 int TICALL dbus_recv(CalcHandle *handle, uint8_t* host, uint8_t* cmd, uint16_t* length, uint8_t* data);
	TIEXPORT3 int TICALL dbus_recv_header(CalcHandle *handle, uint8_t* host, uint8_t* cmd, uint16_t* length);
	TIEXPORT3 int TICALL dbus_recv_data(CalcHandle *handle, uint16_t* length, uint8_t* data);

	// dusb_rpkt.c
	TIEXPORT3 int TICALL dusb_send(CalcHandle *handle, DUSBRawPacket* pkt);
	TIEXPORT3 int TICALL dusb_recv(CalcHandle *handle, DUSBRawPacket* pkt);

	// nsp_rpkt.c
	TIEXPORT3 int TICALL nsp_send(CalcHandle *handle, NSPRawPacket* pkt);
	TIEXPORT3 int TICALL nsp_recv(CalcHandle *handle, NSPRawPacket* pkt);

	/************************/
	/* Deprecated functions */
	/************************/

#ifdef __cplusplus
}
#endif

#endif
