/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 1999-2004  Romain Lievin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
  This program contains a lot of routines for testing various part of the
  library as well as checking file support.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#include "../src/tifiles.h"
#include "../src/files8x.h"
#include "../src/files9x.h"

/*
  Compare 2 files bytes per bytes and show differences
*/
static int compare_files(char *src, char *dst)
{
    FILE *fs, *fd;
    int i;
    int s, d;
    
    fs = fopen(src, "rb");
    fd = fopen(dst, "rb");
    
    if( (fs == NULL) || (fd == NULL) )
    {
	printf("Unable to open these files: <%s> & <%s>.\n", src, dst);
	return -1;
    }
    
    while(!feof(fs))
    {
	if((s = fgetc(fs)) != (d = fgetc(fd)))
	{
	    printf("\nFiles do not match !!!\n");
	    printf("Offset: %08X %i\n", (int)ftell(fs), (int)ftell(fs));
	    printf("Data  (src): %02X ", s);
	    for(i=0; i<16 && !feof(fs); i++)
		printf("%02X ", fgetc(fs));
	    printf("\n");
	    printf("Data  (dst): %02X ", d);
	    for(i=0; i<16 && !feof(fd); i++)
		printf("%02X ", fgetc(fd));
	    printf("\n");
	    return -1;
	}
    }
    
    printf("\n--> Files match !\n");
   
    return 0;
}

static void change_dir(const char *path)
{
#ifdef __WIN32__
	_chdir(path);
#endif
}

