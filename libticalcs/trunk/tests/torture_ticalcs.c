#include <stdio.h>
#include <assert.h>
#include <ticalcs.h>
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

#define PRINTF(FUNCTION, TYPE, args...) \
fprintf(stderr, "%d\t" TYPE "\n", __LINE__, FUNCTION(args))

#define PRINTFVOID(FUNCTION, args...) \
FUNCTION(args); fprintf(stderr, "%d\n", __LINE__)

#define INT "%d"
#define UINT "%u"
#define PTR "%p"
#define STR "\"%s\""
#define VOID ""

static void torture_ticalcs(void)
{
    CalcHandle * handle;
    CableHandle * cable;
    CalcUpdate * update;

// error.c
    PRINTF(ticalcs_error_get, INT, -1, NULL);
    PRINTFVOID(ticalcs_error_free, NULL);
// ticalcs.c
    handle = ticalcs_handle_new(-1);
    PRINTF(, PTR, handle);
    ticalcs_handle_del(handle);
    PRINTF(ticalcs_handle_del, INT, NULL);
    PRINTF(ticalcs_handle_show, INT, NULL);
    PRINTF(ticalcs_get_model, INT, NULL);
    PRINTF(ticalcs_cable_attach, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_cable_detach, INT, NULL);
    cable = ticalcs_cable_get(NULL);
    PRINTF(, PTR, cable);
    PRINTF(ticalcs_update_set, INT, NULL, (void *)0x12345678);

    update = ticalcs_update_get(NULL);
    PRINTF(, PTR, update);
    PRINTF(ticalcs_model_supports_dbus, INT, CALC_NONE);
    PRINTF(ticalcs_model_supports_dusb, INT, CALC_NONE);
    PRINTF(ticalcs_model_supports_nsp, INT, CALC_NONE);
    PRINTF(ticalcs_model_supports_installing_flashapps, INT, CALC_NONE);
    PRINTF(ticalcs_calc_get_event_hook, PTR, NULL);
    PRINTF(ticalcs_calc_set_event_hook, PTR, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_event_user_pointer, PTR, NULL);
    PRINTF(ticalcs_calc_set_event_user_pointer, PTR, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_event_count, UINT, NULL);

    PRINTF(ticalcs_calc_fire_user_event, INT, NULL, 0, 0, (void *)0x12345678, 0);
// calc_xx.c
    PRINTF(ticalcs_calc_features, INT, NULL);
    PRINTF(ticalcs_calc_isready, INT, NULL);
    PRINTF(ticalcs_calc_send_key, INT, NULL, -1);
    PRINTF(ticalcs_calc_execute, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_execute, INT, (void *)0x12345678, NULL, NULL);
    PRINTF(ticalcs_calc_recv_screen, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_screen, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_screen, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_recv_screen_rgb888, INT, NULL, (void *)0x12345678, (void *)0x12345678);

    PRINTF(ticalcs_calc_recv_screen_rgb888, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_screen_rgb888, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTFVOID(ticalcs_free_screen, NULL);
    PRINTF(ticalcs_calc_get_dirlist, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_dirlist, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_dirlist, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_get_memfree, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_memfree, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_get_memfree, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_backup, INT, NULL, (void *)0x12345678);

    PRINTF(ticalcs_calc_send_backup, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_recv_backup, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_backup, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_var, INT, NULL, -1, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_var, INT, (void *)0x12345678, -1, NULL);
    PRINTF(ticalcs_calc_recv_var, INT, NULL, -1, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_var, INT, (void *)0x12345678, -1, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_var, INT, (void *)0x12345678, -1, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_var_ns, INT, NULL, -1, NULL);
    PRINTF(ticalcs_calc_send_var_ns, INT, NULL, -1, (void *)0x12345678);

    PRINTF(ticalcs_calc_recv_var_ns, INT, NULL, -1, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_var_ns, INT, (void *)0x12345678, -1, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_var_ns, INT, (void *)0x12345678, -1, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_app, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_app, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_recv_app, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_app, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_app, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_os, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_os, INT, (void *)0x12345678, NULL);

    PRINTF(ticalcs_calc_recv_idlist, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_idlist, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_dump_rom_1, INT, NULL);
    PRINTF(ticalcs_calc_dump_rom_2, INT, NULL, -1, (void *)0x12345678);
    PRINTF(ticalcs_calc_dump_rom_2, INT, (void *)0x12345678, -1, NULL);
    PRINTF(ticalcs_calc_set_clock, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_set_clock, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_get_clock, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_clock, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_new_fld, INT, NULL, (void *)0x12345678);

    PRINTF(ticalcs_calc_new_fld, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_del_var, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_del_var, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_rename_var, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_rename_var, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_rename_var, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_change_attr, INT, NULL, (void *)0x12345678, -1);
    PRINTF(ticalcs_calc_change_attr, INT, (void *)0x12345678, NULL, -1);
    PRINTF(ticalcs_calc_get_version, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_version, INT, (void *)0x12345678, NULL);

    PRINTF(ticalcs_calc_send_cert, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_cert, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_recv_cert, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_cert, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_tigroup, INT, NULL, (void *)0x12345678, -1);
    PRINTF(ticalcs_calc_send_tigroup, INT, (void *)0x12345678, NULL, -1);
    PRINTF(ticalcs_calc_recv_tigroup, INT, NULL, (void *)0x12345678, -1);
    PRINTF(ticalcs_calc_recv_tigroup, INT, (void *)0x12345678, NULL, -1);
    PRINTF(ticalcs_calc_send_backup2, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_backup2, INT, (void *)0x12345678, NULL);

    PRINTF(ticalcs_calc_recv_backup2, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_backup2, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_var2, INT, NULL, -1, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_var2, INT, (void *)0x12345678, -1, NULL);
    PRINTF(ticalcs_calc_recv_var2, INT, NULL, -1, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_var2, INT, (void *)0x12345678, -1, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_var2, INT, (void *)0x12345678, -1, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_var_ns2, INT, NULL, -1, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_var_ns2, INT, (void *)0x12345678, -1, NULL);
    PRINTF(ticalcs_calc_recv_var_ns2, INT, NULL, -1, (void *)0x12345678, (void *)0x12345678);

    PRINTF(ticalcs_calc_recv_var_ns2, INT, (void *)0x12345678, -1, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_var_ns2, INT, (void *)0x12345678, -1, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_app2, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_app2, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_recv_app2, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_app2, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_app2, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_cert2, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_cert2, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_recv_cert2, INT, NULL, (void *)0x12345678);

    PRINTF(ticalcs_calc_recv_cert2, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_os2, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_send_os2, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_send_tigroup2, INT, NULL, (void *)0x12345678, -1);
    PRINTF(ticalcs_calc_send_tigroup2, INT, (void *)0x12345678, NULL, -1);
    PRINTF(ticalcs_calc_recv_tigroup2, INT, NULL, (void *)0x12345678, -1);
    PRINTF(ticalcs_calc_recv_tigroup2, INT, (void *)0x12345678, NULL, -1);
// dirlist.c
    PRINTFVOID(ticalcs_dirlist_destroy, NULL);
    PRINTFVOID(ticalcs_dirlist_display, NULL);
    PRINTF(ticalcs_dirlist_ram_used, INT, NULL);

    PRINTF(ticalcs_dirlist_flash_used, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_dirlist_flash_used, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_dirlist_ve_count, INT, NULL);
    PRINTF(ticalcs_dirlist_ve_exist, PTR, NULL, (void *)0x12345678);
    PRINTF(ticalcs_dirlist_ve_exist, PTR, (void *)0x12345678, NULL);
    PRINTFVOID(ticalcs_dirlist_ve_add, NULL, (void *)0x12345678);
    PRINTFVOID(ticalcs_dirlist_ve_add, (void *)0x12345678, NULL);
    PRINTFVOID(ticalcs_dirlist_ve_del, NULL, (void *)0x12345678);
    PRINTFVOID(ticalcs_dirlist_ve_del, (void *)0x12345678, NULL);
// type2str.c
    PRINTF(ticalcs_model_to_string, STR, -1);

    PRINTF(ticalcs_string_to_model, INT, NULL);
    PRINTF(ticalcs_scrfmt_to_string, STR, -1);
    PRINTF(ticalcs_string_to_scrfmt, INT, NULL);
    PRINTF(ticalcs_pathtype_to_string, STR, -1);
    PRINTF(ticalcs_string_to_pathtype, INT, NULL);
    PRINTF(ticalcs_memtype_to_string, STR, -1);
    PRINTF(ticalcs_string_to_memtype, INT, NULL);
    PRINTF(ticalcs_infos_to_string, INT, NULL, (void *)0x12345678, 0x12345678);
// clock.c
    PRINTF(ticalcs_clock_format2date, STR, -1, -1);
    PRINTF(ticalcs_clock_date2format, INT, -1, NULL);

    PRINTF(ticalcs_clock_show, INT, -1, NULL);
// screen.c
    PRINTF(ticalcs_screen_convert_bw_to_rgb888, INT, NULL, 0x12345678, 0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_screen_convert_bw_to_rgb888, INT, (void *)0x12345678, 0x12345678, 0x12345678, NULL);
    PRINTF(ticalcs_screen_convert_bw_to_blurry_rgb888, INT, NULL, 0x12345678, 0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_screen_convert_bw_to_blurry_rgb888, INT, (void *)0x12345678, 0x12345678, 0x12345678, NULL);
    PRINTF(ticalcs_screen_convert_gs4_to_rgb888, INT, NULL, 0x12345678, 0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_screen_convert_gs4_to_rgb888, INT, (void *)0x12345678, 0x12345678, 0x12345678, NULL);
    PRINTF(ticalcs_screen_convert_rgb565le_to_rgb888, INT, NULL, 0x12345678, 0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_screen_convert_rgb565le_to_rgb888, INT, (void *)0x12345678, 0x12345678, 0x12345678, NULL);
    PRINTF(ticalcs_screen_convert_native_to_rgb888, INT, 1, NULL, 0x12345678, 0x12345678, (void *)0x12345678);

    PRINTF(ticalcs_screen_convert_native_to_rgb888, INT, 1, (void *)0x12345678, 0x12345678, 0x12345678, NULL);
    PRINTF(ticalcs_screen_nspire_rle_uncompress, INT, 1, (void *)0x12345678, 0x12345678, NULL, 0x12345678);
    PRINTF(ticalcs_screen_nspire_rle_uncompress, INT, 1, NULL, 0x12345678, (void *)0x12345678, 0x12345678);
    PRINTF(ticalcs_screen_84pcse_rle_uncompress, INT, (void *)0x12345678, 0x12345678, NULL, 0x12345678);
    PRINTF(ticalcs_screen_84pcse_rle_uncompress, INT, NULL, 0x12345678, (void *)0x12345678, 0x12345678);
// tikeys.c
    PRINTF(ticalcs_keys_73, PTR, 0);
    PRINTF(ticalcs_keys_83, PTR, 0);
    PRINTF(ticalcs_keys_83p, PTR, 0);
    PRINTF(ticalcs_keys_86, PTR, 0);
    PRINTF(ticalcs_keys_89, PTR, 0);

    PRINTF(ticalcs_keys_92p, PTR, 0);
// probe.c
    PRINTF(ticalcs_probe_calc, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_probe_calc, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_probe_usb_calc, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_probe_usb_calc, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_probe, INT, -1, -1, NULL, -1);
    PRINTF(ticalcs_device_info_to_model, INT, NULL);
    PRINTF(ticalcs_remap_model_from_usb, INT, 0, 0);
    PRINTF(ticalcs_remap_model_to_usb, INT, 0, 0);
// dbus_pkt.c
    PRINTF(dbus_send, INT, NULL, 0, 0, 0, (void *)0x12345678);

    PRINTF(dbus_recv, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(dbus_recv_header, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv_header, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv_header, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(dbus_recv_header, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(dbus_recv_data, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv_data, INT, (void *)0x12345678, NULL, (void *)0x12345678);

    PRINTF(dbus_recv_data, INT, (void *)0x12345678, (void *)0x12345678, NULL);
// dusb_rpkt.c
    PRINTF(dusb_send, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_send, INT, (void *)0x12345678, NULL);
    PRINTF(dusb_recv, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_recv, INT, (void *)0x12345678, NULL);
// nsp_rpkt.c
    PRINTF(nsp_send, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_send, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_recv, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_recv, INT, (void *)0x12345678, NULL);
}

static void torture_nsp(void)
{
    void * ptr;

// nsp_rpkt.c
    PRINTF(nsp_addr2name, STR, 0);
    PRINTF(nsp_sid2name, STR, 0);
    PRINTF(nsp_dissect, INT, CALC_NONE, NULL, (void *)0x12345678, 8, 2);
    PRINTF(nsp_dissect, INT, CALC_NONE, (void *)0x12345678, NULL, 8, 2);
// nsp_vpkt.c
    PRINTF(nsp_vtl_pkt_new, PTR, NULL);
    PRINTF(nsp_vtl_pkt_new_ex, PTR, NULL, 0x12345678, 0x1234, 0x1234, 0x1234, 0x1234, 0x12, (void *)0x12345678);
    PRINTFVOID(nsp_vtl_pkt_fill, NULL, 0x12345678, 0x1234, 0x1234, 0x1234, 0x1234, 0x12, NULL);
    PRINTFVOID(nsp_vtl_pkt_del, NULL, (void *)0x12345678);
    PRINTFVOID(nsp_vtl_pkt_del, (void *)0x12345678, NULL);
    ptr = nsp_vtl_pkt_alloc_data(0);
    PRINTF(, PTR, ptr);
    nsp_vtl_pkt_free_data(ptr);

    ptr = nsp_vtl_pkt_realloc_data(NULL, 1);
    PRINTF(, PTR, ptr);
    nsp_vtl_pkt_free_data(ptr);
    PRINTFVOID(nsp_vtl_pkt_free_data, NULL);
    PRINTF(nsp_session_open, INT, NULL, 0);
    PRINTF(nsp_session_close, INT, NULL);
    PRINTF(nsp_addr_request, INT, NULL);
    PRINTF(nsp_addr_assign, INT, NULL, 0);
    PRINTF(nsp_send_ack, INT, NULL);
    PRINTF(nsp_recv_ack, INT, NULL);
    PRINTF(nsp_send_nack, INT, NULL);
    PRINTF(nsp_send_nack_ex, INT, NULL, 0);

    PRINTF(nsp_send_data, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_send_data, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_recv_data, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_recv_data, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_send_disconnect, INT, NULL);
    PRINTF(nsp_recv_disconnect, INT, NULL);
// nsp_cmd.c
    PRINTF(nsp_cmd_r_login, INT, NULL);
    PRINTF(nsp_cmd_s_status, INT, NULL, 0);
    PRINTF(nsp_cmd_r_status, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_dev_infos, INT, NULL, 0);

    PRINTF(nsp_cmd_r_dev_infos, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(nsp_cmd_r_dev_infos, INT, (void *)0x12345678, (void *)0x12345678, NULL, NULL);
    PRINTF(nsp_cmd_r_dev_infos, INT, (void *)0x12345678, NULL, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_r_dev_infos, INT, (void *)0x12345678, NULL, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_screen_rle, INT, NULL, 0);
    PRINTF(nsp_cmd_r_screen_rle, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(nsp_cmd_r_screen_rle, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(nsp_cmd_r_screen_rle, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_r_screen_rle, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_s_dir_attributes, INT, NULL, (void *)0x12345678);

    PRINTF(nsp_cmd_s_dir_attributes, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_r_dir_attributes, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(nsp_cmd_s_dir_enum_init, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_dir_enum_init, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_r_dir_enum_init, INT, NULL);
    PRINTF(nsp_cmd_s_dir_enum_next, INT, NULL);
    PRINTF(nsp_cmd_r_dir_enum_next, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(nsp_cmd_r_dir_enum_next, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(nsp_cmd_s_dir_enum_done, INT, NULL);
    PRINTF(nsp_cmd_r_dir_enum_done, INT, NULL);

    PRINTF(nsp_cmd_s_put_file, INT, NULL, (void *)0x12345678, 0);
    PRINTF(nsp_cmd_s_put_file, INT, (void *)0x12345678, NULL, 0);
    PRINTF(nsp_cmd_r_put_file, INT, NULL);
    PRINTF(nsp_cmd_s_get_file, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_get_file, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_r_get_file, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_del_file, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_del_file, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_r_del_file, INT, NULL);
    PRINTF(nsp_cmd_s_new_folder, INT, NULL, (void *)0x12345678);

    PRINTF(nsp_cmd_s_new_folder, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_r_new_folder, INT, NULL);
    PRINTF(nsp_cmd_s_del_folder, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_del_folder, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_r_del_folder, INT, NULL);
    PRINTF(nsp_cmd_s_copy_file, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(nsp_cmd_s_copy_file, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_copy_file, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_r_copy_file, INT, NULL);
    PRINTF(nsp_cmd_s_rename_file, INT, NULL, (void *)0x12345678, (void *)0x12345678);

    PRINTF(nsp_cmd_s_rename_file, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_rename_file, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_r_rename_file, INT, NULL);
    PRINTF(nsp_cmd_s_file_ok, INT, NULL);
    PRINTF(nsp_cmd_r_file_ok, INT, NULL);
    PRINTF(nsp_cmd_s_file_contents, INT, NULL, 0, (void *)0x12345678);
    PRINTF(nsp_cmd_s_file_contents, INT, (void *)0x12345678, 0, NULL);
    PRINTF(nsp_cmd_r_file_contents, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(nsp_cmd_r_file_contents, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_r_file_contents, INT, (void *)0x12345678, (void *)0x12345678, NULL);

    PRINTF(nsp_cmd_s_os_install, INT, NULL, 0);
    PRINTF(nsp_cmd_r_os_install, INT, NULL);
    PRINTF(nsp_cmd_s_os_contents, INT, NULL, 0, (void *)0x12345678);
    PRINTF(nsp_cmd_s_os_contents, INT, (void *)0x12345678, 0, NULL);
    PRINTF(nsp_cmd_r_progress, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_r_progress, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_cmd_s_generic_data, INT, NULL, 0, (void *)0x12345678, 0, 0);
    PRINTF(nsp_cmd_r_generic_data, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(nsp_cmd_s_echo, INT, NULL, 0, (void *)0x12345678);
    PRINTF(nsp_cmd_r_echo, INT, NULL, (void *)0x12345678, (void *)0x12345678);

    PRINTF(nsp_cmd_s_keypress_event, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_cmd_s_keypress_event, INT, (void *)0x12345678, NULL);
}

static void torture_dusb(void)
{
    void * ptr;
    DUSBModeSet mode = DUSB_MODE_STARTUP;

// dusb_rpkt.c
    PRINTF(dusb_rpkt_type2name, STR, 0);
    PRINTF(dusb_dissect, INT, CALC_NONE, NULL, (void *)0x12345678, 8, 2, (void *) 0x12345678);
    PRINTF(dusb_dissect, INT, CALC_NONE, (void *)0x12345678, NULL, 8, 2, (void *) 0x12345678);
    PRINTF(dusb_dissect, INT, CALC_NONE, (void *)0x12345678, (void *)0x12345678, 8, 2, NULL);
// dusb_vpkt.c
    PRINTF(dusb_vtl_pkt_new, PTR, NULL);
    PRINTF(dusb_vtl_pkt_new_ex, PTR, NULL, 0, 0, (void *)0x12345678);
    PRINTFVOID(dusb_vtl_pkt_fill, NULL, 0, 0, (void *)0x12345678);
    PRINTFVOID(dusb_vtl_pkt_del, NULL, (void *)0x12345678);
    PRINTFVOID(dusb_vtl_pkt_del, (void *)0x12345678, NULL);
    ptr = dusb_vtl_pkt_alloc_data(0);
    PRINTF(, PTR, ptr);
    dusb_vtl_pkt_free_data(ptr);

    ptr = dusb_vtl_pkt_realloc_data(NULL, 1);
    PRINTF(, PTR, ptr);
    dusb_vtl_pkt_free_data(ptr);
    PRINTFVOID(dusb_vtl_pkt_free_data, NULL);
    PRINTF(dusb_send_buf_size_request, INT, NULL, 0);
    PRINTF(dusb_recv_buf_size_alloc, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_recv_buf_size_request, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_send_buf_size_alloc, INT, NULL, 0);
    PRINTF(dusb_send_data, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_send_data, INT, (void *)0x12345678, NULL);
    PRINTF(dusb_recv_data, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_recv_data, INT, (void *)0x12345678, NULL);

    PRINTF(dusb_recv_data_varsize, INT, NULL, (void *)0x12345678, (void *)0x12345678, 0);
    PRINTF(dusb_recv_data_varsize, INT, (void *)0x12345678, NULL, (void *)0x12345678, 0);
    PRINTF(dusb_recv_data_varsize, INT, (void *)0x12345678, (void *)0x12345678, NULL, 0);
    PRINTF(dusb_send_acknowledge, INT, NULL);
    PRINTF(dusb_recv_acknowledge, INT, NULL);
    PRINTF(dusb_vpkt_type2name, STR, 0);
    PRINTF(dusb_get_buf_size, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_get_buf_size, INT, (void *)0x12345678, NULL);
    PRINTF(dusb_set_buf_size, INT, NULL, 0);
    PRINTF(dusb_cmd_param_type2name, STR, 0);

// dbus_cmd.c
    ptr = dusb_cp_new(NULL, 0, 0);
    PRINTF(, PTR, ptr);
    ptr = dusb_cp_new_ex(NULL, 0, 0, (void *)0x12345678);
    PRINTF(, PTR, ptr);
    PRINTFVOID(dusb_cp_fill, NULL, 0, 0, (void *)0x12345678);
    dusb_cp_del((void *)0x12345678, ptr);
    PRINTFVOID(dusb_cp_del, NULL, (void *)0x12345678);
    PRINTFVOID(dusb_cp_del, (void *)0x12345678, NULL);
    ptr = dusb_cp_new_array(NULL, 0);
    PRINTF(, PTR, ptr);
    dusb_cp_del_array((void *)0x12345678, 0, ptr);
    PRINTFVOID(dusb_cp_del_array, (void *)0x12345678, 0, NULL);
    PRINTFVOID(dusb_cp_del_array, NULL, 0, (void *)0x12345678);
    ptr = dusb_cp_alloc_data(0);
    PRINTF(, PTR, ptr);
    dusb_cp_free_data(ptr);
    ptr = dusb_cp_realloc_data(NULL, 1);
    PRINTF(, PTR, ptr);
    dusb_cp_free_data(ptr);

    PRINTFVOID(dusb_cp_free_data, NULL);
    ptr = dusb_ca_new(NULL, 0, 0);
    PRINTF(, PTR, ptr);
    ptr = dusb_ca_new_ex(NULL, 0, 0, (void *)0x12345678);
    PRINTF(, PTR, ptr);
    PRINTFVOID(dusb_ca_fill, NULL, 0, 0, (void *)0x12345678);
    dusb_ca_del((void *)0x12345678, ptr);
    PRINTFVOID(dusb_ca_del, NULL, (void *)0x12345678);
    PRINTFVOID(dusb_ca_del, (void *)0x12345678, NULL);
    ptr = dusb_ca_new_array(NULL, 0);
    PRINTF(, PTR, ptr);
    dusb_ca_del_array((void *)0x12345678, 0, ptr);
    PRINTFVOID(dusb_ca_del_array, (void *)0x12345678, 0, NULL);
    PRINTFVOID(dusb_ca_del_array, NULL, 0, (void *)0x12345678);
    ptr = dusb_ca_alloc_data(0);
    PRINTF(, PTR, ptr);
    dusb_ca_free_data(ptr);

    ptr = dusb_ca_realloc_data(NULL, 1);
    PRINTF(, PTR, ptr);
    dusb_ca_free_data(ptr);
    PRINTFVOID(dusb_ca_free_data, NULL);
    PRINTF(dusb_cmd_s_mode_set, INT, NULL, mode);
    PRINTF(dusb_cmd_s_os_begin, INT, NULL, 0);
    PRINTF(dusb_cmd_r_os_ack, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_cmd_s_os_header, INT, NULL, 0, 0, 0, 0, (void *)0x12345678);
    PRINTF(dusb_cmd_s_os_header, INT, (void *)0x12345678, 0, 0, 0, 0, NULL);
    PRINTF(dusb_cmd_s_os_data, INT, NULL, 0, 0, 0, 0, (void *)0x12345678);
    PRINTF(dusb_cmd_s_os_data, INT, (void *)0x12345678, 0, 0, 0, 0, NULL);
    PRINTF(dusb_cmd_s_os_header_89, INT, NULL, 0, (void *)0x12345678);

    PRINTF(dusb_cmd_s_os_header_89, INT, (void *)0x12345678, 0, NULL);
    PRINTF(dusb_cmd_s_os_data_89, INT, NULL, 0, (void *)0x12345678);
    PRINTF(dusb_cmd_s_os_data_89, INT, (void *)0x12345678, 0, NULL);
    PRINTF(dusb_cmd_r_eot_ack, INT, NULL);
    PRINTF(dusb_cmd_s_param_request, INT, NULL, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_param_request, INT, (void *)0x12345678, 1, NULL);
    PRINTF(dusb_cmd_r_param_data, INT, NULL, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_r_param_data, INT, (void *)0x12345678, 1, NULL);
    PRINTF(dusb_cmd_r_screenshot, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dusb_cmd_r_screenshot, INT, (void *)0x12345678, NULL, (void *)0x12345678);

    PRINTF(dusb_cmd_r_screenshot, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(dusb_cmd_s_dirlist_request, INT, NULL, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_dirlist_request, INT, (void *)0x12345678, 1, NULL);
    PRINTF(dusb_cmd_r_var_header, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dusb_cmd_r_var_header, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dusb_cmd_r_var_header, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(dusb_cmd_r_var_header, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(dusb_cmd_s_rts, INT, NULL, (void *)0x12345678, (void *)0x12345678, 0, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_rts, INT, (void *)0x12345678, NULL, (void *)0x12345678, 0, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_rts, INT, (void *)0x12345678, (void *)0x12345678, NULL, 0, 1, (void *)0x12345678);

    PRINTF(dusb_cmd_s_rts, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 0, 1, NULL);
    PRINTF(dusb_cmd_s_var_request, INT, NULL, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_request, INT, (void *)0x12345678, NULL, (void *)0x12345678, 1, (void *)0x12345678, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_request, INT, (void *)0x12345678, (void *)0x12345678, NULL, 1, (void *)0x12345678, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_request, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, NULL, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_request, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678, 1, NULL);
    PRINTF(dusb_cmd_s_var_content, INT, NULL, 0, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_content, INT, (void *)0x12345678, 0, NULL);
    PRINTF(dusb_cmd_r_var_content, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dusb_cmd_r_var_content, INT, (void *)0x12345678, (void *)0x12345678, NULL);

    PRINTF(dusb_cmd_s_param_set, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_cmd_s_param_set, INT, (void *)0x12345678, NULL);
    PRINTF(dusb_cmd_s_var_modify, INT, NULL, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (void *)0x12345678, NULL, (void *)0x12345678, 1, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (void *)0x12345678, (void *)0x12345678, NULL, 1, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, NULL, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678, NULL, (void *)0x12345678, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678, (void *)0x12345678, NULL, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_modify, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, NULL);
    PRINTF(dusb_cmd_s_var_delete, INT, NULL, (void *)0x12345678, (void *)0x12345678, 1, (void *)0x12345678);

    PRINTF(dusb_cmd_s_var_delete, INT, (void *)0x12345678, NULL, (void *)0x12345678, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_delete, INT, (void *)0x12345678, (void *)0x12345678, NULL, 1, (void *)0x12345678);
    PRINTF(dusb_cmd_s_var_delete, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, 1, NULL);
    PRINTF(dusb_cmd_s_execute, INT, NULL, (void *)0x12345678, (void *)0x12345678, 0, (void *)0x12345678, 0);  // It's OK to have args = NULL
    PRINTF(dusb_cmd_s_execute, INT, (void *)0x12345678, NULL, (void *)0x12345678, 0, (void *)0x12345678, 0);
    PRINTF(dusb_cmd_s_execute, INT, (void *)0x12345678, (void *)0x12345678, NULL, 0, (void *)0x12345678, 0);
    PRINTF(dusb_cmd_r_mode_ack, INT, NULL);
    PRINTF(dusb_cmd_r_data_ack, INT, NULL);
    PRINTF(dusb_cmd_r_delay_ack, INT, NULL);
    PRINTF(dusb_cmd_s_eot, INT, NULL);

    PRINTF(dusb_cmd_r_eot, INT, NULL);
    PRINTF(dusb_cmd_s_error, INT, NULL, 0);

    PRINTF(dusb_cmd_s_param_set_r_data_ack, INT, NULL, 0, 0, (void *)0x12345678);
}

static void torture_dbus(void)
{
// dbus_pkt.c
    PRINTF(dbus_cmd2name, STR, 0);
    PRINTF(dbus_cmd2officialname, STR, 0);
    PRINTF(dbus_cmd2desc, STR, 0);
    PRINTF(dbus_mid2direction, STR, 0);
    PRINTF(dbus_dissect, INT, CALC_NONE, NULL, (void *)0x12345678, 8);
    PRINTF(dbus_dissect, INT, CALC_NONE, (void *)0x12345678, NULL, 8);
}

static void torture_cmdz80(void)
{
// cmdz80.c
    PRINTF(ti73_send_VAR, INT, NULL, 0, 0, (void *)0x12345678, 0, 0);
    PRINTF(ti73_send_VAR, INT, (void *)0x12345678, 0, 0, NULL, 0, 0);
    PRINTF(ti73_send_VAR2, INT, NULL, 0, 0, 0, 0, 0);
    PRINTF(ti73_send_CTS, INT, NULL);
    PRINTF(ti73_send_XDP, INT, NULL, 0, (void *)0x12345678);
    PRINTF(ti73_send_SKP, INT, NULL, 0);
    PRINTF(ti73_send_ACK, INT, NULL);
    PRINTF(ti73_send_ERR, INT, NULL);
    PRINTF(ti73_send_RDY, INT, NULL);
    PRINTF(ti73_send_SCR, INT, NULL);

    PRINTF(ti73_send_KEY, INT, NULL, 0);
    PRINTF(ti73_send_EOT, INT, NULL);
    PRINTF(ti73_send_REQ, INT, NULL, 0, 0, (void *)0x12345678, 0, 0);
    PRINTF(ti73_send_REQ, INT, (void *)0x12345678, 0, 0, NULL, 0, 0);
    PRINTF(ti73_send_REQ2, INT, NULL, 0, 0, (void *)0x12345678, 0);
    PRINTF(ti73_send_REQ2, INT, (void *)0x12345678, 0, 0, NULL, 0);
    PRINTF(ti73_send_RTS, INT, NULL, 0, 0, (void *)0x12345678, 0, 0);
    PRINTF(ti73_send_RTS, INT, (void *)0x12345678, 0, 0, NULL, 0, 0);
    PRINTF(ti73_send_VER, INT, NULL);
    PRINTF(ti73_send_DEL, INT, NULL, 0, 0, (void *)0x12345678, 0);

    PRINTF(ti73_send_DEL, INT, (void *)0x12345678, 0, 0, NULL, 0);
    PRINTF(ti73_send_DUMP, INT, NULL, 0);
    PRINTF(ti73_send_EKE, INT, NULL);
    PRINTF(ti73_send_DKE, INT, NULL);
    PRINTF(ti73_send_ELD, INT, NULL);
    PRINTF(ti73_send_DLD, INT, NULL);
    PRINTF(ti73_send_GID, INT, NULL);
    PRINTF(ti73_send_RID, INT, NULL);
    PRINTF(ti73_send_SID, INT, NULL, (void *)0x12345678);
    PRINTF(ti73_recv_VAR, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);

    PRINTF(ti73_recv_VAR, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti73_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ti73_recv_VAR2, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_VAR2, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_VAR2, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_VAR2, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_VAR2, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);

    PRINTF(ti73_recv_VAR2, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ti73_recv_CTS, INT, NULL, 0);
    PRINTF(ti73_recv_SKP, INT, NULL, (void *)0x12345678);
    PRINTF(ti73_recv_SKP, INT, (void *)0x12345678, NULL);
    PRINTF(ti73_recv_XDP, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_XDP, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti73_recv_ACK, INT, NULL, (void *)0x12345678);
    PRINTF(ti73_recv_RTS, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_RTS, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);

    PRINTF(ti73_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti73_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti73_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ti80_send_SCR, INT, NULL);
    PRINTF(ti80_recv_XDP, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti80_recv_XDP, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti80_recv_ACK, INT, NULL, (void *)0x12345678);
    PRINTF(ti82_send_VAR, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti82_send_VAR, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti82_send_CTS, INT, NULL);

    PRINTF(ti82_send_XDP, INT, NULL, 0, (void *)0x12345678);
    PRINTF(ti82_send_SKP, INT, NULL, 0);
    PRINTF(ti82_send_ACK, INT, NULL);
    PRINTF(ti82_send_ERR, INT, NULL);
    PRINTF(ti82_send_SCR, INT, NULL);
    PRINTF(ti82_send_KEY, INT, NULL, 0);
    PRINTF(ti82_send_EOT, INT, NULL);
    PRINTF(ti82_send_REQ, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti82_send_REQ, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti82_send_RTS, INT, NULL, 0, 0, (void *)0x12345678);

    PRINTF(ti82_send_RTS, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti82_recv_VAR, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti82_recv_VAR, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti82_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti82_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ti82_recv_CTS, INT, NULL);
    PRINTF(ti82_recv_SKP, INT, NULL, (void *)0x12345678);
    PRINTF(ti82_recv_SKP, INT, (void *)0x12345678, NULL);
    PRINTF(ti82_recv_XDP, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti82_recv_XDP, INT, (void *)0x12345678, NULL, (void *)0x12345678);

    PRINTF(ti82_recv_ACK, INT, NULL, (void *)0x12345678);
    PRINTF(ti82_recv_RTS, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti82_recv_RTS, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti82_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti82_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ti85_send_VAR, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti85_send_VAR, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti85_send_CTS, INT, NULL);
    PRINTF(ti85_send_XDP, INT, NULL, 0, (void *)0x12345678);
    PRINTF(ti85_send_SKP, INT, NULL, 0);

    PRINTF(ti85_send_ACK, INT, NULL);
    PRINTF(ti85_send_ERR, INT, NULL);
    PRINTF(ti85_send_SCR, INT, NULL);
    PRINTF(ti85_send_KEY, INT, NULL, 0);
    PRINTF(ti85_send_EOT, INT, NULL);
    PRINTF(ti85_send_REQ, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti85_send_REQ, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti85_send_RTS, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti85_send_RTS, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti85_recv_VAR, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);

    PRINTF(ti85_recv_VAR, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti85_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti85_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ti85_recv_CTS, INT, NULL);
    PRINTF(ti85_recv_SKP, INT, NULL, (void *)0x12345678);
    PRINTF(ti85_recv_SKP, INT, (void *)0x12345678, NULL);
    PRINTF(ti85_recv_XDP, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti85_recv_XDP, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti85_recv_ACK, INT, NULL, (void *)0x12345678);
    PRINTF(ti85_recv_RTS, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);

    PRINTF(ti85_recv_RTS, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti85_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti85_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
}

static void torture_cmd68k(void)
{
// cmd68k.c
    PRINTF(ti89_send_VAR, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti89_send_CTS, INT, NULL);
    PRINTF(ti89_send_XDP, INT, NULL, 0, (void *)0x12345678);
    PRINTF(ti89_send_SKP, INT, NULL, 0);
    PRINTF(ti89_send_ACK, INT, NULL);
    PRINTF(ti89_send_ERR, INT, NULL);
    PRINTF(ti89_send_RDY, INT, NULL);
    PRINTF(ti89_send_SCR, INT, NULL);
    PRINTF(ti89_send_CNT, INT, NULL);

    PRINTF(ti89_send_KEY, INT, NULL, 0);
    PRINTF(ti89_send_EOT, INT, NULL);
    PRINTF(ti89_send_REQ, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti89_send_REQ, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti89_send_RTS, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti89_send_RTS, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti89_send_RTS2, INT, NULL, 0, 0, 0);
    PRINTF(ti89_send_VER, INT, NULL);
    PRINTF(ti89_send_DEL, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti89_send_DEL, INT, (void *)0x12345678, 0, 0, NULL);

    PRINTF(ti89_recv_VAR, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti89_recv_VAR, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti89_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti89_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ti89_recv_CTS, INT, NULL);
    PRINTF(ti89_recv_SKP, INT, NULL, (void *)0x12345678);
    PRINTF(ti89_recv_SKP, INT, (void *)0x12345678, NULL);
    PRINTF(ti89_recv_XDP, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti89_recv_XDP, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti89_send_ACK, INT, NULL);

    PRINTF(ti89_recv_CNT, INT, NULL);
    PRINTF(ti89_recv_EOT, INT, NULL);
    PRINTF(ti89_recv_RTS, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti89_recv_RTS, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti89_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti89_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ti92_send_VAR, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti92_send_VAR, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti92_send_CTS, INT, NULL);
    PRINTF(ti92_send_XDP, INT, NULL, 0, (void *)0x12345678);

    PRINTF(ti92_send_SKP, INT, NULL, 0);
    PRINTF(ti92_send_ACK, INT, NULL);
    PRINTF(ti92_send_ERR, INT, NULL);
    PRINTF(ti92_send_RDY, INT, NULL);
    PRINTF(ti92_send_SCR, INT, NULL);
    PRINTF(ti92_send_SCR, INT, NULL);
    PRINTF(ti92_send_CNT, INT, NULL);
    PRINTF(ti92_send_KEY, INT, NULL, 0);
    PRINTF(ti92_send_EOT, INT, NULL);
    PRINTF(ti92_send_REQ, INT, NULL, 0, 0, (void *)0x12345678);

    PRINTF(ti92_send_REQ, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti92_send_RTS, INT, NULL, 0, 0, (void *)0x12345678);
    PRINTF(ti92_send_RTS, INT, (void *)0x12345678, 0, 0, NULL);
    PRINTF(ti92_recv_VAR, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti92_recv_VAR, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti92_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti92_recv_VAR, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ti92_recv_CTS, INT, NULL);
    PRINTF(ti92_recv_SKP, INT, NULL, (void *)0x12345678);
    PRINTF(ti92_recv_SKP, INT, (void *)0x12345678, NULL);

    PRINTF(ti92_recv_XDP, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti92_recv_XDP, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti92_send_ACK, INT, NULL);
    PRINTF(ti92_recv_CNT, INT, NULL);
    PRINTF(ti92_recv_EOT, INT, NULL);
    PRINTF(ti92_recv_RTS, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti92_recv_RTS, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ti92_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ti92_recv_RTS, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
}

static void torture_romdump(void)
{
// romdump.c
    PRINTF(rd_dump, INT, NULL, (void *)0x12345678);
    PRINTF(rd_dump, INT, (void *)0x12345678, NULL);
    PRINTF(rd_is_ready, INT, NULL);
    PRINTF(rd_send, INT, NULL, (void *)0x12345678, 0, (void *)0x12345678);
    PRINTF(rd_send, INT, (void *)0x12345678, NULL, 0, (void *)0x12345678);
    PRINTF(rd_send, INT, (void *)0x12345678, (void *)0x12345678, 0, NULL);
    PRINTF(rd_send, INT, (void *)0x12345678, (void *)0x12345678, 0, (void *)0x12345678);
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

static void dissect_functions_unit_test_1(void)
{
    assert(ERR_INVALID_PACKET == dbus_dissect(CALC_NONE, stderr, (void *)0x12345678, 1));
    assert(ERR_INVALID_PACKET == dbus_dissect(CALC_NONE, stderr, (void *)0x12345678, 3));
    assert(ERR_INVALID_PACKET == dbus_dissect(CALC_NONE, stderr, (void *)0x12345678, 65543));
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

static void dissect_functions_unit_test_2(void)
{
    uint8_t first = 1;

    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, (void *)0x12345678, 4, 0, (void *)0x12345678));
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, (void *)0x12345678, 1024, 0, (void *)0x12345678));
    assert(dusb_bad_raw_type_1[4] < 1);
    assert(dusb_bad_raw_type_2[4] > 5);
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, dusb_bad_raw_type_1, 5, 0, (void *)0x12345678));
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, dusb_bad_raw_type_2, 5, 0, (void *)0x12345678));
    assert(ERR_INVALID_PACKET == dusb_dissect(CALC_NONE, stderr, dusb_good_buf_size_req, 6, 0, (void *)0x12345678));
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

static void dissect_functions_unit_test_3(void)
{
    assert(ERR_INVALID_PACKET == nsp_dissect(CALC_NONE, stderr, (void *)0x12345678, 16, 0));
    assert(ERR_INVALID_PACKET == nsp_dissect(CALC_NONE, stderr, (void *)0x12345678, 272, 0));
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
