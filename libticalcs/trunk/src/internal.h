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

#include "error.h"
#include "pause.h"
#include "macros.h"

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
		if (!ticalcs_validate_handle(handle)) \
		{ \
			ticalcs_critical("%s: " #handle " is invalid", __FUNCTION__); \
			return ERR_INVALID_HANDLE; \
		} \
	} while(0);
#define VALIDATE_CALCFNCTS(calc) \
	do \
	{ \
		if (!ticalcs_validate_calcfncts(calc)) \
		{ \
			ticalcs_critical("%s: " # calc " is invalid", __FUNCTION__); \
			return ERR_INVALID_HANDLE; \
		} \
	} while(0);
#define VALIDATE_BACKUPCONTENT(content) \
	do \
	{ \
		if (!ticalcs_validate_backupcontent(content)) \
		{ \
			ticalcs_critical("%s: " #content " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_FILECONTENT(content) \
	do \
	{ \
		if (!ticalcs_validate_filecontent(content)) \
		{ \
			ticalcs_critical("%s: " #content " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_FILECONTENT_ENTRIES(content) \
	do \
	{ \
		if (!ticalcs_validate_filecontent_entries(content)) \
		{ \
			ticalcs_critical("%s: " #content "->entries is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_FLASHCONTENT(content) \
	do \
	{ \
		if (!ticalcs_validate_flashcontent(content)) \
		{ \
			ticalcs_critical("%s: " #content " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_VARENTRY(var) \
	do \
	{ \
		if (!ticalcs_validate_varentry(var)) \
		{ \
			ticalcs_critical("%s: " #var " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_VARREQUEST(var) \
	do \
	{ \
		if (!ticalcs_validate_varrequest(var)) \
		{ \
			ticalcs_critical("%s: " #var " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_CALCUPDATE(upd) \
	do \
	{ \
		if (!ticalcs_validate_calcupdate(upd)) \
		{ \
			ticalcs_critical("%s: " #upd " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_SCREENWIDTH(width) \
	do \
	{ \
		if (width > 320) \
		{ \
			ticalcs_critical("%s: no calculator model known to this library has screens of width > 320 pixels", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_SCREENHEIGHT(height) \
	do \
	{ \
		if (height > 240) \
		{ \
			ticalcs_critical("%s: no calculator model known to this library has screens of height > 240 pixels", __FUNCTION__); \
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
#define RETURN_ERR_MALLOC_IF_NULLPTR(data) \
	if (NULL == data) \
	{ \
		return ERR_MALLOC; \
	}
#define SET_ERR_MALLOC_IF_NULLPTR(data) \
	if (NULL == data) \
	{ \
		ret = ERR_MALLOC; \
	}
#define SET_ERR_MALLOC_AND_BREAK_IF_NULLPTR(data) \
	if (NULL == data) \
	{ \
		ret = ERR_MALLOC; \
		break; \
	}

// Used internally by functions which can be called either directly or indirectly, e.g.
// * dbus_{send,recv,recv_header,recv_data}()
// * dusb_{send,recv}()
// * nsp_{send,recv}()
// * rd_{dump,is_ready}()
// * several other direct or indirect callers of ticables_cable_{send,recv}().
#define SET_HANDLE_BUSY_IF_NECESSARY(handle) \
	int busy = handle->busy; \
	if (!busy) \
	{ \
		handle->busy = 1; \
	}
#define CLEAR_HANDLE_BUSY_IF_NECESSARY(handle) \
	if (!busy) \
	{ \
		handle->busy = 0; \
	}

static inline int ticalcs_validate_handle(CalcHandle * handle)
{
	return handle != nullptr;
}

static inline int ticalcs_validate_calcfncts(const CalcFncts * calc)
{
	return calc != nullptr;
}

static inline int ticalcs_validate_backupcontent(BackupContent * content)
{
	return content != nullptr;
}

static inline int ticalcs_validate_filecontent(FileContent * content)
{
	return content != nullptr;
}

static inline int ticalcs_validate_filecontent_entries(FileContent * content)
{
	return content->num_entries == 0 || content->entries != nullptr;
}

static inline int ticalcs_validate_flashcontent(FlashContent * content)
{
	return content != nullptr;
}

static inline int ticalcs_validate_varentry(VarEntry * var)
{
	return var != nullptr;
}

static inline int ticalcs_validate_varrequest(VarRequest * var)
{
	return var != nullptr;
}

static inline int ticalcs_validate_calcupdate(CalcUpdate * upd)
{
	return upd != nullptr && upd->start != nullptr && upd->stop != nullptr && upd->refresh != nullptr && upd->label != nullptr && upd->pbar != nullptr;
}

static inline void * ticalcs_alloc_screen(size_t len)
{
	return g_malloc(len);
}

static inline void * ticalcs_realloc_screen(void * mem, size_t len)
{
	return g_realloc(mem, len);
}

static inline int dirlist_init_tree(CalcHandle * handle, GNode ** tree, const char * type)
{
	int ret = ERR_MALLOC;

	(*tree) = g_node_new(nullptr);
	if (*tree != nullptr)
	{
		TreeInfo *ti = (TreeInfo *)g_malloc(sizeof(TreeInfo));
		if (ti != nullptr)
		{
			ti->model = handle->model;
			ti->type = type;
			(*tree)->data = ti;
			ret = 0;
		}
	}

	return ret;
}

static inline int dirlist_init_trees(CalcHandle * handle, GNode ** vars, GNode ** apps)
{
	int ret = dirlist_init_tree(handle, vars, VAR_NODE_NAME);

	if (!ret)
	{
		ret = dirlist_init_tree(handle, apps, APP_NODE_NAME);
	}

	return ret;
}

static inline GNode * dirlist_create_append_node(void * data, GNode ** tree)
{
	GNode * node = g_node_new(data);
	if (node != nullptr)
	{
		g_node_append(*tree, node);
	}
	return node;
}

#define ticalcs_slprintf(str, size, format, ...) \
	snprintf(str, size - 1, format, ##__VA_ARGS__); \
	str[size - 1] = 0;

#define ticalcs_strlcpy(dst, src, size) \
	strncpy(dst, src, size - 1); \
	dst[size - 1] = 0;

#define ticalcs_update_start(handle)     handle->updat->start()
#define ticalcs_update_stop(handle)      handle->updat->stop()
#define ticalcs_update_refresh(handle)   handle->updat->refresh()
#define ticalcs_update_pbar(handle)      handle->updat->pbar()
#define ticalcs_update_label(handle)     handle->updat->label()

#define ticalcs_update_canceled(handle)  (handle->updat->cancel)

static inline void ticalcs_event_fill_header(CalcHandle * handle, CalcEventData * event, CalcEventType type, int retval, CalcFnctsIdx operation)
{
	event->version = 1;
	event->type = type;
	event->retval = retval;
	event->attached = handle->attached;
	event->open = handle->open;
	event->operation = operation;
	event->model = handle->model;
}

static inline void ticalcs_event_fill_dbus_pkt(CalcEventData * event, uint16_t length, uint8_t id, uint8_t cmd, uint8_t * data)
{
	event->data.dbus_pkt.length = length;
	event->data.dbus_pkt.id = id;
	event->data.dbus_pkt.cmd = cmd;
	event->data.dbus_pkt.data = data;
}

static inline void ticalcs_event_fill_dusb_rpkt(CalcEventData * event, uint32_t size, uint8_t type, uint8_t * data)
{
	event->data.dusb_rpkt.size = size;
	event->data.dusb_rpkt.type = type;
	event->data.dusb_rpkt.data = data;
}

static inline void ticalcs_event_fill_dusb_vpkt(CalcEventData * event, uint32_t size, uint16_t type, uint8_t * data)
{
	event->data.dusb_vpkt.size = size;
	event->data.dusb_vpkt.type = type;
	event->data.dusb_vpkt.data = data;
}

static inline void ticalcs_event_fill_nsp_rpkt(CalcEventData * event, uint16_t src_addr, uint16_t src_port, uint16_t dst_addr, uint16_t dst_port, uint16_t data_sum, uint8_t data_size, uint8_t ack, uint8_t seq, uint8_t hdr_sum, uint8_t * data)
{
	event->data.nsp_rpkt.src_addr = src_addr;
	event->data.nsp_rpkt.src_port = src_port;
	event->data.nsp_rpkt.dst_addr = dst_addr;
	event->data.nsp_rpkt.dst_port = dst_port;
	event->data.nsp_rpkt.data_sum = data_sum;
	event->data.nsp_rpkt.data_size = data_sum;
	event->data.nsp_rpkt.ack = ack;
	event->data.nsp_rpkt.seq = seq;
	event->data.nsp_rpkt.hdr_sum = hdr_sum;
	event->data.nsp_rpkt.data = data;
}

static inline void ticalcs_event_fill_nsp_vpkt(CalcEventData * event, uint16_t src_addr, uint16_t src_port, uint16_t dst_addr, uint16_t dst_port, uint8_t cmd, uint32_t size, uint8_t * data)
{
	event->data.nsp_vpkt.src_addr = src_addr;
	event->data.nsp_vpkt.src_port = src_port;
	event->data.nsp_vpkt.dst_addr = dst_addr;
	event->data.nsp_vpkt.dst_port = dst_port;
	event->data.nsp_vpkt.cmd = cmd;
	event->data.nsp_vpkt.size = size;
	event->data.nsp_vpkt.data = data;
}

static inline void ticalcs_event_fill_romdump_pkt(CalcEventData * event, uint16_t length, uint16_t cmd, uint8_t * data)
{
	event->data.romdump_pkt.length = length;
	event->data.romdump_pkt.cmd = cmd;
	event->data.romdump_pkt.data = data;
}

static inline void ticalcs_event_fill_lab_equipment_data(CalcEventData * event, CalcLabEquipmentDataType type, uint16_t size, uint16_t items, const uint8_t * data, uint16_t index, uint16_t unknown, uint8_t vartype)
{
	event->data.labeq_data.type = type;
	event->data.labeq_data.size = size;
	event->data.labeq_data.items = items;
	event->data.labeq_data.data = data;
	event->data.labeq_data.index = index;
	event->data.labeq_data.unknown = unknown;
	event->data.labeq_data.vartype = vartype;
}

static inline int ticalcs_event_send_simple_generic(CalcHandle * handle, CalcEventType type, int retval, CalcFnctsIdx operation)
{
	int ret = retval;

	if (handle->event_hook)
	{
		CalcEventData event;
		ticalcs_event_fill_header(handle, &event, type, retval, operation);
		memset((void *)&event.data, 0, sizeof(event.data));
		handle->event_count++;
		ret = handle->event_hook(handle, handle->event_count, &event, handle->user_pointer);
	}

	return ret;
}

static inline int ticalcs_event_send_simple(CalcHandle * handle, CalcEventType type, int retval)
{
	return ticalcs_event_send_simple_generic(handle, type, retval, CALC_FNCT_LAST);
}

static inline int ticalcs_event_send(CalcHandle * handle, CalcEventData * event)
{
	int ret = event->retval;

	if (handle->event_hook)
	{
		handle->event_count++;
		ret = handle->event_hook(handle, handle->event_count, event, handle->user_pointer);
	}

	return ret;
}


// backup.c

int tixx_recv_all_vars_backup(CalcHandle* handle, FileContent* content);


// calc_00.c

int noop_is_ready (CalcHandle* handle);
int noop_send_key (CalcHandle* handle, uint32_t key);
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
int noop_send_lab_equipment_data (CalcHandle* handle, CalcModel model, CalcLabEquipmentData * data);
int noop_get_lab_equipment_data (CalcHandle* handle, CalcModel model, CalcLabEquipmentData * data);
int noop_del_folder (CalcHandle* handle, VarRequest* vr);


// calc_xx.c

int ticalcs_calc_send_var2_(CalcHandle* handle, CalcMode mode, const char* filename, int take_busy);


// cmdz80.c

int ti82_send_asm_exec(CalcHandle*, VarEntry * var);


// dusb_cmd.c

int dusb_check_cmd_data(CalcModel model, const uint8_t * data, uint32_t len, uint32_t vtl_size, uint16_t vtl_type);
int dusb_dissect_cmd_data(CalcModel model, FILE *f, const uint8_t * data, uint32_t len, uint32_t vtl_size, uint16_t vtl_type);


// dusb_cmd.c

#define CA(x)   (const DUSBCalcAttr **)(x)
#define CP(x)   (const DUSBCalcParam **)(x)

#define VALIDATE_ATTRS(nattrs, attrs) \
	if (nattrs != 0 && attrs == NULL) \
	{ \
		ticalcs_critical("%s: " #attrs " is NULL", __FUNCTION__); \
		return ERR_INVALID_PARAMETER; \
	}


// calclabequipmentdata.cc

int tixx_convert_lab_equipment_data_string_to_ti8586_raw_list(const char * lab_equipment_data, CalcLabEquipmentData * out_data);
int tixx_convert_lab_equipment_data_string_to_tiz80_raw_list(const char * lab_equipment_data, CalcLabEquipmentData * out_data);
int tixx_convert_lab_equipment_data_string_to_ti68k_raw_list(const char * lab_equipment_data, CalcLabEquipmentData * out_data);
int tixx_convert_lab_equipment_data_ti8586_raw_list_to_string(CalcLabEquipmentData * lab_equipment_data, uint32_t * out_item_count, double ** raw_values, const char ** out_data);
int tixx_convert_lab_equipment_data_tiz80_raw_list_to_string(CalcLabEquipmentData * lab_equipment_data, uint32_t * out_item_count, double ** raw_values, const char ** out_data);
int tixx_convert_lab_equipment_data_ti68k_raw_list_to_string(CalcLabEquipmentData * lab_equipment_data, uint32_t * out_item_count, double ** raw_values, const char ** out_data);
void tixx_free_converted_lab_equipment_data_item(CalcLabEquipmentData * lab_equipment_data);
void tixx_free_converted_lab_equipment_data_string(void * lab_equipment_data);
void tixx_free_converted_lab_equipment_data_fpvals(double * raw_values);

int tixx_send_lab_equipment_data(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * data);
int tixx_get_lab_equipment_data(CalcHandle* handle, CalcModel model, CalcLabEquipmentData * data);


// ticalcs.cc

int ticalcs_default_event_hook(CalcHandle * handle, uint32_t event_count, const CalcEventData * event, void * user_pointer);


#endif // __TICALCS_INTERNAL__
