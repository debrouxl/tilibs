#include <stdio.h>
#include <ticonv.h>
#include <charset.h>

#define str(s) #s
#define xstr(s) str(s)

#define PRINTF(FUNCTION, TYPE, args...) \
fprintf(stderr, "%d\t" TYPE "\n", i, FUNCTION(args)); i++;

#define PRINTFVOID(FUNCTION, args...) \
fprintf(stderr, "%d\n", i); FUNCTION(args); i++;

#define SIZE "%zd"
#define PTR "%p"
#define STR "\"%s\""
#define VOID ""

int main(int argc, char **argv)
{
    int i = 1;
    char * retval;
// 1
    // ticonv.h
    PRINTF(ticonv_utf16_strlen, SIZE, NULL);
    PRINTF(ticonv_utf8_to_utf16, PTR, NULL);
    PRINTFVOID(ticonv_utf16_free, NULL);
    PRINTF(ticonv_utf16_to_utf8, PTR, NULL);
    PRINTFVOID(ticonv_utf8_free, NULL);

    PRINTF(ticonv_charset_utf16_to_ti_s, PTR, -1, NULL, (void *)0x12345678);
    PRINTF(ticonv_charset_utf16_to_ti_s, PTR, -1, (void *)0x12345678, NULL);
    PRINTF(ticonv_charset_ti_to_utf16_s, PTR, -1, NULL, (void *)0x12345678);
    PRINTF(ticonv_charset_ti_to_utf16_s, PTR, -1, (void *)0x12345678, NULL);
    PRINTF(ticonv_charset_utf16_to_ti, PTR, -1, NULL);
// 11
    PRINTFVOID(ticonv_ti_free, NULL);
    PRINTF(ticonv_charset_ti_to_utf16, PTR, -1, NULL);
    PRINTF(ticonv_varname_to_utf16_s, PTR, -1, NULL, (void *)0x12345678, 0);
    PRINTF(ticonv_varname_to_utf16_s, PTR, -1, (void *)0x12345678, NULL, 0);
    PRINTF(ticonv_varname_to_utf16, PTR, -1, NULL, 0);

    PRINTF(ticonv_varname_to_utf8_s, STR, -1, NULL, (void *)0x12345678, 0);
    PRINTF(ticonv_varname_to_utf8_s, STR, -1, (void *)0x12345678, NULL, 0);
    PRINTF(ticonv_varname_to_utf8, STR, -1, NULL, 0);
    PRINTF(ticonv_varname_to_filename_s, STR, -1, NULL, (void *)0x12345678, 0);
    PRINTF(ticonv_varname_to_filename_s, STR, -1, (void *)0x12345678, NULL, 0);
// 21
    PRINTF(ticonv_varname_to_filename, STR, -1, NULL, 0);
    PRINTF(ticonv_varname_to_tifile_s, STR, -1, NULL, (void *)0x12345678, 0);
    PRINTF(ticonv_varname_to_tifile_s, STR, -1, (void *)0x12345678, NULL, 0);
    PRINTF(ticonv_varname_to_tifile, STR, -1, NULL, 0);
    PRINTF(ticonv_varname_from_tifile_s, STR, -1, NULL, (void *)0x12345678, 0);

    PRINTF(ticonv_varname_from_tifile_s, STR, -1, (void *)0x12345678, NULL, 0);
    PRINTF(ticonv_varname_from_tifile, STR, -1, NULL, 0);
    retval = ticonv_varname_detokenize(-1, NULL, 0);
    PRINTF(, STR, retval);
    ticonv_varname_free(retval);
    retval = ticonv_varname_tokenize(-1, NULL, 0);
    PRINTF(, STR, retval);
    ticonv_varname_free(retval);
    PRINTFVOID(ticonv_varname_free, NULL);
// 31
    retval = ticonv_utf16_to_gfe(-1, NULL);
    PRINTF(, STR, retval);
    ticonv_gfe_free(retval);
    PRINTFVOID(ticonv_gfe_free, NULL);
    retval = ticonv_gfe_to_zfe(-1, NULL);
    PRINTF(, STR, retval);
    ticonv_zfe_free(retval);
    PRINTFVOID(ticonv_zfe_free, NULL);
    // charset.h
    PRINTF(ticonv_ti73_to_utf16, PTR, NULL, (void *)0x12345678);

    PRINTF(ticonv_ti73_to_utf16, PTR, (void *)0x12345678, NULL);
    PRINTF(ticonv_ti80_to_utf16, PTR, NULL, (void *)0x12345678);
    PRINTF(ticonv_ti80_to_utf16, PTR, (void *)0x12345678, NULL);
    PRINTF(ticonv_ti82_to_utf16, PTR, NULL, (void *)0x12345678);
    PRINTF(ticonv_ti82_to_utf16, PTR, (void *)0x12345678, NULL);
// 41
    PRINTF(ticonv_ti83_to_utf16, PTR, NULL, (void *)0x12345678);
    PRINTF(ticonv_ti83_to_utf16, PTR, (void *)0x12345678, NULL);
    PRINTF(ticonv_ti83p_to_utf16, PTR, NULL, (void *)0x12345678);
    PRINTF(ticonv_ti83p_to_utf16, PTR, (void *)0x12345678, NULL);
    PRINTF(ticonv_ti85_to_utf16, PTR, NULL, (void *)0x12345678);

    PRINTF(ticonv_ti85_to_utf16, PTR, (void *)0x12345678, NULL);
    PRINTF(ticonv_ti86_to_utf16, PTR, NULL, (void *)0x12345678);
    PRINTF(ticonv_ti86_to_utf16, PTR, (void *)0x12345678, NULL);
    PRINTF(ticonv_ti9x_to_utf16, PTR, NULL, (void *)0x12345678);
    PRINTF(ticonv_ti9x_to_utf16, PTR, (void *)0x12345678, NULL);
// 51
    PRINTF(ticonv_utf16_to_ti73, STR, NULL, (void *)0x12345678);
    PRINTF(ticonv_utf16_to_ti73, STR, (void *)0x12345678, NULL);
    PRINTF(ticonv_utf16_to_ti80, STR, NULL, (void *)0x12345678);
    PRINTF(ticonv_utf16_to_ti80, STR, (void *)0x12345678, NULL);
    PRINTF(ticonv_utf16_to_ti82, STR, NULL, (void *)0x12345678);

    PRINTF(ticonv_utf16_to_ti82, STR, (void *)0x12345678, NULL);
    PRINTF(ticonv_utf16_to_ti83, STR, NULL, (void *)0x12345678);
    PRINTF(ticonv_utf16_to_ti83, STR, (void *)0x12345678, NULL);
    PRINTF(ticonv_utf16_to_ti83p, STR, NULL, (void *)0x12345678);
    PRINTF(ticonv_utf16_to_ti83p, STR, (void *)0x12345678, NULL);
// 61
    PRINTF(ticonv_utf16_to_ti85, STR, NULL, (void *)0x12345678);
    PRINTF(ticonv_utf16_to_ti85, STR, (void *)0x12345678, NULL);
    PRINTF(ticonv_utf16_to_ti86, STR, NULL, (void *)0x12345678);
    PRINTF(ticonv_utf16_to_ti86, STR, (void *)0x12345678, NULL);
    PRINTF(ticonv_utf16_to_ti9x, STR, NULL, (void *)0x12345678);

    PRINTF(ticonv_utf16_to_ti9x, STR, (void *)0x12345678, NULL);
    PRINTF(ticonv_ti84pusb_to_utf16, PTR, NULL, (void *)0x12345678);// PRINTF(ticonv_ti84pusb_to_utf16, PTR, (void *)0x12345678, NULL);
    PRINTF(ticonv_ti89tusb_to_utf16, PTR, NULL, (void *)0x12345678);// PRINTF(ticonv_ti89tusb_to_utf16, PTR, (void *)0x12345678, NULL);
    PRINTF(ticonv_utf16_to_ti84pusb, STR, NULL, NULL);
    PRINTF(ticonv_utf16_to_ti89tusb, STR, NULL, NULL);

    return 0;
}
