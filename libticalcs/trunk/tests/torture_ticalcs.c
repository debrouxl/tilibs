#include <stdio.h>
#include <ticalcs.h>
#include <nsp_cmd.h>
#include <nsp_vpkt.h>

#define PRINTF(FUNCTION, TYPE, args...) \
printf("%d\t" TYPE "\n", i, FUNCTION(args)); i++

#define PRINTFVOID(FUNCTION, args...) \
printf("%d\n", i); FUNCTION(args); i++

#define INT "%d"
#define PTR "%p"
#define STR "\"%s\""
#define VOID ""

int main(int argc, char **argv)
{
    int i = 1;

    ticalcs_library_init();
// 1
// error.c
    PRINTF(ticalcs_error_get, INT, -1, NULL);
// ticalcs.c
    PRINTF(ticalcs_handle_new, PTR, -1);
    PRINTF(ticalcs_handle_del, INT, NULL);
    PRINTF(ticalcs_handle_show, INT, NULL);
    PRINTF(ticalcs_cable_attach, INT, NULL, (void *)0x12345678);

    PRINTF(ticalcs_cable_detach, INT, NULL);
    PRINTF(ticalcs_update_set, INT, NULL, (void *)0x12345678);
// calc_xx.c
    PRINTF(ticalcs_calc_features, INT, NULL);
    PRINTF(ticalcs_calc_isready, INT, NULL);

    PRINTF(ticalcs_calc_send_key, INT, NULL, -1);
// 11
    PRINTF(ticalcs_calc_execute, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_execute, INT, (void *)0x12345678, NULL, NULL);
    PRINTF(ticalcs_calc_recv_screen, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_screen, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_recv_screen, INT, (void *)0x12345678, (void *)0x12345678, NULL);

    PRINTF(ticalcs_calc_get_dirlist, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_dirlist, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_dirlist, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(ticalcs_calc_get_memfree, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(ticalcs_calc_get_memfree, INT, (void *)0x12345678, (void *)0x12345678, NULL);
// 21
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
// 31
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
// 41
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
// 51
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
// 61
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
// 71
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
// 81
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
// 91
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
// 101
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
// 111
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
// clock.c
    PRINTF(ticalcs_clock_format2date, STR, -1, -1);
// 121
    PRINTF(ticalcs_clock_date2format, INT, -1, NULL);
    PRINTF(ticalcs_clock_show, INT, -1, NULL);
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
// 131
    PRINTF(ticalcs_probe_usb_calc, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_probe_usb_calc, INT, (void *)0x12345678, NULL);
    PRINTF(ticalcs_probe, INT, -1, -1, NULL, -1);
// dbus_pkt.c
    PRINTF(dbus_send, INT, NULL, 0, 0, 0, (void *)0x12345678);
    PRINTF(dbus_recv, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);

    PRINTF(dbus_recv, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
// dusb_rpkt.c
    PRINTF(dusb_send, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_send, INT, (void *)0x12345678, NULL);
// 141
    PRINTF(dusb_recv, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_recv, INT, (void *)0x12345678, NULL);
// nsp_rpkt.c
    PRINTF(nsp_send, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_send, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_recv, INT, NULL, (void *)0x12345678);

    PRINTF(nsp_recv, INT, (void *)0x12345678, NULL);
// nsp_vpkt.c
    PRINTFVOID(nsp_vtl_pkt_del, NULL);
    PRINTF(nsp_session_open, INT, NULL, 0);
    PRINTF(nsp_session_close, INT, NULL);
    PRINTF(nsp_addr_request, INT, NULL);
// 151
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
// 161
    PRINTF(nsp_recv_disconnect, INT, NULL);
// nsp_cmd.c
    PRINTF(cmd_r_login, INT, NULL);
    PRINTF(cmd_s_status, INT, NULL, 0);
    PRINTF(cmd_r_status, INT, NULL, (void *)0x12345678);
    PRINTF(cmd_s_dev_infos, INT, NULL, 0);

    PRINTF(cmd_r_dev_infos, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(cmd_r_dev_infos, INT, (void *)0x12345678, (void *)0x12345678, NULL, NULL);
    PRINTF(cmd_r_dev_infos, INT, (void *)0x12345678, NULL, (void *)0x12345678, NULL);
    PRINTF(cmd_r_dev_infos, INT, (void *)0x12345678, NULL, NULL, (void *)0x12345678);
    PRINTF(cmd_s_screen_rle, INT, NULL, 0);
// 171
    PRINTF(cmd_r_screen_rle, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(cmd_r_screen_rle, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(cmd_r_screen_rle, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(cmd_r_screen_rle, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(cmd_s_dir_attributes, INT, NULL, (void *)0x12345678);

    PRINTF(cmd_s_dir_attributes, INT, (void *)0x12345678, NULL);
    PRINTF(cmd_r_dir_attributes, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(cmd_s_dir_enum_init, INT, NULL, (void *)0x12345678);
    PRINTF(cmd_s_dir_enum_init, INT, (void *)0x12345678, NULL);
    PRINTF(cmd_r_dir_enum_init, INT, NULL);
// 181
    PRINTF(cmd_s_dir_enum_next, INT, NULL);
    PRINTF(cmd_r_dir_enum_next, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(cmd_r_dir_enum_next, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(cmd_s_dir_enum_done, INT, NULL);
    PRINTF(cmd_r_dir_enum_done, INT, NULL);

    PRINTF(cmd_s_put_file, INT, NULL, (void *)0x12345678, 0);
    PRINTF(cmd_s_put_file, INT, (void *)0x12345678, NULL, 0);
    PRINTF(cmd_r_put_file, INT, NULL);
    PRINTF(cmd_s_get_file, INT, NULL, (void *)0x12345678);
    PRINTF(cmd_s_get_file, INT, (void *)0x12345678, NULL);
// 191
    PRINTF(cmd_r_get_file, INT, NULL, (void *)0x12345678);
    PRINTF(cmd_s_del_file, INT, NULL, (void *)0x12345678);
    PRINTF(cmd_s_del_file, INT, (void *)0x12345678, NULL);
    PRINTF(cmd_r_del_file, INT, NULL);
    PRINTF(cmd_s_new_folder, INT, NULL, (void *)0x12345678);

    PRINTF(cmd_s_new_folder, INT, (void *)0x12345678, NULL);
    PRINTF(cmd_r_new_folder, INT, NULL);
    PRINTF(cmd_s_del_folder, INT, NULL, (void *)0x12345678);
    PRINTF(cmd_s_del_folder, INT, (void *)0x12345678, NULL);
    PRINTF(cmd_r_del_folder, INT, NULL);
// 201
    PRINTF(cmd_s_copy_file, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(cmd_s_copy_file, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(cmd_s_copy_file, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(cmd_r_copy_file, INT, NULL);
    PRINTF(cmd_s_rename_file, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    
    PRINTF(cmd_s_rename_file, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(cmd_s_rename_file, INT, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(cmd_r_rename_file, INT, NULL);
    PRINTF(cmd_s_file_ok, INT, NULL);
    PRINTF(cmd_r_file_ok, INT, NULL);
// 211
    PRINTF(cmd_s_file_contents, INT, NULL, 0, (void *)0x12345678);
    PRINTF(cmd_s_file_contents, INT, (void *)0x12345678, 0, NULL);
    PRINTF(cmd_r_file_contents, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(cmd_r_file_contents, INT, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(cmd_r_file_contents, INT, (void *)0x12345678, (void *)0x12345678, NULL);

    PRINTF(cmd_s_os_install, INT, NULL, 0);
    PRINTF(cmd_r_os_install, INT, NULL);
    PRINTF(cmd_s_os_contents, INT, NULL, 0, (void *)0x12345678);
    PRINTF(cmd_s_os_contents, INT, (void *)0x12345678, 0, NULL);
    PRINTF(cmd_r_progress, INT, NULL, (void *)0x12345678);
// 221
    PRINTF(cmd_r_progress, INT, (void *)0x12345678, NULL);
    PRINTF(cmd_s_echo, INT, NULL, 0, (void *)0x12345678);
    PRINTF(cmd_r_echo, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(cmd_s_keypress_event, INT, NULL, (void *)0x12345678);
    PRINTF(cmd_s_keypress_event, INT, (void *)0x12345678, NULL);

    ticalcs_library_exit();

    return 0;
}
