#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <locale.h>
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
#include "../src/internal.h"

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
    PRINTF(ticalcs_error_get_raw_protocol_code, INT, -1, nullptr);
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
    PRINTF(ticalcs_calc_send_all_vars_backup, INT, nullptr, (FileContent *)0x12345678);

    PRINTF(ticalcs_calc_send_all_vars_backup, INT, (CalcHandle *)0x12345678, nullptr);
    PRINTF(ticalcs_calc_recv_all_vars_backup, INT, nullptr, (FileContent *)0x12345678);
    PRINTF(ticalcs_calc_recv_all_vars_backup, INT, (CalcHandle *)0x12345678, nullptr);
    PRINTF(ticalcs_calc_send_lab_equipment_data, INT, nullptr, (CalcModel)-1, (CalcLabEquipmentData *)0x12345678);
    PRINTF(ticalcs_calc_send_lab_equipment_data, INT, (CalcHandle *)0x12345678, (CalcModel)-1, nullptr);
    PRINTF(ticalcs_calc_get_lab_equipment_data, INT, nullptr, (CalcModel)-1, (CalcLabEquipmentData *)0x12345678);
    PRINTF(ticalcs_calc_get_lab_equipment_data, INT, (CalcHandle *)0x12345678, (CalcModel)-1, nullptr);
    PRINTF(ticalcs_calc_control_lab_equipment, INT, nullptr, (CalcModel)-1, (CalcLabEquipmentParameters *)0x12345678);
    PRINTF(ticalcs_calc_del_fld, INT, nullptr, (VarRequest *)0x12345678);
    PRINTF(ticalcs_calc_del_fld, INT, (CalcHandle *)0x12345678, nullptr);

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
    PRINTF(ticalcs_calc_send_lab_equipment_datastr, INT, nullptr, (CalcModel)-1, 0, (const char *)0x12345678);

    PRINTF(ticalcs_calc_send_lab_equipment_datastr, INT, (CalcHandle *)0x12345678, (CalcModel)-1, 0, nullptr);
    PRINTF(ticalcs_calc_get_lab_equipment_datastr, INT, nullptr, (CalcModel)-1, 0, (const char **)0x12345678);
    PRINTF(ticalcs_calc_get_lab_equipment_datastr, INT, (CalcHandle *)0x12345678, (CalcModel)-1, 0, nullptr);
    PRINTFVOID(ticalcs_free_lab_equipment_data_related, nullptr);
    PRINTFVOID(ticalcs_calc_free_lab_equipment_data, nullptr);
    { CalcLabEquipmentData labeq_data = { CALC_LAB_EQUIPMENT_DATA_TYPE_NONE, 0, 0, nullptr, nullptr, 4, 0, 0 }; PRINTFVOID(ticalcs_calc_free_lab_equipment_data, &labeq_data); }
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
    PRINTF(tiz80_send_RTS_lab_equipment_data, INT, nullptr, 0, 0, nullptr);
}

