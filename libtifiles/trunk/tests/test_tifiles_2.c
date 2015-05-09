/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  libtifiles - file format library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
  This program contains a lot of routines for testing various part of the
  library as well as checking file support.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __WIN32__
#include <direct.h>
#endif

#include "../src/tifiles.h"

// Compare 2 files bytes per bytes and show differences
static int compare_files(const char *src, const char *dst)
{
	FILE *fs, *fd;
	int i;
	int s, d;

	fs = fopen(src, "rb");
	if (fs == NULL)
	{
		printf("Unable to open file <%s>\n", src);
		return -1;
	}

	fd = fopen(dst, "rb");
	if (fd == NULL)
	{
		printf("Unable to open file: <%s>\n", dst);
		fclose(fs);
		return -1;
	}

	while (!feof(fs) && !feof(fd))
	{
		if ((s = fgetc(fs)) != (d = fgetc(fd)))
		{
			printf("\nFiles %s and %s do not match !!!\n", src, dst);
			printf("Offset: %08X %i\n", (int)ftell(fs), (int)ftell(fs));

			printf("Data  (src): %02X ", s);
			for (i=0; i<16 && !feof(fs); i++)
			{
				printf("%02X ", fgetc(fs));
			}
			printf("\n");

			printf("Data  (dst): %02X ", d);
			for(i=0; i<16 && !feof(fd); i++)
			{
				printf("%02X ", fgetc(fd));
			}
			printf("\n");

			fclose(fd);
			fclose(fs);
			return -1;
		}
	}

	printf("    Files match !\n");

	fclose(fd);
	fclose(fs);
	return 0;
}

// Rename file
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
static const char* PATH(const char *path)
{
#if defined(__WIN32__) && !defined(__MINGW32__)
	static char str[1024];
	unsigned int i;

	strcpy(str, "C:\\sources\\roms\\tifiles2\\tests\\");
	strcat(str, path);

	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] == '/')
		{
			str[i] = '\\';
		}
	}

	return str;
#else
	return path;
#endif
}

