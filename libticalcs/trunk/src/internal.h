/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2012  Romain Liévin
 *  Copyright (C) 2012       Lionel Debroux
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

/**
 * \file internal.h
 * \brief Definitions for internal (libticalcs) usage.
 */

#ifndef __TICALCS_INTERNAL__
#define __TICALCS_INTERNAL__

#define VALIDATE_NONNULL(ptr) \
	do \
	{ \
		if (ptr == NULL) \
		{ \
			ticalcs_critical("%s: " #ptr " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_HANDLE(handle) \
	do \
	{ \
		if (handle == NULL) \
		{ \
			ticalcs_critical("%s: " #handle " is NULL", __FUNCTION__); \
			return ERR_INVALID_HANDLE; \
		} \
	} while(0);
#define VALIDATE_CALCFNCTS(calc) \
	do \
	{ \
		if (calc == NULL) \
		{ \
			ticalcs_critical("%s: " # calc " is NULL", __FUNCTION__); \
			return ERR_INVALID_HANDLE; \
		} \
	} while(0);
#define VALIDATE_BACKUPCONTENT(content) \
	do \
	{ \
		if (content == NULL) \
		{ \
			ticalcs_critical("%s: " #content " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_FILECONTENT(content) \
	do \
	{ \
		if (content == NULL) \
		{ \
			ticalcs_critical("%s: " #content " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_FLASHCONTENT(content) \
	do \
	{ \
		if (content == NULL) \
		{ \
			ticalcs_critical("%s: " #content " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_VARENTRY(var) \
	do \
	{ \
		if (var == NULL) \
		{ \
			ticalcs_critical("%s: " #var " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_VARREQUEST(var) \
	do \
	{ \
		if (var == NULL) \
		{ \
			ticalcs_critical("%s: " #var " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);

#define RETURN_IF_HANDLE_NOT_ATTACHED(handle) \
	do \
	{ \
		if (!handle->attached) \
		{ \
			return ERR_NO_CABLE; \
		} \
	} while(0);
#define RETURN_IF_HANDLE_NOT_OPEN(handle) \
	do \
	{ \
		if (!handle->open) \
		{ \
			return ERR_NO_CABLE; \
		} \
	} while(0);
#define RETURN_IF_HANDLE_BUSY(handle) \
	do \
	{ \
		if (handle->busy) \
		{ \
			return ERR_BUSY; \
		} \
	} while(0);


// backup.c
int tixx_recv_all_vars_backup(CalcHandle* handle, FileContent* content);


// dbus_pkt.c

int dbus_recv_2(CalcHandle* cable, uint8_t* host, uint8_t* cmd, uint16_t* length, uint8_t* data);

#ifndef WORDS_BIGENDIAN
# define fixup(x) (x &= 0x0000ffff)
#else
# define fixup(x) (x >>= 16)
#endif /* !WORDS_BIGENDIAN */


// calc_00.c

int noop_is_ready (CalcHandle* handle);
int noop_send_key (CalcHandle* handle, uint16_t key);
int noop_execute (CalcHandle* handle, VarEntry *ve, const char* args);
int noop_recv_screen (CalcHandle* handle, CalcScreenCoord* sc, uint8_t** bitmap);
int noop_get_dirlist (CalcHandle* handle, GNode** vars, GNode** apps);
int noop_get_memfree (CalcHandle* handle, uint32_t* ram, uint32_t* flash);
int noop_send_backup (CalcHandle* handle, BackupContent* content);
int noop_recv_backup (CalcHandle* handle, BackupContent* content);
int noop_send_var (CalcHandle* handle, CalcMode mode, FileContent* content);
int noop_recv_var (CalcHandle* handle, CalcMode mode, FileContent* content, VarRequest* vr);
int noop_send_var_ns (CalcHandle* handle, CalcMode mode, FileContent* content);
int noop_recv_var_ns (CalcHandle* handle, CalcMode mode, FileContent* content, VarEntry** ve);
int noop_send_flash (CalcHandle* handle, FlashContent* content);
int noop_recv_flash (CalcHandle* handle, FlashContent* content, VarRequest* vr);
int noop_send_os (CalcHandle* handle, FlashContent* content);
int noop_recv_idlist (CalcHandle* handle, uint8_t* idlist);
int noop_dump_rom_1 (CalcHandle* handle);
int noop_dump_rom_2 (CalcHandle* handle, CalcDumpSize size, const char *filename);
int noop_set_clock (CalcHandle* handle, CalcClock* _clock);
int noop_get_clock (CalcHandle* handle, CalcClock* _clock);
int noop_del_var (CalcHandle* handle, VarRequest* vr);
int noop_new_folder (CalcHandle* handle, VarRequest* vr);
int noop_get_version (CalcHandle* handle, CalcInfos* infos);
int noop_send_cert (CalcHandle* handle, FlashContent* content);
int noop_recv_cert (CalcHandle* handle, FlashContent* content);
int noop_rename_var (CalcHandle* handle, VarRequest* oldname, VarRequest* newname);
int noop_change_attr (CalcHandle* handle, VarRequest* vr, FileAttr attr);
int noop_send_all_vars_backup (CalcHandle* handle, FileContent* content);
int noop_recv_all_vars_backup (CalcHandle* handle, FileContent* content);


// calc_84p.c

int ti84pcse_decompress_screen(uint8_t *dest, uint32_t dest_length, const uint8_t *src, uint32_t src_length);


// cmdz80.c

int ti82_send_asm_exec(CalcHandle*, VarEntry * var);


// dusb_vpkt.c

void dusb_vtl_pkt_purge(void);


// nsp_vpkt.c

extern uint16_t nsp_src_port;
extern uint16_t nsp_dst_port;
void nsp_vtl_pkt_purge(void);


// dusb_cmd.c

#define CA(x)   (const DUSBCalcAttr **)(x)
#define CP(x)   (const DUSBCalcParam **)(x)

#define VALIDATE_ATTRS(nattrs, attrs) \
	if (nattrs != 0 && attrs == NULL) \
	{ \
		ticalcs_critical("%s: " #attrs " is NULL", __FUNCTION__); \
		return ERR_INVALID_PARAMETER; \
	}


#endif // __TICALCS_INTERNAL__