static void torture_cmd68k()
{
// cmd68k.c
    PRINTF(ti68k_model_to_dbus_mid, INT, CALC_NONE);
    PRINTF(ti68k_handle_to_dbus_mid, INT, NULL);
    PRINTF(ti89_send_VAR, INT, nullptr, 0, 0, (const char*)0x12345678);
    PRINTF(ti68k_send_VAR_lab_equipment_data, INT, nullptr, 0, 0, 0);
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

// The list returned by the "Get" corresponding to the Send({7}) status command, as captured from a 89(T): 17 items, in the TI-68k wire format.
static const uint8_t ti68k_labeq_17_items_89[] = {
0x11, 0x00, 0x00, 0x00,
0x20, 0x2b, 0x35, 0x2e, 0x30, 0x31, 0x31, 0x33, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +5.01130E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x38, 0x2e, 0x38, 0x38, 0x38, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x33, // " +8.88800E+03"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x31, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +1.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x20, 0x2b, 0x30, 0x2e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x45, 0x2b, 0x30, 0x30, // " +0.00000E+00"
0x00
};

// The converters fill long doubles from decimal text or BCD digits: compare tolerantly.
static int ld_close(long double a, long double b)
{
	long double diff = a - b;
	if (diff < 0)
	{
		diff = -diff;
	}
	return diff <= 1e-15L * ((a < 0 ? -a : a) + (b < 0 ? -b : b)) + 1e-320L;
}

static void lab_equipment_data_unit_test()
{
    CalcLabEquipmentData labeq_data;
    uint32_t item_count;
    long double * raw_values;
    char * out_data;
    size_t i;

    // String -> TI-68k raw list: valid inputs.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{7}"));
    assert(CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST == labeq_data.type);
    assert(7 == labeq_data.size); // [01 00 00 00][0x20 37 00]
    assert(1 == labeq_data.items);
    assert(4 == labeq_data.vartype);
    assert(labeq_data.data[0] == 0x01 && labeq_data.data[1] == 0 && labeq_data.data[2] == 0 && labeq_data.data[3] == 0);
    assert(labeq_data.data[4] == 0x20 && labeq_data.data[5] == '7' && labeq_data.data[6] == 0);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);
    assert(labeq_data.data == nullptr && labeq_data.size == 0);

    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1,2,3}"));
    assert(3 == labeq_data.items);
    assert(11 == labeq_data.size); // [03 00 00 00][0x20 31 0x20 32 0x20 33 00]
    assert(labeq_data.data[0] == 0x03 && labeq_data.data[1] == 0 && labeq_data.data[2] == 0 && labeq_data.data[3] == 0);
    assert(memcmp(labeq_data.data + 4, " 1 2 3", 6) == 0 && labeq_data.data[10] == 0);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    // String -> TI-68k raw list: floating-point, negative and exponent numbers.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{3,-1}"));
    assert(2 == labeq_data.items);
    assert(memcmp(labeq_data.data + 4, " 3 -1", 5) == 0);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{3,.1,-1,0}"));
    assert(4 == labeq_data.items);
    assert(memcmp(labeq_data.data + 4, " 3 .1 -1 0", 10) == 0);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{+1.5e-3,-.25E+2}"));
    assert(2 == labeq_data.items);
    assert(memcmp(labeq_data.data + 4, " +1.5e-3 -.25E+2", 16) == 0);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    // String -> TI-68k raw list: invalid inputs.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "7"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{a}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1,}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1.2.3}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1e}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1e+}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{-.}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1,2 3}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, ""));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, nullptr));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(nullptr, "{1}"));
    // An input string with more than 65535 items must be rejected rather than overflowing the item count.
    {
        char * huge = (char *)g_malloc(4 + 2 * 65536);
        assert(nullptr != huge);
        huge[0] = '{';
        for (i = 0; i < 65536; i++)
        {
            huge[1 + 2 * i] = '1';
            huge[2 + 2 * i] = ',';
        }
        huge[1 + 2 * 65536 - 1] = '}';
        huge[1 + 2 * 65536] = 0;
        ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
        assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, huge));
        g_free(huge);
    }

    // String -> TI-Z80 raw list (9-byte floating-point format): valid inputs.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{7}"));
    assert(CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST == labeq_data.type);
    assert(11 == labeq_data.size); // [01 00][00 80 70 00 00 00 00 00 00]
    assert(1 == labeq_data.items);
    assert(0x01 == labeq_data.vartype);
    {
        static const uint8_t expected[] = { 0x01, 0x00, 0x00, 0x80, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        assert(0 == memcmp(labeq_data.data, expected, sizeof(expected)));
    }
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{0,-8888.8}"));
    assert(20 == labeq_data.size && 2 == labeq_data.items);
    {
        static const uint8_t expected[] = { 0x02, 0x00,
                                            0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x80, 0x83, 0x88, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00 };
        assert(0 == memcmp(labeq_data.data, expected, sizeof(expected)));
    }
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{3.14159265358979}"));
    {
        // Lossless round trip: the full 14-digit mantissa survives the string conversion.
        assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
        assert(0 == strcmp(out_data, "{3.1415926535898}"));
        ticalcs_free_lab_equipment_data_related(out_data);
        ticalcs_free_lab_equipment_data_related(raw_values);
    }
    assert(11 == labeq_data.size);
    {
        static const uint8_t expected[] = { 0x01, 0x00, 0x00, 0x80, 0x31, 0x41, 0x59, 0x26, 0x53, 0x58, 0x98 };
        assert(0 == memcmp(labeq_data.data, expected, sizeof(expected)));
    }
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1e99,1e-99}"));
    assert(20 == labeq_data.size && 2 == labeq_data.items);
    assert(labeq_data.data[2] == 0x00 && labeq_data.data[3] == 0xE3);
    assert(memcmp(labeq_data.data + 4, "\x10\x00\x00\x00\x00\x00\x00", 7) == 0);
    assert(labeq_data.data[11] == 0x00 && labeq_data.data[12] == 0x1D);
    assert(memcmp(labeq_data.data + 13, "\x10\x00\x00\x00\x00\x00\x00", 7) == 0);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    // String -> TI-Z80 raw list: invalid inputs.
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "7"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{a}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1,}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, nullptr));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(nullptr, "{1}"));
    // Magnitudes beyond the +/-99 exponent range of the TI-Z80 models must be rejected rather than truncated silently.
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_OUT_OF_RANGE == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1e100}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_OUT_OF_RANGE == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{-1e100}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_OUT_OF_RANGE == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1e-100}"));
    // Rounding this value up to the next power of ten would overflow the exponent range.
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_OUT_OF_RANGE == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{9.999999999999994e99}"));
    // An input string yielding an oversized list payload must be rejected.
    {
        char * huge = (char *)g_malloc(2 + 2 * 7500 + 2);
        assert(nullptr != huge);
        huge[0] = '{';
        for (i = 0; i < 7500; i++)
        {
            huge[1 + 2 * i] = '1';
            huge[2 + 2 * i] = ',';
        }
        huge[1 + 2 * 7500 - 1] = '}';
        huge[1 + 2 * 7500] = 0;
        ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
        assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, huge));
        ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
        assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, huge));
        g_free(huge);
    }
    // The largest TI-Z80 list which fits into a 16-bit XDP packet length is accepted.
    {
        const unsigned int count = (0xFFFF - 2) / 9;
        char * huge = (char *)g_malloc(2 + 2 * count + 2);
        assert(nullptr != huge);
        huge[0] = '{';
        for (i = 0; i < count; i++)
        {
            huge[1 + 2 * i] = '1';
            huge[2 + 2 * i] = ',';
        }
        huge[1 + 2 * count - 1] = '}';
        huge[1 + 2 * count] = 0;
        ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
        assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, huge));
        assert((uint16_t)(2 + count * 9) == labeq_data.size);
        ticalcs_calc_free_lab_equipment_data(&labeq_data);
        g_free(huge);
    }

    // String -> TI-85/86 raw list (10-byte floating-point format): valid inputs.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{7}"));
    assert(CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST == labeq_data.type);
    assert(12 == labeq_data.size); // [01 00][00 00 FC 70 00 00 00 00 00 00]
    assert(1 == labeq_data.items);
    assert(0x04 == labeq_data.vartype);
    {
        static const uint8_t expected[] = { 0x01, 0x00, 0x00, 0x00, 0xFC, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        assert(0 == memcmp(labeq_data.data, expected, sizeof(expected)));
    }
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{-8888.8,1e-99}"));
    assert(22 == labeq_data.size && 2 == labeq_data.items);
    {
        static const uint8_t expected[] = { 0x02, 0x00,
                                            0x80, 0x03, 0xFC, 0x88, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x9D, 0xFB, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        assert(0 == memcmp(labeq_data.data, expected, sizeof(expected)));
    }
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    // The TI-85/86 16-bit exponent field spans +/-999, far beyond the +/-99 limit of the 9-byte format
    // and beyond the range of doubles themselves: magnitudes which the TI-Z80 format rejects are valid.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{-1e100}"));
    assert(12 == labeq_data.size);
    {
        static const uint8_t expected[] = { 0x01, 0x00, 0x80, 0x64, 0xFC, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        assert(0 == memcmp(labeq_data.data, expected, sizeof(expected)));
    }
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    // Rounding this value up to the next power of ten lands at exponent 100: within the TI-85/86 range.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{9.999999999999994e99}"));
    assert(12 == labeq_data.size);
    assert(labeq_data.data[3] == 0x64 && labeq_data.data[4] == 0xFC);
    assert(memcmp(labeq_data.data + 5, "\x10\x00\x00\x00\x00\x00\x00", 7) == 0);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1e308}"));
    assert(12 == labeq_data.size);
    assert(labeq_data.data[2] == 0x00 && labeq_data.data[3] == 0x34 && labeq_data.data[4] == 0xFD);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    // Decimal exponents beyond the double range are handled exactly, without a detour through doubles:
    // the TI-85/86 format accepts them just like real calculators do.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1e400,-1e-400}"));
    assert(22 == labeq_data.size && 2 == labeq_data.items);
    assert(labeq_data.data[2] == 0x00 && labeq_data.data[3] == 0x90 && labeq_data.data[4] == 0xFD);
    assert(memcmp(labeq_data.data + 5, "\x10\x00\x00\x00\x00\x00\x00", 7) == 0);
    assert(labeq_data.data[12] == 0x80 && labeq_data.data[13] == 0x70 && labeq_data.data[14] == 0xFA);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    // String -> TI-85/86 raw list: invalid inputs.
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "7"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{a}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, nullptr));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_from_string(nullptr, "{1}"));
    // The +/-999 exponent range of the format must be enforced.
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_OUT_OF_RANGE == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1e1000}"));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(ERR_OUT_OF_RANGE == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{1e-1000}"));

    // TI-68k raw list -> string: empty list.
    static const uint8_t empty_list[] = { 0x00, 0x00, 0x00, 0x00 };
    memset(&labeq_data, 0, sizeof(labeq_data));
    labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
    labeq_data.size = sizeof(empty_list);
    labeq_data.items = 0;
    labeq_data.data = empty_list;
    assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
    assert(0 == item_count && nullptr == raw_values);
    assert(0 == strcmp(out_data, "{}"));
    ticalcs_free_lab_equipment_data_related(out_data);

    // TI-68k raw list -> string: the captured 17-item list.
    memset(&labeq_data, 0, sizeof(labeq_data));
    labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
    labeq_data.size = sizeof(ti68k_labeq_17_items_89);
    labeq_data.items = 17;
    labeq_data.data = ti68k_labeq_17_items_89;
    assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
    assert(17 == item_count && nullptr != raw_values);
    assert(0 == strcmp(out_data, "{+5.01130E+00,+0.00000E+00,+0.00000E+00,+8.88800E+03,+0.00000E+00,+0.00000E+00,+0.00000E+00,+0.00000E+00,+0.00000E+00,+0.00000E+00,+0.00000E+00,+0.00000E+00,+0.00000E+00,+1.00000E+00,+0.00000E+00,+0.00000E+00,+0.00000E+00}"));
    assert(ld_close(raw_values[0], 5.0113L) && ld_close(raw_values[3], 8888.0L) && ld_close(raw_values[13], 1.0L) && ld_close(raw_values[16], 0.0L));
    ticalcs_free_lab_equipment_data_related(out_data);
    ticalcs_free_lab_equipment_data_related(raw_values);

    // TI-68k raw list -> string: round trip through the string form.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{5,6,7}"));
    assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
    assert(0 == strcmp(out_data, "{5,6,7}"));
    assert(3 == item_count && ld_close(raw_values[0], 5.0L) && ld_close(raw_values[2], 7.0L));
    ticalcs_free_lab_equipment_data_related(out_data);
    ticalcs_free_lab_equipment_data_related(raw_values);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    // TI-68k raw list -> string: payload without a trailing NUL must still be parsed.
    static const uint8_t payload_no_nul[] = { 0x01, 0x00, 0x00, 0x00, 0x20, 0x37 };
    memset(&labeq_data, 0, sizeof(labeq_data));
    labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
    labeq_data.size = sizeof(payload_no_nul);
    labeq_data.items = 1;
    labeq_data.data = payload_no_nul;
    assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
    assert(0 == strcmp(out_data, "{7}"));
    ticalcs_free_lab_equipment_data_related(out_data);
    ticalcs_free_lab_equipment_data_related(raw_values);

    // TI-68k raw list -> string: malformed inputs must be rejected, not overrun.
    static const uint8_t too_small[] = { 0x01, 0x00, 0x00 };
    static const uint8_t garbage_payload[] = { 0x01, 0x00, 0x00, 0x00, 0x20, 0x58, 0x58, 0x00 }; // " XX"
    static const uint8_t item_count_mismatch[] = { 0x02, 0x00, 0x00, 0x00, 0x20, 0x37, 0x00 }; // 2 items announced, 1 provided
    static const uint8_t too_many_items[] = { 0x00, 0x80, 0x00, 0x00, 0x20, 0x37, 0x00 }; // 32768 items announced
    static const uint8_t huge_count[] = { 0x01, 0x00, 0x01, 0x00, 0x20, 0x37, 0x00 }; // 65537 items announced
    memset(&labeq_data, 0, sizeof(labeq_data));
    labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
    labeq_data.size = sizeof(too_small);
    labeq_data.items = 1;
    labeq_data.data = too_small;
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));

    memset(&labeq_data, 0, sizeof(labeq_data));
    labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
    labeq_data.size = sizeof(garbage_payload);
    labeq_data.items = 1;
    labeq_data.data = garbage_payload;
    assert(ERR_INVALID_PACKET == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));

    memset(&labeq_data, 0, sizeof(labeq_data));
    labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
    labeq_data.size = sizeof(item_count_mismatch);
    labeq_data.items = 2;
    labeq_data.data = item_count_mismatch;
    assert(ERR_INVALID_PACKET == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));

    memset(&labeq_data, 0, sizeof(labeq_data));
    labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
    labeq_data.size = sizeof(too_many_items);
    labeq_data.items = 32768;
    labeq_data.data = too_many_items;
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));

    memset(&labeq_data, 0, sizeof(labeq_data));
    labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST;
    labeq_data.size = sizeof(huge_count);
    labeq_data.items = (uint16_t)(65537 & 0xFFFF);
    labeq_data.data = huge_count;
    // The 16-bit items field can't represent the 32-bit count carried by the raw data.
    assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));

    // Raw list -> string, TI-Z80 9-byte floating-point format: valid inputs.
    {
        static const uint8_t raw[] = { 0x02, 0x00,
                                       0x00, 0x80, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x80, 0x83, 0x88, 0x88, 0x80, 0x00, 0x00, 0x00, 0x00 };
        memset(&labeq_data, 0, sizeof(labeq_data));
        labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST;
        labeq_data.size = sizeof(raw);
        labeq_data.items = 2;
        labeq_data.data = raw;
        assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
        assert(2 == item_count && nullptr != raw_values && nullptr != out_data);
        assert(0 == strcmp(out_data, "{7,-8888.8}"));
        assert(ld_close(raw_values[0], 7.0L) && ld_close(raw_values[1], -8888.8L));
        ticalcs_free_lab_equipment_data_related(out_data);
        ticalcs_free_lab_equipment_data_related(raw_values);
    }

    // Round trip through the string form.
    memset(&labeq_data, 0, sizeof(labeq_data));
    ticalcs_fill_lab_equipment_data(&labeq_data, CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST, 0, 0, nullptr, nullptr, 0, 0, 0);
    assert(0 == ticalcs_convert_lab_equipment_data_from_string(&labeq_data, "{5.0113,0,8888.8,1}"));
    assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
    assert(4 == item_count);
    assert(0 == strcmp(out_data, "{5.0113,0,8888.8,1}"));
    ticalcs_free_lab_equipment_data_related(out_data);
    ticalcs_free_lab_equipment_data_related(raw_values);
    ticalcs_calc_free_lab_equipment_data(&labeq_data);

    // The elements of the captured Get(L1 dumps decode as expected.
    {
        static const uint8_t dump82[] = { 0x04, 0x00,
                                          0x00, 0x80, 0x50, 0x11, 0x30, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x83, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x80, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        memset(&labeq_data, 0, sizeof(labeq_data));
        labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST;
        labeq_data.size = sizeof(dump82);
        labeq_data.items = 4;
        labeq_data.data = dump82;
        assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
        assert(0 == strcmp(out_data, "{5.0113,8888,1,0}"));
        ticalcs_free_lab_equipment_data_related(out_data);
        ticalcs_free_lab_equipment_data_related(raw_values);
    }
    {
        // Same values in the 10-byte format used by the TI-85/86 and by the Get direction
        // of the TI-73/83/83+/84+ lab equipment protocol.
        static const uint8_t dump85[] = { 0x04, 0x00,
                                          0x00, 0x00, 0xFC, 0x50, 0x11, 0x30, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x03, 0xFC, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0xFC, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        memset(&labeq_data, 0, sizeof(labeq_data));
        labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST;
        labeq_data.size = sizeof(dump85);
        labeq_data.items = 4;
        labeq_data.data = dump85;
        assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
        assert(0 == strcmp(out_data, "{5.0113,8888,1,0}"));
        ticalcs_free_lab_equipment_data_related(out_data);
        ticalcs_free_lab_equipment_data_related(raw_values);
    }

    // Raw list -> string: empty list.
    memset(&labeq_data, 0, sizeof(labeq_data));
    labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST;
    labeq_data.size = 2;
    labeq_data.items = 0;
    labeq_data.data = (const uint8_t *)"\x00\x00";
    assert(0 == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));
    assert(0 == item_count && nullptr == raw_values);
    assert(0 == strcmp(out_data, "{}"));
    ticalcs_free_lab_equipment_data_related(out_data);

    // Raw list -> string: invalid inputs.
    {
        static const uint8_t bad_count[] = { 0x03, 0x00, 0x00, 0x80, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        static const uint8_t bad_exp[] = { 0x01, 0x00, 0x00, 0xFF, 0x3F, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        memset(&labeq_data, 0, sizeof(labeq_data));
        labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST;
        labeq_data.size = sizeof(bad_count);
        labeq_data.items = 1; // embedded count says 3
        labeq_data.data = bad_count;
        assert(ERR_INVALID_PACKET == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));

        memset(&labeq_data, 0, sizeof(labeq_data));
        labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI8586_RAW_LIST;
        labeq_data.size = sizeof(bad_exp);
        labeq_data.items = 1; // exponent field 0x3FFF - 0xFC00 = 1023 > 999
        labeq_data.data = bad_exp;
        assert(ERR_OUT_OF_RANGE == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));

        memset(&labeq_data, 0, sizeof(labeq_data));
        labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TIZ80_RAW_LIST;
        labeq_data.size = sizeof(bad_count) - 1; // truncated payload
        labeq_data.items = 1;
        labeq_data.data = bad_count;
        assert(ERR_INVALID_PACKET == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));

        memset(&labeq_data, 0, sizeof(labeq_data));
        labeq_data.type = CALC_LAB_EQUIPMENT_DATA_TYPE_TI68K_RAW_LIST; // wrong type for the converter
        labeq_data.size = sizeof(bad_count);
        labeq_data.items = 1;
        labeq_data.data = bad_count;
        assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, &item_count, &raw_values, &out_data));

        assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_to_string(nullptr, &item_count, &raw_values, &out_data));
        assert(ERR_INVALID_PARAMETER == ticalcs_convert_lab_equipment_data_to_string(&labeq_data, nullptr, &raw_values, &out_data));
    }

    ticalcs_calc_free_lab_equipment_data(nullptr);
}

int main(int argc, char **argv)
{
    ticalcs_library_init();
    // Pin the decimal point to '.', so that the expected output strings of the conversion tests are locale-independent.
    setlocale(LC_ALL, "C");

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
    lab_equipment_data_unit_test();

    ticalcs_library_exit();

    return 0;
}
