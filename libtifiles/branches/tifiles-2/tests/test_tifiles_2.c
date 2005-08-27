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
#ifdef __WIN32__
#include <direct.h>
#endif

#include "../src/tifiles.h"

/*
  Compare 2 files bytes per bytes and show differences
*/
static int compare_files(const char *src, const char *dst)
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
    
    while(!feof(fs) && !feof(fd))
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
    
    printf("    Files match !\n");
   
    return 0;
}

static int move_file(const char *oldpath, const char *newpath)
{
#if defined(__WIN32__) && !defined(__MINGW32__)
    return 0;
#else
    return rename(oldpath, newpath);
#endif
}

// Set output directory
static void change_dir(const char *path)
{
#if defined(__WIN32__) && !defined(__MINGW32__)
	_chdir(path);
#endif
}

// Build a portable path for Linux/Win32
static const char* BUILD_PATH(const char *path)
{
	static char str[1024];
	unsigned int i;

#if defined(__WIN32__) && !defined(__MINGW32__)
	strcpy(str, "C:\\sources\\roms\\tifiles2\\tests\\");
	strcat(str, path);

	for(i = 0; i < strlen(str); i++)
		if(str[i] == '/')
			str[i] = '\\';

	return str;
#else
	return path;
#endif
}

static int test_ti73_backup_support(void);
static int test_ti73_regular_support(void);
static int test_ti73_group_support(void);
static int test_ti73_ungroup_support(void);

static int test_ti82_backup_support(void);
static int test_ti82_regular_support(void);
static int test_ti82_group_support(void);
static int test_ti82_ungroup_support(void);

static int test_ti83_backup_support(void);
static int test_ti83_regular_support(void);
static int test_ti83_group_support(void);
static int test_ti83_ungroup_support(void);

static int test_ti84p_backup_support(void);
static int test_ti84p_regular_support(void);
static int test_ti84p_group_support(void);
static int test_ti84p_ungroup_support(void);	
static int test_ti84p_flash_support(void);  

static int test_ti85_regular_support(void);

static int test_ti86_backup_support(void);
static int test_ti86_regular_support(void);
static int test_ti86_group_support(void);
static int test_ti86_ungroup_support(void);

static int test_ti89_regular_support(void);
static int test_ti89_flash_support(void);

static int test_ti92_backup_support(void);
static int test_ti92_regular_support(void);
static int test_ti92_group_support(void);
static int test_ti92_ungroup_support(void);

static int test_ti8x_cert_support();
static int test_ti9x_cert_support();

