/* Hey EMACS -*- linux-c -*- */
/* $Id: misc.h 368 2004-03-22 18:42:08Z roms $ */

/*  libtifiles - file format library, a part of the TiLP project
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __TIFILES_MISC__
#define __TIFILES_MISC__

/********************/
/* Read/Write bytes */
/********************/

/*
   Read a block of 'n' bytes from a file
   - s [out]: a buffer for storing the data
   - f [in]: a file descriptor
   - [out]: -1 if error, 0 otherwise.
*/
static inline int fread_n_bytes(FILE * f, unsigned int n, uint8_t *s)
{
	if (s == nullptr)
	{
		for (unsigned int i = 0; i < n; i++)
		{
			if (fgetc(f) == EOF)
			{
				return -1;
			}
		}
	}
	else
	{
		if (fread(s, 1, n, f) < (size_t)n)
		{
			return -1;
		}
	}

	return 0;
}

/*
  Write a string of 'n' chars max (NULL padded) to a file
  - s [in]: a string
  - f [in]: a file descriptor
  - [out]: -1 if error, 0 otherwise.
*/
static inline int fwrite_n_bytes(FILE * f, unsigned int n, const uint8_t *s)
{
	if (fwrite(s, 1, n, f) < (size_t)n)
	{
		return -1;
	}

	return 0;
}

int fread_n_chars(FILE * f, unsigned int n, char *s);
int fwrite_n_chars(FILE * f, unsigned int n, const char *s);
int fwrite_n_chars2(FILE * f, unsigned int n, const char *s);

static inline int fread_8_chars(FILE * f, char *s)
{
	return fread_n_chars(f, 8, s);
}

static inline int fwrite_8_chars(FILE * f, const char *s)
{
	return fwrite_n_chars(f, 8, s);
}

static inline int fskip(FILE * f, int n)
{
	return fseek(f, n, SEEK_CUR);
}

/***************************/
/* Read byte/word/longword */
/***************************/

static inline int fread_byte(FILE * f, uint8_t * data)
{
	if (data != nullptr)
	{
		return (fread((void *) data, sizeof(uint8_t), 1, f) < 1) ? -1 : 0;
	}
	else
	{
		return fskip(f, 1);
	}
}

static inline int fread_word(FILE * f, uint16_t * data)
{
	int ret = 0;
	uint16_t localdata;

	if (data != nullptr)
	{
		ret = (fread((void *)&localdata, sizeof(uint16_t), 1, f) < 1) ? -1 : 0;
		*data = GUINT16_FROM_LE(localdata);
	}
	else
	{
		ret = fskip(f, 2);
	}

	return ret;
}

static inline int fread_long(FILE * f, uint32_t * data)
{
	int ret = 0;
	uint32_t localdata;

	if (data != nullptr)
	{
		ret = (fread((void *)&localdata, sizeof(uint32_t), 1, f) < 1) ? -1 : 0;
		*data = GUINT32_FROM_LE(localdata);
	}
	else
	{
		ret = fskip(f, 4);
	}

	return ret;
}


/****************************/
/* Write byte/word/longword */
/****************************/

static inline int fwrite_byte(FILE * f, uint8_t data)
{
	return (fwrite(&data, sizeof(uint8_t), 1, f) < 1) ? -1 : 0;
}

static inline int fwrite_word(FILE * f, uint16_t data)
{
	data = GUINT16_TO_LE(data);
	return (fwrite(&data, sizeof(uint16_t), 1, f) < 1) ? -1 : 0;
}

static inline int fwrite_long(FILE * f, uint32_t data)
{
	data = GUINT32_TO_LE(data);
	return (fwrite(&data, sizeof(uint32_t), 1, f) < 1) ? -1 : 0;
}

#endif
