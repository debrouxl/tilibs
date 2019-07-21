#include <stdio.h>
#include <assert.h>
#include <tifiles.h>
#include "../src/error.h"

#define PRINTF(FUNCTION, TYPE, args...) \
fprintf(stderr, "%d\t" TYPE "\n", __LINE__, FUNCTION(args))

#define PRINTFVOID(FUNCTION, args...) \
FUNCTION(args); fprintf(stderr, "%d\n", __LINE__)

#define INT "%d"
#define PTR "%p"
#define STR "\"%s\""
#define VOID ""

static const uint8_t certdata[] =
{
                                                                                    0x03, 0x0E,
0x02, 0x0A, 0x00, 0x10, 0x01, 0x01, 0x07, 0x04, 0x05, 0x01, 0x23, 0x45, 0x67, 0x89, 0x03, 0x26,
0x09, 0x04, 0x12, 0x34, 0x56, 0x78, 0x02, 0x0D, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x07, 0x0E, 0x01, 0xAF, 0x07, 0x11, 0x04,
0x07, 0x3D, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,
0x3E, 0x3F, 0x40, 0x07, 0x12, 0x01, 0x04, 0x07, 0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05,
0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x07, 0x12, 0x52, 0x04, 0x07,
0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,
0x3E, 0x3F, 0x40, 0x07, 0x13, 0x03, 0x5F, 0x04, 0x07, 0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04,
0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34,
0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x07, 0x12, 0x32, 0x04,
0x07, 0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C,
0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C,
0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C,
0x3D, 0x3E, 0x3F, 0x40, 0x07, 0x12, 0x22, 0x04, 0x07, 0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04,
0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34,
0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x02, 0x0D, 0x40, 0x01,
0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21,
0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40
};

static const uint16_t field_path1[] = { 0x0300, 0x0320, 0x0900 };
static const uint16_t field_path2[] = { 0x0300, 0x0700, 0x0730 };
static const uint16_t field_path3[] = { 0x0300, 0x0400, 0x0730 };

