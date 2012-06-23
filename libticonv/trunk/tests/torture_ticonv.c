#include <stdio.h>
#include <ticonv.h>

#define str(s) #s
#define xstr(s) str(s)

#define PRINTF(FUNCTION, TYPE, args...) \
printf("%d\t" TYPE "\n", i, FUNCTION(args)); i++

#define SIZE "%zd"
#define PTR "%p"
#define STR "\"%s\""

int main(int argc, char **argv)
{
    int i = 1;
// 1
    PRINTF(ticonv_utf16_strlen, SIZE, NULL);
    PRINTF(ticonv_utf8_to_utf16, PTR, NULL);
    PRINTF(ticonv_utf16_to_utf8, PTR, NULL);
    PRINTF(ticonv_charset_utf16_to_ti_s, PTR, -1, NULL, (void *)0x12345678);
    PRINTF(ticonv_charset_utf16_to_ti_s, PTR, -1, (void *)0x12345678, NULL);

    PRINTF(ticonv_charset_ti_to_utf16_s, PTR, -1, NULL, (void *)0x12345678);
    PRINTF(ticonv_charset_ti_to_utf16_s, PTR, -1, (void *)0x12345678, NULL);
    PRINTF(ticonv_charset_utf16_to_ti, PTR, -1, NULL);
    PRINTF(ticonv_charset_ti_to_utf16, PTR, -1, NULL);
    PRINTF(ticonv_varname_to_utf16_s, PTR, -1, NULL, (void *)0x12345678, 0);
// 11
    PRINTF(ticonv_varname_to_utf16_s, PTR, -1, (void *)0x12345678, NULL, 0);
    PRINTF(ticonv_varname_to_utf16, PTR, -1, NULL, 0);
    PRINTF(ticonv_varname_to_utf8_s, STR, -1, NULL, (void *)0x12345678, 0);
    PRINTF(ticonv_varname_to_utf8_s, STR, -1, (void *)0x12345678, NULL, 0);
    PRINTF(ticonv_varname_to_utf8, STR, -1, NULL, 0);

    PRINTF(ticonv_varname_to_filename_s, STR, -1, NULL, (void *)0x12345678, 0);
    PRINTF(ticonv_varname_to_filename_s, STR, -1, (void *)0x12345678, NULL, 0);
    PRINTF(ticonv_varname_to_filename, STR, -1, NULL, 0);
    PRINTF(ticonv_varname_to_tifile_s, STR, -1, NULL, (void *)0x12345678, 0);
    PRINTF(ticonv_varname_to_tifile_s, STR, -1, (void *)0x12345678, NULL, 0);
// 21
    PRINTF(ticonv_varname_to_tifile, STR, -1, NULL, 0);
    PRINTF(ticonv_varname_from_tifile_s, STR, -1, NULL, (void *)0x12345678, 0);
    PRINTF(ticonv_varname_from_tifile_s, STR, -1, (void *)0x12345678, NULL, 0);
    PRINTF(ticonv_varname_from_tifile, STR, -1, NULL, 0);
    PRINTF(ticonv_varname_detokenize, STR, -1, NULL, 0);

    PRINTF(ticonv_varname_tokenize, STR, -1, NULL, 0);
    PRINTF(ticonv_utf16_to_gfe, STR, -1, NULL);
    PRINTF(ticonv_gfe_to_zfe, STR, -1, NULL);

    return 0;
}