/*
  The main function
*/
int main(int argc, char **argv)
{
	char *msg;
	char str[16];
	char buffer[256];
	int i;
	int ret;
//	Ti9xFlash content = { 0 };

	// init library
	tifiles_library_init();

	// test tifiles.c
	printf("Library version : <%s>\n", tifiles_version_get());
	printf("--\n");

	// test error.c
	tifiles_error_get(515, &msg);
	printf("Error message: <%s>\n", msg);
	//bug
	//free(msg);
	printf("--\n");

	// test type2str.c
	printf("tifiles_string_to_model: <%i> <%i>\n", CALC_TI92, 
	       tifiles_string_to_model(tifiles_model_to_string(CALC_TI92)));
	printf("tifiles_string_to_attribute: <%i> <%i>\n", ATTRB_LOCKED, 
	       tifiles_string_to_attribute(tifiles_attribute_to_string(ATTRB_LOCKED)));
	printf("tifiles_string_to_class: <%i> <%i>\n", TIFILE_SINGLE, 
	       tifiles_string_to_class(tifiles_class_to_string(TIFILE_SINGLE)));
	printf("--\n");

	// test transcode.c
	tifiles_transcode_detokenize (CALC_TI82, buffer, "", 0x0C/*TI82_ZSTO*/);
	printf("<%s>\n", buffer);

	tifiles_transcode_varname(CALC_TI82, buffer, "", 0x0C/*TI82_ZSTO*/);
	printf("<%s>\n", buffer);

	tifiles_transcoding_set(ENCODING_ASCII);
	str[0] = 0x5d; str[1] = 0x02;
	tifiles_transcode_varname(CALC_TI82, buffer, str, -1);
	printf("<%s>\n", buffer);
	printf("--\n");

	// test filetypes.c
	for(i = CALC_TI73; i <= CALC_V200; i++)
	    printf("%s ", tifiles_fext_of_group(i));
	printf("\n");

	for(i = CALC_TI73; i <= CALC_V200; i++)
            printf("%s ", tifiles_fext_of_backup(i));
        printf("\n");

	for(i = CALC_TI73; i <= CALC_V200; i++)
            printf("%s ", tifiles_fext_of_flash_os(i));
        printf("\n");

	for(i = CALC_TI73; i <= CALC_V200; i++)
            printf("%s ", tifiles_fext_of_flash_app(i));
        printf("\n");

	printf("<%s> <%s>\n", "foo.bar", tifiles_fext_get("foo.bar"));

	ret = tifiles_file_is_ti("C:\\sources\\roms\\tifiles-2\\tests\\misc\\str.92s"); 
	printf("tifiles_file_is_ti: %i\n", ret);

	ret = tifiles_file_is_single("C:\\sources\\roms\\tifiles-2\\tests\\misc\\str.92s");
	printf("tifiles_file_is_single: %i\n", ret);

	ret = tifiles_file_is_group("C:\\sources\\roms\\tifiles-2\\tests\\misc\\group.92g");
        printf("tifiles_file_is_group: %i\n", ret);

	ret = tifiles_file_is_regular("C:\\sources\\roms\\tifiles-2\\tests\\misc\\str.92s");
        printf("tifiles_file_is_regular: %i\n", ret);

	ret = tifiles_file_is_regular("C:\\sources\\roms\\tifiles-2\\tests\\misc\\group.92g");
        printf("tifiles_file_is_regular: %i\n", ret);

	ret = tifiles_file_is_backup("C:\\sources\\roms\\tifiles-2\\tests\\misc\\backup.83b");
	printf("tifiles_file_is_backup: %i\n", ret);

	ret = tifiles_file_is_flash("C:\\sources\\roms\\tifiles-2\\tests\\misc\\ticabfra.89k");
        printf("tifiles_file_is_flash: %i\n", ret);

	ret = tifiles_file_is_flash("C:\\sources\\roms\\tifiles-2\\tests\\misc\\TI73_OS160.73U");
        printf("tifiles_file_is_flash: %i\n", ret);

	ret = tifiles_file_is_tib("C:\\sources\\roms\\tifiles-2\\tests\\misc\\ams100.tib");
        printf("tifiles_file_is_tib: %i\n", ret);
	printf("--\n");

	// test typesxx.c
	printf("tifiles_file_get_model: %s\n", 
         tifiles_model_to_string(tifiles_file_get_model("C:\\sources\\roms\\tifiles-2\\tests\\misc\\str.92s")));

	printf("tifiles_file_get_class: %s\n",
         tifiles_class_to_string(tifiles_file_get_class("C:\\sources\\roms\\tifiles-2\\tests\\misc\\group.92g")));

	printf("tifiles_file_get_type: %s\n",
	       tifiles_file_get_type("C:\\sources\\roms\\tifiles-2\\tests\\misc\\TI73_OS160.73U"));
	printf("tifiles_file_get_icon: %s\n",
	       tifiles_file_get_icon("C:\\sources\\roms\\tifiles-2\\tests\\misc\\str.92s"));
	printf("--\n");

	// test misc.c

	printf("tifiles_calc_is_ti8x: %i\n", tifiles_calc_is_ti8x(CALC_TI83));
	printf("tifiles_calc_is_ti9x: %i\n", tifiles_calc_is_ti9x(CALC_TI89));

	printf("tifiles_has_folder: %i\n", tifiles_has_folder(CALC_TI92));
	printf("tifiles_is_flash: %i\n", tifiles_is_flash(CALC_TI73));

	printf("tifiles_get_varname: <%s>\n", tifiles_get_varname("fld\\var"));
	printf("tifiles_get_fldname: <%s>\n", tifiles_get_fldname("fld\\var"));
	tifiles_build_fullname(CALC_TI89, buffer, "fld", "var");
	printf("tifiles_build_fullname: <%s>\n", buffer);
	printf("--\n");
	printf("--\n");

	// test filesxx.c & grouped.c

	// TI73 support
	// no file

	// TI82 support
ti82:
	change_dir("C:\\sources\\roms\\tifiles-2\\tests\\ti82");
	test_ti82_backup_support();
	test_ti82_regular_support();
	test_ti82_group_support();
	test_ti82_ungroup_support();

  // TI83+ support
ti84p:
	change_dir("C:\\sources\\roms\\tifiles-2\\tests\\ti84p");
	//test_ti84p_backup_support();
	test_ti84p_regular_support();
	test_ti84p_group_support();
	test_ti84p_ungroup_support();	
	test_ti84p_flash_support();  
	goto end;

ti86:
	change_dir("C:\\sources\\roms\\tifiles-2\\tests\\ti86");
	test_ti86_backup_support();
	test_ti86_regular_support();
	test_ti86_group_support();
	test_ti86_ungroup_support();

  // TI92 support
ti92:
	change_dir("C:\\sources\\roms\\tifiles-2\\tests\\ti92");
	test_ti92_backup_support();
	test_ti92_regular_support();
	test_ti92_group_support();
	test_ti92_ungroup_support();

ti89:
	change_dir("C:\\sources\\roms\\tifiles-2\\tests\\ti89");
	test_ti89_regular_support();
	test_ti89_flash_support();
	goto end;

end:
	tifiles_library_exit();

  return 0;
}

