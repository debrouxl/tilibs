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

static int test_ti82_backup_support()
{
  Ti8xBackup content;

  printf("--> Testing backup support...\n");
  ti8x_file_display("./ti82/backup.82b");
  ti8x_file_read_backup("./ti82/backup.82b", &content);
  ti8x_file_write_backup("./ti82/backup.82b_", &content);
  compare_files("./ti82/backup.82b", "./ti82/backup.82b_");

  return 0;
}

static int test_ti82_regular_support()
{
  Ti8xRegular content;
  char *unused;

  printf("--> Testing regular support (single)...\n");
  ti8x_file_display("./ti82/math.82p");
  ti8x_file_read_regular("./ti82/math.82p", &content);
  ti8x_file_write_regular("./ti82/math.82p_", &content, &unused);
  compare_files("./ti82/math.82p", "./ti82/math.82p_");

  printf("\n");

  printf("--> Testing regular support (group)...\n");
  ti8x_file_display("./ti82/group.82g");  
  ti8x_file_read_regular("./ti82/group.82g", &content);
  ti8x_file_write_regular("./ti82/group.82g_", &content, &unused);
  compare_files("./ti82/group.82g", "./ti82/group.82g_");

  return 0;
}

static int test_ti82_group_support()
{
  char *array[] = { "./ti82/aa.82n", "./ti82/bb.82n", NULL };
  
  printf("--> Testing grouping of files...\n");
  tifiles_group_files(array, "./ti82/aabb.82g_");
  tifiles_file_display("./ti82/aabb.82g_");
  compare_files("./ti82/group.82g", "./ti82/aabb.82g_");
  
  return 0;
}

