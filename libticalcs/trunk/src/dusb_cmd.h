/* Hey EMACS -*- linux-c -*- */
/* $Id: cmd84p.h 2074 2006-03-31 08:36:06Z roms $ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
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

// /!\ NOTE: for this file, backwards compatibility will not necessarily be maintained as strongly as it is for ticalcs.h !

#ifndef __DUSB_CMDS__
#define __DUSB_CMDS__

#ifdef __cplusplus
extern "C" {
#endif

#define DUSB_DFL_BUF_SIZE	1024

// Parameter IDs
#define DUSB_PID_PRODUCT_NUMBER         0x0001
#define DUSB_PID_PRODUCT_NAME           0x0002
#define DUSB_PID_MAIN_PART_ID           0x0003
#define DUSB_PID_HW_VERSION             0x0004
#define DUSB_PID_FULL_ID                0x0005
#define DUSB_PID_LANGUAGE_ID            0x0006
#define DUSB_PID_SUBLANG_ID             0x0007
#define DUSB_PID_DEVICE_TYPE            0x0008
#define DUSB_PID_BOOT_VERSION           0x0009
#define DUSB_PID_OS_MODE                0x000A
#define DUSB_PID_OS_VERSION             0x000B
#define DUSB_PID_PHYS_RAM               0x000C
#define DUSB_PID_USER_RAM               0x000D
#define DUSB_PID_FREE_RAM               0x000E
#define DUSB_PID_PHYS_FLASH             0x000F
#define DUSB_PID_USER_FLASH             0x0010
#define DUSB_PID_FREE_FLASH             0x0011
#define DUSB_PID_USER_PAGES             0x0012
#define DUSB_PID_FREE_PAGES             0x0013
// 0x0014-0x0018 (no access): 84+SE OS 2.43, 84+CSE OS 4.0, 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) and 89T AMS 3.10 refuse being requested these parameter IDs.
#define DUSB_PID_HAS_SCREEN             0x0019
// 0x001A (read-only): 84+SE OS 2.43, 84+CSE OS 4.0 and 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) reply 00, 89T AMS 3.10 refuses being requested this parameter ID.
#define DUSB_PID_COLOR_AVAILABLE        0x001B
// 0x001C (read-only): 84+SE OS 2.43 replies 01, 84+CSE OS 4.0 and 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) reply 10, 89T AMS 3.10 refuses being requested this parameter ID.
#define DUSB_PID_COLOR_DEPTH            0x001C
#define DUSB_PID_BITS_PER_PIXEL         0x001D
#define DUSB_PID_LCD_WIDTH              0x001E
#define DUSB_PID_LCD_HEIGHT             0x001F
// 0x0020 (read-only): 84+SE OS 2.43 and 84+CSE OS 4.0 refuse being requested this parameter ID, 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 00 C8, 89T AMS 3.10 replies 00 23.
// 0x0021 (read-only): 84+SE OS 2.43 and 84+CSE OS 4.0 refuse being requested this parameter ID, 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 00 C8, 89T AMS 3.10 replies 00 23.
#define DUSB_PID_SCREENSHOT             0x0022
// 0x0023 (read-only): 84+SE OS 2.43, 84+CSE OS 4.0 and 89T AMS 3.10 reply 01; 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) refuses being requested this parameter ID.
#define DUSB_PID_CLASSIC_CLK_SUPPORT    0x0023
#define DUSB_PID_CLK_ON                 0x0024
#define DUSB_PID_CLK_SEC_SINCE_1997     0x0025
// 0x0026 (read-write): 84+SE OS 2.43 and 84+CSE OS 4.0 refuse being requested or set this parameter ID, 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) and 89T AMS 3.10 reply 00 00.
#define DUSB_PID_CLK_TZ                 0x0026
#define DUSB_PID_CLK_DATE_FMT           0x0027
#define DUSB_PID_CLK_TIME_FMT           0x0028
// 0x0029 (read-only): 84+SE OS 2.43 replies 00, 84+CSE OS 4.0 and 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) reply 01, 89T AMS 3.10 refuses being requested this parameter ID.
// 0x002A: 84+SE OS 2.43, 84+CSE OS 4.0, 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) and 89T AMS 3.10 refuse being requested this parameter ID.
// 0x002B (read-write): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies initially 00. 84+SE OS 2.43, 84+CSE OS 4.0 and 89T AMS 3.10 refuse being requested or set this parameter ID.
// 0x002C (read-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 00. 84+SE OS 2.43, 84+CSE OS 4.0 and 89T AMS 3.10 refuse being requested this parameter ID.
// Enough battery for Flash operations?
#define DUSB_PID_BATTERY_ENOUGH         0x002D
// 0x002E (read-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 64, 89T AMS 3.10 replies 01, 84+SE OS 2.43 and 84+CSE OS 4.0 refuse being requested this parameter ID.
#define DUSB_PID_BATTERY_LEVEL          0x002E
// 0x002F (read-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 01. 84+SE OS 2.43, 84+CSE OS 4.0 and 89T AMS 3.10 refuse being requested this parameter ID.
#define DUSB_PID_HAS_EXTERNAL_POWER     0x002F
// 0x0030 (read-write): 84+SE OS 2.43, 84+CSE OS 4.0 and 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) reply with 0x20 (32) bytes of data, initially all zeros. 89T AMS 3.10 refuses being requested this parameter ID.
// Data does not survive RAM clear. DUSB counterpart of DBUS RID + SID pair.
// 84+SE OS 2.43 reacts oddly to writes: written data does not necessarily read back ?
#define DUSB_PID_USER_DATA_1            0x0030
#define DUSB_PID_FLASHAPPS              0x0031 // 0x0031 (read-only): 83PCE (OS 5.1.5.0019, 5.2.0.0035) and 89T AMS 3.10 reply with a subset of FlashApp headers. 84+SE and 84+CSE perform a lengthy operation.
// 0x0032 (read-only): 84+SE OS 2.43, 84+CSE OS 4.0 and 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) reply 00, 89T AMS 3.10 refuses being requested this parameter ID.
// 0x0033-0x0034 (no access): 84+SE OS 2.43, 84+CSE OS 4.0, 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) and 89T AMS 3.10 refuse being requested these parameter IDs.
// 0x0035 (read-write): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies with 0x0A (10) bytes of data, initially all zeros. 84+SE OS 2.43, 84+CSE OS 4.0 and 89T AMS 3.10 refuse being requested this parameter ID.
// Data does not survive RAM clear. Behaves like PID 0x0030, only smaller. May have another purpose ?
#define DUSB_PID_USER_DATA_2            0x0035
#define DUSB_PID_MAIN_PART_ID_STRING    0x0036
#define DUSB_PID_HOMESCREEN             0x0037
#define DUSB_PID_BUSY                   0x0038
#define DUSB_PID_SCREEN_SPLIT           0x0039
// ---------- 84+SE OS 2.43, 84+CSE OS 4.0 and 89T AMS 3.10 refuse being requested or set parameter IDs beyond this ----------
// 0x003A (read-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 01
#define DUSB_PID_NEW_CLK_SUPPORT        0x003A
#define DUSB_PID_CLK_SECONDS            0x003B
#define DUSB_PID_CLK_MINUTES            0x003C
#define DUSB_PID_CLK_HOURS              0x003D
#define DUSB_PID_CLK_DAY                0x003E
#define DUSB_PID_CLK_MONTH              0x003F
#define DUSB_PID_CLK_YEAR               0x0040
// 0x0041 (read-write): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) initially replies 07 D0. Value is range-checked.
// 0x0042 (read-write): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 00
// 0x0043 (read-write): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 00
// 0x0044 (read-write): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 00
// 0x0045 (read-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies F0 0F 00 00
#define DUSB_PID_ANS_TYPE               0x0045
#define DUSB_PID_ANS                    0x0046
// 0x0047 (read-only): 83PCE replies 00 (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035)
#define DUSB_PID_OS_BUILD_NUMBER        0x0048
#define DUSB_PID_BOOT_BUILD_NUMBER      0x0049
// 0x004A (read-only): 83PCE replies 00 (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035)
#define DUSB_PID_LAST_ERROR_CODE        0x004A
#define DUSB_PID_MATH_CAPABILITIES      0x004B
// 0x004C (read-only): 83PCE replies with 0x20 (32) bytes of data, no clear pattern: boot code SHA-256 hash.
// boot 5.0.0.0089: D6 98 7E 21 90 54 2F 1C 32 75 F5 EC A1 AF DF B5
//                  B2 20 14 A2 D3 E7 65 04 52 B1 D1 BD 3D 9D 1D 18
#define DUSB_PID_BOOT_HASH              0x004C
// 0x004D (read-only): 83PCE replies with 0x20 (32) bytes of data, no clear pattern: OS SHA-256 hash.
// OS 5.1.0.0110: 0D 83 11 A0 3C 9D 74 F0 6D 8C A4 22 6E 9A 30 BC
//                4F 87 E0 0C 7A 18 7A 6F 01 FC 3E 0C 04 E2 B7 88
// OS 5.1.1.0112: 03 65 22 56 EA 98 7C AE AD A4 29 85 70 A4 9D FA
//                05 28 97 71 0E 65 0B D7 DE 5F 15 93 1D A6 7C DB
// OS 5.1.5.0019: 1C 9A CA 19 26 00 41 B6 0A C4 C8 FB D0 B9 C3 72
//                AA 4F 1B 6C DC 49 B4 23 58 C6 14 E7 5E D6 D8 3D
// OS 5.2.0.0035: C4 52 E6 F4 8C 78 37 13 B8 AB B7 FE F2 20 DD 12
//                C5 C3 28 BA 23 BE A6 F3 68 57 77 DA 4F A5 C3 79
#define DUSB_PID_OS_HASH                0x004D
// 0x004E (write-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) refuses being requested these parameter IDs but acknowledges writes.
// 0x004F (write-only, multiple writes OR together): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) refuses being requested these parameter IDs but acknowledges writes.
// Writing 01 00 00 01 enables PTT mode with features 01 00 00.
// Writing 01 23 45 67 enables PTT mode with features 23 45 67.
// Writing 01 FE DC BA on top of the previous write enables PTT mode with features FF DD FF.
// Writing 01 00 02 00 on top of the previous write enables PTT mode with features FF DF FF.
// Right after writing 01 xx xx xx, the PTT mode is not completely activated yet: the bar at the top of the screen is not automatically updated, the PTT LED doesn't blink.
// Has a side effect on cursor position: moves it to the top left of the screen, overwriting top bar ??
#define DUSB_PID_PTT_MODE_SET           0x004F
// 0x0050 (write-only, but values sometimes refused ?): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) refuses being requested these parameter IDs.
// 0x0051 (no access): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) refuses being requested these parameter IDs
#define DUSB_PID_OS_VERSION_STRING      0x0052
#define DUSB_PID_BOOT_VERSION_STRING    0x0053
// 0x0054 (read-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 00 when not in PTT mode, 01 when in PTT mode.
#define DUSB_PID_PTT_MODE_STATE         0x0054
// 0x0055 (read-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies 00 00 00 when not in PTT mode, 28 02 00 when in default PTT mode, and whatever was written there
#define DUSB_PID_PTT_MODE_FEATURES      0x0055
// 0x0056-0x0057 (read-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) replies with invalid format.
// 0x0058 (write-once): 83PCE OS 5.2.0.0035 replies with size 0. Error code 0012 occurs upon subsequent writes.
// 0x0059 (write-only): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) refuses being requested this parameter ID. But it can be written on OS 5.2.0.0035.
#define DUSB_PID_STOPWATCH_START        0x0059
// 0x005A (read-only): 83PCE OS 5.2.0.0035 replies 4 bytes. Fast up-counter, which runs even when stopwatch is stopped, reset by reads but not by keypresses.
// 0x005B (read-only): 83PCE OS 5.2.0.0035 replies 4 bytes. Fast up-counter, which runs only when stopwatch is started, not reset by reads.
#define DUSB_PID_STOPWATCH_VALUE1       0x005B
// 0x005C (read-only): 83PCE OS 5.2.0.0035 replies 4 bytes. Fast up-counter, which runs only when stopwatch is started, not reset by reads.
#define DUSB_PID_STOPWATCH_VALUE2       0x005C
// 0x005D-0x008F (no access): 83PCE (OS 5.1.0.0110, 5.1.1.0112, 5.1.5.0019, 5.2.0.0035) refuses being requested these parameter IDs; writes yield a 0004 error code.
// Newer TI-eZ80 OS versions accept 0x005D.
#define DUSB_PID_PYTHON_ON_BOARD        0x005D


// Attributes IDs
#define DUSB_AID_VAR_SIZE               0x01
#define DUSB_AID_VAR_TYPE               0x02
#define DUSB_AID_ARCHIVED               0x03
#define DUSB_AID_UNKNOWN_04             0x04
#define DUSB_AID_4APPVAR                0x05
#define DUSB_AID_VAR_VERSION            0x08
#define DUSB_AID_VAR_TYPE2              0x11
#define DUSB_AID_ARCHIVED2              0x13
#define DUSB_AID_LOCKED                 0x41
#define DUSB_AID_UNKNOWN_42             0x42
#define DUSB_AID_BACKUP_HEADER          0xFFFE

// Execute commands
#define DUSB_EID_PRGM                   0x00
#define DUSB_EID_ASM                    0x01
#define DUSB_EID_APP                    0x02
#define DUSB_EID_KEY                    0x03
#define DUSB_EID_UNKNOWN                0x04

// Modes
#define DUSB_MODE_STARTUP               { 1, 1, 0, 0, 0x07d0 }
#define DUSB_MODE_BASIC                 { 2, 1, 0, 0, 0x07d0 }
#define DUSB_MODE_NORMAL                { 3, 1, 0, 0, 0x07d0 }

// Structures
typedef struct
{
	uint16_t	arg1;
	uint16_t	arg2;
	uint16_t	arg3;
	uint16_t	arg4;
	uint16_t	arg5;
} DUSBModeSet;

typedef struct
{
	uint16_t	id;
	uint8_t		ok;
	uint16_t	size;
	uint8_t*	data;
} DUSBCalcParam;

typedef struct
{
	uint16_t	id;
	uint8_t		ok;
	uint16_t	size;
	uint8_t*	data;
} DUSBCalcAttr;

// Type to string
TIEXPORT3 const char* TICALL dusb_cmd_param_type2name(uint16_t id);

// Helpers for calc params and attrs.
TIEXPORT3 DUSBCalcParam *  TICALL dusb_cp_new(CalcHandle * handle, uint16_t id, uint16_t size);
TIEXPORT3 DUSBCalcParam *  TICALL dusb_cp_new_ex(CalcHandle * handle, uint16_t id, uint16_t size, uint8_t * data);
TIEXPORT3 void             TICALL dusb_cp_fill(DUSBCalcParam * cp, uint16_t id, uint16_t size, uint8_t * data);
TIEXPORT3 void             TICALL dusb_cp_del(CalcHandle * handle, DUSBCalcParam * cp);
TIEXPORT3 DUSBCalcParam ** TICALL dusb_cp_new_array(CalcHandle * handle, unsigned int size);
TIEXPORT3 DUSBCalcParam *  TICALL dusb_cp_new_array2(CalcHandle * handle, unsigned int size);
TIEXPORT3 void             TICALL dusb_cp_del_array(CalcHandle * handle, DUSBCalcParam ** params, unsigned int size);
TIEXPORT3 void             TICALL dusb_cp_del_array2(CalcHandle * handle, DUSBCalcParam * params, unsigned int size, int is_allocated);
TIEXPORT3 void *           TICALL dusb_cp_alloc_data(CalcHandle * handle, uint16_t size);
TIEXPORT3 DUSBCalcParam *  TICALL dusb_cp_realloc_data(CalcHandle * handle, DUSBCalcParam * cp, uint16_t size);
TIEXPORT3 void             TICALL dusb_cp_free_data(CalcHandle * handle, void * data);
TIEXPORT3 void             TICALL dusb_cp_free_array_data(CalcHandle * handle, DUSBCalcParam * params, unsigned int size);

TIEXPORT3 DUSBCalcAttr  *  TICALL dusb_ca_new(CalcHandle * handle, uint16_t id, uint16_t size);
TIEXPORT3 DUSBCalcAttr  *  TICALL dusb_ca_new_ex(CalcHandle * handle, uint16_t id, uint16_t size, uint8_t * data);
TIEXPORT3 void             TICALL dusb_ca_fill(DUSBCalcAttr * ca, uint16_t id, uint16_t size, uint8_t * data);
TIEXPORT3 void             TICALL dusb_ca_del(CalcHandle * handle, DUSBCalcAttr * ca);
TIEXPORT3 DUSBCalcAttr  ** TICALL dusb_ca_new_array(CalcHandle * handle, unsigned int size);
TIEXPORT3 DUSBCalcAttr  *  TICALL dusb_ca_new_array2(CalcHandle * handle, unsigned int size);
TIEXPORT3 void             TICALL dusb_ca_del_array(CalcHandle * handle, DUSBCalcAttr ** attrs, unsigned int size);
TIEXPORT3 void             TICALL dusb_ca_del_array2(CalcHandle * handle, DUSBCalcAttr * attrs, unsigned int size, int is_allocated);
TIEXPORT3 void *           TICALL dusb_ca_alloc_data(CalcHandle * handle, uint16_t size);
TIEXPORT3 DUSBCalcAttr  *  TICALL dusb_ca_realloc_data(CalcHandle * handle, DUSBCalcAttr * cp, uint16_t size);
TIEXPORT3 void             TICALL dusb_ca_free_data(CalcHandle * handle, void * data);
TIEXPORT3 void             TICALL dusb_ca_free_array_data(CalcHandle * handle, DUSBCalcAttr * attrs, unsigned int size);

// Command wrappers
TIEXPORT3 int TICALL dusb_cmd_s_mode_set(CalcHandle *handle, const DUSBModeSet mode);

TIEXPORT3 int TICALL dusb_cmd_s_os_begin(CalcHandle *handle, uint32_t size);
TIEXPORT3 int TICALL dusb_cmd_r_os_ack(CalcHandle *handle, uint32_t *size);

TIEXPORT3 int TICALL dusb_cmd_s_os_header(CalcHandle *handle, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data);
TIEXPORT3 int TICALL dusb_cmd_s_os_data(CalcHandle *handle, uint16_t addr, uint8_t page, uint8_t flag, uint32_t size, uint8_t *data);

TIEXPORT3 int TICALL dusb_cmd_s_os_header_89(CalcHandle *handle, uint32_t size, uint8_t *data);
TIEXPORT3 int TICALL dusb_cmd_s_os_data_89(CalcHandle *handle, uint32_t size, uint8_t *data);
TIEXPORT3 int TICALL dusb_cmd_s_os_data_834pce(CalcHandle *handle, uint32_t addr, uint32_t size, uint8_t *data);

TIEXPORT3 int TICALL dusb_cmd_r_eot_ack(CalcHandle *handle);

TIEXPORT3 int TICALL dusb_cmd_s_param_request(CalcHandle *handle, unsigned int npids, const uint16_t *pids);
TIEXPORT3 int TICALL dusb_cmd_r_param_data(CalcHandle *handle, unsigned int nparams, DUSBCalcParam **params);
TIEXPORT3 int TICALL dusb_cmd_r_screenshot(CalcHandle *handle, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL dusb_cmd_s_dirlist_request(CalcHandle *handle, unsigned int naids, const uint16_t *aids);
TIEXPORT3 int TICALL dusb_cmd_r_var_header(CalcHandle *handle, char *folder, char *name, DUSBCalcAttr **attr);

TIEXPORT3 int TICALL dusb_cmd_s_rts(CalcHandle *handle, const char *folder, const char *name, uint32_t size, unsigned int nattrs, const DUSBCalcAttr **attrs);
TIEXPORT3 int TICALL dusb_cmd_s_rts_ns(CalcHandle *handle, const char *folder, const char *name, uint32_t size, unsigned int nattrs, const DUSBCalcAttr **attrs);
TIEXPORT3 int TICALL dusb_cmd_s_var_request(CalcHandle *handle, const char *folder, const char *name, unsigned int naids, const uint16_t *aids, unsigned int nattrs, const DUSBCalcAttr **attrs);

TIEXPORT3 int TICALL dusb_cmd_s_var_content(CalcHandle *handle, uint32_t  size, uint8_t  *data);
TIEXPORT3 int TICALL dusb_cmd_r_var_content(CalcHandle *handle, uint32_t *size, uint8_t **data);

TIEXPORT3 int TICALL dusb_cmd_s_param_set(CalcHandle *handle, const DUSBCalcParam *param);

TIEXPORT3 int TICALL dusb_cmd_s_var_modify(CalcHandle *handle, const char *src_folder, const char *src_name, unsigned int n_src_attrs, const DUSBCalcAttr **src_attrs, const char *dst_folder, const char *dst_name, unsigned int n_dst_attrs, const DUSBCalcAttr **dst_attrs);
TIEXPORT3 int TICALL dusb_cmd_s_var_delete(CalcHandle *handle, const char *folder, const char *name, unsigned int nattrs, const DUSBCalcAttr **attrs);
TIEXPORT3 int TICALL dusb_cmd_s_execute(CalcHandle *handle, const char *folder, const char *name, uint8_t action, const char *args, uint16_t code);

TIEXPORT3 int TICALL dusb_cmd_r_mode_ack(CalcHandle *handle);
TIEXPORT3 int TICALL dusb_cmd_r_data_ack(CalcHandle *handle);

TIEXPORT3 int TICALL dusb_cmd_r_delay_ack(CalcHandle *handle);

TIEXPORT3 int TICALL dusb_cmd_s_eot(CalcHandle *handle);
TIEXPORT3 int TICALL dusb_cmd_r_eot(CalcHandle *handle);

TIEXPORT3 int TICALL dusb_cmd_s_error(CalcHandle *handle, uint16_t code);

// Higher-level helpers for command wrappers
TIEXPORT3 int TICALL dusb_cmd_s_param_set_r_data_ack(CalcHandle *handle, uint16_t id, uint16_t size, const uint8_t * data);

#ifdef __cplusplus
}
#endif

#endif
