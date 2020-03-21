#include <stdio.h>
#include <assert.h>
#include <ticalcs.h>
#include <inttypes.h>
#include <nsp_rpkt.h>
#include <nsp_vpkt.h>
#include <nsp_cmd.h>
#include <dusb_rpkt.h>
#include <dusb_vpkt.h>
#include <dusb_cmd.h>
#include <cmdz80.h>
#include <cmd68k.h>
#include <romdump.h>
#include "../src/error.h"

#define PRINTF(FUNCTION, TYPE, ...) \
fprintf(stderr, "%d\t" TYPE "\n", __LINE__, FUNCTION(__VA_ARGS__))

#define PRINTFVOID(FUNCTION, ...) \
FUNCTION(__VA_ARGS__); fprintf(stderr, "%d\n", __LINE__)

#define INT "%d"
#define UINT "%u"
#define X32 "%" PRIX32
#define X64 "%" PRIX64
#define PTR "%p"
#define STR "\"%s\""

static void torture_ticalcs()
{
    CalcHandle * handle;
    CableHandle * cable;
    CalcUpdate * update;

// error.c
    PRINTF(ticalcs_error_get, INT, -1, nullptr);
    PRINTFVOID(ticalcs_error_free, nullptr);
// ticalcs.c
    PRINTF(ticalcs_version_get, STR);
    PRINTF(ticalcs_supported_calcs, X64);
    PRINTF(ticalcs_supported_protocols, X32);
    PRINTF(ticalcs_max_calc_function_idx, UINT);
    handle = ticalcs_handle_new((CalcModel)- 1);
    PRINTF(, PTR, handle);
    ticalcs_handle_del(handle);
    PRINTF(ticalcs_handle_del, INT, nullptr);
    PRINTF(ticalcs_handle_show, INT, nullptr);
    PRINTF(ticalcs_get_model, INT, nullptr);

    PRINTF(ticalcs_cable_attach, INT, nullptr, (CableHandle *)0x12345678);
    PRINTF(ticalcs_cable_detach, INT, nullptr);
    cable = ticalcs_cable_get(nullptr);
    PRINTF(, PTR, cable);
    PRINTF(ticalcs_update_set, INT, nullptr, (CalcUpdate *)0x12345678);
    update = ticalcs_update_get(nullptr);
    PRINTF(, PTR, update);
    PRINTF(ticalcs_model_supports_dbus, INT, CALC_NONE);
    PRINTF(ticalcs_model_supports_dusb, INT, CALC_NONE);
    PRINTF(ticalcs_model_supports_nsp, INT, CALC_NONE);
    PRINTF(ticalcs_model_supports_installing_flashapps, INT, CALC_NONE);
    PRINTF(ticalcs_calc_get_event_hook, PTR, nullptr);

    PRINTF(ticalcs_calc_set_event_hook, PTR, nullptr, (ticalcs_event_hook_type)0x12345678);
    PRINTF(ticalcs_calc_get_event_user_pointer, PTR, nullptr);
    PRINTF(ticalcs_calc_set_event_user_pointer, PTR, nullptr, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_event_count, UINT, nullptr);
    PRINTF(ticalcs_calc_fire_user_event, INT, nullptr, (CalcEventType)0, 0, (void *)0x12345678, 0);
// calc_xx.c
    PRINTF(ticalcs_calc_features, INT, nullptr);
    PRINTF(ticalcs_calc_isready, INT, nullptr);
    PRINTF(ticalcs_calc_send_key, INT, nullptr, -1);
    PRINTF(ticalcs_calc_execute, INT, nullptr, (VarEntry*)0x12345678, (const char*)0x12345678);
    PRINTF(ticalcs_calc_execute, INT, (CalcHandle*)0x12345678, nullptr, nullptr);

    PRINTF(ticalcs_calc_recv_screen, INT, nullptr, (CalcScreenCoord*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(ticalcs_calc_recv_screen, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t**)0x12345678);
    PRINTF(ticalcs_calc_recv_screen, INT, (CalcHandle*)0x12345678, (CalcScreenCoord*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_recv_screen_rgb888, INT, nullptr, (CalcScreenCoord*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(ticalcs_calc_recv_screen_rgb888, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t**)0x12345678);
    PRINTF(ticalcs_calc_recv_screen_rgb888, INT, (CalcHandle*)0x12345678, (CalcScreenCoord*)0x12345678, nullptr);
    PRINTFVOID(ticalcs_free_screen, nullptr);
    PRINTF(ticalcs_calc_get_dirlist, INT, nullptr, (GNode**)0x12345678, (GNode**)0x12345678);
    PRINTF(ticalcs_calc_get_dirlist, INT, (CalcHandle*)0x12345678, nullptr, (GNode**)0x12345678);
    PRINTF(ticalcs_calc_get_dirlist, INT, (CalcHandle*)0x12345678, (GNode**)0x12345678, nullptr);

    PRINTF(ticalcs_calc_get_memfree, INT, nullptr, (uint32_t*)0x12345678, (uint32_t*)0x12345678);
    PRINTF(ticalcs_calc_get_memfree, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_get_memfree, INT, (CalcHandle*)0x12345678, nullptr, (uint32_t*)0x12345678);
    PRINTF(ticalcs_calc_send_backup, INT, nullptr, (BackupContent*)0x12345678);
    PRINTF(ticalcs_calc_send_backup, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_recv_backup, INT, nullptr, (BackupContent*)0x12345678);
    PRINTF(ticalcs_calc_recv_backup, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_var, INT, nullptr, (CalcMode)-1, (FileContent*)0x12345678);
    PRINTF(ticalcs_calc_send_var, INT, (CalcHandle*)0x12345678, (CalcMode)-1, nullptr);
    PRINTF(ticalcs_calc_recv_var, INT, nullptr, (CalcMode)-1, (FileContent*)0x12345678, (VarRequest*)0x12345678);

    PRINTF(ticalcs_calc_recv_var, INT, (CalcHandle *)0x12345678, (CalcMode)-1, nullptr, (VarRequest*)0x12345678);
    PRINTF(ticalcs_calc_recv_var, INT, (CalcHandle *)0x12345678, (CalcMode)-1, (FileContent*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_var_ns, INT, nullptr, (CalcMode)-1, nullptr);
    PRINTF(ticalcs_calc_send_var_ns, INT, nullptr, (CalcMode)-1, (FileContent*)0x12345678);
    PRINTF(ticalcs_calc_recv_var_ns, INT, nullptr, (CalcMode)-1, (FileContent*)0x12345678, (VarEntry**)0x12345678);
    PRINTF(ticalcs_calc_recv_var_ns, INT, (CalcHandle*)0x12345678, (CalcMode)-1, nullptr, (VarEntry**)0x12345678);
    PRINTF(ticalcs_calc_recv_var_ns, INT, (CalcHandle*)0x12345678, (CalcMode)-1, (FileContent*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_app, INT, nullptr, (FlashContent*)0x12345678);
    PRINTF(ticalcs_calc_send_app, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_recv_app, INT, nullptr, (FlashContent*)0x12345678, (VarRequest*)0x12345678);

    PRINTF(ticalcs_calc_recv_app, INT, (CalcHandle*)0x12345678, nullptr, (VarRequest*)0x12345678);
    PRINTF(ticalcs_calc_recv_app, INT, (CalcHandle*)0x12345678, (FlashContent*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_os, INT, nullptr, (FlashContent*)0x12345678);
    PRINTF(ticalcs_calc_send_os, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_recv_idlist, INT, nullptr, (uint8_t*)0x12345678);
    PRINTF(ticalcs_calc_recv_idlist, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_dump_rom_1, INT, nullptr);
    PRINTF(ticalcs_calc_dump_rom_2, INT, nullptr, (CalcDumpSize)-1, (const char*)0x12345678);
    PRINTF(ticalcs_calc_dump_rom_2, INT, (CalcHandle*)0x12345678, (CalcDumpSize)-1, nullptr);
    PRINTF(ticalcs_calc_set_clock, INT, nullptr, (CalcClock*)0x12345678);

    PRINTF(ticalcs_calc_set_clock, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_get_clock, INT, nullptr, (CalcClock*)0x12345678);
    PRINTF(ticalcs_calc_get_clock, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_new_fld, INT, nullptr, (VarRequest*)0x12345678);
    PRINTF(ticalcs_calc_new_fld, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_del_var, INT, nullptr, (VarRequest*)0x12345678);
    PRINTF(ticalcs_calc_del_var, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_rename_var, INT, nullptr, (VarRequest*)0x12345678, (VarRequest*)0x12345678);
    PRINTF(ticalcs_calc_rename_var, INT, (CalcHandle*)0x12345678, nullptr, (VarRequest*)0x12345678);
    PRINTF(ticalcs_calc_rename_var, INT, (CalcHandle*)0x12345678, (VarRequest*)0x12345678, nullptr);

    PRINTF(ticalcs_calc_change_attr, INT, nullptr, (VarRequest*)0x12345678, (FileAttr)-1);
    PRINTF(ticalcs_calc_change_attr, INT, (CalcHandle*)0x12345678, nullptr, (FileAttr)-1);
    PRINTF(ticalcs_calc_get_version, INT, nullptr, (CalcInfos*)0x12345678);
    PRINTF(ticalcs_calc_get_version, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_cert, INT, nullptr, (FlashContent*)0x12345678);
    PRINTF(ticalcs_calc_send_cert, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_recv_cert, INT, nullptr, (FlashContent*)0x12345678);
    PRINTF(ticalcs_calc_recv_cert, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_tigroup, INT, nullptr, (TigContent*)0x12345678, (TigMode)-1);
    PRINTF(ticalcs_calc_send_tigroup, INT, (CalcHandle*)0x12345678, nullptr, (TigMode)-1);

    PRINTF(ticalcs_calc_recv_tigroup, INT, nullptr, (TigContent*)0x12345678, (TigMode)-1);
    PRINTF(ticalcs_calc_recv_tigroup, INT, (CalcHandle*)0x12345678, nullptr, (TigMode)-1);
    PRINTF(ticalcs_calc_send_backup2, INT, nullptr, (const char*)0x12345678);
    PRINTF(ticalcs_calc_send_backup2, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_recv_backup2, INT, nullptr, (const char*)0x12345678);
    PRINTF(ticalcs_calc_recv_backup2, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_var2, INT, nullptr, (CalcMode)-1, (const char*)0x12345678);
    PRINTF(ticalcs_calc_send_var2, INT, (CalcHandle*)0x12345678, (CalcMode)-1, nullptr);
    PRINTF(ticalcs_calc_recv_var2, INT, nullptr, (CalcMode)-1, (const char*)0x12345678, (VarRequest*)0x12345678);
    PRINTF(ticalcs_calc_recv_var2, INT, (CalcHandle*)0x12345678, (CalcMode)-1, nullptr, (VarRequest*)0x12345678);

    PRINTF(ticalcs_calc_recv_var2, INT, (CalcHandle*)0x12345678, (CalcMode)-1, (const char*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_var_ns2, INT, nullptr, (CalcMode)-1, (const char*)0x12345678);
    PRINTF(ticalcs_calc_send_var_ns2, INT, (CalcHandle*)0x12345678, (CalcMode)-1, nullptr);
    PRINTF(ticalcs_calc_recv_var_ns2, INT, nullptr, (CalcMode)-1, (const char*)0x12345678, (VarEntry**)0x12345678);
    PRINTF(ticalcs_calc_recv_var_ns2, INT, (CalcHandle*)0x12345678, (CalcMode)-1, nullptr, (VarEntry**)0x12345678);
    PRINTF(ticalcs_calc_recv_var_ns2, INT, (CalcHandle*)0x12345678, (CalcMode)-1, (const char*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_app2, INT, nullptr, (const char*)0x12345678);
    PRINTF(ticalcs_calc_send_app2, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_recv_app2, INT, nullptr, (const char*)0x12345678, (VarRequest*)0x12345678);
    PRINTF(ticalcs_calc_recv_app2, INT, (CalcHandle*)0x12345678, nullptr, (VarRequest*)0x12345678);

    PRINTF(ticalcs_calc_recv_app2, INT, nullptr, (const char*)0x12345678, (VarRequest*)0x12345678);
    PRINTF(ticalcs_calc_send_cert2, INT, nullptr, (const char*)0x12345678);
    PRINTF(ticalcs_calc_send_cert2, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_recv_cert2, INT, nullptr, (const char*)0x12345678);
    PRINTF(ticalcs_calc_recv_cert2, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_os2, INT, nullptr, (const char*)0x12345678);
    PRINTF(ticalcs_calc_send_os2, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_tigroup2, INT, nullptr, (const char*)0x12345678, (TigMode)-1);
    PRINTF(ticalcs_calc_send_tigroup2, INT, (CalcHandle*)0x12345678, nullptr, (TigMode)-1);
    PRINTF(ticalcs_calc_recv_tigroup2, INT, nullptr, (const char*)0x12345678, (TigMode)-1);

    PRINTF(ticalcs_calc_recv_tigroup2, INT, (CalcHandle*)0x12345678, nullptr, (TigMode)-1);
// dirlist.c
    PRINTFVOID(ticalcs_dirlist_destroy, nullptr);
    PRINTFVOID(ticalcs_dirlist_display, nullptr);
    PRINTF(ticalcs_dirlist_ram_used, INT, nullptr);
    PRINTF(ticalcs_dirlist_flash_used, INT, nullptr, (GNode*)0x12345678);
    PRINTF(ticalcs_dirlist_flash_used, INT, (GNode*)0x12345678, nullptr);
    PRINTF(ticalcs_dirlist_ve_count, INT, nullptr);
    PRINTF(ticalcs_dirlist_ve_exist, PTR, nullptr, (VarEntry*)0x12345678);
    PRINTF(ticalcs_dirlist_ve_exist, PTR, (GNode*)0x12345678, nullptr);
    PRINTFVOID(ticalcs_dirlist_ve_add, nullptr, (VarEntry*)0x12345678);

    PRINTFVOID(ticalcs_dirlist_ve_add, (GNode*)0x12345678, nullptr);
    PRINTFVOID(ticalcs_dirlist_ve_del, nullptr, (VarEntry*)0x12345678);
    PRINTFVOID(ticalcs_dirlist_ve_del, (GNode*)0x12345678, nullptr);
// type2str.c
    PRINTF(ticalcs_model_to_string, STR, (CalcModel)-1);
    PRINTF(ticalcs_string_to_model, INT, nullptr);
    PRINTF(ticalcs_scrfmt_to_string, STR, (CalcScreenFormat)-1);
    PRINTF(ticalcs_string_to_scrfmt, INT, nullptr);
    PRINTF(ticalcs_pathtype_to_string, STR, (CalcPathType)-1);
    PRINTF(ticalcs_string_to_pathtype, INT, nullptr);
    PRINTF(ticalcs_memtype_to_string, STR, (CalcMemType)-1);

    PRINTF(ticalcs_string_to_memtype, INT, nullptr);
    PRINTF(ticalcs_infos_to_string, INT, nullptr, (char*)0x12345678, 0x12345678);
// clock.c
    PRINTF(ticalcs_clock_format2date, STR, (CalcModel)-1, -1);
    PRINTF(ticalcs_clock_date2format, INT, (CalcModel)-1, nullptr);
    PRINTF(ticalcs_clock_show, INT, (CalcModel)-1, nullptr);
// screen.c
    PRINTF(ticalcs_screen_convert_bw_to_rgb888, INT, nullptr, 0x12345678, 0x12345678, (uint8_t*)0x12345678);
    PRINTF(ticalcs_screen_convert_bw_to_rgb888, INT, (const uint8_t*)0x12345678, 0x12345678, 0x12345678, nullptr);
    PRINTF(ticalcs_screen_convert_bw_to_blurry_rgb888, INT, nullptr, 0x12345678, 0x12345678, (uint8_t*)0x12345678);
    PRINTF(ticalcs_screen_convert_bw_to_blurry_rgb888, INT, (const uint8_t*)0x12345678, 0x12345678, 0x12345678, nullptr);
    PRINTF(ticalcs_screen_convert_gs4_to_rgb888, INT, nullptr, 0x12345678, 0x12345678, (uint8_t*)0x12345678);

    PRINTF(ticalcs_screen_convert_gs4_to_rgb888, INT, (const uint8_t*)0x12345678, 0x12345678, 0x12345678, nullptr);
    PRINTF(ticalcs_screen_convert_rgb565le_to_rgb888, INT, nullptr, 0x12345678, 0x12345678, (uint8_t*)0x12345678);
    PRINTF(ticalcs_screen_convert_rgb565le_to_rgb888, INT, (const uint8_t*)0x12345678, 0x12345678, 0x12345678, nullptr);
    PRINTF(ticalcs_screen_convert_native_to_rgb888, INT, (CalcPixelFormat)1, nullptr, 0x12345678, 0x12345678, (uint8_t*)0x12345678);
    PRINTF(ticalcs_screen_convert_native_to_rgb888, INT, (CalcPixelFormat)1, (const uint8_t*)0x12345678, 0x12345678, 0x12345678, nullptr);
    PRINTF(ticalcs_screen_nspire_rle_uncompress, INT, (CalcPixelFormat)1, (const uint8_t*)0x12345678, 0x12345678, nullptr, 0x12345678);
    PRINTF(ticalcs_screen_nspire_rle_uncompress, INT, (CalcPixelFormat)1, nullptr, 0x12345678, (uint8_t*)0x12345678, 0x12345678);
    PRINTF(ticalcs_screen_84pcse_rle_uncompress, INT, (const uint8_t*)0x12345678, 0x12345678, nullptr, 0x12345678);
    PRINTF(ticalcs_screen_84pcse_rle_uncompress, INT, nullptr, 0x12345678, (uint8_t*)0x12345678, 0x12345678);
// tikeys.c
    PRINTF(ticalcs_keys_73, PTR, 0);

    PRINTF(ticalcs_keys_83, PTR, 0);
    PRINTF(ticalcs_keys_83p, PTR, 0);
    PRINTF(ticalcs_keys_86, PTR, 0);
    PRINTF(ticalcs_keys_89, PTR, 0);
    PRINTF(ticalcs_keys_92p, PTR, 0);
// probe.c
    PRINTF(ticalcs_probe_calc, INT, nullptr, (CalcModel *)0x12345678);
    PRINTF(ticalcs_probe_calc, INT, (CableHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_probe_usb_calc, INT, nullptr, (CalcModel *)0x12345678);
    PRINTF(ticalcs_probe_usb_calc, INT, (CableHandle*)0x12345678, nullptr);
    PRINTF(ticalcs_probe, INT, (CableModel)-1, (CablePort)-1, nullptr, -1);

    PRINTF(ticalcs_device_info_to_model, INT, nullptr);
    PRINTF(ticalcs_remap_model_from_usb, INT, (CableModel)0, (CalcModel)0);
    PRINTF(ticalcs_remap_model_to_usb, INT, (CableModel)0, (CalcModel)0);
// dbus_pkt.c
    PRINTF(dbus_send, INT, nullptr, 0, 0, 0, (uint8_t*)0x12345678);
    PRINTF(dbus_recv, INT, nullptr, (uint8_t*)0x12345678, (uint8_t*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(dbus_recv, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(dbus_recv, INT, (CalcHandle*)0x12345678, (uint8_t*)0x12345678, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(dbus_recv, INT, (CalcHandle*)0x12345678, (uint8_t*)0x12345678, (uint8_t*)0x12345678, nullptr, (uint8_t*)0x12345678);
    PRINTF(dbus_recv_header, INT, nullptr, (uint8_t*)0x12345678, (uint8_t*)0x12345678, (uint16_t*)0x12345678);
    PRINTF(dbus_recv_header, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678, (uint16_t*)0x12345678);

    PRINTF(dbus_recv_header, INT, (CalcHandle*)0x12345678, (uint8_t*)0x12345678, nullptr, (uint16_t*)0x12345678);
    PRINTF(dbus_recv_header, INT, (CalcHandle*)0x12345678, (uint8_t*)0x12345678, (uint8_t*)0x12345678, nullptr);
    PRINTF(dbus_recv_data, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(dbus_recv_data, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678);
    PRINTF(dbus_recv_data, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, nullptr);
// dusb_rpkt.c
    PRINTF(dusb_send, INT, nullptr, (DUSBRawPacket*)0x12345678);
    PRINTF(dusb_send, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(dusb_recv, INT, nullptr, (DUSBRawPacket*)0x12345678);
    PRINTF(dusb_recv, INT, (CalcHandle*)0x12345678, nullptr);
// nsp_rpkt.c
    PRINTF(nsp_send, INT, nullptr, (NSPRawPacket*)0x12345678);

    PRINTF(nsp_send, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_recv, INT, nullptr, (NSPRawPacket*)0x12345678);
    PRINTF(nsp_recv, INT, (CalcHandle*)0x12345678, nullptr);
}

static void torture_nsp()
{
    void * ptr;

// nsp_rpkt.c
    PRINTF(nsp_addr2name, STR, 0);
    PRINTF(nsp_sid2name, STR, 0);
    PRINTF(nsp_dissect, INT, CALC_NONE, nullptr, (const uint8_t*)0x12345678, 8, 2);
    PRINTF(nsp_dissect, INT, CALC_NONE, (FILE*)0x12345678, nullptr, 8, 2);
// nsp_vpkt.c
    PRINTF(nsp_vtl_pkt_new, PTR, nullptr);
    PRINTF(nsp_vtl_pkt_new_ex, PTR, nullptr, 0x12345678, 0x1234, 0x1234, 0x1234, 0x1234, 0x12, (uint8_t*)0x12345678);
    PRINTFVOID(nsp_vtl_pkt_fill, nullptr, 0x12345678, 0x1234, 0x1234, 0x1234, 0x1234, 0x12, nullptr);
    PRINTFVOID(nsp_vtl_pkt_del, nullptr, (NSPVirtualPacket*)0x12345678);
    PRINTFVOID(nsp_vtl_pkt_del, (CalcHandle*)0x12345678, nullptr);
    ptr = nsp_vtl_pkt_alloc_data(nullptr, 0);
    PRINTF(, PTR, ptr);
    nsp_vtl_pkt_free_data(nullptr, ptr);

    ptr = nsp_vtl_pkt_realloc_data(nullptr, nullptr, 1);
    PRINTF(, PTR, ptr);
    nsp_vtl_pkt_free_data(nullptr, ptr);
    ptr = nsp_vtl_pkt_realloc_data(nullptr, (NSPVirtualPacket *)0x12345678, 1);
    PRINTF(, PTR, ptr);
    nsp_vtl_pkt_free_data(nullptr, ptr);
    PRINTFVOID(nsp_vtl_pkt_free_data, nullptr, (void *)0x12345678);
    PRINTF(nsp_session_open, INT, nullptr, 0);
    PRINTF(nsp_session_close, INT, nullptr);
    PRINTF(nsp_addr_request, INT, nullptr);
    PRINTF(nsp_addr_assign, INT, nullptr, 0);
    PRINTF(nsp_send_ack, INT, nullptr);
    PRINTF(nsp_recv_ack, INT, nullptr);
    PRINTF(nsp_send_nack, INT, nullptr);

    PRINTF(nsp_send_nack_ex, INT, nullptr, 0);
    PRINTF(nsp_send_data, INT, nullptr, (NSPVirtualPacket*)0x12345678);
    PRINTF(nsp_send_data, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_recv_data, INT, nullptr, (NSPVirtualPacket*)0x12345678);
    PRINTF(nsp_recv_data, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_send_disconnect, INT, nullptr);
    PRINTF(nsp_recv_disconnect, INT, nullptr);
// nsp_cmd.c
    PRINTF(nsp_cmd_r_login, INT, nullptr);
    PRINTF(nsp_cmd_s_status, INT, nullptr, 0);
    PRINTF(nsp_cmd_r_status, INT, nullptr, (uint8_t*)0x12345678);

    PRINTF(nsp_cmd_s_dev_infos, INT, nullptr, 0);
    PRINTF(nsp_cmd_r_dev_infos, INT, nullptr, (uint8_t*)0x12345678, (uint32_t*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(nsp_cmd_r_dev_infos, INT, (CalcHandle*)0x12345678, (uint8_t*)0x12345678, nullptr, nullptr);
    PRINTF(nsp_cmd_r_dev_infos, INT, (CalcHandle*)0x12345678, nullptr, (uint32_t*)0x12345678, nullptr);
    PRINTF(nsp_cmd_r_dev_infos, INT, (CalcHandle*)0x12345678, nullptr, nullptr, (uint8_t**)0x12345678);
    PRINTF(nsp_cmd_s_screen_rle, INT, nullptr, 0);
    PRINTF(nsp_cmd_r_screen_rle, INT, nullptr, (uint8_t*)0x12345678, (uint32_t*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(nsp_cmd_r_screen_rle, INT, (CalcHandle*)0x12345678, nullptr, (uint32_t*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(nsp_cmd_r_screen_rle, INT, (CalcHandle*)0x12345678, (uint8_t*)0x12345678, nullptr, (uint8_t**)0x12345678);
    PRINTF(nsp_cmd_r_screen_rle, INT, (CalcHandle*)0x12345678, (uint8_t*)0x12345678, (uint32_t*)0x12345678, nullptr);

    PRINTF(nsp_cmd_s_dir_attributes, INT, nullptr, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_dir_attributes, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_cmd_r_dir_attributes, INT, nullptr, (uint32_t*)0x12345678, (uint8_t*)0x12345678, (uint32_t*)0x12345678);
    PRINTF(nsp_cmd_s_dir_enum_init, INT, nullptr, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_dir_enum_init, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_cmd_r_dir_enum_init, INT, nullptr);
    PRINTF(nsp_cmd_s_dir_enum_next, INT, nullptr);
    PRINTF(nsp_cmd_r_dir_enum_next, INT, nullptr, (char*)0x12345678, (uint32_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(nsp_cmd_r_dir_enum_next, INT, (CalcHandle*)0x12345678, nullptr, (uint32_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(nsp_cmd_s_dir_enum_done, INT, nullptr);

    PRINTF(nsp_cmd_r_dir_enum_done, INT, nullptr);
    PRINTF(nsp_cmd_s_put_file, INT, nullptr, (const char*)0x12345678, 0);
    PRINTF(nsp_cmd_s_put_file, INT, (CalcHandle*)0x12345678, nullptr, 0);
    PRINTF(nsp_cmd_r_put_file, INT, nullptr);
    PRINTF(nsp_cmd_s_get_file, INT, nullptr, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_get_file, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_cmd_r_get_file, INT, nullptr, (uint32_t*)0x12345678);
    PRINTF(nsp_cmd_s_del_file, INT, nullptr, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_del_file, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_cmd_r_del_file, INT, nullptr);

    PRINTF(nsp_cmd_s_new_folder, INT, nullptr, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_new_folder, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_cmd_r_new_folder, INT, nullptr);
    PRINTF(nsp_cmd_s_del_folder, INT, nullptr, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_del_folder, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_cmd_r_del_folder, INT, nullptr);
    PRINTF(nsp_cmd_s_copy_file, INT, nullptr, (const char*)0x12345678, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_copy_file, INT, (CalcHandle*)0x12345678, nullptr, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_copy_file, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, nullptr);
    PRINTF(nsp_cmd_r_copy_file, INT, nullptr);

    PRINTF(nsp_cmd_s_rename_file, INT, nullptr, (const char*)0x12345678, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_rename_file, INT, (CalcHandle*)0x12345678, nullptr, (const char*)0x12345678);
    PRINTF(nsp_cmd_s_rename_file, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, nullptr);
    PRINTF(nsp_cmd_r_rename_file, INT, nullptr);
    PRINTF(nsp_cmd_s_file_ok, INT, nullptr);
    PRINTF(nsp_cmd_r_file_ok, INT, nullptr);
    PRINTF(nsp_cmd_s_file_contents, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(nsp_cmd_s_file_contents, INT, (CalcHandle*)0x12345678, 0, nullptr);
    PRINTF(nsp_cmd_r_file_contents, INT, nullptr, (uint32_t*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(nsp_cmd_r_file_contents, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t**)0x12345678);

    PRINTF(nsp_cmd_r_file_contents, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, nullptr);
    PRINTF(nsp_cmd_s_os_install, INT, nullptr, 0);
    PRINTF(nsp_cmd_r_os_install, INT, nullptr);
    PRINTF(nsp_cmd_s_os_contents, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(nsp_cmd_s_os_contents, INT, (CalcHandle*)0x12345678, 0, nullptr);
    PRINTF(nsp_cmd_r_progress, INT, nullptr, (uint8_t*)0x12345678);
    PRINTF(nsp_cmd_r_progress, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(nsp_cmd_s_generic_data, INT, nullptr, 0, (uint8_t*)0x12345678, 0, 0);
    PRINTF(nsp_cmd_r_generic_data, INT, nullptr, (uint32_t*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(nsp_cmd_s_echo, INT, nullptr, 0, (uint8_t*)0x12345678);

    PRINTF(nsp_cmd_r_echo, INT, nullptr, (uint32_t*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(nsp_cmd_s_keypress_event, INT, nullptr, (const uint8_t*)0x12345678);
    PRINTF(nsp_cmd_s_keypress_event, INT, (CalcHandle*)0x12345678, nullptr);
}

static void torture_dusb()
{
    void* ptr;
    const DUSBModeSet mode = DUSB_MODE_STARTUP;

// dusb_rpkt.c
    PRINTF(dusb_rpkt_type2name, STR, 0);
    PRINTF(dusb_dissect, INT, CALC_NONE, nullptr, (const uint8_t *)0x12345678, 8, 2, (uint8_t *)0x12345678);
    PRINTF(dusb_dissect, INT, CALC_NONE, (FILE *)0x12345678, nullptr, 8, 2, (uint8_t *)0x12345678);
    PRINTF(dusb_dissect, INT, CALC_NONE, (FILE *)0x12345678, (const uint8_t *)0x12345678, 8, 2, nullptr);
// dusb_vpkt.c
    PRINTF(dusb_vtl_pkt_new, PTR, nullptr);
    PRINTF(dusb_vtl_pkt_new_ex, PTR, nullptr, 0, 0, (uint8_t*)0x12345678);
    PRINTFVOID(dusb_vtl_pkt_fill, nullptr, 0, 0, (uint8_t*)0x12345678);
    PRINTFVOID(dusb_vtl_pkt_del, nullptr, (DUSBVirtualPacket*)0x12345678);
    PRINTFVOID(dusb_vtl_pkt_del, (CalcHandle*)0x12345678, nullptr);
    ptr = dusb_vtl_pkt_alloc_data(nullptr, 0);
    PRINTF(, PTR, ptr);
    dusb_vtl_pkt_free_data(nullptr, ptr);

    ptr = dusb_vtl_pkt_realloc_data(nullptr, nullptr, 1);
    PRINTF(, PTR, ptr);
    dusb_vtl_pkt_free_data(nullptr, ptr);
    ptr = dusb_vtl_pkt_realloc_data(nullptr, (DUSBVirtualPacket *)0x12345678, 1);
    PRINTF(, PTR, ptr);
    dusb_vtl_pkt_free_data(nullptr, ptr);
    PRINTFVOID(dusb_vtl_pkt_free_data, nullptr, (void *)0x12345678);
    PRINTF(dusb_send_buf_size_request, INT, nullptr, 0);
    PRINTF(dusb_recv_buf_size_alloc, INT, nullptr, (uint32_t*)0x12345678);
    PRINTF(dusb_recv_buf_size_request, INT, nullptr, (uint32_t*)0x12345678);
    PRINTF(dusb_send_buf_size_alloc, INT, nullptr, 0);
    PRINTF(dusb_send_data, INT, nullptr, (DUSBVirtualPacket*)0x12345678);
    PRINTF(dusb_send_data, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(dusb_recv_data, INT, nullptr, (DUSBVirtualPacket*)0x12345678);

    PRINTF(dusb_recv_data, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(dusb_recv_data_varsize, INT, nullptr, (DUSBVirtualPacket*)0x12345678, (uint32_t*)0x12345678, 0);
    PRINTF(dusb_recv_data_varsize, INT, (CalcHandle*)0x12345678, nullptr, (uint32_t*)0x12345678, 0);
    PRINTF(dusb_recv_data_varsize, INT, (CalcHandle*)0x12345678, (DUSBVirtualPacket*)0x12345678, nullptr, 0);
    PRINTF(dusb_send_acknowledge, INT, nullptr);
    PRINTF(dusb_recv_acknowledge, INT, nullptr);
    PRINTF(dusb_vpkt_type2name, STR, 0);
    PRINTF(dusb_get_buf_size, INT, nullptr, (uint32_t*)0x12345678);
    PRINTF(dusb_get_buf_size, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(dusb_set_buf_size, INT, nullptr, 0);

    PRINTF(dusb_cmd_param_type2name, STR, 0);
// dbus_cmd.c
    ptr = dusb_cp_new(nullptr, 0, 0);
    PRINTF(, PTR, ptr);
    ptr = dusb_cp_new_ex(nullptr, 0, 0, (uint8_t*)0x12345678);
    PRINTF(, PTR, ptr);
    PRINTFVOID(dusb_cp_fill, nullptr, 0, 0, (uint8_t*)0x12345678);
    dusb_cp_del((CalcHandle*)0x12345678, (DUSBCalcParam*)ptr);
    PRINTFVOID(dusb_cp_del, nullptr, (DUSBCalcParam*)0x12345678);
    PRINTFVOID(dusb_cp_del, (CalcHandle*)0x12345678, nullptr);
    ptr = dusb_cp_new_array(nullptr, 0);
    PRINTF(, PTR, ptr);
    dusb_cp_del_array((CalcHandle*)0x12345678, (DUSBCalcParam **)ptr, 0);
    ptr = dusb_cp_new_array2(nullptr, 0);
    PRINTF(, PTR, ptr);
    dusb_cp_del_array((CalcHandle *)0x12345678, (DUSBCalcParam **)ptr, 0);
    PRINTFVOID(dusb_cp_del_array, (CalcHandle *)0x12345678, nullptr, 0);
    PRINTFVOID(dusb_cp_del_array, nullptr, (DUSBCalcParam **)0x12345678, 0);

    PRINTFVOID(dusb_cp_del_array2, (CalcHandle *)0x12345678, nullptr, 0, 0);
    PRINTFVOID(dusb_cp_del_array2, nullptr, (DUSBCalcParam *)0x12345678, 0, 1);
    ptr = dusb_cp_alloc_data(nullptr, 0);
    PRINTF(, PTR, ptr);
    dusb_cp_free_data(nullptr, ptr);
    ptr = dusb_cp_realloc_data(nullptr, nullptr, 1);
    PRINTF(, PTR, ptr);
    dusb_cp_free_data(nullptr, ptr);
    ptr = dusb_cp_realloc_data(nullptr, (DUSBCalcParam *)0x12345678, 1);
    PRINTF(, PTR, ptr);
    dusb_cp_free_data(nullptr, ptr);
    PRINTFVOID(dusb_cp_free_data, nullptr, (void *)0x12345678);
    PRINTFVOID(dusb_cp_free_data, (CalcHandle *)0x12345678, nullptr);
    PRINTFVOID(dusb_cp_free_array_data, nullptr, (DUSBCalcParam *)0x12345678, 0);
    PRINTFVOID(dusb_cp_free_array_data, (CalcHandle *)0x12345678, nullptr, 0);
    ptr = dusb_ca_new(nullptr, 0, 0);
    PRINTF(, PTR, ptr);

    ptr = dusb_ca_new_ex(nullptr, 0, 0, (uint8_t*)0x12345678);
    PRINTF(, PTR, ptr);
    PRINTFVOID(dusb_ca_fill, nullptr, 0, 0, (uint8_t*)0x12345678);
    dusb_ca_del((CalcHandle*)0x12345678, (DUSBCalcAttr *)ptr);
    PRINTFVOID(dusb_ca_del, nullptr, (DUSBCalcAttr *)0x12345678);
    PRINTFVOID(dusb_ca_del, (CalcHandle*)0x12345678, nullptr);
    ptr = dusb_ca_new_array(nullptr, 0);
    PRINTF(, PTR, ptr);
    dusb_ca_del_array((CalcHandle*)0x12345678, (DUSBCalcAttr **)ptr, 0);
    ptr = dusb_ca_new_array2(nullptr, 0);
    PRINTF(, PTR, ptr);
    dusb_ca_del_array((CalcHandle *)0x12345678, (DUSBCalcAttr **)ptr, 0);
    PRINTFVOID(dusb_ca_del_array, (CalcHandle *)0x12345678, nullptr, 0);
    PRINTFVOID(dusb_ca_del_array, nullptr, (DUSBCalcAttr **)0x12345678, 0);
    PRINTFVOID(dusb_ca_del_array2, (CalcHandle *)0x12345678, nullptr, 0, 0);
    PRINTFVOID(dusb_ca_del_array2, nullptr, (DUSBCalcAttr *)0x12345678, 0, 1);

    ptr = dusb_ca_alloc_data(nullptr, 0);
    PRINTF(, PTR, ptr);
    dusb_ca_free_data(nullptr, ptr);
    ptr = dusb_ca_realloc_data(nullptr, nullptr, 1);
    PRINTF(, PTR, ptr);
    dusb_ca_free_data(nullptr, ptr);
    ptr = dusb_ca_realloc_data(nullptr, (DUSBCalcAttr *)0x12345678, 1);
    PRINTF(, PTR, ptr);
    dusb_ca_free_data(nullptr, ptr);
    PRINTFVOID(dusb_ca_free_data, nullptr, (void *)0x12345678);
    PRINTFVOID(dusb_ca_free_data, (CalcHandle *)0x12345678, nullptr);
    PRINTFVOID(dusb_ca_free_array_data, nullptr, (DUSBCalcAttr *)0x12345678, 0);
    PRINTFVOID(dusb_ca_free_array_data, (CalcHandle *)0x12345678, nullptr, 0);
    PRINTF(dusb_cmd_s_mode_set, INT, nullptr, mode);
    PRINTF(dusb_cmd_s_os_begin, INT, nullptr, 0);
    PRINTF(dusb_cmd_r_os_ack, INT, nullptr, (uint32_t*)0x12345678);

    PRINTF(dusb_cmd_s_os_header, INT, nullptr, 0, 0, 0, 0, (uint8_t*)0x12345678);
    PRINTF(dusb_cmd_s_os_header, INT, (CalcHandle*)0x12345678, 0, 0, 0, 0, nullptr);
    PRINTF(dusb_cmd_s_os_data, INT, nullptr, 0, 0, 0, 0, (uint8_t*)0x12345678);
    PRINTF(dusb_cmd_s_os_data, INT, (CalcHandle*)0x12345678, 0, 0, 0, 0, nullptr);
    PRINTF(dusb_cmd_s_os_header_89, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(dusb_cmd_s_os_header_89, INT, (CalcHandle*)0x12345678, 0, nullptr);
    PRINTF(dusb_cmd_s_os_data_89, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(dusb_cmd_s_os_data_89, INT, (CalcHandle*)0x12345678, 0, nullptr);
    PRINTF(dusb_cmd_r_eot_ack, INT, nullptr);
    PRINTF(dusb_cmd_s_param_request, INT, nullptr, 1, (const uint16_t*)0x12345678);

    PRINTF(dusb_cmd_s_param_request, INT, (CalcHandle*)0x12345678, 1, nullptr);
    PRINTF(dusb_cmd_r_param_data, INT, nullptr, 1, (DUSBCalcParam**)0x12345678);
    PRINTF(dusb_cmd_r_param_data, INT, (CalcHandle*)0x12345678, 1, nullptr);
    PRINTF(dusb_cmd_r_screenshot, INT, nullptr, (uint32_t*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(dusb_cmd_r_screenshot, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t**)0x12345678);
    PRINTF(dusb_cmd_r_screenshot, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, nullptr);
    PRINTF(dusb_cmd_s_dirlist_request, INT, nullptr, 1, (const uint16_t*)0x12345678);
    PRINTF(dusb_cmd_s_dirlist_request, INT, (CalcHandle*)0x12345678, 1, nullptr);
    PRINTF(dusb_cmd_r_var_header, INT, nullptr, (char*)0x12345678, (char*)0x12345678, (DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_r_var_header, INT, (CalcHandle*)0x12345678, nullptr, (char*)0x12345678, (DUSBCalcAttr**)0x12345678);

    PRINTF(dusb_cmd_r_var_header, INT, (CalcHandle*)0x12345678, (char*)0x12345678, nullptr, (DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_r_var_header, INT, (CalcHandle*)0x12345678, (char*)0x12345678, (char*)0x12345678, nullptr);
    PRINTF(dusb_cmd_s_rts, INT, nullptr, (const char*)0x12345678, (const char*)0x12345678, 0, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_rts, INT, (CalcHandle*)0x12345678, nullptr, (const char*)0x12345678, 0, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_rts, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, nullptr, 0, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_rts, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 0, 1, nullptr);
    PRINTF(dusb_cmd_s_var_request, INT, nullptr, (const char*)0x12345678, (const char*)0x12345678, 1, (const uint16_t*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_request, INT, (CalcHandle*)0x12345678, nullptr, (const char*)0x12345678, 1, (const uint16_t*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_request, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, nullptr, 1, (const uint16_t*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_request, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, nullptr, 1, (const DUSBCalcAttr**)0x12345678);

    PRINTF(dusb_cmd_s_var_request, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, (const uint16_t*)0x12345678, 1, nullptr);
    PRINTF(dusb_cmd_s_var_content, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(dusb_cmd_s_var_content, INT, (CalcHandle*)0x12345678, 0, nullptr);
    PRINTF(dusb_cmd_r_var_content, INT, nullptr, (uint32_t*)0x12345678, (uint8_t**)0x12345678);
    PRINTF(dusb_cmd_r_var_content, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, nullptr);
    PRINTF(dusb_cmd_s_param_set, INT, nullptr, (const DUSBCalcParam*)0x12345678);
    PRINTF(dusb_cmd_s_param_set, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(dusb_cmd_s_var_modify, INT, nullptr, (const char*)0x12345678, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (CalcHandle*)0x12345678, nullptr, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, nullptr, 1, (const DUSBCalcAttr**)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);

    PRINTF(dusb_cmd_s_var_modify, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, nullptr, (const char*)0x12345678, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678, nullptr, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678, (const char*)0x12345678, nullptr, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, nullptr);
    PRINTF(dusb_cmd_s_var_delete, INT, nullptr, (const char*)0x12345678, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_delete, INT, (CalcHandle*)0x12345678, nullptr, (const char*)0x12345678, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_delete, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, nullptr, 1, (const DUSBCalcAttr**)0x12345678);
    PRINTF(dusb_cmd_s_var_delete, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, (const char*)0x12345678, 1, nullptr);
    PRINTF(dusb_cmd_s_execute, INT, nullptr, (const char*)0x12345678, (const char*)0x12345678, 0, (const char*)0x12345678, 0);  // It's OK to have args = nullptr
    PRINTF(dusb_cmd_s_execute, INT, (CalcHandle*)0x12345678, nullptr, (const char*)0x12345678, 0, (const char*)0x12345678, 0);

    PRINTF(dusb_cmd_s_execute, INT, (CalcHandle*)0x12345678, (const char*)0x12345678, nullptr, 0, (const char*)0x12345678, 0);
    PRINTF(dusb_cmd_r_mode_ack, INT, nullptr);
    PRINTF(dusb_cmd_r_data_ack, INT, nullptr);
    PRINTF(dusb_cmd_r_delay_ack, INT, nullptr);
    PRINTF(dusb_cmd_s_eot, INT, nullptr);
    PRINTF(dusb_cmd_r_eot, INT, nullptr);
    PRINTF(dusb_cmd_s_error, INT, nullptr, 0);
    PRINTF(dusb_cmd_s_param_set_r_data_ack, INT, nullptr, 0, 0, (const uint8_t*)0x12345678);
}

static void torture_dbus()
{
// dbus_pkt.c
    PRINTF(dbus_cmd2name, STR, 0);
    PRINTF(dbus_cmd2officialname, STR, 0);
    PRINTF(dbus_cmd2desc, STR, 0);
    PRINTF(dbus_mid2direction, STR, 0);
    PRINTF(dbus_dissect, INT, CALC_NONE, nullptr, (const uint8_t*)0x12345678, 8);
    PRINTF(dbus_dissect, INT, CALC_NONE, (FILE*)0x12345678, nullptr, 8);
}

static void torture_cmdz80()
{
// cmdz80.c
    PRINTF(tiz80_model_to_dbus_mid, INT, CALC_NONE);
    PRINTF(tiz80_handle_to_dbus_mid, INT, NULL);
    PRINTF(ti73_send_VAR, INT, nullptr, 0, 0, (const char*)0x12345678, 0, 0);
    PRINTF(ti73_send_VAR, INT, (CalcHandle*)0x12345678, 0, 0, nullptr, 0, 0);
    PRINTF(ti73_send_VAR2, INT, nullptr, 0, 0, 0, 0, 0);
    PRINTF(ti73_send_CTS, INT, nullptr);
    PRINTF(ti73_send_XDP, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(ti73_send_SKP, INT, nullptr, 0);
    PRINTF(ti73_send_ACK, INT, nullptr);
    PRINTF(ti73_send_ERR, INT, nullptr);

    PRINTF(ti73_send_RDY, INT, nullptr);
    PRINTF(ti73_send_SCR, INT, nullptr);
    PRINTF(ti73_send_KEY, INT, nullptr, 0);
    PRINTF(ti73_send_EOT, INT, nullptr);
    PRINTF(ti73_send_REQ, INT, nullptr, 0, 0, (const char*)0x12345678, 0, 0);
    PRINTF(ti73_send_REQ, INT, (CalcHandle*)0x12345678, 0, 0, nullptr, 0, 0);
    PRINTF(ti73_send_REQ2, INT, nullptr, 0, 0, (const char*)0x12345678, 0);
    PRINTF(ti73_send_REQ2, INT, (CalcHandle*)0x12345678, 0, 0, nullptr, 0);
    PRINTF(ti73_send_RTS, INT, nullptr, 0, 0, (const char*)0x12345678, 0, 0);
    PRINTF(ti73_send_RTS, INT, (CalcHandle*)0x12345678, 0, 0, nullptr, 0, 0);

    PRINTF(ti73_send_VER, INT, nullptr);
    PRINTF(ti73_send_DEL, INT, nullptr, 0, 0, (const char*)0x12345678, 0);
    PRINTF(ti73_send_DEL, INT, (CalcHandle*)0x12345678, 0, 0, nullptr, 0);
    PRINTF(ti73_send_DUMP, INT, nullptr, 0);
    PRINTF(ti73_send_EKE, INT, nullptr);
    PRINTF(ti73_send_DKE, INT, nullptr);
    PRINTF(ti73_send_ELD, INT, nullptr);
    PRINTF(ti73_send_DLD, INT, nullptr);
    PRINTF(ti73_send_GID, INT, nullptr);
    PRINTF(ti73_send_RID, INT, nullptr);

    PRINTF(ti73_send_SID, INT, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_VAR, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678, (uint8_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_VAR, INT, (CalcHandle *)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678, (uint8_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_VAR, INT, (CalcHandle *)0x12345678, (uint16_t *)0x12345678, nullptr, (char*)0x12345678, (uint8_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_VAR, INT, (CalcHandle *)0x12345678, (uint16_t *)0x12345678, (uint8_t *)0x12345678, nullptr, (uint8_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_VAR, INT, (CalcHandle *)0x12345678, (uint16_t *)0x12345678, (uint8_t *)0x12345678, (char*)0x12345678, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_VAR, INT, (CalcHandle *)0x12345678, (uint16_t *)0x12345678, (uint8_t *)0x12345678, (char*)0x12345678, (uint8_t*)0x12345678, nullptr);
    PRINTF(ti73_recv_VAR2, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678, (uint16_t*)0x12345678, (uint16_t*)0x12345678);
    PRINTF(ti73_recv_VAR2, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678, (uint16_t*)0x12345678, (uint16_t*)0x12345678);
    PRINTF(ti73_recv_VAR2, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, nullptr, (char*)0x12345678, (uint16_t*)0x12345678, (uint16_t*)0x12345678);

    PRINTF(ti73_recv_VAR2, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678, nullptr, (uint16_t*)0x12345678, (uint16_t*)0x12345678);
    PRINTF(ti73_recv_VAR2, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678, nullptr, (uint16_t*)0x12345678);
    PRINTF(ti73_recv_VAR2, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678, (uint16_t*)0x12345678, nullptr);
    PRINTF(ti73_recv_CTS, INT, nullptr, 0);
    PRINTF(ti73_recv_SKP, INT, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_SKP, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ti73_recv_XDP, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_XDP, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_ACK, INT, nullptr, (uint16_t*)0x12345678);
    PRINTF(ti73_recv_RTS, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678, (uint8_t*)0x12345678, (uint8_t*)0x12345678);

    PRINTF(ti73_recv_RTS, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678, (uint8_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_RTS, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, nullptr, (char*)0x12345678, (uint8_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_RTS, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678, nullptr, (uint8_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_RTS, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti73_recv_RTS, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678, (uint8_t*)0x12345678, nullptr);
    PRINTF(ti80_send_SCR, INT, nullptr);
    PRINTF(ti80_recv_XDP, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti80_recv_XDP, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti80_recv_ACK, INT, nullptr, (uint16_t*)0x12345678);
    PRINTF(ti82_send_VAR, INT, nullptr, 0, 0, (const char*)0x12345678);

    PRINTF(ti82_send_VAR, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti82_send_CTS, INT, nullptr);
    PRINTF(ti82_send_XDP, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(ti82_send_SKP, INT, nullptr, 0);
    PRINTF(ti82_send_ACK, INT, nullptr);
    PRINTF(ti82_send_ERR, INT, nullptr);
    PRINTF(ti82_send_SCR, INT, nullptr);
    PRINTF(ti82_send_KEY, INT, nullptr, 0);
    PRINTF(ti82_send_EOT, INT, nullptr);
    PRINTF(ti82_send_REQ, INT, nullptr, 0, 0, (const char*)0x12345678);

    PRINTF(ti82_send_REQ, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti82_send_RTS, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti82_send_RTS, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti82_recv_VAR, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti82_recv_VAR, INT, (CalcHandle *)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti82_recv_VAR, INT, (CalcHandle *)0x12345678, (uint16_t *)0x12345678, nullptr, (char*)0x12345678);
    PRINTF(ti82_recv_VAR, INT, (CalcHandle *)0x12345678, (uint16_t *)0x12345678, (uint8_t *)0x12345678, nullptr);
    PRINTF(ti82_recv_CTS, INT, nullptr);
    PRINTF(ti82_recv_SKP, INT, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti82_recv_SKP, INT, (CalcHandle*)0x12345678, nullptr);

    PRINTF(ti82_recv_XDP, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti82_recv_XDP, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti82_recv_ACK, INT, nullptr, (uint16_t*)0x12345678);
    PRINTF(ti82_recv_RTS, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti82_recv_RTS, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti82_recv_RTS, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, nullptr, (char*)0x12345678);
    PRINTF(ti82_recv_RTS, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678, nullptr);
    PRINTF(ti85_send_VAR, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti85_send_VAR, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti85_send_CTS, INT, nullptr);

    PRINTF(ti85_send_XDP, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(ti85_send_SKP, INT, nullptr, 0);
    PRINTF(ti85_send_ACK, INT, nullptr);
    PRINTF(ti85_send_ERR, INT, nullptr);
    PRINTF(ti85_send_SCR, INT, nullptr);
    PRINTF(ti85_send_KEY, INT, nullptr, 0);
    PRINTF(ti85_send_EOT, INT, nullptr);
    PRINTF(ti85_send_REQ, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti85_send_REQ, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti85_send_RTS, INT, nullptr, 0, 0, (const char*)0x12345678);

    PRINTF(ti85_send_RTS, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti85_recv_VAR, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti85_recv_VAR, INT, (CalcHandle *)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti85_recv_VAR, INT, (CalcHandle *)0x12345678, (uint16_t *)0x12345678, nullptr, (char*)0x12345678);
    PRINTF(ti85_recv_VAR, INT, (CalcHandle *)0x12345678, (uint16_t *)0x12345678, (uint8_t *)0x12345678, nullptr);
    PRINTF(ti85_recv_CTS, INT, nullptr);
    PRINTF(ti85_recv_SKP, INT, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti85_recv_SKP, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ti85_recv_XDP, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti85_recv_XDP, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678);

    PRINTF(ti85_recv_ACK, INT, nullptr, (uint16_t*)0x12345678);
    PRINTF(ti85_recv_RTS, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti85_recv_RTS, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti85_recv_RTS, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, nullptr, (char*)0x12345678);
    PRINTF(ti85_recv_RTS, INT, (CalcHandle*)0x12345678, (uint16_t*)0x12345678, (uint8_t*)0x12345678, nullptr);
}

static void torture_cmd68k()
{
// cmd68k.c
    PRINTF(ti68k_model_to_dbus_mid, INT, CALC_NONE);
    PRINTF(ti68k_handle_to_dbus_mid, INT, NULL);
    PRINTF(ti89_send_VAR, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti89_send_CTS, INT, nullptr);
    PRINTF(ti89_send_XDP, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(ti89_send_SKP, INT, nullptr, 0);
    PRINTF(ti89_send_ACK, INT, nullptr);
    PRINTF(ti89_send_ERR, INT, nullptr);
    PRINTF(ti89_send_RDY, INT, nullptr);
    PRINTF(ti89_send_SCR, INT, nullptr);

    PRINTF(ti89_send_CNT, INT, nullptr);
    PRINTF(ti89_send_KEY, INT, nullptr, 0);
    PRINTF(ti89_send_EOT, INT, nullptr);
    PRINTF(ti89_send_REQ, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti89_send_REQ, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti89_send_RTS, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti89_send_RTS, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti89_send_RTS2, INT, nullptr, 0, 0, 0);
    PRINTF(ti89_send_VER, INT, nullptr);
    PRINTF(ti89_send_DEL, INT, nullptr, 0, 0, (const char*)0x12345678);

    PRINTF(ti89_send_DEL, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti89_recv_VAR, INT, nullptr, (uint32_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti89_recv_VAR, INT, (CalcHandle *)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti89_recv_VAR, INT, (CalcHandle *)0x12345678, (uint32_t *)0x12345678, nullptr, (char*)0x12345678);
    PRINTF(ti89_recv_VAR, INT, (CalcHandle *)0x12345678, (uint32_t *)0x12345678, (uint8_t *)0x12345678, nullptr);
    PRINTF(ti89_recv_CTS, INT, nullptr);
    PRINTF(ti89_recv_SKP, INT, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti89_recv_SKP, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ti89_recv_XDP, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti89_recv_XDP, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678);

    PRINTF(ti89_send_ACK, INT, nullptr);
    PRINTF(ti89_recv_CNT, INT, nullptr);
    PRINTF(ti89_recv_EOT, INT, nullptr);
    PRINTF(ti89_recv_RTS, INT, nullptr, (uint32_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti89_recv_RTS, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti89_recv_RTS, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, nullptr, (char*)0x12345678);
    PRINTF(ti89_recv_RTS, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, (uint8_t*)0x12345678, nullptr);
    PRINTF(ti92_send_VAR, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti92_send_VAR, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti92_send_CTS, INT, nullptr);

    PRINTF(ti92_send_XDP, INT, nullptr, 0, (uint8_t*)0x12345678);
    PRINTF(ti92_send_SKP, INT, nullptr, 0);
    PRINTF(ti92_send_ACK, INT, nullptr);
    PRINTF(ti92_send_ERR, INT, nullptr);
    PRINTF(ti92_send_RDY, INT, nullptr);
    PRINTF(ti92_send_SCR, INT, nullptr);
    PRINTF(ti92_send_SCR, INT, nullptr);
    PRINTF(ti92_send_CNT, INT, nullptr);
    PRINTF(ti92_send_KEY, INT, nullptr, 0);
    PRINTF(ti92_send_EOT, INT, nullptr);

    PRINTF(ti92_send_REQ, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti92_send_REQ, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti92_send_RTS, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti92_send_RTS, INT, (CalcHandle*)0x12345678, 0, 0, nullptr);
    PRINTF(ti92_recv_VAR, INT, nullptr, (uint32_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti92_recv_VAR, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti92_recv_VAR, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, nullptr, (char*)0x12345678);
    PRINTF(ti92_recv_VAR, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, (uint8_t*)0x12345678, nullptr);
    PRINTF(ti92_recv_CTS, INT, nullptr);
    PRINTF(ti92_recv_SKP, INT, nullptr, (uint8_t*)0x12345678);

    PRINTF(ti92_recv_SKP, INT, (CalcHandle*)0x12345678, nullptr);
    PRINTF(ti92_recv_XDP, INT, nullptr, (uint16_t*)0x12345678, (uint8_t*)0x12345678);
    PRINTF(ti92_recv_XDP, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678);
    PRINTF(ti92_send_ACK, INT, nullptr);
    PRINTF(ti92_recv_CNT, INT, nullptr);
    PRINTF(ti92_recv_EOT, INT, nullptr);
    PRINTF(ti92_recv_RTS, INT, nullptr, (uint32_t*)0x12345678, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti92_recv_RTS, INT, (CalcHandle*)0x12345678, nullptr, (uint8_t*)0x12345678, (char*)0x12345678);
    PRINTF(ti92_recv_RTS, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, nullptr, (char*)0x12345678);
    PRINTF(ti92_recv_RTS, INT, (CalcHandle*)0x12345678, (uint32_t*)0x12345678, (uint8_t*)0x12345678, nullptr);
}

static void torture_romdump()
{
// romdump.c
    PRINTF(rd_send_dumper, INT, nullptr, (const char *)0x12345678, 0, (uint8_t *)0x12345678);
    PRINTF(rd_send_dumper, INT, (CalcHandle *)0x12345678, nullptr, 0, (uint8_t *)0x12345678);
    PRINTF(rd_send_dumper, INT, (CalcHandle *)0x12345678, (const char*)0x12345678, 0, nullptr);
    PRINTF(rd_send_dumper, INT, (CalcHandle *)0x12345678, (const char*)0x12345678, 0, (uint8_t *)0x12345678);
    PRINTF(rd_send_dumper2, INT, nullptr, (const char*)0x12345678);
    PRINTF(rd_send_dumper2, INT, (CalcHandle *)0x12345678, nullptr);
    PRINTF(rd_is_ready, INT, nullptr);
    PRINTF(rd_read_dump, INT, nullptr, (const char *)0x12345678);
    PRINTF(rd_read_dump, INT, (CalcHandle *)0x12345678, nullptr);
}

static const uint8_t dbus_bad_req_ver_80[] = { 0x00, 0x2D };
static const uint8_t dbus_good_req_scr_80[] = { 0x00, 0x6D };
static const uint8_t dbus_good_rep_ack_80[] = { 0x80, 0x56 };
static const uint8_t dbus_good_rep_xdp_80[] = { 0x80, 0x15, 0x04, 0x00, 0x12, 0x34, 0x56, 0x78, 0x14, 0x01 };
static const uint8_t dbus_good_ready_generic[] = { 0x00, 0x56, 0x00, 0x00 };
static const uint8_t dbus_good_ready_89[] = { 0x98, 0x56, 0x00, 0x00 };
static const uint8_t dbus_good_cts_cbl[] = { 0x19, 0x09, 0x00, 0x00 };
// These three packets are the important ones for Send({7}) followed by Get L1, sent by a 89(T) to lab equipment such as CBL2.
static const uint8_t dbus_good_var_inline_list_single_element_single_byte_89[] = { 0x89, 0x06, 0x08, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04, 0x01, 0xFF, 0x00, 0x07, 0x01 };
static const uint8_t dbus_good_xdp_send_inline_list_single_element_single_byte_value_7_89[] = { 0x89, 0x15, 0x07, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x37, 0x00, 0x58, 0x00 };
static const uint8_t dbus_good_req_get_l1_89[] = { 0x89, 0xA2, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00 };

static void dissect_functions_unit_test_1()
{
    assert(ERR_INVALID_PACKET == dbus_dissect(CALC_NONE, stderr, (const uint8_t *)0x12345678, 1));
    assert(ERR_INVALID_PACKET == dbus_dissect(CALC_NONE, stderr, (const uint8_t *)0x12345678, 3));
    assert(ERR_INVALID_PACKET == dbus_dissect(CALC_NONE, stderr, (const uint8_t *)0x12345678, 65543));
    assert(ERR_INVALID_PACKET == dbus_dissect(CALC_NONE, stderr, dbus_bad_req_ver_80, sizeof(dbus_bad_req_ver_80)));
    assert(0 == dbus_dissect(CALC_NONE, stderr, dbus_good_req_scr_80, sizeof(dbus_good_req_scr_80)));
    assert(0 == dbus_dissect(CALC_NONE, stderr, dbus_good_rep_ack_80, sizeof(dbus_good_rep_ack_80)));
    assert(0 == dbus_dissect(CALC_NONE, stderr, dbus_good_rep_xdp_80, sizeof(dbus_good_rep_xdp_80)));
    assert(0 == dbus_dissect(CALC_NONE, stderr, dbus_good_ready_generic, sizeof(dbus_good_ready_generic)));
    assert(0 == dbus_dissect(CALC_NONE, stderr, dbus_good_ready_89, sizeof(dbus_good_ready_89)));
    assert(0 == dbus_dissect(CALC_NONE, stderr, dbus_good_cts_cbl, sizeof(dbus_good_cts_cbl)));
    assert(0 == dbus_dissect(CALC_NONE, stderr, dbus_good_var_inline_list_single_element_single_byte_89, sizeof(dbus_good_var_inline_list_single_element_single_byte_89)));
    assert(0 == dbus_dissect(CALC_NONE, stderr, dbus_good_xdp_send_inline_list_single_element_single_byte_value_7_89, sizeof(dbus_good_xdp_send_inline_list_single_element_single_byte_value_7_89)));
    assert(0 == dbus_dissect(CALC_NONE, stderr, dbus_good_req_get_l1_89, sizeof(dbus_good_req_get_l1_89)));
}

static const uint8_t dusb_bad_raw_type_1[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t dusb_bad_raw_type_2[] = { 0x00, 0x00, 0x00, 0x00, 0x06 };

static const uint8_t dusb_good_buf_size_req[] = { 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x00, 0x04, 0x00 };
static const uint8_t dusb_good_buf_size_alloc[] = { 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x00, 0x00, 0xFA };

static const uint8_t dusb_bad_vpkt_data_final_0001[] = {
0x00, 0x00, 0x00, 0x10,
0x04,
0x00, 0x00, 0x00, 0x09,
0x00, 0x01,
0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0F
};

static const uint8_t dusb_good_vpkt_data_final_0001[] = {
0x00, 0x00, 0x00, 0x10,
0x04,
0x00, 0x00, 0x00, 0x0A,
0x00, 0x01,
0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xA0
};

static const uint8_t dusb_bad_vpkt_data_final_0002[] = {
0x00, 0x00, 0x00, 0x11,
0x04,
0x00, 0x00, 0x00, 0x0A,
0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x92
};

static const uint8_t dusb_good_vpkt_data_final_0002[] = {
0x00, 0x00, 0x00, 0x11,
0x04,
0x00, 0x00, 0x00, 0x0B,
0x00, 0x02,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x92, 0x3B
};

static const uint8_t dusb_good_vpkt_data_final_0003[] = {
0x00, 0x00, 0x00, 0x0E,
0x04,
0x00, 0x00, 0x00, 0x08,
0x00, 0x03,
0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t dusb_good_vpkt_data_final_0007[] = {
0x00, 0x00, 0x00, 0x0A,
0x04,
0x00, 0x00, 0x00, 0x04,
0x00, 0x07,
0x00, 0x01, 0x00, 0x0A
};

static const uint8_t dusb_good_vpkt_data_final_0008[] = {
0x00, 0x00, 0x00, 0x0E,
0x04,
0x00, 0x00, 0x00, 0x08,
0x00, 0x08,
0x00, 0x01, 0x00, 0x0A, 0x00, 0x00, 0x01, 0x01
};

static const uint8_t dusb_good_vpkt_data_final_0012[] = {
0x00, 0x00, 0x00, 0x0A,
0x04,
0x00, 0x00, 0x00, 0x04,
0x00, 0x12,
0x00, 0x00, 0x0F, 0xA0
};

static const uint8_t dusb_good_vpkt_data_final_BB00[] = {
0x00, 0x00, 0x00, 0x0A,
0x04,
0x00, 0x00, 0x00, 0x04,
0xBB, 0x00,
0xFF, 0xFF, 0xFF, 0xFF
};

static const uint8_t dusb_good_vpkt_data_final_DD00[] = {
0x00, 0x00, 0x00, 0x06,
0x04,
0x00, 0x00, 0x00, 0x00,
0xdd, 0x00
};

static const uint8_t dusb_good_vpkt_data_ack[] = { 0x00, 0x00, 0x00, 0x02, 0x05, 0xE0, 0x00 };

static void dissect_functions_unit_test_2()
{
    uint8_t first = 1;

    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, (const uint8_t *)0x12345678, 4, 0, (uint8_t *)0x12345678));
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, (const uint8_t *)0x12345678, 1024, 0, (uint8_t *)0x12345678));
    assert(dusb_bad_raw_type_1[4] < 1);
    assert(dusb_bad_raw_type_2[4] > 5);
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, dusb_bad_raw_type_1, 5, 0, (uint8_t *)0x12345678));
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, dusb_bad_raw_type_2, 5, 0, (uint8_t *)0x12345678));
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, dusb_good_buf_size_req, 6, 0, (uint8_t *)0x12345678));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_buf_size_req, sizeof(dusb_good_buf_size_req), 2, &first));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_buf_size_alloc, sizeof(dusb_good_buf_size_alloc), 1, &first));
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_TI84P_USB, stderr, dusb_bad_vpkt_data_final_0001, sizeof(dusb_bad_vpkt_data_final_0001), 2, &first));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_vpkt_data_final_0001, sizeof(dusb_good_vpkt_data_final_0001), 2, &first));
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_TI84P_USB, stderr, dusb_bad_vpkt_data_final_0002, sizeof(dusb_bad_vpkt_data_final_0002), 2, &first));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_vpkt_data_final_0002, sizeof(dusb_good_vpkt_data_final_0002), 2, &first));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_vpkt_data_final_0003, sizeof(dusb_good_vpkt_data_final_0003), 1, &first));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_vpkt_data_final_0007, sizeof(dusb_good_vpkt_data_final_0007), 2, &first));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_vpkt_data_final_0008, sizeof(dusb_good_vpkt_data_final_0008), 1, &first));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_vpkt_data_final_BB00, sizeof(dusb_good_vpkt_data_final_BB00), 1, &first));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_vpkt_data_final_DD00, sizeof(dusb_good_vpkt_data_final_DD00), 1, &first));
    assert(0 == dusb_dissect(CALC_TI84P_USB, stderr, dusb_good_vpkt_data_ack, sizeof(dusb_good_vpkt_data_ack), 1, &first));
}

static const uint8_t nsp_bad_device_address_request[] = {
0x54, 0xFD, 0x64, 0x00, 0x40, 0x03, 0x64, 0x01, 0x40, 0x03, 0x13, 0x43, 0xFF, 0x00, 0x01, 0xFB,
0x64, 0x01, 0xFF, 0x00
};

static const uint8_t nsp_bad_device_address_request_2[] = {
0x54, 0xFD, 0x64, 0x00, 0x40, 0x03, 0x64, 0x01, 0x40, 0x03, 0x13, 0x43, 0x05, 0x00, 0x01, 0xFB,
0x64, 0x01, 0xFF, 0x00
};

static const uint8_t nsp_good_device_address_request[] = {
0x54, 0xFD, 0x64, 0x00, 0x40, 0x03, 0x64, 0x01, 0x40, 0x03, 0x13, 0x43, 0x04, 0x00, 0x01, 0xFB,
0x64, 0x01, 0xFF, 0x00
};

static const uint8_t nsp_good_keypress_home[] = {
0x54, 0xFD, 0x64, 0x00, 0x80, 0x03, 0x64, 0x01, 0x40, 0x42, 0x4A, 0x7D, 0x1A, 0x00, 0x03, 0x03,
0x00, 0x00, 0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0xFD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void dissect_functions_unit_test_3()
{
    assert(ERR_INVALID_PACKET == nsp_dissect(CALC_NONE, stderr, (const uint8_t *)0x12345678, 16, 0));
    assert(ERR_INVALID_PACKET == nsp_dissect(CALC_NONE, stderr, (const uint8_t *)0x12345678, 272, 0));
    assert(ERR_INVALID_PACKET == nsp_dissect(CALC_NONE, stderr, nsp_bad_device_address_request, sizeof(nsp_bad_device_address_request), 0));
    assert(ERR_INVALID_PACKET == nsp_dissect(CALC_NONE, stderr, nsp_bad_device_address_request_2, sizeof(nsp_bad_device_address_request_2), 0));
    assert(0 == nsp_dissect(CALC_NONE, stderr, nsp_good_device_address_request, sizeof(nsp_good_device_address_request), 0));
    assert(0 == nsp_dissect(CALC_NONE, stderr, nsp_good_keypress_home, sizeof(nsp_good_keypress_home), 0));
}

int main(int argc, char **argv)
{
    ticalcs_library_init();

    torture_ticalcs();
    torture_nsp();
    torture_dusb();
    torture_dbus();
    torture_cmdz80();
    torture_cmd68k();
    torture_romdump();

    dissect_functions_unit_test_1();
    dissect_functions_unit_test_2();
    dissect_functions_unit_test_3();

    ticalcs_library_exit();

    return 0;
}