/*
	About TI formatted file: we can sort calc like this:
	- TI73
	- TI82/83
	- TI83+/TI84+
	- TI85/86
	- TI92
	- TI89/92+/V200

	Testing order:
	- backup
	- regular (single and group)
	- flash
	- grouping
	- ungrouping
*/

/*********/
/* TI-82 */
/*********/

static int test_ti82_backup_support()
{
  Ti8xBackup content;

  printf("--> Testing backup support...\n");
  ti8x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\backup.82b");
  ti8x_file_read_backup("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\backup.82b", &content);
  ti8x_file_write_backup("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\backup.82b_", &content);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\backup.82b", "C:\\sources\\roms\\tifiles-2\\tests\\ti82\\backup.82b_");

  return 0;
}

static int test_ti82_regular_support()
{
  Ti8xRegular content;
  char *unused;

  printf("--> Testing regular support (single)...\n");
  ti8x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\math.82p");
  ti8x_file_read_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\math.82p", &content);
  ti8x_file_write_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\math.82p_", &content, &unused);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\math.82p", "C:\\sources\\roms\\tifiles-2\\tests\\ti82\\math.82p_");

  printf("\n");

  printf("--> Testing regular support (group)...\n");
  ti8x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\group.82g");  
  ti8x_file_read_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\group.82g", &content);
  ti8x_file_write_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\group.82g_", &content, &unused);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\group.82g", "C:\\sources\\roms\\tifiles-2\\tests\\ti82\\group.82g_");

  return 0;
}

static int test_ti82_group_support()
{
  char *array[] = { "C:\\sources\\roms\\tifiles-2\\tests\\ti82\\aa.82n", "C:\\sources\\roms\\tifiles-2\\tests\\ti82\\bb.82n", NULL };
  
  printf("--> Testing grouping of files...\n");
  tifiles_group_files(array, "C:\\sources\\roms\\tifiles-2\\tests\\ti82\\aabb.82g_");
  tifiles_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\aabb.82g_");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\group.82g", "C:\\sources\\roms\\tifiles-2\\tests\\ti82\\aabb.82g_");
  
  return 0;
}

static int test_ti82_ungroup_support()
{
  printf("--> Testing ungrouping of files...\n");
  tifiles_ungroup_file("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\group.82g");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\A.82n", "C:\\sources\\roms\\tifiles-2\\tests\\ti82\\aa.82n");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti82\\B.82n", "C:\\sources\\roms\\tifiles-2\\tests\\ti82\\bb.82n");

  return 0;
}

static int test_ti82_group_ungroup_support()
{
  /*
  Ti8xRegular src1;
  Ti8xRegular **dst1;
  Ti8xRegular **src2;
  Ti8xRegular dst2;
  Ti8xRegular **ptr;

  ti8x_read_regular_file("group.82g", &src1);
  ti8x_ungroup_content(&src1, &dst1);
  src2 = dst1;
  ti8x_group_content(src2, dst2);
  ti8x_write_regular_file("group2.82g_", dst2);
  */
  return 0;
}

/**********/
/* TI-84+ */
/**********/

