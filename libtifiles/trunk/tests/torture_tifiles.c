#include <stdio.h>
#include <tifiles.h>

#define PRINTF(FUNCTION, TYPE, args...) \
fprintf(stderr, "%d\t" TYPE "\n", i, FUNCTION(args)); i++;

#define PRINTFVOID(FUNCTION, args...) \
fprintf(stderr, "%d\n", i); FUNCTION(args); i++;

#define INT "%d"
#define PTR "%p"
#define STR "\"%s\""
#define VOID ""

int main(int argc, char **argv)
{
    int i = 1;
    void * ptr;

    tifiles_library_init();

// 1
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
// 11
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
// 21
    PRINTF(tifiles_file_is_backup, INT, NULL);
    PRINTF(tifiles_file_is_os, INT, NULL);
    PRINTF(tifiles_file_is_app, INT, NULL);
    PRINTF(tifiles_file_is_tib, INT, NULL);
    PRINTF(tifiles_file_is_flash, INT, NULL);

    PRINTF(tifiles_file_is_tigroup, INT, NULL);
    PRINTF(tifiles_file_is_tno, INT, NULL);
    PRINTF(tifiles_file_test, INT, NULL, -1, -1);
    PRINTF(tifiles_file_get_model, INT, NULL);
    PRINTF(tifiles_file_get_class, INT, NULL);
// 31
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
// 41
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
// 51
    PRINTF(tifiles_hexdump, INT, NULL, 1);
    PRINTF(tifiles_get_varname, STR, NULL);
    PRINTF(tifiles_get_fldname, STR, NULL);
    PRINTF(tifiles_build_fullname, STR, -1, NULL, NULL, (void *)0x12345678);
    PRINTF(tifiles_build_fullname, STR, -1, NULL, (void *)0x12345678, NULL);

    PRINTF(tifiles_build_fullname, STR, -1, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_build_filename, STR, -1, NULL);
    PRINTFVOID(tifiles_filename_free, NULL);
    ptr = tifiles_content_create_regular(-1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_regular(ptr);
    PRINTF(tifiles_content_delete_regular, INT, NULL);
// 61
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
// 71
    PRINTF(tifiles_file_write_backup, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_write_backup, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_display_backup, INT, NULL);
    ptr = tifiles_content_create_flash(-1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_flash(ptr);
    PRINTF(tifiles_content_delete_flash, INT, NULL);

    PRINTF(tifiles_file_read_flash, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_read_flash, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_flash, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_write_flash, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_flash2, INT, NULL, (void *)0x12345678, NULL);
// 81
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
// 91
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
// 101
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
    ptr = tifiles_te_create_array(0);
    PRINTF(, PTR, ptr);
    tifiles_te_delete_array(ptr);
    ptr = tifiles_te_resize_array(NULL, 0);
    PRINTF(, PTR, ptr);
    tifiles_te_delete_array(ptr);
    PRINTFVOID(tifiles_te_delete_array, NULL);
// 131
    PRINTF(tifiles_te_sizeof_array, INT, NULL);
    PRINTF(tifiles_comment_set_single, STR);
    PRINTF(tifiles_comment_set_group, STR);
    PRINTF(tifiles_comment_set_backup, STR);
    PRINTF(tifiles_comment_set_tigroup, STR);

    ptr = tifiles_ve_create();
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);
    ptr = tifiles_ve_create_with_data(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);
    PRINTFVOID(tifiles_ve_delete, NULL);
    ptr = tifiles_ve_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_free_data(ptr);
    ptr = tifiles_ve_realloc_data(NULL, 1);
    PRINTF(, PTR, ptr);
    tifiles_ve_free_data(ptr);
// 141
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
    ptr = tifiles_fp_create_with_data(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete(ptr);
    PRINTFVOID(tifiles_fp_delete, NULL);
    ptr = tifiles_fp_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_free_data(ptr);
// 151
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

    tifiles_library_exit();

    return 0;
}
