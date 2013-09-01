#include <stdio.h>
#include <tifiles.h>

#define PRINTF(FUNCTION, TYPE, args...) \
fprintf(stderr, "%d\t" TYPE "\n", i, FUNCTION(args)); i++

#define PRINTFVOID(FUNCTION, args...) \
fprintf(stderr, "%d\n", i); FUNCTION(args); i++

#define INT "%d"
#define PTR "%p"
#define STR "\"%s\""
#define VOID ""

int main(int argc, char **argv)
{
    int i = 1;

    tifiles_library_init();
// 1
    PRINTF(tifiles_error_get, INT, -1, NULL);
    PRINTF(tifiles_model_to_string, STR, -1);
    PRINTF(tifiles_string_to_model, INT, NULL);
    PRINTF(tifiles_attribute_to_string, STR, -1);
    PRINTF(tifiles_string_to_attribute, INT, NULL);

    PRINTF(tifiles_class_to_string, STR, -1);
    PRINTF(tifiles_string_to_class, INT, NULL);
    PRINTF(tifiles_fext_of_group, STR, -1);
    PRINTF(tifiles_fext_of_backup, STR, -1);
    PRINTF(tifiles_fext_of_flash_app, STR, -1);
// 11
    PRINTF(tifiles_fext_of_flash_os, STR, -1);
    PRINTF(tifiles_fext_of_certif, STR, -1);
    PRINTF(tifiles_fext_get, STR, NULL);
    PRINTF(tifiles_fext_dup, STR, NULL);
    PRINTF(tifiles_file_is_ti, INT, NULL);

    PRINTF(tifiles_file_is_single, INT, NULL);
    PRINTF(tifiles_file_is_group, INT, NULL);
    PRINTF(tifiles_file_is_regular, INT, NULL);
    PRINTF(tifiles_file_is_backup, INT, NULL);
    PRINTF(tifiles_file_is_os, INT, NULL);
// 21
    PRINTF(tifiles_file_is_app, INT, NULL);
    PRINTF(tifiles_file_is_tib, INT, NULL);
    PRINTF(tifiles_file_is_flash, INT, NULL);
    PRINTF(tifiles_file_is_tigroup, INT, NULL);
    PRINTF(tifiles_file_is_tno, INT, NULL);

    PRINTF(tifiles_file_test, INT, NULL, -1, -1);
    PRINTF(tifiles_file_get_model, INT, NULL);
    PRINTF(tifiles_file_get_class, INT, NULL);
    PRINTF(tifiles_file_get_type, STR, NULL);
    PRINTF(tifiles_file_get_icon, STR, NULL);
// 31
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
// 41
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
// 51
    PRINTF(tifiles_get_fldname, STR, NULL);
    PRINTF(tifiles_build_fullname, STR, -1, NULL, NULL, (void *)0x12345678);
    PRINTF(tifiles_build_fullname, STR, -1, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_build_fullname, STR, -1, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_build_filename, STR, -1, NULL);

    PRINTF(tifiles_content_create_regular, PTR, -1);
    PRINTF(tifiles_content_delete_regular, INT, NULL);
    PRINTF(tifiles_file_read_regular, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_read_regular, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_regular, INT, NULL, NULL, (void *)0x12345678);
// 61
    PRINTF(tifiles_file_write_regular, INT, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_regular, INT, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_file_display_regular, INT, NULL);
    PRINTF(tifiles_content_create_backup, PTR, -1);
    PRINTF(tifiles_content_delete_backup, INT, NULL);

    PRINTF(tifiles_file_read_backup, INT, NULL, NULL);
    PRINTF(tifiles_file_read_backup, INT, NULL, NULL);
    PRINTF(tifiles_file_write_backup, INT, NULL, NULL);
    PRINTF(tifiles_file_write_backup, INT, NULL, NULL);
    PRINTF(tifiles_file_display_backup, INT, NULL);
// 71
    PRINTF(tifiles_content_create_flash, PTR, -1);
    PRINTF(tifiles_content_delete_flash, INT, NULL);
    PRINTF(tifiles_file_read_flash, INT, NULL, NULL);
    PRINTF(tifiles_file_read_flash, INT, NULL, NULL);
    PRINTF(tifiles_file_write_flash, INT, NULL, NULL);

    PRINTF(tifiles_file_write_flash, INT, NULL, NULL);
    PRINTF(tifiles_file_write_flash2, INT, NULL, NULL, NULL);
    PRINTF(tifiles_file_write_flash2, INT, NULL, NULL, NULL);
    PRINTF(tifiles_file_display_flash, INT, NULL);
    PRINTF(tifiles_content_dup_regular, PTR, NULL);
// 81
    PRINTF(tifiles_content_dup_flash, PTR, NULL);
    PRINTF(tifiles_file_display, INT, NULL);
    PRINTF(tifiles_content_create_group, PTR, 0);
    PRINTF(tifiles_content_delete_group, INT, NULL);
    PRINTF(tifiles_group_contents, INT, NULL, NULL);

    PRINTF(tifiles_group_contents, INT, NULL, NULL);
    PRINTF(tifiles_ungroup_content, INT, NULL, NULL);
    PRINTF(tifiles_ungroup_content, INT, NULL, NULL);
    PRINTF(tifiles_group_files, INT, NULL, NULL);
    PRINTF(tifiles_group_files, INT, NULL, NULL);
// 91
    PRINTF(tifiles_ungroup_file, INT, NULL, NULL);
    PRINTF(tifiles_ungroup_file, INT, NULL, NULL);
    PRINTF(tifiles_content_add_entry, INT, NULL, NULL);
    PRINTF(tifiles_content_add_entry, INT, NULL, NULL);
    PRINTF(tifiles_content_del_entry, INT, NULL, NULL);

    PRINTF(tifiles_content_del_entry, INT, NULL, NULL);
    PRINTF(tifiles_group_add_file, INT, NULL, NULL);
    PRINTF(tifiles_group_add_file, INT, NULL, NULL);
    PRINTF(tifiles_group_del_file, INT, NULL, NULL);
    PRINTF(tifiles_group_del_file, INT, NULL, NULL);
// 101
    PRINTF(tifiles_content_create_tigroup, PTR, -1, -1);
    PRINTF(tifiles_content_delete_tigroup, INT, NULL);
    PRINTF(tifiles_file_read_tigroup, INT, NULL, NULL);
    PRINTF(tifiles_file_read_tigroup, INT, NULL, NULL);
    PRINTF(tifiles_file_write_tigroup, INT, NULL, NULL);

    PRINTF(tifiles_file_write_tigroup, INT, NULL, NULL);
    PRINTF(tifiles_file_display_tigroup, INT, NULL);
    PRINTF(tifiles_tigroup_contents, INT, NULL, NULL, NULL);
    PRINTF(tifiles_tigroup_contents, INT, NULL, NULL, NULL);
    PRINTF(tifiles_tigroup_contents, INT, NULL, NULL, NULL);
// 111
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
// 121
    PRINTF(tifiles_content_del_te, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_add_file, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_add_file, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_del_file, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_del_file, INT, NULL, NULL);

    PRINTF(tifiles_te_create, PTR, NULL, -1, -1);
    PRINTF(tifiles_te_delete, INT, NULL);
    PRINTF(tifiles_te_create_array, PTR, 0);
    PRINTF(tifiles_te_resize_array, PTR, NULL, 0);
    PRINTFVOID(tifiles_te_delete_array, NULL);
// 131
    PRINTF(tifiles_te_sizeof_array, INT, NULL);
    PRINTF(tifiles_comment_set_single, STR);
    PRINTF(tifiles_comment_set_group, STR);
    PRINTF(tifiles_comment_set_backup, STR);
    PRINTF(tifiles_comment_set_tigroup, STR);

    PRINTF(tifiles_ve_create, PTR);
    PRINTF(tifiles_ve_create_with_data, PTR, 0);
    PRINTFVOID(tifiles_ve_delete, NULL);
    PRINTF(tifiles_ve_alloc_data, PTR, 0);
    PRINTF(tifiles_ve_copy, PTR, NULL, NULL);
// 141
    PRINTF(tifiles_ve_copy, PTR, NULL, NULL);
    PRINTF(tifiles_ve_dup, PTR, NULL);
    PRINTF(tifiles_ve_create_array, PTR, 0);
    PRINTF(tifiles_ve_resize_array, PTR, NULL, 0);
    PRINTFVOID(tifiles_ve_delete_array, NULL);

    PRINTF(tifiles_fp_create, PTR);
    PRINTF(tifiles_fp_create_with_data, PTR, 0);
    PRINTFVOID(tifiles_fp_delete, NULL);
    PRINTF(tifiles_fp_alloc_data, PTR, 0);
    PRINTF(tifiles_fp_create_array, PTR, 0);  
// 151
    PRINTFVOID(tifiles_fp_delete_array, NULL);
    PRINTF(tifiles_create_table_of_entries, PTR, NULL, NULL);
    PRINTF(tifiles_create_table_of_entries, PTR, NULL, NULL);

    tifiles_library_exit();

    return 0;
}