static int test_ti84p_regular_support()
{
  Ti8xRegular content;
  char *unused;

  printf("--> Testing regular support (single)...\n");
  ti8x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\romdump.8xp");
  ti8x_file_read_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\romdump.8xp", &content);
  ti8x_file_write_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\romdump.8xp_", &content, &unused);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\romdump.8xp", "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\romdump.8xp_");

  printf("\n");

  printf("--> Testing regular support (group)...\n");
  ti8x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\group.8Xg");  
  ti8x_file_read_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\group.8Xg", &content);
  ti8x_file_write_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\group.8Xg_", &content, &unused);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\group.8Xg", "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\group.8Xg_");

  return 0;
}

static int test_ti84p_group_support()
{
  char *array[] = { "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\aa.8Xn", "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\bb.8Xn", NULL };
  
  printf("--> Testing grouping of files...\n");
  tifiles_group_files(array, "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\aabb.8Xg_");
  tifiles_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\aabb.8Xg_");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\group.8Xg", "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\aabb.8Xg_");
  
  return 0;
}

static int test_ti84p_ungroup_support()
{
  printf("--> Testing ungrouping of files...\n");
  tifiles_ungroup_file("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\group.8Xg");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\A.8Xn", "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\aa.8Xn");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\B.8Xn", "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\bb.8Xn");

  return 0;
}

static int test_ti84p_flash_support()
{
  Ti8xFlash content;

  printf("--> Testing flash support...\n");

  ti8x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\chembio.8Xk");
  ti8x_file_read_flash("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\chembio.8Xk", &content);
  ti8x_file_write_flash("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\chembio.8Xk_", &content);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\chembio.8Xk", "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\chembio.8Xk_");

  return 0;

  ti8x_file_display("./ti83+/base112.8Xu");
  //ti8x_read_flash_file("./ti83+/base112.8Xu", &content);
  //hexdump(content.pages[0].data, 256);
  //hexdump(content.pages[content.num_pages-1].data, 256);
  //ti8x_write_flash_file("./ti83+/base112.8Xu_", &content);
  //compare_files("./ti83+/base112.8Xu", "./ti83+/base112.8Xu_");

  return 0;
}

/*********/
/* TI-86 */
/*********/

static int test_ti86_backup_support()
{
  Ti8xBackup content;

  printf("--> Testing backup support...\n");
  ti8x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\backup.85b");
  ti8x_file_read_backup("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\backup.85b", &content);
  ti8x_file_write_backup("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\backup.85b_", &content);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\backup.85b", "C:\\sources\\roms\\tifiles-2\\tests\\ti86\\backup.85b_");

  return 0;
}

static int test_ti86_regular_support()
{
  Ti8xRegular content;
  char *unused;

  printf("--> Testing regular support (single)...\n");
  ti8x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\prgm.86p");
  ti8x_file_read_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\prgm.86p", &content);
  ti8x_file_write_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\prgm.86p_", &content, &unused);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\prgm.86p", "C:\\sources\\roms\\tifiles-2\\tests\\ti86\\prgm.86p_");

  printf("\n");

  printf("--> Testing regular support (group)...\n");
  ti8x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\group.86g");  
  ti8x_file_read_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\group.86g", &content);
  ti8x_file_write_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\group.86g_", &content, &unused);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\group.86g", "C:\\sources\\roms\\tifiles-2\\tests\\ti86\\group.86g_");

  return 0;
}

static int test_ti86_group_support()
{
  char *array[] = { "C:\\sources\\roms\\tifiles-2\\tests\\ti86\\yy.86n", "C:\\sources\\roms\\tifiles-2\\tests\\ti86\\xx.86n", NULL };
  
  printf("--> Testing grouping of files...\n");
  tifiles_group_files(array, "C:\\sources\\roms\\tifiles-2\\tests\\ti86\\xxyy.86g_");
  tifiles_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\xxyy.86g_");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\group.86g", "C:\\sources\\roms\\tifiles-2\\tests\\ti86\\xxyy.86g_");
  
  return 0;
}

static int test_ti86_ungroup_support()
{
  printf("--> Testing ungrouping of files...\n");
  tifiles_ungroup_file("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\group.86g");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\X.86n", "C:\\sources\\roms\\tifiles-2\\tests\\ti86\\xx.86n");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti86\\Y.86n", "C:\\sources\\roms\\tifiles-2\\tests\\ti86\\yy.86n");

  return 0;
}

