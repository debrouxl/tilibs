#include <stdio.h>
#include <ticalcs.h>

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
    PRINTF(ticalcs_error_get, INT, -1, NULL);
    PRINTF(ticalcs_handle_new, PTR, -1);
    PRINTF(ticalcs_handle_del, INT, NULL);
    PRINTF(ticalcs_handle_show, INT, NULL);
    PRINTF(ticalcs_cable_attach, INT, NULL, (void *)0x12345678);

    PRINTF(ticalcs_cable_detach, INT, NULL);
    PRINTF(ticalcs_update_set, INT, NULL, (void *)0x12345678);
    PRINTF(ticalcs_calc_features, INT, NULL);
    PRINTF(ticalcs_calc_isready, INT, NULL);

    PRINTF(ticalcs_calc_send_key, INT, NULL, -1);
// 11
    PRINTF(ticalcs_calc_execute, INT, NULL, NULL, NULL);
    PRINTF(ticalcs_calc_recv_screen, INT, NULL, NULL, NULL);
    PRINTF(ticalcs_calc_get_dirlist, INT, NULL, NULL, NULL);
    PRINTF(ticalcs_calc_get_memfree, INT, NULL, NULL, NULL);
    PRINTF(ticalcs_calc_send_backup, INT, NULL, NULL);

    PRINTF(ticalcs_calc_recv_backup, INT, NULL, NULL);
    PRINTF(ticalcs_calc_send_var, INT, NULL, -1, NULL);
    PRINTF(ticalcs_calc_recv_var, INT, NULL, -1, NULL, NULL);
    PRINTF(ticalcs_calc_send_var_ns, INT, NULL, -1, NULL);
    PRINTF(ticalcs_calc_recv_var_ns, INT, NULL, -1, NULL, NULL);
// 21
    PRINTF(ticalcs_calc_send_app, INT, NULL, NULL);
    PRINTF(ticalcs_calc_recv_app, INT, NULL, NULL, NULL);
    PRINTF(ticalcs_calc_send_os, INT, NULL, NULL);
    PRINTF(ticalcs_calc_recv_idlist, INT, NULL, NULL);
    PRINTF(ticalcs_calc_dump_rom_1, INT, NULL);

    PRINTF(ticalcs_calc_dump_rom_2, INT, NULL, -1, NULL);
    PRINTF(ticalcs_calc_set_clock, INT, NULL, NULL);
    PRINTF(ticalcs_calc_get_clock, INT, NULL, NULL);
    PRINTF(ticalcs_calc_new_fld, INT, NULL, NULL);
    PRINTF(ticalcs_calc_del_var, INT, NULL, NULL);
// 31
    PRINTF(ticalcs_calc_get_version, INT, NULL, NULL);
    PRINTF(ticalcs_calc_send_cert, INT, NULL, NULL);
    PRINTF(ticalcs_calc_recv_cert, INT, NULL, NULL);
    PRINTF(ticalcs_calc_send_tigroup, INT, NULL, NULL, -1);
    PRINTF(ticalcs_calc_recv_tigroup, INT, NULL, NULL, -1);

    PRINTF(ticalcs_calc_send_backup2, INT, NULL, NULL);
    PRINTF(ticalcs_calc_recv_backup2, INT, NULL, NULL);
    PRINTF(ticalcs_calc_send_var2, INT, NULL, -1, NULL);
    PRINTF(ticalcs_calc_recv_var2, INT, NULL, -1, NULL, NULL);
    PRINTF(ticalcs_calc_send_var_ns2, INT, NULL, -1, NULL);
// 41
    PRINTF(ticalcs_calc_recv_var_ns2, INT, NULL, -1, NULL, NULL);
    PRINTF(ticalcs_calc_send_app2, INT, NULL, NULL);
    PRINTF(ticalcs_calc_recv_app2, INT, NULL, NULL, NULL);
    PRINTF(ticalcs_calc_send_cert2, INT, NULL, NULL);
    PRINTF(ticalcs_calc_recv_cert2, INT, NULL, NULL);

    PRINTF(ticalcs_calc_send_os2, INT, NULL, NULL);
    PRINTF(ticalcs_calc_send_tigroup2, INT, NULL, NULL, -1);
    PRINTF(ticalcs_calc_recv_tigroup2, INT, NULL, NULL, -1);

    PRINTFVOID(ticalcs_dirlist_destroy, NULL);
    PRINTFVOID(ticalcs_dirlist_display, NULL);
// 51
    PRINTF(ticalcs_dirlist_ram_used, INT, NULL);
    PRINTF(ticalcs_dirlist_flash_used, INT, NULL, NULL);
    PRINTF(ticalcs_dirlist_ve_count, INT, NULL);
    PRINTF(ticalcs_dirlist_ve_exist, PTR, NULL, NULL);
    PRINTFVOID(ticalcs_dirlist_ve_add, NULL, NULL);

    PRINTFVOID(ticalcs_dirlist_ve_del, NULL, NULL);
    PRINTF(ticalcs_model_to_string, STR, -1);
    PRINTF(ticalcs_string_to_model, INT, NULL);
    PRINTF(ticalcs_scrfmt_to_string, STR, -1);
    PRINTF(ticalcs_string_to_scrfmt, INT, NULL);
// 61
    PRINTF(ticalcs_pathtype_to_string, STR, -1);
    PRINTF(ticalcs_string_to_pathtype, INT, NULL);
    PRINTF(ticalcs_memtype_to_string, STR, -1);
    PRINTF(ticalcs_string_to_memtype, INT, NULL);
    PRINTF(ticalcs_clock_format2date, STR, -1, -1);

    PRINTF(ticalcs_clock_date2format, INT, -1, NULL);
    PRINTF(ticalcs_clock_show, INT, -1, NULL);
    PRINTF(ticalcs_keys_73, PTR, 0);
    PRINTF(ticalcs_keys_83 , PTR, 0);
    PRINTF(ticalcs_keys_83p, PTR, 0);
// 71
    PRINTF(ticalcs_keys_86, PTR, 0);
    PRINTF(ticalcs_keys_89, PTR, 0);
    PRINTF(ticalcs_keys_92p, PTR, 0);
    PRINTF(ticalcs_probe_calc, INT, NULL, NULL);
    PRINTF(ticalcs_probe_usb_calc, INT, NULL, NULL);

    PRINTF(ticalcs_probe, INT, -1, -1, NULL, -1);
    PRINTF(dbus_send, INT, NULL, 0, 0, 0, (void *)0x12345678);
    PRINTF(dbus_send, INT, (void *)0x12345678, 0, 0, 0, NULL);
    PRINTF(dbus_recv, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
// 81
    PRINTF(dbus_recv, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(dbus_recv, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(dbus_recv, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);
    PRINTF(dusb_send, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_send, INT, (void *)0x12345678, NULL);

    PRINTF(dusb_recv, INT, NULL, (void *)0x12345678);
    PRINTF(dusb_recv, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_send, INT, NULL, (void *)0x12345678);
    PRINTF(nsp_send, INT, (void *)0x12345678, NULL);
    PRINTF(nsp_recv, INT, NULL, (void *)0x12345678);
// 91
    PRINTF(nsp_recv, INT, (void *)0x12345678, NULL);

    ticalcs_library_exit();

    return 0;
}