static void cert_functions_unit_test(void)
{
    uint16_t field_type;
    const uint8_t * contents = NULL;
    const uint8_t * data;
    uint32_t field_size;

    assert(ERR_INVALID_PARAM == tifiles_cert_field_get(NULL, 0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_get((void *)0x12345678, 0x12345678, NULL, NULL, NULL));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_get((void *)0x12345678, 1, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_CERT_TRUNCATED == tifiles_cert_field_get(certdata, 3, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_CERT_TRUNCATED == tifiles_cert_field_get(certdata, 0x100, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678));
    assert(0 == tifiles_cert_field_get(certdata, sizeof(certdata) / sizeof(certdata[0]), &field_type, &contents, &field_size));
    assert(0x030E == field_type);
    assert(NULL != contents);
    assert(0x020A == field_size);

    data = certdata;
    field_size = sizeof(certdata) / sizeof(certdata[0]);
    assert(ERR_INVALID_PARAM == tifiles_cert_field_next(NULL, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_next((void *)0x12345678, NULL));
    assert(0 == tifiles_cert_field_next(&data, &field_size));
    assert(0 == tifiles_cert_field_get(data, field_size, &field_type, &contents, &field_size));
    assert(0x020D == field_type);
    assert(NULL != contents);
    assert(0x0040 == field_size);

    assert(ERR_INVALID_PARAM == tifiles_cert_field_find(NULL, 0x12345678, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_find((void *)0x12345678, 1, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(0 == tifiles_cert_field_find(certdata, sizeof(certdata) / sizeof(certdata[0]), 0x300, &contents, &field_size));
    assert(certdata + 2 + 2 == contents);
    assert(0x020A == field_size);
    assert(0 == tifiles_cert_field_find(certdata, sizeof(certdata) / sizeof(certdata[0]), 0x30E, &contents, &field_size));
    assert(certdata + 2 + 2 == contents);
    assert(0x020A == field_size);
    assert(0 == tifiles_cert_field_find(certdata, sizeof(certdata) / sizeof(certdata[0]), 0x20D, &contents, &field_size));
    assert(certdata + 2 + 2 + 0x20A + 2 + 1 == contents);
    assert(0x0040 == field_size);

    assert(ERR_INVALID_PARAM == tifiles_cert_field_find_path(NULL, 0x12345678, (void *)0x12345678, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_find_path((void *)0x12345678, 0x12345678, NULL, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_find_path((void *)0x12345678, 1, NULL, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_find_path((void *)0x12345678, 0x12345678, (void *)0x12345678, 0, (void *)0x12345678, (void *)0x12345678));
    assert(0 == tifiles_cert_field_find_path(certdata, sizeof(certdata) / sizeof(certdata[0]), field_path1, sizeof(field_path1) / sizeof(field_path1[0]), &contents, &field_size));
    assert(0x0004 == field_size);
    assert(0 == tifiles_cert_field_find_path(certdata, sizeof(certdata) / sizeof(certdata[0]), field_path2, sizeof(field_path2) / sizeof(field_path2[0]), &contents, &field_size));
    assert(0x0040 == field_size);
    assert(0 != tifiles_cert_field_find_path(certdata, sizeof(certdata) / sizeof(certdata[0]), field_path3, sizeof(field_path3) / sizeof(field_path3[0]), &contents, &field_size));
}

int main(int argc, char **argv)
{
    void * ptr;

    tifiles_library_init();

    PRINTF(tifiles_error_get, INT, -1, NULL);
    PRINTF(tifiles_error_free, INT, NULL);
    PRINTF(tifiles_model_to_string, STR, -1);
    PRINTF(tifiles_string_to_model, INT, NULL);
    PRINTF(tifiles_attribute_to_string, STR, -1);
    PRINTF(tifiles_string_to_attribute, INT, NULL);
    PRINTF(tifiles_class_to_string, STR, -1);
    PRINTF(tifiles_string_to_class, INT, NULL);
    PRINTF(tifiles_fext_of_group, STR, -1);
    PRINTF(tifiles_fext_of_backup, STR, -1);

    PRINTF(tifiles_fext_of_flash_app, STR, -1);
    PRINTF(tifiles_fext_of_flash_os, STR, -1);
    PRINTF(tifiles_fext_of_certif, STR, -1);
    PRINTF(tifiles_fext_get, STR, NULL);
    ptr = tifiles_fext_dup(NULL);
    PRINTF(, PTR, ptr);
    tifiles_fext_free(ptr);
    PRINTFVOID(tifiles_fext_free, NULL);
    PRINTF(tifiles_file_is_ti, INT, NULL);
    PRINTF(tifiles_file_is_single, INT, NULL);
    PRINTF(tifiles_file_is_group, INT, NULL);
    PRINTF(tifiles_file_is_regular, INT, NULL);

    PRINTF(tifiles_file_is_backup, INT, NULL);
    PRINTF(tifiles_file_is_os, INT, NULL);
    PRINTF(tifiles_file_is_app, INT, NULL);
    PRINTF(tifiles_file_is_tib, INT, NULL);
    PRINTF(tifiles_file_is_flash, INT, NULL);
    PRINTF(tifiles_file_is_tigroup, INT, NULL);
    PRINTF(tifiles_file_is_tno, INT, NULL);
    PRINTF(tifiles_file_has_ti_header, INT, NULL);
    PRINTF(tifiles_file_has_tib_header, INT, NULL);
    PRINTF(tifiles_file_has_tig_header, INT, NULL);

    PRINTF(tifiles_file_has_tifl_header, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(tifiles_file_has_tno_header, INT, NULL);
    PRINTF(tifiles_model_to_dev_type, INT, -1);
    PRINTF(tifiles_file_test, INT, NULL, -1, -1);
    PRINTF(tifiles_file_get_model, INT, NULL);
    PRINTF(tifiles_file_get_class, INT, NULL);
    PRINTF(tifiles_file_get_type, STR, NULL);
    PRINTF(tifiles_file_get_icon, STR, NULL);
    PRINTF(tifiles_vartype2string, STR, -1, -1);
    PRINTF(tifiles_string2vartype, INT, -1, NULL);

    PRINTF(tifiles_vartype2fext, STR, -1, -1);
    PRINTF(tifiles_fext2vartype, INT, -1, NULL);
    PRINTF(tifiles_vartype2type, STR, -1, -1);
    PRINTF(tifiles_vartype2icon, STR, -1, -1);
    PRINTF(tifiles_calctype2signature, STR, -1);
    PRINTF(tifiles_signature2calctype, INT, NULL);
    PRINTF(tifiles_folder_type, INT, -1);
    PRINTF(tifiles_flash_type, INT, -1);
    PRINTF(tifiles_idlist_type, INT, -1);
    PRINTF(tifiles_calc_is_ti8x, INT, -1);

    PRINTF(tifiles_calc_is_ti9x, INT, -1);
    PRINTF(tifiles_calc_are_compat, INT, -1, -1);
    PRINTF(tifiles_has_folder, INT, -1);
    PRINTF(tifiles_is_flash, INT, -1);
    PRINTF(tifiles_has_backup, INT, -1);
    PRINTF(tifiles_checksum, INT, NULL, 1234567891);
    PRINTF(tifiles_hexdump, INT, NULL, 1);
    PRINTF(tifiles_get_varname, STR, NULL);
    PRINTF(tifiles_get_fldname, STR, NULL);
    PRINTF(tifiles_get_fldname_s, STR, NULL, (void *)0x12345678);

    PRINTF(tifiles_get_fldname_s, STR, (void *)0x12345678, NULL);
    PRINTF(tifiles_build_fullname, STR, -1, NULL, NULL, (void *)0x12345678);
    PRINTF(tifiles_build_fullname, STR, -1, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_build_fullname, STR, -1, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_build_filename, STR, -1, NULL);
    PRINTFVOID(tifiles_filename_free, NULL);
    ptr = tifiles_content_create_regular(-1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_regular(ptr);
    PRINTF(tifiles_content_delete_regular, INT, NULL);
    PRINTF(tifiles_file_read_regular, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_read_regular, INT, (void *)0x12345678, NULL);

    PRINTF(tifiles_file_write_regular, INT, NULL, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_write_regular, INT, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_regular, INT, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_file_display_regular, INT, NULL);
    ptr = tifiles_content_create_backup(-1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_backup(ptr);
    PRINTF(tifiles_content_delete_backup, INT, NULL);
    PRINTF(tifiles_file_read_backup, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_read_backup, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_backup, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_write_backup, INT, (void *)0x12345678, NULL);

    PRINTF(tifiles_file_display_backup, INT, NULL);
    ptr = tifiles_content_create_flash(-1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_flash(ptr);
    PRINTF(tifiles_file_read_flash, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_content_delete_flash, INT, NULL);
    PRINTF(tifiles_file_read_flash, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_flash, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_write_flash, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_flash2, INT, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_flash2, INT, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_file_display_flash, INT, NULL);

    PRINTF(tifiles_content_dup_regular, PTR, NULL);
    PRINTF(tifiles_content_dup_flash, PTR, NULL);
    PRINTF(tifiles_file_display, INT, NULL);
    ptr = tifiles_content_create_group(0);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_group(ptr);
    PRINTF(tifiles_content_delete_group, INT, NULL);
    PRINTF(tifiles_group_contents, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_group_contents, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_ungroup_content, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_ungroup_content, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_group_files, INT, NULL, (void *)0x12345678);

    PRINTF(tifiles_group_files, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_ungroup_file, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_content_add_entry, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_content_del_entry, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_group_add_file, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_group_add_file, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_group_del_file, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_group_del_file, INT, (void *)0x12345678, NULL);
    ptr = tifiles_content_create_tigroup(-1, -1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_tigroup(ptr);
    PRINTF(tifiles_content_delete_tigroup, INT, NULL);

    PRINTF(tifiles_file_read_tigroup, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_read_tigroup, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_tigroup, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_write_tigroup, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_display_tigroup, INT, NULL);
    PRINTF(tifiles_tigroup_contents, INT, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_tigroup_contents, INT, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_tigroup_contents, INT, NULL, NULL, NULL);
    PRINTF(tifiles_untigroup_content, INT, NULL, NULL, NULL);
    PRINTF(tifiles_untigroup_content, INT, NULL, NULL, NULL);

    PRINTF(tifiles_untigroup_content, INT, NULL, NULL, NULL);
    PRINTF(tifiles_tigroup_files, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_files, INT, NULL, NULL);
    PRINTF(tifiles_untigroup_file, INT, NULL, NULL);
    PRINTF(tifiles_untigroup_file, INT, NULL, NULL);
    PRINTF(tifiles_content_add_te, INT, NULL, NULL);
    PRINTF(tifiles_content_add_te, INT, NULL, NULL);
    PRINTF(tifiles_content_del_te, INT, NULL, NULL);
    PRINTF(tifiles_content_del_te, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_add_file, INT, NULL, NULL);

    PRINTF(tifiles_tigroup_add_file, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_del_file, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_del_file, INT, NULL, NULL);
    PRINTF(tifiles_te_create, PTR, NULL, -1, -1);
    PRINTF(tifiles_te_delete, INT, NULL);
    ptr = tifiles_te_create_array(0);
    PRINTF(, PTR, ptr);
    tifiles_te_delete_array(ptr);
    ptr = tifiles_te_resize_array(NULL, 0);
    PRINTF(, PTR, ptr);
    tifiles_te_delete_array(ptr);
    PRINTFVOID(tifiles_te_delete_array, NULL);
    PRINTF(tifiles_te_sizeof_array, INT, NULL);
    PRINTF(tifiles_comment_set_single, STR);

    PRINTF(tifiles_comment_set_group, STR);
    PRINTF(tifiles_comment_set_backup, STR);
    PRINTF(tifiles_comment_set_tigroup, STR);
    PRINTF(tifiles_comment_set_single_sn, STR, NULL, 1234567891);
    PRINTF(tifiles_comment_set_group_sn, STR, NULL, 1234567891);
    PRINTF(tifiles_comment_set_backup_sn, STR, NULL, 1234567891);
    PRINTF(tifiles_comment_set_tigroup_sn, STR, NULL, 1234567891);
    ptr = tifiles_ve_create();
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);
    ptr = tifiles_ve_create_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);
    ptr = tifiles_ve_create_with_data(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);
    ptr = tifiles_ve_create_with_data2(0, NULL);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);
    PRINTFVOID(tifiles_ve_delete, NULL);

    ptr = tifiles_ve_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_free_data(ptr);
    ptr = tifiles_ve_realloc_data(NULL, 1);
    PRINTF(, PTR, ptr);
    tifiles_ve_free_data(ptr);
    PRINTFVOID(tifiles_ve_free_data, NULL);
    PRINTF(tifiles_ve_copy, PTR, NULL, NULL);
    PRINTF(tifiles_ve_dup, PTR, NULL);
    ptr = tifiles_ve_create_array(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete_array(ptr);
    ptr = tifiles_ve_resize_array(NULL, 0);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete_array(ptr);
    PRINTFVOID(tifiles_ve_delete_array, NULL);
    ptr = tifiles_fp_create();
    PRINTF(, PTR, ptr);
    tifiles_fp_delete(ptr);
    ptr = tifiles_fp_create_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete(ptr);

    ptr = tifiles_fp_create_with_data(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete(ptr);
    ptr = tifiles_fp_create_with_data2(0, NULL);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete(ptr);
    PRINTFVOID(tifiles_fp_delete, NULL);
    ptr = tifiles_fp_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_free_data(ptr);
    ptr = tifiles_fp_realloc_data(NULL, 1);
    PRINTF(, PTR, ptr);
    tifiles_fp_free_data(ptr);
    PRINTFVOID(tifiles_fp_free_data, NULL);
    ptr = tifiles_fp_create_array(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete_array(ptr);
    ptr = tifiles_fp_resize_array(NULL, 0);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete_array(ptr);
    PRINTFVOID(tifiles_fp_delete_array, NULL);
    ptr = tifiles_create_table_of_entries(NULL, NULL);
    PRINTF(, PTR, ptr);
    tifiles_free_table_of_entries(ptr);

    PRINTFVOID(tifiles_free_table_of_entries, NULL);

    cert_functions_unit_test();

    tifiles_library_exit();

    return 0;
}
