#include <stdio.h>
#include <tiopers.h>

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

    tiopers_library_init();
// 1
// error.c
    PRINTF(tiopers_error_get, INT, -1, NULL);
// tiopers.c
// TODO
// opers_xx.c
    PRINTF(tiopers_recv_idlist, INT, NULL, (void *)0x12345678, NULL, NULL);
    PRINTF(tiopers_recv_idlist, INT, (void *)0x12345678, NULL, NULL, NULL);
    PRINTF(tiopers_dump_rom, INT, NULL, -1, (void *)0x12345678);
    PRINTF(tiopers_dump_rom, INT, (void *)0x12345678, -1, NULL);

    PRINTF(tiopers_get_infos, INT, NULL, (void *)0x12345678, NULL, 0);
    PRINTF(tiopers_get_infos, INT, (void *)0x12345678, NULL, NULL, 0);
    PRINTFVOID(tiopers_format_bytes, 0x12345678, NULL);

    tiopers_library_exit();

    return 0;
}
