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
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_ZLIB
//#undef WIN32
//#include "minizip/iowin32.h"
#include "minizip/zip.h"
#include "minizip/unzip.h"
#endif

#include "tifiles.h"
#include "logging.h"

#define WRITEBUFFERSIZE (8192)

/**
 * tifiles_file_read_tigroup:
 * @filename: the name of file to load.
 * @content: where to store content.
 *
 * This function load & TiGroup and place its content into content.
 *
 * Return value: an error code if unsuccessful, 0 otherwise.
 **/
TIEXPORT int TICALL tifiles_file_read_tigroup(const char *filename, FileContent *content)
{
	unzFile uf = NULL;
	unz_global_info gi;
	unz_file_info file_info;
	int err = UNZ_OK;
	char filename_inzip[256];
	unsigned i;
	void* buf;
    unsigned size_buf = WRITEBUFFERSIZE;
	const char *password = NULL;

	// Open ZIP archive
	uf = unzOpen(filename);
	if (uf == NULL)
    {
		printf("Can't open this file: <%s>", filename);
		return -1;
	}

	// Allocate
	buf = (void*)malloc(size_buf);
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
	printf("# entries: %u\n", gi.number_entry);

	// Parse archive for files
	for (i = 0; i < gi.number_entry; i++)
    {
		//VarEntry *ve = tifiles_ve_create_with_data(file_info.uncompressed_size);
		//VarEntry *p = ve;

		// get infos
		err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
		if (err!=UNZ_OK)
		{
			printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
			goto tfrt_exit;
		}
		printf("Extracting <%s> with %u bytes\n", filename_inzip, file_info.uncompressed_size);

		err = unzOpenCurrentFilePassword(uf,password);
        if (err!=UNZ_OK)
        {
            printf("error %d with zipfile in unzOpenCurrentFilePassword\n",err);
        }

		// extract/uncompress
		do
        {
            err = unzReadCurrentFile(uf,buf,size_buf);
            if (err<0)
            {
                printf("error %d with zipfile in unzReadCurrentFile\n",err);
                break;
            }
            if (err>0)
			{
				// place data
				//memcpy(p, buf, err);
				//p += err;
            }
        }
		    while (err>0);

		//tifiles_content_add_entry(content, ve);
	}	

	// Close
tfrt_exit:
	free(buf);
	unzCloseCurrentFile(uf);
	return err;
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
TIEXPORT int TICALL tifiles_file_write_tigroup(const char *filename, FileContent *content)
{
	zipFile zf;
	zip_fileinfo zi;
	int err=ZIP_OK;
	int i;
	int size_buf=0;
    void* buf=NULL;

	// Open ZIP archive
	zf = zipOpen(filename,APPEND_STATUS_ADDINZIP);
	if (zf == NULL)
    {
		printf("Can't open this file: <%s>", filename);
		return -1;
	}

	// Parse
	for(i = 0; i < content->num_entries; i++)
	{
		FILE * fin;
        int size_read;
		char filenameinzip[256];
		unsigned long crcFile=0;
		int size_buf=0;
		void* buf=NULL;

		zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
        zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
        zi.dosDate = 0;
        zi.internal_fa = 0;
        zi.external_fa = 0;
        filetime(filenameinzip,&zi.tmz_date,&zi.dosDate);

		err = zipOpenNewFileInZip3(zf,filenameinzip,&zi,
                                 NULL,0,NULL,0,NULL /* comment*/,
                                 0 /* store, no comp */,
                                 0 /*comp level */,0,
                                 -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                 NULL /* no pwd*/,crcFile);
        if (err != ZIP_OK)
            printf("error in opening %s in zipfile\n",filenameinzip);

		size_buf = WRITEBUFFERSIZE;
		buf = (void*)malloc(size_buf);
		if (buf==NULL)
		{
			printf("Error allocating memory\n");
			return ZIP_INTERNALERROR;
		}

		do
        {
			// feed with data
            err = ZIP_OK;
            //size_read = (int)fread(buf,1,size_buf,fin);
            if (size_read < size_buf)
                if (feof(fin)==0)
            {
                printf("error in reading %s\n",filenameinzip);
                err = ZIP_ERRNO;
            }

            if (size_read>0)
            {
                err = zipWriteInFileInZip (zf,buf,size_read);
                if (err<0)
                {
                    printf("error in writing %s in the zipfile\n",
                                     filenameinzip);
                }

            }
        } while ((err == ZIP_OK) && (size_read>0));

		// close file
		err = zipCloseFileInZip(zf);
                    if (err!=ZIP_OK)
                        printf("error in closing %s in the zipfile\n",
                                    filenameinzip);
	}

	// close archive
	err = zipClose(zf,NULL);
        if (err != ZIP_OK)
            printf("error in closing %s\n",filename);

		free(buf);

	return 0;
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
}