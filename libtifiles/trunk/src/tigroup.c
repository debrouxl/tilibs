/* Hey EMACS -*- linux-c -*- */
/* $Id: grouped.c 1737 2006-01-23 12:54:47Z roms $ */

/*  libtifiles - Ti File Format library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Lievin
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
	TiGroup (*.tig) management
	A TiGroup file is in fact a ZIP archive with no compression (stored).

	Please note that I don't use USEWIN32IOAPI!
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_LIBZ
#include "minizip/zip.h"
#include "minizip/unzip.h"
#endif
/*
#ifdef WIN32
# define USEWIN32IOAPI
# include "minizip/iowin32.h"
#endif
*/

#include <ticonv.h>
#include "tifiles.h"
#include "logging.h"
#include "error.h"
#include "rwfile.h"

#define WRITEBUFFERSIZE (8192)

#ifdef HAVE_LIBZ
extern uLong filetime(char *f, tm_zip *tmzip, uLong *dt);
extern int do_list(unzFile uf);
#endif

#if !GLIB_CHECK_VERSION(2, 8, 0)
#include <errno.h>
// Code taken from Glib
int g_chdir (const gchar *path)
{
#ifdef __WIN32__
  if (G_WIN32_HAVE_WIDECHAR_API ())
    {
      wchar_t *wpath = g_utf8_to_utf16 (path, -1, NULL, NULL, NULL);
      int retval;
      int save_errno;

      if (wpath == NULL)
	{
	  errno = EINVAL;
	  return -1;
	}

      retval = _wchdir (wpath);
      save_errno = errno;

      g_free (wpath);
      
      errno = save_errno;
      return retval;
    }
  else
    {
      gchar *cp_path = g_locale_from_utf8 (path, -1, NULL, NULL, NULL);
      int retval;
      int save_errno;

      if (cp_path == NULL)
	{
	  errno = EINVAL;
	  return -1;
	}

      retval = chdir (cp_path);
      save_errno = errno;

      g_free (cp_path);

      errno = save_errno;
      return retval;
    }
#else
  return chdir (path);
#endif
}
#endif

/**
 * tifiles_te_create:
 * @filename: internal filename in archive.
 * @type: file type (regular or flash)
 * @model: calculator model
 *
 * Allocates a TigEntry structure and allocates fields.
 *
 * Return value: the allocated block.
 **/
TIEXPORT TigEntry* TICALL tifiles_te_create(const char *filename, FileClass type, CalcModel model)
{
	TigEntry *entry;

	entry = (TigEntry *)calloc(1, sizeof(TigEntry));

	entry->filename = strdup(filename);
	entry->type = type;

	if(type == TIFILE_FLASH)
		entry->content.flash = tifiles_content_create_flash(model);
	else
		entry->content.regular = tifiles_content_create_regular(model);

	return entry;
}

/**
 * tifiles_te_delete:
 * @entry: a #TigEntry structure.
 *
 * Destroy a #TigEntry structure as well as fields.
 *
 * Return value: none.
 **/
TIEXPORT int TICALL tifiles_te_delete(TigEntry* entry)
{
	free(entry->filename);
	if(entry->type == TIFILE_FLASH)
		tifiles_content_delete_flash(entry->content.flash);
	else
		tifiles_content_delete_regular(entry->content.regular);

	free(entry);
	return 0;
}

/**
 * tifiles_content_create_tigroup:
 * @model: a calculator model or CALC_NONE.
 * @n: number of #tigEntry entries
 *
 * Allocates a TigContent structure. Note: the calculator model is not required
 * if the content is used for file reading but is compulsory for file writing.
 *
 * Return value: the allocated block.
 **/
TIEXPORT TigContent* TICALL tifiles_content_create_tigroup(CalcModel model, int n)
{
	TigContent* content = calloc(1, sizeof(FileContent));

	content->model = content->model_dst = model;
	content->comment = strdup(tifiles_comment_set_tigroup());
	content->comp_level = 3;
	content->num_entries = n;
	content->entries = (TigEntry **)calloc(n + 1, sizeof(TigEntry *));

	return content;
}

/**
 * tifiles_content_delete_tigroup:
 *
 * Free the whole content of a @TigContent structure and the content itself.
 *
 * Return value: none.
 **/
TIEXPORT int TICALL tifiles_content_delete_tigroup(TigContent *content)
{
	int i, n;
	
	// counter number of files to group
	for(n = 0; content->entries[n]; n++);

	// release allocated memory in structures
	for (i = 0; i < n; i++) 
	{
		TigEntry* entry = content->entries[i];
		tifiles_te_delete(entry);
	}
	free(content);

  return 0;
}

