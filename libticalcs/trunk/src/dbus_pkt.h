/* Hey EMACS -*- linux-c -*- */
/* $Id: packets.h 1179 2005-06-06 14:42:32Z roms $ */

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

#ifndef __TICALCS_DBUS__
#define __TICALCS_DBUS__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/*************/
/* Constants */
/*************/

// Machine IDs (host <-> target)

#define DBUS_MID_PC_TIXX   0x00

#define DBUS_MID_PC_TI73   0x07
#define DBUS_MID_TI73_PC   0x74
#define DBUS_MID_CBL_TI73  0x12
#define DBUS_MID_TI73_CBL  0x95

#define DBUS_MID_PC_TI80   0x00
#define DBUS_MID_TI80_PC   0x80

#define DBUS_MID_PC_TI82   0x02
#define DBUS_MID_TI82_PC   0x82
#define DBUS_MID_CBL_TI82  0x12
#define DBUS_MID_TI82_CBL  0x82

#define DBUS_MID_PC_TI83   0x03
#define DBUS_MID_TI83_PC   0x83
#define DBUS_MID_CBL_TI83  0x12
#define DBUS_MID_TI83_CBL  0x95

#define DBUS_MID_PC_TI83p  0x23
#define DBUS_MID_TI83p_PC  0x73
#define DBUS_MID_CBL_TI83p 0x12
#define DBUS_MID_TI83p_CBL 0x95

#define DBUS_MID_PC_TI84p  0x23
#define DBUS_MID_TI84p_PC  0x73
#define DBUS_MID_CBL_TI84p 0x12
#define DBUS_MID_TI84p_CBL 0x95

#define DBUS_MID_PC_TI85   0x05
#define DBUS_MID_TI85_PC   0x85
#define DBUS_MID_CBL_TI85  0x15
#define DBUS_MID_TI85_CBL  0x85

#define DBUS_MID_PC_TI86   0x06
#define DBUS_MID_TI86_PC   0x86
#define DBUS_MID_CBL_TI86  0x15
#define DBUS_MID_TI86_CBL  0x85

#define DBUS_MID_PC_TI89   0x08
#define DBUS_MID_TI89_PC   0x98
#define DBUS_MID_CBL_TI89  0x19
#define DBUS_MID_TI89_CBL  0x89

#define DBUS_MID_PC_TI89t  0x08
#define DBUS_MID_TI89t_PC  0x98
#define DBUS_MID_CBL_TI89t 0x19
#define DBUS_MID_TI89t_CBL 0x89

#define DBUS_MID_PC_TI92   0x09
#define DBUS_MID_TI92_PC   0x89
#define DBUS_MID_CBL_TI92  0x19
#define DBUS_MID_TI92_CBL  0x89

#define DBUS_MID_PC_TI92p  0x08
#define DBUS_MID_TI92p_PC  0x88
#define DBUS_MID_CBL_TI92p 0x19
#define DBUS_MID_TI92p_CBL 0x89

#define DBUS_MID_PC_V200   0x08
#define DBUS_MID_V200_PC   0x88
#define DBUS_MID_CBL_V200  0x19
#define DBUS_MID_V200_CBL  0x89

#define DBUS_MID_CBL2_PC         0x42
#define DBUS_MID_LABPRO_PC       0x42
#define DBUS_MID_PC_TIPRESENTER  0x33
#define DBUS_MID_TIPRESENTER_PC  0x43

// Command IDs
/*
 * means there is data attached.
 # means the device id is ignored with this command.
 $ means it's a silent command
*/
// Official names found in TI's DBUS implementation in the activitygrapher TINCS Navigator plugin.
#define DBUS_CMD_VAR  0x06      // [*  ] Variable Header - includes a std variable header (official name: RTS)
#define DBUS_CMD_CTS  0x09      // [   ] Continue - used to signal OK to send a variable (official name: RDY)
#define DBUS_CMD_XDP  0x15      // [*  ] Data packet - Pure data (official name: XDP)
#define DBUS_CMD_ELD  0x29      // [  $] Enable Lockdown (official name: EBL)
#define DBUS_CMD_DLD  0x2A      // [  $] Disable Lockdown (official name: DBL)
#define DBUS_CMD_EKE  0x2B      // [  $] Enable Key Echo - start sending keypresses as scan codes in remote control packets (official name: EKE)
#define DBUS_CMD_DKE  0x2C      // [  $] Disable Key Echo (official name: DKE)
#define DBUS_CMD_VER  0x2D      // [  $] Request version (official name: RDI)
#define DBUS_CMD_SKP  0x36      // [*  ] Skip/Exit - used when duplicate name is found (official name: EOR)
#define DBUS_CMD_SID  0x47      // [*#$] Send calc ID (official name: SIOA)
#define DBUS_CMD_ACK  0x56      // [   ] Acknowledge (official name: ACK)
#define DBUS_CMD_ERR  0x5A      // [   ] Checksum error: send last packet again (official name: CHK)
#define DBUS_CMD_RDY  0x68      // [ #$] Test if calc is ready (official name: RDC)
#define DBUS_CMD_SCR  0x6D      // [  $] Request screenshot (official name: REQ)
#define DBUS_CMD_GID  0x6E      // [  $] Get Calculator ID - on 83+ reads 9 bytes right from cert
#define DBUS_CMD_DMP  0x6F      // [* $] Request dump of memory page
#define DBUS_CMD_TG2  0x70      // [* $] Testguard2 packet - its long and complicated.
#define DBUS_CMD_RID  0x74      // [ #$] Request calc ID (official name: RIOA)
#define DBUS_CMD_CNT  0x78      // [  $] Continue (official name: RTC)
#define DBUS_CMD_KEY  0x87      // [  $] Send key (official name: SKY)
#define DBUS_CMD_DEL  0x88      // [* $] Delete variable/app (official name: DVL)
#define DBUS_CMD_RUN  0x89      // [* $] Run variable/app (official name: EPL)
#define DBUS_CMD_EOT  0x92      // [   ] End Of Transmission: no more variables to send (official name: DONE)
#define DBUS_CMD_REQ  0xA2      // [* $] Request variable - includes a std var header (official name: GET)
#define DBUS_CMD_ERR2 0xB4      // [   ] Checksum error: send last packet again
#define DBUS_CMD_IND  0xB7      // [* $] Request variable index (official name: GTE)
#define DBUS_CMD_RTS  0xC9      // [* $] Request to send - includes a padded var header (official name: SEND)
#define DBUS_CMD_RSE  0xCA      // [* $] Request to send and execute - same as above but runs it after send (official name: SETP)

// Rejection codes (CMD82_SKIP)

#define DBUS_REJ_EXIT     1
#define DBUS_REJ_SKIP     2
#define DBUS_REJ_MEMORY   3
#define DBUS_REJ_VERSION  4

// Functions

TIEXPORT3 const char* TICALL dbus_cmd2name(uint8_t id);
TIEXPORT3 const char* TICALL dbus_cmd2officialname(uint8_t id);
TIEXPORT3 const char* TICALL dbus_cmd2desc(uint8_t id);

TIEXPORT3 const char* TICALL dbus_mid2direction(uint8_t id);

TIEXPORT3 int TICALL dbus_dissect(CalcModel model, FILE * f, const uint8_t * data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