/*********/
/* TI-92 */
/*********/

static int test_ti92_backup_support()
{
  Ti9xBackup content;

  printf("--> Testing backup support...\n");
  ti9x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\backup.92b");
  ti9x_file_read_backup("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\backup.92b", &content);
  ti9x_file_write_backup("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\backup.92b_", &content);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\backup.92b", "C:\\sources\\roms\\tifiles-2\\tests\\ti92\\backup.92b_");

  return 0;
}

static int test_ti92_regular_support()
{
  Ti9xRegular content;
  char *unused;

  printf("--> Testing regular support (single)...\n");
  ti9x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\str.92s");
  ti9x_file_read_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\str.92s", &content);
  ti9x_file_write_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\str.92s_", &content, &unused);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\str.92s", "C:\\sources\\roms\\tifiles-2\\tests\\ti92\\str.92s_");

  printf("\n");
  
  printf("--> --> Testing regular support (group)...\n");
  ti9x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\group.92g");  
  ti9x_file_read_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\group.92g", &content);
  ti9x_file_write_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\group.92g_", &content, &unused);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\group.92g", "C:\\sources\\roms\\tifiles-2\\tests\\ti92\\group.92g_");

  return 0;
}

static int test_ti92_group_support()
{
  char *array[] = { "C:\\sources\\roms\\tifiles-2\\tests\\ti92\\xx.92s", "C:\\sources\\roms\\tifiles-2\\tests\\ti92\\yy.92s", NULL };
  
  printf("--> Testing grouping of files...\n");
  tifiles_group_files(array, "C:\\sources\\roms\\tifiles-2\\tests\\ti92\\xxyy.92g_");
  tifiles_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\xxyy.92g_");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\group.92g", "C:\\sources\\roms\\tifiles-2\\tests\\ti92\\xxyy.92g_");
  
  return 0;
}

static int test_ti92_ungroup_support()
{
  printf("--> Testing ungrouping of files...\n");
  tifiles_ungroup_file("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\group.92g");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\x.92s", "C:\\sources\\roms\\tifiles-2\\tests\\ti92\\xx.92s");
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti92\\y.92s", "C:\\sources\\roms\\tifiles-2\\tests\\ti92\\yy.92s");

  return 0;
}

/*********/
/* TI-89 */
/*********/

static int test_ti89_regular_support()
{
  Ti9xRegular content;
  char *unused;

  printf("--> Testing regular support (group)...\n");
  ti9x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti89\\group.89g");
  ti9x_file_read_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti89\\group.89g", &content);
  ti9x_file_write_regular("C:\\sources\\roms\\tifiles-2\\tests\\ti89\\group.89g_", &content, &unused);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti89\\group.89g", "C:\\sources\\roms\\tifiles-2\\tests\\ti89\\group.89g_");

  return 0;
}

static int test_ti89_flash_support()
{
  Ti9xFlash content;

  printf("--> Testing flash support...\n");
  ti9x_file_display("C:\\sources\\roms\\tifiles-2\\tests\\ti89\\ticabfra.89k");
  ti9x_file_read_flash("C:\\sources\\roms\\tifiles-2\\tests\\ti89\\ticabfra.89k", &content);
  ti9x_file_write_flash("C:\\sources\\roms\\tifiles-2\\tests\\ti89\\ticabfra.89k_", &content);
  compare_files("C:\\sources\\roms\\tifiles-2\\tests\\ti89\\ticabfra.89k", "C:\\sources\\roms\\tifiles-2\\tests\\ti89\\ticabfra.89k_");

  return 0;
}


static int test_v200_regular_support()
{
  Ti9xRegular content;
  char *unused;

  printf("--> Testing regular support (group)...\n");
  ti9x_file_display("./v200/xy.v2g");
  ti9x_file_read_regular("./v200/xy.v2g", &content);
  ti9x_file_write_regular("./v200/xy.v2g_", &content, &unused);
  compare_files("./v200/xy.v2g", "./v200/xy.v2g_");

  return 0;
}