TIEXPORT int TICALL tifiles_content_add_te(TigContent *content, TigEntry *te)
{
	int n;

	for(n = 0; content->entries[n]; n++);

	content->entries = realloc(content->entries, (n + 2) * sizeof(TigEntry *));
	content->entries[n++] = te;
	content->entries[n] = NULL;
	content->num_entries = n;

	return content->num_entries;
}

TIEXPORT int TICALL tifiles_content_del_te(TigContent *content, TigEntry *te)
{
	return 0;
}

/**
 * tifiles_file_read_tigroup:
 * @filename: the name of file to load.
 * @content: where to store content (may be re-allocated).
 *
 * This function load & TiGroup and place its content into content.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_file_read_tigroup(const char *filename, TigContent *content)
{
#ifdef HAVE_LIBZ
	unzFile uf = NULL;
	unz_global_info gi;
	unz_file_info file_info;
	int cnt, err = UNZ_OK;
	char filename_inzip[256];
	unsigned i;
	void* buf = NULL;
	const char *password = NULL;
	int ri = 0;

	// Open ZIP archive
	uf = unzOpen(filename);
	if (uf == NULL)
    {
		printf("Can't open this file: <%s>", filename);
		return ERR_FILE_ZIP;
	}

	// Allocate
	buf = (void*)malloc(WRITEBUFFERSIZE);
    if (buf==NULL)
    {
        printf("Error allocating memory\n");
		goto tfrt_exit;
    }
	
	// Size of comment and number of files in archive
	err = unzGetGlobalInfo (uf,&gi);
    if (err!=UNZ_OK)
	{
		printf("error %d with zipfile in unzGetGlobalInfo \n",err);
		goto tfrt_exit;
	}        
	printf("# entries: %lu\n", gi.number_entry);

	free(content->entries);
	content->entries = (TigEntry **)calloc(gi.number_entry + 1, sizeof(TigEntry *));
	content->num_entries = 0;

	// Get comment
	free(content->comment);
	content->comment = (char *)malloc((gi.size_comment+1) * sizeof(char));
	err = unzGetGlobalComment(uf, content->comment, gi.size_comment);

	// Parse archive for files
	for (i = 0; i < gi.number_entry; i++)
    {
		FILE *f;
		gchar *filename;	// beware: mask global 'filename' !
		gchar *utf8;
		gchar *gfe;

		// get infos
		err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
		if (err!=UNZ_OK)
		{
			printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
			goto tfrt_exit;
		}
		printf("Extracting <%s> with %lu bytes\n", filename_inzip, file_info.uncompressed_size);

		err = unzOpenCurrentFilePassword(uf,password);
        if (err!=UNZ_OK)
        {
            printf("error %d with zipfile in unzOpenCurrentFilePassword\n",err);
			goto tfrt_exit;
        }

		// extract/uncompress into temporary file
		utf8 = g_locale_to_utf8(filename_inzip, -1, NULL, NULL, NULL);
		gfe = g_filename_from_utf8(utf8, -1, NULL, NULL, NULL);		
		filename = g_strconcat(g_get_tmp_dir(), G_DIR_SEPARATOR_S, gfe, NULL);
		g_free(utf8);
		g_free(gfe);

		f = gfopen(filename, "wb");
		if(f == NULL)
		{
			err = ERR_FILE_OPEN;
			goto tfrt_exit;
		}

		do
        {
            err = unzReadCurrentFile(uf,buf,WRITEBUFFERSIZE);
            if (err<0)
            {
                printf("error %d with zipfile in unzReadCurrentFile\n",err);
				fclose(f);
                goto tfrt_exit;
            }
            if (err>0)
			{
				cnt = fwrite(buf, 1, err, f);
				if(cnt == -1)
                {
                    printf("error in writing extracted file\n");
                    err=UNZ_ERRNO;
					fclose(f);
                    goto tfrt_exit;
                }
            }
        }
		while (err>0);
		fclose(f);

		// add to TigContent
		{
			content->model = tifiles_file_get_model(filename);

			if(tifiles_file_is_regular(filename))
			{
				TigEntry *entry = tifiles_te_create(filename_inzip, tifiles_file_get_class(filename), tifiles_file_get_model(filename));

				tifiles_file_read_regular(filename, entry->content.regular);
				content->entries[ri++] = entry;
				content->num_entries++;
			}
			else if(tifiles_file_is_flash(filename))
			{
				TigEntry *entry = tifiles_te_create(filename_inzip, tifiles_file_get_class(filename), tifiles_file_get_model(filename));

				tifiles_file_read_flash(filename, entry->content.flash);
				content->entries[ri++] = entry;
				content->num_entries++;
			}
			else
			{
				// skip
			}
		}
		unlink(filename);
		g_free(filename);

		// next file
		if ((i+1) < gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err!=UNZ_OK)
			{
				printf("error %d with zipfile in unzGoToNextFile\n",err);
				goto tfrt_exit;
			}
		}

	}	

	// Close
tfrt_exit:
	free(buf);
	unzCloseCurrentFile(uf);
	return err ? ERR_FILE_ZIP : 0;
#else
	return ERR_UNSUPPORTED;
#endif
}

/**
 * tifiles_file_write_tigroup:
 * @filename: the name of file to load.
 * @content: where to store content.
 *
 * This function load & TiGroup and place its content into content.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_file_write_tigroup(const char *filename, TigContent *content)
{
#ifdef HAVE_LIBZ
	zipFile zf;
	zip_fileinfo zi;
	int err = ZIP_OK;
    void* buf=NULL;
	gchar *old_dir = g_get_current_dir();
	TigEntry **ptr;

	// Open ZIP archive (and set comment)
#ifdef USEWIN32IOAPI
        zlib_filefunc_def ffunc;
        fill_win32_filefunc(&ffunc);

		g_chdir(g_get_tmp_dir());
        zf = zipOpen2(filename, APPEND_STATUS_CREATE, &(content->comment), &ffunc);
#else
		g_chdir(g_get_tmp_dir());
        zf = zipOpen(filename, APPEND_STATUS_CREATE);
#endif
	if (zf == NULL)
    {
		printf("Can't open this file: <%s>", filename);
		g_chdir(old_dir);
		return ERR_FILE_ZIP;
	}

	// Allocate buffer
	buf = (void*)malloc(WRITEBUFFERSIZE);
	if (buf==NULL)
	{
		printf("Error allocating memory\n");
		goto tfwt_exit;
	}

	// Parse entries and store
	for(ptr = content->entries; *ptr; ptr++)
	{
		FILE *f;
		char filenameinzip[256];
		unsigned long crcFile=0;
		int size_read;
		TigEntry* entry = *ptr;
		char *filename;	// beware: mask global 'filename' !

		// ZIP archives don't like greek chars
		filename = ticonv_gfe_to_zfe(content->model, entry->filename);

		// write TI file into tmp folder
		if(entry->type == TIFILE_FLASH)
		{
			TRYC(tifiles_file_write_flash(filename, entry->content.flash));
		}
		else
		{	
			TRYC(tifiles_file_write_regular(filename, entry->content.regular, NULL));
		}

		// missing tmp file !
		f = gfopen(filename, "rb");
		if(f == NULL)
		{
			err = ERR_FILE_OPEN;
			goto tfwt_exit;
		}
		strcpy(filenameinzip, filename);
		g_free(filename);

		// update time stamp (to do ?)
		zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
        zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;
        filetime(filenameinzip,&zi.tmz_date,&zi.dosDate);

		err = zipOpenNewFileInZip3(zf,filenameinzip,&zi,
                                 NULL,0,NULL,0,NULL /* comment*/,
                                 content->comp_level ? Z_DEFLATED : 0 /* comp method */,
                                 1 /*comp level */,0,
                                 -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                 NULL /* no pwd*/,crcFile);
        if (err != ZIP_OK)
		{
            printf("error in opening %s in zipfile\n",filenameinzip);
			return ERR_FILE_ZIP;
		}		

		do
        {
			// feed with our data
            err = ZIP_OK;
			size_read = fread(buf, 1, WRITEBUFFERSIZE, f);

            if (size_read < WRITEBUFFERSIZE)
			{
				if (!feof(f))
				{
					printf("error in reading %s\n",filenameinzip);
					err = ZIP_ERRNO;
					goto tfwt_exit;
				}
			}
            if (size_read > 0)
            {
                err = zipWriteInFileInZip (zf,buf,size_read);
                if (err<0)
                {
                    printf("error in writing %s in the zipfile\n", filenameinzip);
					goto tfwt_exit;
                }

            }
        } while ((err == ZIP_OK) && (size_read>0));

		// close file
		err = zipCloseFileInZip(zf);
        if (err!=ZIP_OK)
		{
            printf("error in closing %s in the zipfile\n", filenameinzip);
			goto tfwt_exit;
		}

		fclose(f);
		unlink(filename);
	}

	// close archive
tfwt_exit:
	err = zipClose(zf,NULL);
    if (err != ZIP_OK)
        printf("error in closing %s\n",filename);
	free(buf);
	g_chdir(old_dir);
	return err;
#else
	return ERR_UNSUPPORTED;
#endif
}

/**
 * tifiles_file_display_tigroup:
 * @filename: the name of file to load.
 *
 * This function shows file conten ( = "unzip -l filename.tig").
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_file_display_tigroup(const char *filename)
{
#ifdef HAVE_LIBZ
	unzFile uf = NULL;

	uf = unzOpen(filename);
	if (uf==NULL)
    {
		tifiles_warning("Can't open this file: <%s>", filename);
		return -1;
	}

	do_list(uf);
	unzCloseCurrentFile(uf);

	return 0;
#else
	return ERR_UNSUPPORTED;
#endif
}