static int test_ti82_ungroup_support()
{
  printf("--> Testing ungrouping of files...\n");
  tifiles_ungroup_file("./ti82/group.82g");
  rename("A.82n", "./ti82/A.82n");
  rename("B.82n", "./ti82/B.82n");
  compare_files("./ti82/A.82n", "./ti82/aa.82n");
  compare_files("./ti82/B.82n", "./ti82/bb.82n");

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

static int test_ti83p_regular_support()
{
  Ti8xRegular content;
  char *unused;

  printf("--> Testing regular support (group)...\n");
  ti8x_file_display("./ti83+/group.8xg");
  ti8x_file_read_regular("./ti83+/group.8xg", &content);
  ti8x_file_write_regular("./ti83+/group.8xg_", &content, &unused);
  compare_files("./ti83+/group.8xg", "./ti83+/group.8xg_");
  
  return 0;
  printf("\n");

  ti8x_file_display("./ti83+/group.8xg");
  ti8x_file_read_regular("./ti83+/group.8xg", &content);
  ti8x_file_write_regular("./ti83+/group.8xg_", &content, &unused);
  compare_files("./ti83+/group.8xg", "./ti83+/group.8xg_");

  return 0;
}

static int test_ti86_backup_support()
{
  Ti8xBackup content;

  printf("--> Testing backup support...\n");
  ti8x_file_display("./ti86/backup.86b");
  ti8x_file_read_backup("./ti86/backup.86b", &content);
  ti8x_file_write_backup("./ti86/backup.86b_", &content);
  compare_files("./ti86/backup.86b", "./ti86/backup.86b_");

  return 0;
}

static int test_ti86_regular_support()
{
  Ti8xRegular content;
  char *unused;

  printf("--> Testing regular support (single)...\n");
  ti8x_file_display("./ti86/x.86n");
  ti8x_file_read_regular("./ti86/x.86n", &content);
  ti8x_file_write_regular("./ti86/x.86n_", &content, &unused);
  compare_files("./ti86/x.86n", "./ti86/x.86n_");

  printf("\n");

  printf("--> Testing regular support (group)...\n");
  ti8x_file_display("./ti86/group.86g");
  ti8x_file_read_regular("./ti86/group.86g", &content);
  ti8x_file_write_regular("./ti86/group.86g_", &content, &unused);
  compare_files("./ti86/group.86g", "./ti86/group.86g_");

  return 0;
}

static int test_ti89_regular_support()
{
  Ti9xRegular content;
  char *unused;

  printf("--> Testing regular support (group)...\n");
  ti9x_file_display("./ti89/group.89g");
  ti9x_file_read_regular("./ti89/group.89g", &content);
  ti9x_file_write_regular("./ti89/group.89g_", &content, &unused);
  compare_files("./ti89/group.89g", "./ti89/group.89g_");

  return 0;
}

static int test_ti92_backup_support()
{
  Ti9xBackup content;

  printf("--> Testing backup support...\n");
  ti9x_file_display("./ti92/backup.92b");
  ti9x_file_read_backup("./ti92/backup.92b", &content);
  ti9x_file_write_backup("./ti92/backup.92b_", &content);
  compare_files("./ti92/backup.92b", "./ti92/backup.92b_");

  return 0;
}

static int test_ti92_regular_support()
{
  Ti9xRegular content;
  char *unused;

  printf("--> Testing regular support (single)...\n");
  ti9x_file_display("./ti92/str.92s");
  ti9x_file_read_regular("./ti92/str.92s", &content);
  ti9x_file_write_regular("./ti92/str.92s_", &content, &unused);
  //compare_files("./ti92/str.92s", "./ti92/str.92s_");

  printf("\n");
  
  printf("--> --> Testing regular support (group)...\n");
  ti9x_file_display("./ti92/group.92g");  
  ti9x_file_read_regular("./ti92/group.92g", &content);
  ti9x_file_write_regular("./ti92/group.92g_", &content, &unused);
  //compare_files("./ti92/group.92g", "./ti92/group.92g_");

  return 0;
}

static int test_ti92_group_support()
{
  char *array[] = { "./ti92/xx.92s", "./ti92/yy.92s", "./ti92/zz.92s", NULL };
  
  printf("--> Testing grouping of files...\n");
  tifiles_group_files(array, "./ti92/xxyyzz.92g_");
  tifiles_file_display("./ti92/xxyyzz.92g_");
  compare_files("./ti92/group.92g", "./ti92/xxyyzz.92g_");
  
  return 0;
}

static int test_ti92_ungroup_support()
{
  printf("--> Testing ungrouping of files...\n");
  tifiles_ungroup_file("./ti92/group.92g");
  rename("x.92s", "./ti92/x.92s");
  rename("y.92s", "./ti92/y.92s");
  compare_files("./ti92/x.92s", "./ti92/xx.92s");
  compare_files("./ti92/y.92s", "./ti92/yy.92s");

  return 0;
}

static int test_ti89_flash_support()
{
  Ti9xFlash content;

  printf("--> Testing flash support...\n");
  ti9x_file_display("./ti89/ticabfra.89k");
  ti9x_file_read_flash("./ti89/ticabfra.89k", &content);
  ti9x_file_write_flash("./ti89/ticabfra.89k_", &content);
  compare_files("./ti89/ticabfra.89k", "./ti89/ticabfra.89k_");

  return 0;
}

static int test_ti83p_flash_support()
{
  Ti8xFlash content;

  printf("--> Testing flash support...\n");

  ti8x_file_display("./ti83+/base112.8Xu");
  //ti8x_read_flash_file("./ti83+/base112.8Xu", &content);
  //hexdump(content.pages[0].data, 256);
  //hexdump(content.pages[content.num_pages-1].data, 256);
  //ti8x_write_flash_file("./ti83+/base112.8Xu_", &content);
  //compare_files("./ti83+/base112.8Xu", "./ti83+/base112.8Xu_");

  return 0;

  ti8x_file_display("./ti83+/chembio.8Xk");
  ti8x_file_read_flash("./ti83+/chembio.8Xk", &content);
  ti8x_file_write_flash("./ti83+/chembio.8Xk_", &content);
  compare_files("./ti83+/chembio.8Xk", "./ti83+/chembio.8Xk_");

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


/*
  The main function
*/
int main(int argc, char **argv)
{
	char *msg;
	char buffer[256];
	int i;
	int ret;
//	Ti9xFlash content = { 0 };

	// init library
	tifiles_library_init();

	// test tifiles.c
	printf("Library version : <%s>\n", tifiles_version_get());

	// test error.c
	tifiles_error_get(515, &msg);
	printf("Error message: <%s>\n", msg);
	free(msg);

	// test type2str.c
	printf("tifiles_string_to_model: <%i> <%i>\n", CALC_TI92, 
	       tifiles_string_to_model(tifiles_model_to_string(CALC_TI92)));
	printf("tifiles_string_to_attribute: <%i> <%i>\n", ATTRB_LOCKED, 
	       tifiles_string_to_attribute(tifiles_attribute_to_string(ATTRB_LOCKED)));
	printf("tifiles_string_to_class: <%i> <%i>\n", TIFILE_SINGLE, 
	       tifiles_string_to_class(tifiles_class_to_string(TIFILE_SINGLE)));

	// test transcode.c
	//tifiles_transcode_detokenize (CALC_TI82, "", buffer,0x0C/*TI82_ZSTO*/);
	//printf("<%s>\n", buffer);
// bug ?!
	//tifiles_transcode_varname(CALC_TI82, "", buffer, 0x0C/*TI82_ZSTO*/);
	//printf("<%s>\n", buffer);
	// bug ?!
	tifiles_transcode_detokenize (CALC_TI82, "\0x5d\0x00", buffer, -1);
	//printf("<%s>\n", buffer);
	//printf("\n");

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

	ret = tifiles_file_is_ti(""); 
	printf("tifiles_file_is_ti: %i\n", ret);

	// test typesxx.c

	// test misc.c
	return 0;
	// test filesxx.c

	// test grouped.c
/*
	// test OS file
	tifiles_set_calc(CALC_TI89);
	ti9x_display_file("c:\\msvc\\tilp\\os.89u");
	ti9x_read_flash_file("c:\\msvc\\tilp\\os.89u", &content);

	// test TIB file
	memset(&content, 0, sizeof(content));
	ti9x_display_file("c:\\msvc\\tilp\\os.tib");
	ti9x_read_flash_file("c:\\msvc\\tilp\\os.tib", &content);
*/
  
  goto ti83p;

  // TI82 support
 ti82:
  test_ti82_backup_support();
  test_ti82_regular_support();
  test_ti82_group_support();
  test_ti82_ungroup_support();
  goto end;

  // TI83+ support
 ti83p:
  test_ti83p_regular_support();
  //test_ti83p_flash_support();  
  goto end;

 ti86:
  //test_ti86_backup_support();
  test_ti86_regular_support();
  goto end;

  // TI92 support
 ti92:
  test_ti92_backup_support();
  test_ti92_regular_support();
  test_ti92_group_support();
  test_ti92_ungroup_support();
  goto end;

 ti89:
  test_ti89_flash_support();
  // test_ti89_regular_support();
  goto end;

  // V200 support
 v200:
  test_v200_regular_support();
  goto end;

 end:
  tifiles_library_exit();

  return 0;
}