// Build a portable path for Linux/Win32
static const char* PATH2(const char *path)
{
#if defined(__WIN32__) && !defined(__MINGW32__)
	static char str[1024];
	unsigned int i;

	strcpy(str, "C:\\sources\\roms\\tifiles2\\tests\\");
	strcat(str, path);

	for(i = 0; i < strlen(str); i++)
	{
		if(str[i] == '/')
		{
			str[i] = '\\';
		}

	return str;
#else
	return path;
#endif
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

/****************************/
/* Generic helper functions */
/****************************/
static int test_tixx_backup_support(const char * message,
                                    CalcModel calculator,
                                    const char * input_file,
                                    const char * output_file)
{
	BackupContent *content;
	int ret = -1;

	printf("%s", message);
	tifiles_file_display(PATH(input_file));

	content = tifiles_content_create_backup(calculator);
	if (content != NULL)
	{
		ret = tifiles_file_read_backup(PATH(input_file), content);
		if (!ret)
		{
			ret = tifiles_file_write_backup(PATH(output_file), content);
			if (!ret)
			{
				ret = compare_files(PATH(input_file), PATH2(output_file));
			}
			tifiles_content_delete_backup(content);
		}
	}

	return ret;
}

static int test_tixx_regular_support_single(const char * message,
                                            CalcModel calculator,
                                            const char * input_file,
                                            const char * output_file)
{
	FileContent *content;
	char *unused = NULL;
	int ret = -1;

	printf("%s", message);
	tifiles_file_display(PATH(input_file));

	content = tifiles_content_create_regular(calculator);
	if (content != NULL)
	{
		ret = tifiles_file_read_regular(PATH(input_file), content);
		if (!ret)
		{
			ret = tifiles_file_write_regular(PATH(output_file), content, &unused);
			if (!ret)
			{
				ret = compare_files(PATH(input_file), PATH2(output_file));
			}
			tifiles_filename_free(unused);
			tifiles_content_delete_regular(content);
		}
	}

	return ret;
}

static int test_tixx_regular_support_group(const char * message,
                                            CalcModel calculator,
                                            const char * input_group,
                                            const char * output_group)
{
	FileContent *content;
	char *unused = NULL;
	int ret = -1;

	printf("%s", message);
	tifiles_file_display(PATH(input_group));

	content = tifiles_content_create_regular(calculator);
	if (content != NULL)
	{
		ret = tifiles_file_read_regular(PATH(input_group), content);
		if (!ret)
		{
			ret = tifiles_file_write_regular(PATH(output_group), content, &unused);
			if (!ret)
			{
				compare_files(PATH(input_group), PATH2(output_group));
			}
			tifiles_filename_free(unused);
			tifiles_content_delete_regular(content);
		}
	}

	return ret;
}

static int test_tixx_group_support(const char * message,
                                   const char * input_file_1,
                                   const char * input_file_2,
                                   const char * input_group_file,
                                   const char * output_group_file)
{
	char files[2][1024];
	char *array[3] = { 0 };
	int ret = -1;

	strncpy(files[0], PATH(input_file_1), 1023);
	files[0][1023] = 0;
	strncpy(files[1], PATH(input_file_2), 1023);
	files[1][1023] = 0;
	array[0] = files[0];
	array[1] = files[1];

	printf("%s", message);
	ret = tifiles_group_files(array, PATH(output_group_file));
	if (!ret)
	{
		tifiles_file_display(PATH(output_group_file));
		ret = compare_files(PATH(input_group_file), PATH2(output_group_file));
	}

	return ret;
}

static int test_tixx_ungroup_support(const char * message,
                                     const char * input_group,
                                     const char * input_file_1,
                                     const char * dest_file_1,
                                     const char * input_file_2,
                                     const char * dest_file_2,
                                     const char * input_file_3,
                                     const char * input_file_4)
{
	int ret;
	printf("%s", message);
	ret = tifiles_ungroup_file(PATH(input_group), NULL);
	if (!ret)
	{
		move_file(input_file_1, dest_file_1);
		move_file(input_file_2, dest_file_2);
		ret = compare_files(PATH(dest_file_1), PATH2(input_file_3));
		if (!ret)
		{
			ret = compare_files(PATH(dest_file_2), PATH2(input_file_3));
		}
	}

	return ret;
}

static int test_tixx_flash_support(const char * message,
                                   CalcModel calculator,
                                   const char * input_file,
                                   const char * output_file)
{
	FlashContent *content;
	int ret = -1;

	printf("%s", message);
	tifiles_file_display(PATH(input_file));

	content = tifiles_content_create_flash(calculator);
	if (content != NULL)
	{
		ret = tifiles_file_read_flash(PATH(input_file), content);
		if (!ret)
		{
			ret = tifiles_file_write_flash(PATH(output_file), content);
			if (!ret)
			{
				ret = compare_files(PATH(input_file), PATH2(output_file));
			}
			tifiles_content_delete_flash(content);
		}
	}

	return ret;
}

/*********/
/* TI-73 */
/*********/

static int test_ti73_backup_support()
{
	return test_tixx_backup_support("--> Testing TI73 backup support...\n",
	                                CALC_TI73,
	                                "ti73/backup.73b",
	                                "ti73/backup.73b_");
}

static int test_ti73_regular_support()
{
	int ret;

	ret = test_tixx_regular_support_single("--> Testing TI73 regular support (single)...\n",
	                                       CALC_TI73,
	                                       "ti73/romdump.73p",
	                                       "ti73/romdump.73p_");

	if (!ret)
	{
		ret = test_tixx_regular_support_group("--> Testing TI73 regular support (group)...\n",
		                                      CALC_TI73,
		                                      "ti73/group.73g",
		                                      "ti73/group.73g_");
	}

	return ret;
}

static int test_ti73_group_support()
{
	return test_tixx_group_support("--> Testing TI73 grouping of files...\n",
	                               "ti73/L1L1.73l",
	                               "ti73/L2L2.73l",
	                               "ti73/group.73g",
	                               "ti73/L1L2.73g_");
}

static int test_ti73_ungroup_support()
{
	return test_tixx_ungroup_support("--> Testing TI73 ungrouping of files...\n",
	                                 "ti73/group.73g",
	                                 "L1.73l",
	                                 "ti73/L1.73l",
	                                 "L2.73l",
	                                 "ti73/L2.73l",
	                                 "ti73/L1L1.73l",
	                                 "ti73/L2L2.73l");
}


/*********/
/* TI-82 */
/*********/

static int test_ti82_backup_support()
{
	return test_tixx_backup_support("--> Testing TI82 backup support...\n",
	                                CALC_TI82,
	                                "ti82/backup.82b",
	                                "ti82/backup.82b_");
}

static int test_ti82_regular_support()
{
	int ret;

	ret = test_tixx_regular_support_single("--> Testing TI82 regular support (single)...\n",
	                                       CALC_TI82,
	                                       "ti82/math.82p",
	                                       "ti82/math.82p_");

	if (!ret)
	{
		ret = test_tixx_regular_support_group("--> Testing TI82 regular support (group)...\n",
		                                      CALC_TI82,
		                                      "ti82/group.82g",
		                                      "ti82/group.82g_");
	}

	return ret;
}

static int test_ti82_group_support()
{
	return test_tixx_group_support("--> Testing TI82 grouping of files...\n",
	                               "ti82/aa.82n",
	                               "ti82/bb.82n",
	                               "ti82/group.82g",
	                               "ti82/aabb.82g_");
}

static int test_ti82_ungroup_support()
{
	return test_tixx_ungroup_support("--> Testing TI82 ungrouping of files...\n",
	                                 "ti82/group.82g",
	                                 "A.82n",
	                                 "ti82/A.82n",
	                                 "B.82n",
	                                 "ti82/B.82n",
	                                 "ti82/aa.82n",
	                                 "ti82/bb.82n");
}


/*********/
/* TI-83 */
/*********/

static int test_ti83_backup_support()
{
	return test_tixx_backup_support("--> Testing TI83 backup support...\n",
	                                CALC_TI83,
	                                "ti83/backup.83b",
	                                "ti83/backup.83b_");
}

static int test_ti83_regular_support()
{
	int ret;

	ret = test_tixx_regular_support_single("--> Testing ti83 regular support (single)...\n",
	                                       CALC_TI83,
	                                       "ti83/romdump.83p",
	                                       "ti83/romdump.83p_");

	if (!ret)
	{
		ret = test_tixx_regular_support_group("--> Testing ti83 regular support (group)...\n",
		                                      CALC_TI83,
		                                      "ti83/group.83g",
		                                      "ti83/group.83g_");
	}

	return ret;
}

static int test_ti83_group_support()
{
	return test_tixx_group_support("--> Testing TI82 grouping of files...\n",
	                               "ti83/aa.83n",
	                               "ti83/bb.83n",
	                               "ti83/group.83g",
	                               "ti83/aabb.83g_");
}

static int test_ti83_ungroup_support()
{
	return test_tixx_ungroup_support("--> Testing TI83 ungrouping of files...\n",
	                                 "ti83/group.83g",
	                                 "A.83n",
	                                 "ti83/A.83n",
	                                 "B.83n",
	                                 "ti83/B.83n",
	                                 "ti83/aa.83n",
	                                 "ti83/bb.83n");
}


/**********/
/* TI-84+ */
/**********/

static int test_ti84p_regular_support()
{
	int ret;

	ret = test_tixx_regular_support_single("--> Testing TI84+ regular support (single)...\n",
	                                       CALC_TI84P,
	                                       "ti84p/romdump.8Xp",
	                                       "ti84p/romdump.8Xp_");

	if (!ret)
	{
		ret = test_tixx_regular_support_group("--> Testing TI84+ regular support (group)...\n",
		                                      CALC_TI84P,
		                                      "ti84p/group.8Xg",
		                                      "ti84p/group.8Xg_");
	}

	return ret;
}

static int test_ti84p_group_support()
{
	return test_tixx_group_support("--> Testing TI84+ grouping of files...\n",
	                               "ti84p/aa.8Xn",
	                               "ti84p/bb.8Xn",
	                               "ti84p/group.8Xg",
	                               "ti84p/aabb.8Xg_");
}

static int test_ti84p_ungroup_support()
{
	return test_tixx_ungroup_support("--> Testing TI84+ ungrouping of files...\n",
	                                 "ti84p/group.8Xg",
	                                 "A.8Xn",
	                                 "ti84p/A.8Xn",
	                                 "B.8Xn",
	                                 "ti84p/B.8Xn",
	                                 "ti84p/aa.8Xn",
	                                 "ti84p/bb.8Xn");
}

static int test_ti84p_flash_support()
{
	int ret;

	ret = test_tixx_flash_support("--> Testing TI84+ flashapp support...\n",
	                              CALC_TI84P,
	                              "ti84p/LogIn_1.8Xk",
	                              "ti84p/LogIn_1.8Xk_");

	if (!ret)
	{
		ret = test_tixx_flash_support("--> Testing TI84+ flashapp support...\n",
		                              CALC_TI84P,
		                              "ti84p/chembio.8Xk",
		                              "ti84p/chembio.8Xk_");

		if (!ret)
		{
			ret = test_tixx_flash_support("--> Testing TI84+ flash OS support...\n",
			                              CALC_TI84P,
			                              "ti84p/TI84Plus_OS.8Xu",
			                              "ti84p/TI84Plus_OS.8Xu_");
		}
	}

	return ret;
}


/*********/
/* TI-85 */
/*********/

static int test_ti85_regular_support()
{
	int ret;

	ret = test_tixx_regular_support_single("--> Testing TI85 regular support (single)...\n",
	                                       CALC_TI85,
	                                       "ti85/AA.85n",
	                                       "ti85/AA.85n_");

	if (!ret)
	{
		ret = test_tixx_regular_support_group("--> Testing TI85 regular support (group)...\n",
		                                      CALC_TI85,
		                                      "ti85/group.85g",
		                                      "ti85/group.85g_");
	}

	return ret;
}


/*********/
/* TI-86 */
/*********/

static int test_ti86_backup_support()
{
	return test_tixx_backup_support("--> Testing TI86 backup support...\n",
	                                CALC_TI86,
	                                "ti86/backup.86b",
	                                "ti86/backup.86b_");
}

static int test_ti86_regular_support()
{
	int ret;

	ret = test_tixx_regular_support_single("--> Testing TI86 regular support (single)...\n",
	                                       CALC_TI86,
	                                       "ti86/prgm.86p",
	                                       "ti86/prgm.86p_");

	if (!ret)
	{
		ret = test_tixx_regular_support_single("--> Testing TI86 regular support (single)...\n",
		                                       CALC_TI86,
		                                       "ti86/ellipse.86p",
		                                       "ti86/ellipse.86p_");

		if (!ret)
		{
			ret = test_tixx_regular_support_group("--> Testing TI86 regular support (group)...\n",
			                                      CALC_TI86,
			                                      "ti86/group.86g",
			                                      "ti86/group.86g_");
		}
	}

	return ret;
}

static int test_ti86_group_support()
{
	return test_tixx_group_support("--> Testing TI86 grouping of files...\n",
	                               "ti86/yy.86n",
	                               "ti86/xx.86n",
	                               "ti86/group.86g",
	                               "ti86/xxyy.86g_");
}

static int test_ti86_ungroup_support()
{
	return test_tixx_ungroup_support("--> Testing TI86 ungrouping of files...\n",
	                                 "ti86/group.86g",
	                                 "X.86n",
	                                 "ti86/X.86n",
	                                 "Y.86n",
	                                 "ti86/Y.86n",
	                                 "ti86/xx.86n",
	                                 "ti86/xx.86n");
}

/*********/
/* TI-92 */
/*********/

static int test_ti92_backup_support()
{
	return test_tixx_backup_support("--> Testing TI92 backup support...\n",
	                                CALC_TI92,
	                                "ti92/backup.92b",
	                                "ti92/backup.92b_");
}

static int test_ti92_regular_support()
{
	int ret;

	ret = test_tixx_regular_support_single("--> Testing TI92 regular support (single)...\n",
	                                       CALC_TI92,
	                                       "ti92/str.92s",
	                                       "ti92/str.92s_");

	if (!ret)
	{
		ret = test_tixx_regular_support_group("--> Testing TI92 regular support (group)...\n",
		                                      CALC_TI92,
		                                      "ti92/group.92g",
		                                      "ti92/group.92g_");
	}

	return ret;
}

static int test_ti92_group_support()
{
	return test_tixx_group_support("--> Testing TI92 grouping of files...\n",
	                               "ti92/xx.92s",
	                               "ti92/yy.92s",
	                               "ti92/group.92g",
	                               "ti92/xxyy.92g_");
}

static int test_ti92_ungroup_support()
{
	return test_tixx_ungroup_support("--> Testing TI92 ungrouping of files...\n",
	                                 "ti92/group.92g",
	                                 "X.92s",
	                                 "ti92/X.92s",
	                                 "Y.92s",
	                                 "ti92/Y.92s",
	                                 "ti92/xx.92s",
	                                 "ti92/yy.92s");
}

/*********/
/* TI-89 */
/*********/

static int test_ti89_regular_support()
{
	int ret;

	ret = test_tixx_regular_support_group("--> Testing TI89 regular support (group)...\n",
	                                      CALC_TI89,
	                                      "ti89/group.89g",
	                                      "ti89/group.89g_");

	return ret;
}

static int test_ti89_flash_support()
{
	int ret;

	ret = test_tixx_flash_support("--> Testing TI89 flashapp support...\n",
	                              CALC_TI89,
	                              "ti89/ticabfra.89k",
	                              "ti89/ticabfra.89k_");

	return ret;
}


static int test_v200_regular_support()
{
	int ret;

	ret = test_tixx_regular_support_group("--> Testing V200 regular support (group)...\n",
	                                      CALC_V200,
	                                      "./v200/xy.v2g",
	                                      "./v200/xy.v2g_");

	return ret;
}

/*********/
/* TI-XX */
/*********/

static int test_ti8x_cert_support()
{
	int ret;

	ret = test_tixx_flash_support("--> Testing TI8X certif support...\n",
	                              CALC_TI84P,
	                              "certs/celsheet1.8Xk",
	                              "certs/celsheet1.8Xk_");

	return ret;
}

static int test_ti9x_cert_support()
{
	int ret;

	ret = test_tixx_flash_support("--> Testing TI9X certif support...\n",
	                              CALC_TI92P,
	                              "certs/ticsheet.9xk",
	                              "certs/ticsheet.9xk_");

	return ret;
}

static int test_ti8x_group_merge()
{
	VarEntry ve;
	int ret;

	printf("--> Testing add/del from group support (r/w)...\n");
	ret = tifiles_group_add_file(PATH("misc/group1.8Xg"), PATH2("misc/group2.8Xg"));
	if (!ret)
	{
		strncpy(ve.name, "A", sizeof(ve.name) - 1);
		ve.name[sizeof(ve.name) - 1] = 0;
		ret = tifiles_group_del_file(&ve, PATH("misc/group2.8Xg"));
		if (!ret)
		{
			strncpy(ve.name, "B", sizeof(ve.name) - 1);
			ve.name[sizeof(ve.name) - 1] = 0;
			ret = tifiles_group_del_file(&ve, PATH("misc/group2.8Xg"));
			if (!ret)
			{
				ret = compare_files(PATH("misc/group1.8Xg"), PATH2("misc/group2.8Xg"));
			}
		}
	}

	return ret;
}

static int test_tigroup()
{
	TigContent *content = NULL;
	TigEntry te = { NULL, 0, { NULL } };
	int ret = -1;

	// SVN can't handle file like 'pépé'. You will have to rename it from pepe to pépé and
	// uncomment line below and another line.
	//char *name = g_filename_from_utf8("tig/p\xC3\xA9p\xC3\xA9.tig", -1, NULL, NULL, NULL);

	char *array[2];
	char files[2][1024];

	printf("--> Testing TiGroup support (r/w)...\n");
	tifiles_file_display_tigroup(PATH("tig/test.tig"));

	content = tifiles_content_create_tigroup(CALC_NONE, 0);
	if (content != NULL)
	{
		ret = tifiles_file_read_tigroup(PATH("tig/test2.tig"), content);
		if (!ret)
		{
			ret = tifiles_file_write_tigroup(PATH("tig/test2_.tig"), content);
		}
		tifiles_content_delete_tigroup(content);
	}

	if (!ret)
	{
		content = tifiles_content_create_tigroup(CALC_NONE, 0);
		if (content != NULL)
		{
			ret = tifiles_file_read_tigroup(PATH("tig/test.tig"), content);
			if (!ret)
			{
				ret = tifiles_file_write_tigroup(PATH("tig/test_.tig"), content);
				if (!ret)
				{
					ret = compare_files(PATH("tig/test.tig"), PATH2("tig/test_.tig"));
				}
			}
			tifiles_content_delete_tigroup(content);
		}
	}

	if (!ret)
	{
		printf("--> Testing add/del from TiGroup support (r/w)...\n");
		ret = tifiles_tigroup_add_file(PATH("tig/C.8Xn"), PATH2("tig/test2.tig"));
		if (!ret)
		{
			ret = tifiles_tigroup_add_file(PATH("tig/D.8Xn"), PATH2("tig/test2.tig"));
			if (!ret)
			{
				te.filename = strdup("C.8Xn");
				ret = tifiles_tigroup_del_file(&te, PATH("tig/test2.tig"));
				if (!ret)
				{
					te.filename = strdup("D.8Xn");
					ret = tifiles_tigroup_del_file(&te, PATH("tig/test2.tig"));
					if (!ret)
					{
						tifiles_file_display_tigroup(PATH("tig/test2.tig"));
						ret = compare_files(PATH("tig/test.tig"), PATH2("tig/test2.tig"));
					}
				}
			}
		}
	}

	if (!ret)
	{
		printf("--> Testing TiGroup support (group/ungroup)...\n");

		strncpy(files[0], PATH("tig/str.89s"), 1023);
		files[0][1023] = 0;
		strncpy(files[1], PATH("tig/ticabfra.89k"), 1023);
		files[1][1023] = 0;
		array[0] = files[0];
		array[1] = files[1];
		ret = tifiles_tigroup_files(array, PATH("tig/test_.tig"));
		if (!ret)
		{
			tifiles_file_display(PATH("tig/test_.tig"));
			ret = tifiles_untigroup_file(PATH("tig/test.tig"), NULL);
			if (!ret)
			{
				move_file("A.8Xn", "tig/AA.8Xn");
				move_file("B.8Xn", "tig/BB.8Xn");
				ret = compare_files(PATH("tig/A.8Xn"), PATH2("tig/AA.8Xn"));
				if (!ret)
				{
					ret = compare_files(PATH("tig/B.8Xn"), PATH2("tig/BB.8Xn"));
				}
			}
		}
	}

	return ret;
}

// The main function
int main(int argc, char **argv)
{
	char *msg = NULL;
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

	// test filetypes.c
	for(i = CALC_TI73; i <= CALC_V200; i++)
	{
		printf("%s (%i) ", tifiles_fext_of_group(i), i);
	}
	printf("\n");

	for(i = CALC_TI73; i <= CALC_V200; i++)
	{
		printf("%s ", tifiles_fext_of_backup(i));
	}
	printf("\n");

	for(i = CALC_TI73; i <= CALC_V200; i++)
	{
		printf("%s ", tifiles_fext_of_flash_os(i));
	}
	printf("\n");

	for(i = CALC_TI73; i <= CALC_V200; i++)
	{
		printf("%s ", tifiles_fext_of_flash_app(i));
	}
	printf("\n");
	printf("--\n");

	printf("<%s> <%s>\n", "foo.bar", tifiles_fext_get("foo.bar"));

	ret = tifiles_file_is_ti(PATH("misc/str.92s"));
	printf("tifiles_file_is_ti: %i\n", ret);

	ret = tifiles_file_is_single(PATH("misc/str.92s"));
	printf("tifiles_file_is_single: %i\n", ret);

	ret = tifiles_file_is_group(PATH("misc/group.92g"));
        printf("tifiles_file_is_group: %i\n", ret);

	ret = tifiles_file_is_regular(PATH("misc/str.92s"));
        printf("tifiles_file_is_regular: %i\n", ret);

	ret = tifiles_file_is_regular(PATH("misc/group.92g"));
        printf("tifiles_file_is_regular: %i\n", ret);

	ret = tifiles_file_is_backup(PATH("misc/backup.83b"));
	printf("tifiles_file_is_backup: %i\n", ret);

	ret = tifiles_file_is_flash(PATH("misc/ticabfra.89k"));
        printf("tifiles_file_is_flash: %i\n", ret);

	ret = tifiles_file_is_flash(PATH("misc/TI73_OS160.73U"));
	printf("tifiles_file_is_flash: %i\n", ret);

	ret = tifiles_file_is_tib(PATH("misc/ams100.tib"));
	printf("tifiles_file_is_tib: %i\n", ret);

	ret = tifiles_file_is_tigroup(PATH("misc/test.tig"));
	printf("tifiles_file_is_tigroup: %i\n", ret);
	printf("--\n");

	// test typesxx.c
	printf("tifiles_file_get_model: %s\n",
	       tifiles_model_to_string(tifiles_file_get_model(PATH("misc/str.92s"))));

	printf("tifiles_file_get_class: %s\n",
	       tifiles_class_to_string(tifiles_file_get_class(PATH("misc/group.92g"))));

	printf("tifiles_file_get_type: %s\n",
	       tifiles_file_get_type(PATH("misc/TI73_OS160.73U")));
	printf("tifiles_file_get_icon: %s\n",
	       tifiles_file_get_icon(PATH("misc/str.92s")));
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
	do
	{
		// TI73 support
		change_dir(PATH("ti73"));
		ret = test_ti73_backup_support(); if (ret) break;
		ret = test_ti73_regular_support(); if (ret) break;
		ret = test_ti73_group_support(); if (ret) break;
		ret = test_ti73_ungroup_support(); if (ret) break;

		// TI82 support
		change_dir(PATH("ti82"));
		ret = test_ti82_backup_support(); if (ret) break;
		ret = test_ti82_regular_support(); if (ret) break;
		ret = test_ti82_group_support(); if (ret) break;
		ret = test_ti82_ungroup_support(); if (ret) break;

		// TI83 support
		change_dir(PATH("ti83"));
		ret = test_ti83_backup_support(); if (ret) break;
		ret = test_ti83_regular_support(); if (ret) break;
		ret = test_ti83_group_support(); if (ret) break;
		ret = test_ti83_ungroup_support(); if (ret) break;

		// TI84+ support
		change_dir(PATH("ti84p"));
		ret = test_ti84p_regular_support(); if (ret) break;
		ret = test_ti84p_group_support(); if (ret) break;
		ret = test_ti84p_ungroup_support(); if (ret) break;
		ret = test_ti84p_flash_support(); if (ret) break;

		// TI85 support
		change_dir(PATH("ti85"));
		ret = test_ti85_regular_support(); if (ret) break;

		// TI86 support
		change_dir(PATH("ti86"));
		ret = test_ti86_backup_support(); if (ret) break;
		ret = test_ti86_regular_support(); if (ret) break;
		ret = test_ti86_group_support(); if (ret) break;
		ret = test_ti86_ungroup_support(); if (ret) break;

		// TI89 support
		change_dir(PATH("ti89"));
		ret = test_ti89_regular_support(); if (ret) break;
		ret = test_ti89_flash_support(); if (ret) break;
		ret = test_v200_regular_support(); if (ret) break;

		// TI92 support
		change_dir(PATH("ti92"));
		ret = test_ti92_backup_support(); if (ret) break;
		ret = test_ti92_regular_support(); if (ret) break;
		ret = test_ti92_group_support(); if (ret) break;
		ret = test_ti92_ungroup_support(); if (ret) break;

		// TIXX certificates
		change_dir(PATH("certs"));
		ret = test_ti8x_cert_support(); if (ret) break;
		ret = test_ti9x_cert_support(); if (ret) break;

		// Add/Del files
		change_dir(PATH("misc"));
		ret = test_ti8x_group_merge(); if (ret) break;

		change_dir(PATH("tig"));
		ret = test_tigroup();
	} while(0);

	// end of test
	tifiles_library_exit();

	return ret;
}
