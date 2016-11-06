#include <stdio.h>
#include <ticables.h>

#define PRINTF(FUNCTION, TYPE, args...) \
fprintf(stderr, "%d\t" TYPE "\n", __LINE__, FUNCTION(args))

#define PRINTFVOID(FUNCTION, args...) \
FUNCTION(args); fprintf(stderr, "%d\n", __LINE__)

#define INT "%d"
#define PTR "%p"
#define STR "\"%s\""
#define VOID ""

int main(int argc, char **argv)
{
    ticables_library_init();

// error.c
    PRINTF(ticables_error_get, INT, -1, NULL);
    PRINTFVOID(ticables_error_free, NULL);
// ticables.c
    PRINTF(ticables_handle_new, PTR, -1, -1);
    PRINTF(ticables_handle_del, INT, NULL);
    PRINTF(ticables_options_set_timeout, INT, NULL, -1);
    PRINTF(ticables_options_set_delay, INT, NULL, -1);
    PRINTF(ticables_get_model, INT, NULL);
    PRINTF(ticables_get_port, INT, NULL);
    PRINTF(ticables_get_device, STR, NULL);
    PRINTF(ticables_handle_show, INT, NULL);
// link_xxx.c
    PRINTF(ticables_cable_open, INT, NULL);
    PRINTF(ticables_cable_close, INT, NULL);
    PRINTF(ticables_cable_reset, INT, NULL);
    PRINTF(ticables_cable_probe, INT, NULL, (void *)0x12345678);
    PRINTF(ticables_cable_set_device, INT, NULL, (void *)0x12345678);
    PRINTF(ticables_cable_set_device, INT, (void *)0x12345678, NULL);
    PRINTF(ticables_cable_send, INT, NULL, (void *)0x12345678, -1);
    PRINTF(ticables_cable_recv, INT, NULL, (void *)0x12345678, -1);
    PRINTF(ticables_cable_check, INT, NULL, (void *)0x12345678);
    PRINTF(ticables_cable_set_d0, INT, NULL, -1);

    PRINTF(ticables_cable_set_d1, INT, NULL, -1);
    PRINTF(ticables_cable_get_d0, INT, NULL);
    PRINTF(ticables_cable_get_d1, INT, NULL);
    PRINTF(ticables_cable_set_raw, INT, NULL, 0);
    PRINTF(ticables_cable_get_raw, INT, NULL, (void *)0x12345678);
    PRINTF(ticables_cable_get_raw, INT, (void *)0x12345678, NULL);
    PRINTF(ticables_progress_reset, INT, NULL);
    PRINTF(ticables_progress_get, INT, NULL, NULL, NULL, NULL);
    PRINTF(ticables_cable_put, INT, NULL, -1);
    PRINTF(ticables_cable_get, INT, NULL, (void *)0x12345678);

    PRINTF(ticables_cable_get_pre_send_hook, PTR, NULL);
    PRINTF(ticables_cable_set_pre_send_hook, PTR, NULL, (void *)0x12345678);
    PRINTF(ticables_cable_get_post_send_hook, PTR, NULL);
    PRINTF(ticables_cable_set_post_send_hook, PTR, NULL, (void *)0x12345678);
    PRINTF(ticables_cable_get_pre_recv_hook, PTR, NULL);
    PRINTF(ticables_cable_set_pre_recv_hook, PTR, NULL, (void *)0x12345678);
    PRINTF(ticables_cable_get_post_recv_hook, PTR, NULL);
    PRINTF(ticables_cable_set_post_recv_hook, PTR, NULL, (void *)0x12345678);
// type2str.c
    PRINTF(ticables_model_to_string, STR, -1);
    PRINTF(ticables_string_to_model, INT, NULL);

    PRINTF(ticables_port_to_string, STR, -1);
    PRINTF(ticables_string_to_port, INT, NULL);
    PRINTF(ticables_usbpid_to_string, STR, -1);
    PRINTF(ticables_string_to_usbpid, INT, NULL);
// probe.c
    PRINTF(ticables_probing_do, INT, NULL, -1, -1);
    PRINTF(ticables_probing_finish, INT, NULL);
    PRINTFVOID(ticables_probing_show, NULL);
    PRINTF(ticables_probing_found, INT, NULL);
    PRINTF(ticables_is_usb_enabled, INT);
    PRINTF(ticables_get_usb_devices, INT, NULL, NULL);

    PRINTF(ticables_free_usb_devices, INT, NULL);

    ticables_library_exit();

    return 0;
}
