/*  libtifiles - TI files library, a part of the TiLP project
 *  Copyright (C) 2002  Romain Lievin
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/file_int.h"
#include "../src/file_def.h"

#undef VERSION
#define VERSION "Test program"

char *unused;

/*
  Compare 2 files bytes per bytes
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
  ti8x_display_file("./ti82/backup.82b");
  ti8x_read_backup_file("./ti82/backup.82b", &content);
  ti8x_write_backup_file("./ti82/backup.82b_", &content);
  compare_files("./ti82/backup.82b", "./ti82/backup.82b_");

  return 0;
}

static int test_ti82_regular_support()
{
  Ti8xRegular content;

  printf("--> Testing regular support (single)...\n");
  ti8x_display_file("./ti82/math.82p");
  ti8x_read_regular_file("./ti82/math.82p", &content);
  ti8x_write_regular_file("./ti82/math.82p_", &content, &unused);
  compare_files("./ti82/math.82p", "./ti82/math.82p_");

  printf("\n");

  printf("--> Testing regular support (group)...\n");
  ti8x_display_file("./ti82/group.82g");  
  ti8x_read_regular_file("./ti82/group.82g", &content);
  ti8x_write_regular_file("./ti82/group.82g_", &content, &unused);
  compare_files("./ti82/group.82g", "./ti82/group.82g_");

  return 0;
}

static int test_ti82_group_support()
{
  char *array[] = { "./ti82/aa.82n", "./ti82/bb.82n", NULL };
  
  printf("--> Testing grouping of files...\n");
  tifiles_group_files(array, "./ti82/aabb.82g_");
  tifiles_display_file("./ti82/aabb.82g_");
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

  printf("--> Testing regular support (group)...\n");
  ti8x_display_file("./ti83+/group.8xg");
  ti8x_read_regular_file("./ti83+/group.8xg", &content);
  ti8x_write_regular_file("./ti83+/group.8xg_", &content, &unused);
  compare_files("./ti83+/group.8xg", "./ti83+/group.8xg_");
  
  return 0;
  printf("\n");

  ti8x_display_file("./ti83+/group.8xg");
  ti8x_read_regular_file("./ti83+/group.8xg", &content);
  ti8x_write_regular_file("./ti83+/group.8xg_", &content, &unused);
  compare_files("./ti83+/group.8xg", "./ti83+/group.8xg_");

  return 0;
}

static int test_ti86_backup_support()
{
  Ti8xBackup content;

  printf("--> Testing backup support...\n");
  ti8x_display_file("./ti86/backup.86b");
  ti8x_read_backup_file("./ti86/backup.86b", &content);
  ti8x_write_backup_file("./ti86/backup.86b_", &content);
  compare_files("./ti86/backup.86b", "./ti86/backup.86b_");

  return 0;
}

static int test_ti86_regular_support()
{
  Ti8xRegular content;

  printf("--> Testing regular support (single)...\n");
  ti8x_display_file("./ti86/x.86n");
  ti8x_read_regular_file("./ti86/x.86n", &content);
  ti8x_write_regular_file("./ti86/x.86n_", &content, &unused);
  compare_files("./ti86/x.86n", "./ti86/x.86n_");

  printf("\n");

  printf("--> Testing regular support (group)...\n");
  ti8x_display_file("./ti86/group.86g");
  ti8x_read_regular_file("./ti86/group.86g", &content);
  ti8x_write_regular_file("./ti86/group.86g_", &content, &unused);
  compare_files("./ti86/group.86g", "./ti86/group.86g_");

  return 0;
}

static int test_ti89_regular_support()
{
  Ti9xRegular content;

  printf("--> Testing regular support (group)...\n");
  ti9x_display_file("./ti89/group.89g");
  ti9x_read_regular_file("./ti89/group.89g", &content);
  ti9x_write_regular_file("./ti89/group.89g_", &content, &unused);
  compare_files("./ti89/group.89g", "./ti89/group.89g_");

  return 0;
}

static int test_ti92_backup_support()
{
  Ti9xBackup content;

  printf("--> Testing backup support...\n");
  ti9x_display_file("./ti92/backup.92b");
  ti9x_read_backup_file("./ti92/backup.92b", &content);
  ti9x_write_backup_file("./ti92/backup.92b_", &content);
  compare_files("./ti92/backup.92b", "./ti92/backup.92b_");

  return 0;
}

static int test_ti92_regular_support()
{
  Ti9xRegular content;

  printf("--> Testing regular support (single)...\n");
  ti9x_display_file("./ti92/str.92s");
  ti9x_read_regular_file("./ti92/str.92s", &content);
  ti9x_write_regular_file("./ti92/str.92s_", &content, &unused);
  //compare_files("./ti92/str.92s", "./ti92/str.92s_");

  printf("\n");
  
  printf("--> --> Testing regular support (group)...\n");
  ti9x_display_file("./ti92/group.92g");  
  ti9x_read_regular_file("./ti92/group.92g", &content);
  ti9x_write_regular_file("./ti92/group.92g_", &content, &unused);
  //compare_files("./ti92/group.92g", "./ti92/group.92g_");

  return 0;
}

static int test_ti92_group_support()
{
  char *array[] = { "./ti92/xx.92s", "./ti92/yy.92s", "./ti92/zz.92s", NULL };
  
  printf("--> Testing grouping of files...\n");
  tifiles_group_files(array, "./ti92/xxyyzz.92g_");
  tifiles_display_file("./ti92/xxyyzz.92g_");
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
  ti9x_display_file("./ti89/ticabfra.89k");
  ti9x_read_flash_file("./ti89/ticabfra.89k", &content);
  ti9x_write_flash_file("./ti89/ticabfra.89k_", &content);
  compare_files("./ti89/ticabfra.89k", "./ti89/ticabfra.89k_");

  return 0;
}

static int test_ti83p_flash_support()
{
  Ti8xFlash content;

  printf("--> Testing flash support...\n");

  ti8x_display_file("./ti83+/base112.8Xu");
  //ti8x_read_flash_file("./ti83+/base112.8Xu", &content);
  //hexdump(content.pages[0].data, 256);
  //hexdump(content.pages[content.num_pages-1].data, 256);
  //ti8x_write_flash_file("./ti83+/base112.8Xu_", &content);
  //compare_files("./ti83+/base112.8Xu", "./ti83+/base112.8Xu_");

  return 0;

  ti8x_display_file("./ti83+/chembio.8Xk");
  ti8x_read_flash_file("./ti83+/chembio.8Xk", &content);
  ti8x_write_flash_file("./ti83+/chembio.8Xk_", &content);
  compare_files("./ti83+/chembio.8Xk", "./ti83+/chembio.8Xk_");

  return 0;
}

static int test_v200_regular_support()
{
  Ti9xRegular content;

  printf("--> Testing regular support (group)...\n");
  ti9x_display_file("./v200/xy.v2g");
  ti9x_read_regular_file("./v200/xy.v2g", &content);
  ti9x_write_regular_file("./v200/xy.v2g_", &content, &unused);
  compare_files("./v200/xy.v2g", "./v200/xy.v2g_");

  return 0;
}


/*
  The main function
*/
int main(int argc, char **argv)
{
  tifiles_init();

  goto ti83p;

  // TI82 support
 ti82:
  tifiles_set_calc(CALC_TI82);
  test_ti82_backup_support();
  test_ti82_regular_support();
  test_ti82_group_support();
  test_ti82_ungroup_support();
  goto end;

  // TI83+ support
 ti83p:
  tifiles_set_calc(CALC_TI83P);
  test_ti83p_regular_support();
  //test_ti83p_flash_support();  
  goto end;

 ti86:
  tifiles_set_calc(CALC_TI86);
  //test_ti86_backup_support();
  test_ti86_regular_support();
  goto end;

  // TI92 support
 ti92:
  tifiles_set_calc(CALC_TI92);
  test_ti92_backup_support();
  test_ti92_regular_support();
  test_ti92_group_support();
  test_ti92_ungroup_support();
  goto end;

 ti89:
  tifiles_set_calc(CALC_TI89);
  test_ti89_flash_support();
  // test_ti89_regular_support();
  goto end;

  // V200 support
 v200:
  tifiles_set_calc(CALC_V200);
  test_v200_regular_support();
  goto end;

 end:
  tifiles_exit();

  return 0;
}