/*
  The main function
*/
int main(int argc, char **argv)
{
	char *msg = NULL;
	char str[16];
	char buffer[256];
	int i;
	int ret;

	// init library
	tifiles_library_init();

	// test tifiles.c
	printf("Library version : <%s>\n", tifiles_version_get());
	printf("--\n");

	// test error.c
	tifiles_error_get(515, &msg);
	printf("Error message: <%s>\n", msg);
#ifndef __WIN32__
	free(msg);
#endif
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
	    printf("%s (%i) ", tifiles_fext_of_group(i), i);
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
	printf("--\n");

	printf("<%s> <%s>\n", "foo.bar", tifiles_fext_get("foo.bar"));

	ret = tifiles_file_is_ti(BUILD_PATH("misc/str.92s")); 
	printf("tifiles_file_is_ti: %i\n", ret);

	ret = tifiles_file_is_single(BUILD_PATH("misc/str.92s"));
	printf("tifiles_file_is_single: %i\n", ret);

	ret = tifiles_file_is_group(BUILD_PATH("misc/group.92g"));
        printf("tifiles_file_is_group: %i\n", ret);

	ret = tifiles_file_is_regular(BUILD_PATH("misc/str.92s"));
        printf("tifiles_file_is_regular: %i\n", ret);

	ret = tifiles_file_is_regular(BUILD_PATH("misc/group.92g"));
        printf("tifiles_file_is_regular: %i\n", ret);

	ret = tifiles_file_is_backup(BUILD_PATH("misc/backup.83b"));
	printf("tifiles_file_is_backup: %i\n", ret);

	ret = tifiles_file_is_flash(BUILD_PATH("misc/ticabfra.89k"));
        printf("tifiles_file_is_flash: %i\n", ret);

	ret = tifiles_file_is_flash(BUILD_PATH("misc/TI73_OS160.73U"));
        printf("tifiles_file_is_flash: %i\n", ret);

	ret = tifiles_file_is_tib(BUILD_PATH("misc/ams100.tib"));
        printf("tifiles_file_is_tib: %i\n", ret);
	printf("--\n");

	// test typesxx.c
	printf("tifiles_file_get_model: %s\n", 
         tifiles_model_to_string(tifiles_file_get_model(BUILD_PATH("misc/str.92s"))));

	printf("tifiles_file_get_class: %s\n",
         tifiles_class_to_string(tifiles_file_get_class(BUILD_PATH("misc/group.92g"))));

	printf("tifiles_file_get_type: %s\n",
	       tifiles_file_get_type(BUILD_PATH("misc/TI73_OS160.73U")));
	printf("tifiles_file_get_icon: %s\n",
	       tifiles_file_get_icon(BUILD_PATH("misc/str.92s")));
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

	// test filesxx.c & grouped.c

	// TI73 support
#if 1
	change_dir(BUILD_PATH("ti73"));
	test_ti73_backup_support();
	test_ti73_regular_support();
	test_ti73_group_support();
	test_ti73_ungroup_support();
#endif

	// TI82 support
#if 0
	change_dir(BUILD_PATH("ti82"));
	test_ti82_backup_support();
	test_ti82_regular_support();
	test_ti82_group_support();
	//test_ti82_ungroup_support();
#endif

	// TI83 support
#if 0
	change_dir(BUILD_PATH("ti83"));
	test_ti83_backup_support();
	test_ti83_regular_support();
	test_ti83_group_support();
	test_ti83_ungroup_support();
#endif

	// TI83+ support
#if 0
	change_dir(BUILD_PATH("ti84p"));
	//test_ti84p_backup_support();
	test_ti84p_regular_support();
	test_ti84p_group_support();
	test_ti84p_ungroup_support();	
	test_ti84p_flash_support();
#endif

	// TI85 support
#if 0
	change_dir(BUILD_PATH("ti85"));
	test_ti85_regular_support();
#endif

	// TI86 support
#if 0
	change_dir(BUILD_PATH("ti86"));
	test_ti86_backup_support();
	test_ti86_regular_support();
	test_ti86_group_support();
	test_ti86_ungroup_support();
#endif

	// TI89 support
#if 0
	change_dir(BUILD_PATH("ti89"));
	test_ti89_regular_support();
	test_ti89_flash_support();
#endif

	// TI92 support
#if 0
	change_dir(BUILD_PATH("ti92"));
	test_ti92_backup_support();
	test_ti92_regular_support();
	test_ti92_group_support();
	test_ti92_ungroup_support();
#endif

	// TIXX certificates
#if 0
	change_dir(BUILD_PATH("certs"));
	test_ti8x_cert_support();
	//test_ti9x_cert_support();
#endif

	// end of test
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
/* TI-73 */
/*********/

static int test_ti73_backup_support()
{
  BackupContent content;

  printf("--> Testing TI73 backup support...\n");
  tifiles_file_display(BUILD_PATH("ti73/backup.73b"));
  tifiles_file_read_backup(BUILD_PATH("ti73/backup.73b"), &content);
  tifiles_file_write_backup(BUILD_PATH("ti73/backup.73b_"), &content);
  tifiles_content_delete_backup(&content);
  compare_files(BUILD_PATH("ti73/backup.73b"), BUILD_PATH("ti73/backup.73b_"));

  return 0;
}

static int test_ti73_regular_support()
{
  FileContent content;
  char *unused;

  printf("--> Testing TI73 regular support (single)...\n");
  tifiles_file_display(BUILD_PATH("ti73/romdump.73p"));
  tifiles_file_read_regular(BUILD_PATH("ti73/romdump.73p"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti73/romdump.73p_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti73/romdump.73p"), BUILD_PATH("ti73/romdump.73p_"));

  printf("--> Testing TI73 regular support (group)...\n");
  tifiles_file_display(BUILD_PATH("ti73/group.73g"));  
  tifiles_file_read_regular(BUILD_PATH("ti73/group.73g"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti73/group.73g_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti73/group.73g"), BUILD_PATH("ti73/group.73g_"));

  return 0;
}

static int test_ti73_group_support()
{
  //char *array[] = { "ti73/L1.73l", "ti73/bb.73l", NULL };
  char files[2][1024];
  char *array[3] = { 0 };

  strcpy(files[0], BUILD_PATH("ti73/L1L1.73l"));
  strcpy(files[1], BUILD_PATH("ti73/L2L2.73l"));
  array[0] = files[0];
  array[1] = files[1];
  
  printf("--> Testing TI73 grouping of files...\n");
  tifiles_group_files(array, BUILD_PATH("ti73/L1L2.73g_"));
  tifiles_file_display(BUILD_PATH("ti73/L1L2.73g_"));
  compare_files(BUILD_PATH("ti73/group.73g"), BUILD_PATH("ti73/L1L2.73g_"));
  
  return 0;
}

static int test_ti73_ungroup_support()
{
  printf("--> Testing TI73 ungrouping of files...\n");
  tifiles_ungroup_file(BUILD_PATH("ti73/group.73g"), NULL);
  move_file("L1.73l", "ti73/L1.73l");
  move_file("L2.73l", "ti73/L2.73l");
  compare_files(BUILD_PATH("ti73/L1.73l"), BUILD_PATH("ti73/L1L1.73l"));
  compare_files(BUILD_PATH("ti73/L2.73l"), BUILD_PATH("ti73/L2L2.73l"));

  return 0;
}

/*********/
/* TI-82 */
/*********/

static int test_ti82_backup_support()
{
  BackupContent content;

  printf("--> Testing TI82 backup support...\n");
  tifiles_file_display(BUILD_PATH("ti82/backup.82b"));
  tifiles_file_read_backup(BUILD_PATH("ti82/backup.82b"), &content);
  tifiles_file_write_backup(BUILD_PATH("ti82/backup.82b_"), &content);
  tifiles_content_delete_backup(&content);
  compare_files(BUILD_PATH("ti82/backup.82b"), BUILD_PATH("ti82/backup.82b_"));

  return 0;
}

static int test_ti82_regular_support()
{
  FileContent content;
  char *unused;

  printf("--> Testing TI82 regular support (single)...\n");
  tifiles_file_display(BUILD_PATH("ti82/math.82p"));
  tifiles_file_read_regular(BUILD_PATH("ti82/math.82p"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti82/math.82p_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti82/math.82p"), BUILD_PATH("ti82/math.82p_"));

  printf("--> Testing TI82 regular support (group)...\n");
  tifiles_file_display(BUILD_PATH("ti82/group.82g"));  
  tifiles_file_read_regular(BUILD_PATH("ti82/group.82g"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti82/group.82g_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti82/group.82g"), BUILD_PATH("ti82/group.82g_"));

  return 0;
}

static int test_ti82_group_support()
{
  //char *array[] = { "ti82/aa.82n", "ti82/bb.82n", NULL };
  char files[2][1024];
  char *array[3] = { 0 };

  strcpy(files[0], BUILD_PATH("ti82/aa.82n"));
  strcpy(files[1], BUILD_PATH("ti82/bb.82n"));
  array[0] = files[0];
  array[1] = files[1];
  
  printf("--> Testing TI82 grouping of files...\n");
  tifiles_group_files(array, BUILD_PATH("ti82/aabb.82g_"));
  tifiles_file_display(BUILD_PATH("ti82/aabb.82g_"));
  compare_files(BUILD_PATH("ti82/group.82g"), BUILD_PATH("ti82/aabb.82g_"));
  
  return 0;
}

static int test_ti82_ungroup_support()
{
  printf("--> Testing TI82 ungrouping of files...\n");
  tifiles_ungroup_file(BUILD_PATH("ti82/group.82g"), NULL);
  move_file("A.82n", "ti82/A.82n");
  move_file("B.82n", "ti82/B.82n");
  compare_files(BUILD_PATH("ti82/A.82n"), BUILD_PATH("ti82/aa.82n"));
  compare_files(BUILD_PATH("ti82/B.82n"), BUILD_PATH("ti82/bb.82n"));

  return 0;
}

static int test_ti82_group_ungroup_support()
{
  /*
  FileContent src1;
  FileContent **dst1;
  FileContent **src2;
  FileContent dst2;
  FileContent **ptr;

  ti8x_read_regular_file("group.82g", &src1);
  ti8x_ungroup_content(&src1, &dst1);
  src2 = dst1;
  ti8x_group_content(src2, dst2);
  ti8x_write_regular_file("group2.82g_", dst2);
  */
  return 0;
}

/*********/
/* TI-83 */
/*********/

static int test_ti83_backup_support()
{
  BackupContent content;

  printf("--> Testing ti83 backup support...\n");
  tifiles_file_display(BUILD_PATH("ti83/backup.83b"));
  tifiles_file_read_backup(BUILD_PATH("ti83/backup.83b"), &content);
  tifiles_file_write_backup(BUILD_PATH("ti83/backup.83b_"), &content);
  tifiles_content_delete_backup(&content);
  compare_files(BUILD_PATH("ti83/backup.83b"), BUILD_PATH("ti83/backup.83b_"));

  return 0;
}

static int test_ti83_regular_support()
{
  FileContent content;
  char *unused;

  printf("--> Testing ti83 regular support (single)...\n");
  tifiles_file_display(BUILD_PATH("ti83/romdump.83p"));
  tifiles_file_read_regular(BUILD_PATH("ti83/romdump.83p"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti83/romdump.83p_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti83/romdump.83p"), BUILD_PATH("ti83/romdump.83p_"));

  printf("--> Testing ti83 regular support (group)...\n");
  tifiles_file_display(BUILD_PATH("ti83/group.83g"));  
  tifiles_file_read_regular(BUILD_PATH("ti83/group.83g"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti83/group.83g_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti83/group.83g"), BUILD_PATH("ti83/group.83g_"));

  return 0;
}

static int test_ti83_group_support()
{
  char files[2][1024];
  char *array[3] = { 0 };

  strcpy(files[0], BUILD_PATH("ti83/aa.83n"));
  strcpy(files[1], BUILD_PATH("ti83/bb.83n"));
  array[0] = files[0];
  array[1] = files[1];
  
  printf("--> Testing ti83 grouping of files...\n");
  tifiles_group_files(array, BUILD_PATH("ti83/aabb.83g_"));
  tifiles_file_display(BUILD_PATH("ti83/aabb.83g_"));
  compare_files(BUILD_PATH("ti83/group.83g"), BUILD_PATH("ti83/aabb.83g_"));
  
  return 0;
}

static int test_ti83_ungroup_support()
{
  printf("--> Testing ti83 ungrouping of files...\n");
  tifiles_ungroup_file(BUILD_PATH("ti83/group.83g"), NULL);
  move_file("A.83n", "ti83/A.83n");
  move_file("B.83n", "ti83/B.83n");
  compare_files(BUILD_PATH("ti83/A.83n"), BUILD_PATH("ti83/aa.83n"));
  compare_files(BUILD_PATH("ti83/B.83n"), BUILD_PATH("ti83/bb.83n"));

  return 0;
}

static int test_ti83_group_ungroup_support()
{
  /*
  FileContent src1;
  FileContent **dst1;
  FileContent **src2;
  FileContent dst2;
  FileContent **ptr;

  ti8x_read_regular_file("group.83g", &src1);
  ti8x_ungroup_content(&src1, &dst1);
  src2 = dst1;
  ti8x_group_content(src2, dst2);
  ti8x_write_regular_file("group2.83g_", dst2);
  */
  return 0;
}

/**********/
/* TI-84+ */
/**********/

static int test_ti84p_regular_support()
{
  FileContent content;
  char *unused;

  printf("--> Testing TI84+ regular support (single)...\n");
  tifiles_file_display(BUILD_PATH("ti84p/romdump.8Xp"));
  tifiles_file_read_regular(BUILD_PATH("ti84p/romdump.8Xp"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti84p/romdump.8Xp_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti84p/romdump.8Xp"), BUILD_PATH("ti84p/romdump.8Xp_"));

  printf("--> Testing TI84+ regular support (group)...\n");
  tifiles_file_display(BUILD_PATH("ti84p/group.8Xg"));  
  tifiles_file_read_regular(BUILD_PATH("ti84p/group.8Xg"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti84p/group.8Xg_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti84p/group.8Xg"), BUILD_PATH("ti84p/group.8Xg_"));

  return 0;
}

static int test_ti84p_group_support()
{
  //char *array[] = { "ti84p/aa.8Xn", "ti84p/bb.8Xn", NULL };
  char files[2][1024];
  char *array[3] = { 0 };

  strcpy(files[0], BUILD_PATH("ti84p/aa.8Xn"));
  strcpy(files[1], BUILD_PATH("ti84p/bb.8Xn"));
  array[0] = files[0];
  array[1] = files[1];
  
  printf("--> Testing TI84+ grouping of files...\n");
  tifiles_group_files(array, BUILD_PATH("ti84p/aabb.8Xg_"));
  tifiles_file_display(BUILD_PATH("ti84p/aabb.8Xg_"));
  compare_files(BUILD_PATH("ti84p/group.8Xg"), BUILD_PATH("ti84p/aabb.8Xg_"));
  
  return 0;
}

static int test_ti84p_ungroup_support()
{
  printf("--> Testing TI84+ ungrouping of files...\n");
  tifiles_ungroup_file(BUILD_PATH("ti84p/group.8Xg"), NULL);
  move_file("A.8Xn", "ti84p/A.8Xn");
  move_file("B.8Xn", "ti84p/B.8Xn");
  compare_files(BUILD_PATH("ti84p/A.8Xn"), BUILD_PATH("ti84p/aa.8Xn"));
  compare_files(BUILD_PATH("ti84p/B.8Xn"), BUILD_PATH("ti84p/bb.8Xn"));

  return 0;
}

/*
static int special_test(FlashContent *content)
{
	const char *filename = "C:\\sources\\roms\\tifiles-2\\tests\\ti84p\\out.txt";
	FILE *f;
	int i, j, k;

	f = fopen(filename, "wt");
	if(f == NULL)
		return -1;

	for(i = 0; i < content->num_pages; i++)
	{
		fprintf(f, "%02x %04X\n", content->pages[i].flag, content->pages[i].page);
		for(j = 0; j < content->pages[i].size; j += 32)
		{
			fprintf(f, "%04X: ", content->pages[i].addr + j);
			for(k = 0; k < 32; k++)
				fprintf(f, "%02X", content->pages[i].data[j+k]);
			fprintf(f, "\n");
		}
	}

	fclose(f);
}
*/

static int test_ti84p_flash_support()
{
  FlashContent content;

  printf("--> Testing TI84+ flash support...\n");

  tifiles_file_display(BUILD_PATH("ti84p/chembio.8Xk"));
  tifiles_file_read_flash(BUILD_PATH("ti84p/chembio.8Xk"), &content);
  tifiles_file_write_flash(BUILD_PATH("ti84p/chembio.8Xk_"), &content);
  tifiles_content_delete_flash(&content);
  compare_files(BUILD_PATH("ti84p/chembio.8Xk"), BUILD_PATH("ti84p/chembio.8Xk_"));

  return 0;

  tifiles_file_display(BUILD_PATH("ti84p/TI84Plus_OS.8Xu"));
  tifiles_file_read_flash(BUILD_PATH("ti84p/TI84Plus_OS.8Xu"), &content);
  tifiles_file_write_flash(BUILD_PATH("ti84p/TI84Plus_OS.8Xu_"), &content);
  tifiles_content_delete_flash(&content);
  compare_files(BUILD_PATH("ti84p/TI84Plus_OS.8Xu"), BUILD_PATH("ti84p/TI84Plus_OS.8Xu_"));

  return 0;
}

/*********/
/* TI-85 */
/*********/

static int test_ti85_regular_support()
{
  FileContent content;
  char *unused;

  printf("--> Testing TI85 regular support (single)...\n");
  tifiles_file_display(BUILD_PATH("ti85/AA.85n"));
  tifiles_file_read_regular(BUILD_PATH("ti85/AA.85n"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti85/AA.85n_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti85/AA.85n"), BUILD_PATH("ti85/AA.85n_"));

  printf("--> Testing TI85 regular support (group)...\n");
  tifiles_file_display(BUILD_PATH("ti85/group.85g"));  
  tifiles_file_read_regular(BUILD_PATH("ti85/group.85g"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti85/group.85g_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti85/group.85g"), BUILD_PATH("ti85/group.85g_"));

  return 0;
}

/*********/
/* TI-86 */
/*********/

static int test_ti86_backup_support()
{
  BackupContent content;

  printf("--> Testing TI86 backup support...\n");
  tifiles_file_display(BUILD_PATH("ti86/backup.86b"));
  tifiles_file_read_backup(BUILD_PATH("ti86/backup.86b"), &content);
  tifiles_file_write_backup(BUILD_PATH("ti86/backup.86b_"), &content);
  tifiles_content_delete_backup(&content);
  compare_files(BUILD_PATH("ti86/backup.86b"), BUILD_PATH("ti86/backup.86b_"));

  return 0;
}

static int test_ti86_regular_support()
{
  FileContent content;
  char *unused;

  printf("--> Testing TI86 regular support (single)...\n");
  tifiles_file_display(BUILD_PATH("ti86/prgm.86p"));
  tifiles_file_read_regular(BUILD_PATH("ti86/prgm.86p"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti86/prgm.86p_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti86/prgm.86p"), BUILD_PATH("ti86/prgm.86p_"));

  printf("--> Testing TI86 regular support (group)...\n");
  tifiles_file_display(BUILD_PATH("ti86/group.86g"));  
  tifiles_file_read_regular(BUILD_PATH("ti86/group.86g"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti86/group.86g_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti86/group.86g"), BUILD_PATH("ti86/group.86g_"));

  return 0;
}

static int test_ti86_regular_support_()
{
  FileContent content;
  char *unused;

  printf("--> Testing TI86 regular support (single)...\n");
  tifiles_file_display(BUILD_PATH("ti86/ellipse.86p"));
  tifiles_file_read_regular(BUILD_PATH("ti86/ellipse.86p"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti86/ellipse.86p_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti86/ellipse.86p"), BUILD_PATH("ti86/ellipse.86p_"));

  return 0;
}

static int test_ti86_group_support()
{
  //char *array[] = { "ti86/yy.86n", "ti86/xx.86n", NULL };
  char files[2][1024];
  char *array[3] = { 0 };

  strcpy(files[0], BUILD_PATH("ti86/yy.86n"));
  strcpy(files[1], BUILD_PATH("ti86/xx.86n"));  
  array[0] = files[0];
  array[1] = files[1];
  
  printf("--> Testing TI86 grouping of files...\n");
  tifiles_group_files(array, BUILD_PATH("ti86/xxyy.86g_"));
  tifiles_file_display(BUILD_PATH("ti86/xxyy.86g_"));
  compare_files(BUILD_PATH("ti86/group.86g"), BUILD_PATH("ti86/xxyy.86g_"));
  
  return 0;
}

static int test_ti86_ungroup_support()
{
  printf("--> Testing TI86 ungrouping of files...\n");
  tifiles_ungroup_file(BUILD_PATH("ti86/group.86g"), NULL);
  move_file("X.86n", "ti86/X.86n");
  move_file("Y.86n", "ti86/Y.86n");
  compare_files(BUILD_PATH("ti86/X.86n"), BUILD_PATH("ti86/xx.86n"));
  compare_files(BUILD_PATH("ti86/Y.86n"), BUILD_PATH("ti86/yy.86n"));

  return 0;
}

/*********/
/* TI-92 */
/*********/

static int test_ti92_backup_support()
{
  BackupContent content;

  printf("--> Testing TI92 backup support...\n");
  tifiles_file_display(BUILD_PATH("ti92/backup.92b"));
  tifiles_file_read_backup(BUILD_PATH("ti92/backup.92b"), &content);
  tifiles_file_write_backup(BUILD_PATH("ti92/backup.92b_"), &content);
  tifiles_content_delete_backup(&content);
  compare_files(BUILD_PATH("ti92/backup.92b"), BUILD_PATH("ti92/backup.92b_"));

  return 0;
}

static int test_ti92_regular_support()
{
  FileContent content;
  char *unused;

  printf("--> Testing TI92 regular support (single)...\n");
  tifiles_file_display(BUILD_PATH("ti92/str.92s"));
  tifiles_file_read_regular(BUILD_PATH("ti92/str.92s"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti92/str.92s_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti92/str.92s"), BUILD_PATH("ti92/str.92s_"));


  printf("--> --> Testing TI92 regular support (group)...\n");
  tifiles_file_display(BUILD_PATH("ti92/group.92g"));  
  tifiles_file_read_regular(BUILD_PATH("ti92/group.92g"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti92/group.92g_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti92/group.92g"), BUILD_PATH("ti92/group.92g_"));

  return 0;
}

static int test_ti92_group_support()
{
  //char *array[] = { "ti92/xx.92s", "ti92/yy.92s", NULL };
  char files[2][1024];
  char *array[3] = { 0 };

  strcpy(files[0], BUILD_PATH("ti92/xx.92s"));
  strcpy(files[1], BUILD_PATH("ti92/yy.92s"));
  array[0] = files[0];
  array[1] = files[1];
  
  printf("--> Testing TI92 grouping of files...\n");
  tifiles_group_files(array, BUILD_PATH("ti92/xxyy.92g_"));
  tifiles_file_display(BUILD_PATH("ti92/xxyy.92g_"));
  compare_files(BUILD_PATH("ti92/group.92g"), BUILD_PATH("ti92/xxyy.92g_"));
  
  return 0;
}

static int test_ti92_ungroup_support()
{
  printf("--> Testing TI92 ungrouping of files...\n");
  tifiles_ungroup_file(BUILD_PATH("ti92/group.92g"), NULL);
  move_file("X.92s", "ti92/X.92s");
  move_file("Y.92s", "ti92/Y.92s");
  compare_files(BUILD_PATH("ti92/X.92s"), BUILD_PATH("ti92/xx.92s"));
  compare_files(BUILD_PATH("ti92/Y.92s"), BUILD_PATH("ti92/yy.92s"));

  return 0;
}

/*********/
/* TI-89 */
/*********/

static int test_ti89_regular_support()
{
  FileContent content;
  char *unused;

  printf("--> Testing TI89 regular support (group)...\n");
  tifiles_file_display(BUILD_PATH("ti89/group.89g"));
  tifiles_file_read_regular(BUILD_PATH("ti89/group.89g"), &content);
  tifiles_file_write_regular(BUILD_PATH("ti89/group.89g_"), &content, &unused);
  tifiles_content_delete_regular(&content);
  compare_files(BUILD_PATH("ti89/group.89g"), BUILD_PATH("ti89/group.89g_"));

  return 0;
}

static int test_ti89_flash_support()
{
  FlashContent content;

  printf("--> Testing TI89 flash support...\n");
  tifiles_file_display(BUILD_PATH("ti89/ticabfra.89k"));
  tifiles_file_read_flash(BUILD_PATH("ti89/ticabfra.89k"), &content);
  tifiles_file_write_flash(BUILD_PATH("ti89/ticabfra.89k_"), &content);
  tifiles_content_delete_flash(&content);
  compare_files(BUILD_PATH("ti89/ticabfra.89k"), BUILD_PATH("ti89/ticabfra.89k_"));

  return 0;
}


static int test_v200_regular_support()
{
  FileContent content;
  char *unused;

  printf("--> Testing regular support (group)...\n");
  tifiles_file_display("./v200/xy.v2g");
  tifiles_file_read_regular("./v200/xy.v2g", &content);
  tifiles_file_write_regular("./v200/xy.v2g_", &content, &unused);
  compare_files("./v200/xy.v2g", "./v200/xy.v2g_");

  return 0;
}

/*********/
/* TI-XX */
/*********/

static int test_ti8x_cert_support()
{
  FlashContent content;

  printf("--> Testing TI8X certif support...\n");

  tifiles_file_display(BUILD_PATH("certs/celsheet1.8Xk"));
  tifiles_file_read_flash(BUILD_PATH("certs/celsheet1.8Xk"), &content);
  tifiles_file_write_flash(BUILD_PATH("certs/celsheet1.8Xk_"), &content);
  tifiles_content_delete_flash(&content);
  compare_files(BUILD_PATH("certs/celsheet1.8Xk"), BUILD_PATH("certs/celsheet1.8Xk_"));

  return 0;
}

static int test_ti9x_cert_support()
{
	FlashContent content;

	printf("--> Testing TI9X certif support...\n");

  tifiles_file_display(BUILD_PATH("certs/ticsheet.9xk"));
  tifiles_file_read_flash(BUILD_PATH("certs/ticsheet.9xk"), &content);
  tifiles_file_write_flash(BUILD_PATH("certs/ticsheet.9xk_"), &content);
  tifiles_content_delete_flash(&content);
  compare_files(BUILD_PATH("certs/ticsheet.9xk"), BUILD_PATH("certs/ticsheet.9xk_"));

  return 0;
}