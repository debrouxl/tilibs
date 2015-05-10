/* Hey EMACS -*- linux-c -*- */
/* $Id: files9x.c 1343 2005-07-06 15:26:11Z roms $ */

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

/*
	VarEntry structure management routines
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "tifiles.h"
#include "logging.h"

/**
 * tifiles_ve_create:
 *
 * Allocate a new VarEntry structure.
 *
 * Return value: the entry or NULL if error.
 **/
TIEXPORT2 VarEntry* TICALL tifiles_ve_create(void)
{
	return g_malloc0(sizeof(VarEntry));
}

/**
 * tifiles_ve_alloc_data:
 * @size: length of data.
 *
 * Allocate space for data field of VarEntry.
 *
 * Return value: allocated space or NULL if error.
 **/
TIEXPORT2 void * TICALL tifiles_ve_alloc_data(size_t size)
{
	return g_malloc0((size+1) * sizeof(uint8_t));
}

/**
 * tifiles_ve_realloc_data:
 * @size: length of data.
 *
 * Reallocate space for data field of VarEntry.
 *
 * Return value: allocated space or NULL if error.
 **/
TIEXPORT2 VarEntry * TICALL tifiles_ve_realloc_data(VarEntry* ve, size_t size)
{
	if (ve != NULL)
	{
		uint8_t * data = g_realloc(ve->data, (size+1) * sizeof(uint8_t));
		if (size > ve->size)
		{
			memset(data + ve->size, 0x00, size - ve->size);
		}
		ve->data = data;
	}
	return ve;
}

/**
 * tifiles_ve_free_data:
 * @data: length of data.
 *
 * Free space for data field of VarEntry.
 **/
TIEXPORT2 void TICALL tifiles_ve_free_data(void * data)
{
	return g_free(data);
}

/**
 * tifiles_ve_create_with_data:
 * @size: length of data.
 *
 * Allocate a new VarEntry structure and space for data.
 *
 * Return value: the entry or NULL if error.
 **/
TIEXPORT2 VarEntry* TICALL tifiles_ve_create_with_data(uint32_t size)
{
	VarEntry* ve = tifiles_ve_create();
	if (ve != NULL)
	{
		ve->data = (uint8_t *)g_malloc0(size);
	}

	return ve;
}

/**
 * tifiles_ve_create_array:
 * @nelts: size of NULL-terminated array (number of VarEntry structures).
 *
 * Allocate a NULL-terminated array of VarEntry structures. You have to allocate
 * each element of the array by yourself.
 *
 * Return value: the array or NULL if error.
 **/
TIEXPORT2 VarEntry** TICALL tifiles_ve_create_array(int nelts)
{
	return g_malloc0((nelts + 1) * sizeof(VarEntry *));
}

/**
 * tifiles_ve_resize_array:
 * @array: address of array
 * @nelts: size of NULL-terminated array (number of VarEntry structures).
 *
 * Re-allocate a NULL-terminated array of VarEntry structures. You have to allocate
 * each element of the array by yourself.
 *
 * Return value: the array or NULL if error.
 **/
TIEXPORT2 VarEntry** TICALL tifiles_ve_resize_array(VarEntry** array, int nelts)
{
	VarEntry ** ptr = g_realloc(array, (nelts + 1) * sizeof(VarEntry *));
	if (ptr != NULL)
	{
		ptr[nelts] = NULL;
	}
	return ptr;
}

/**
 * tifiles_ve_delete:
 * @ve: var entry.
 *
 * Free data buffer and the structure itself.
 *
 * Return value: none.
 **/
TIEXPORT2 void TICALL tifiles_ve_delete(VarEntry* ve)
{
	if (ve != NULL)
	{
		g_free(ve->data);
		g_free(ve);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}
}

/**
 * tifiles_ve_delete_array:
 * @array: an NULL-terminated array of VarEntry structures.
 *
 * Free the whole array (data buffer, VarEntry structure and array itself).
 *
 * Return value: none.
 **/
TIEXPORT2 void TICALL tifiles_ve_delete_array(VarEntry** array)
{
	VarEntry** ptr;

	if (array != NULL)
	{
		for (ptr = array; *ptr; ptr++)
		{
			tifiles_ve_delete(*ptr);
		}
		g_free(array);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}
}

/**
 * tifiles_ve_copy:
 * @dst: destination entry.
 * @src: source entry.
 *
 * Copy VarEntry and its content from src to dst.
 * If data is NULL, a new buffer is allocated before copying.
 *
 * Return value: the dst pointer or NULL if malloc error.
 **/
TIEXPORT2 VarEntry* TICALL tifiles_ve_copy(VarEntry* dst, VarEntry* src)
{
	int alloc;

	if (src != NULL && dst != NULL)
	{
		alloc = (dst->data == NULL);

		memcpy(dst, src, sizeof(VarEntry));
		if (alloc)
		{
			dst->data = (uint8_t *)g_malloc0(dst->size);
			if (dst->data == NULL)
				return NULL;
		}
		memcpy(dst->data, src->data, src->size);
	}
	else
	{
		tifiles_critical("%s(): an argument is NULL", __FUNCTION__);
		return NULL;
	}

	return dst;
}

/**
 * tifiles_ve_dup:
 * @src: source entry.
 *
 * Duplicate VarEntry and its content from src to dst (full copy).
 *
 * Return value: a newly allocated entry (must be freed with #tifiles_ve_delete when no longer needed).
 **/
TIEXPORT2 VarEntry* TICALL tifiles_ve_dup(VarEntry* src)
{
	VarEntry* dst = NULL;

	if (src != NULL)
	{
		dst = g_malloc0(sizeof(VarEntry));
		if (dst != NULL)
		{
			memcpy(dst, src, sizeof(VarEntry));
			dst->data = (uint8_t *)g_malloc0(dst->size);

			if(src->data != NULL && dst->data != NULL)
			{
				memcpy(dst->data, src->data, dst->size);
			}
		}
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}

	return dst;
}

// ---

/**
 * tifiles_fp_create:
 *
 * Allocate a new FlashPage structure.
 *
 * Return value: the entry or NULL if error.
 **/
TIEXPORT2 FlashPage* TICALL tifiles_fp_create(void)
{
	return g_malloc0(sizeof(FlashPage));
}

/**
 * tifiles_fp_alloc_data:
 * @size: length of data.
 *
 * Allocate space for data field of FlashPage.
 *
 * Return value: allocated space or NULL if error.
 **/
TIEXPORT2 void * TICALL tifiles_fp_alloc_data(size_t size)
{
	uint8_t *data;

	data = g_malloc0((size+1) * sizeof(uint8_t));
	if (data != NULL)
	{
		memset(data, 0xFF, size);
	}

	return data;
}

/**
 * tifiles_fp_realloc_data:
 * @size: new length of data.
 *
 * Reallocate space for data field of FlashPage.
 *
 * Return value: flash page, or NULL if error.
 **/
TIEXPORT2 FlashPage * TICALL tifiles_fp_realloc_data(FlashPage* fp, size_t size)
{
	if (fp != NULL)
	{
		uint8_t * data = g_realloc(fp->data, (size+1) * sizeof(uint8_t));
		if (size > fp->size)
		{
			memset(data + fp->size, 0xFF, size - fp->size);
		}
		fp->data = data;
	}
	return fp;
}

/**
 * tifiles_fp_free_data:
 * @data: length of data.
 *
 * Free space for data field of FlashPage.
 **/
TIEXPORT2 void TICALL tifiles_fp_free_data(void * data)
{
	return g_free(data);
}

/**
 * tifiles_fp_create_with_data:
 * @size: length of data.
 *
 * Allocate a new FlashPage structure and space for data.
 *
 * Return value: the entry or NULL if error.
 **/
TIEXPORT2 FlashPage* TICALL tifiles_fp_create_with_data(uint32_t size)
{
	FlashPage* ve = tifiles_fp_create();
	if (ve != NULL)
	{
		ve->data = (uint8_t *)g_malloc0(size);
	}

	return ve;
}

/**
 * tifiles_fp_create_array:
 * @nelts: size of NULL-terminated array (number of FlashPage structures).
 *
 * Allocate a NULL-terminated array of FlashPage structures. You have to allocate
 * each element of the array by yourself.
 *
 * Return value: the array or NULL if error.
 **/
TIEXPORT2 FlashPage** TICALL tifiles_fp_create_array(int nelts)
{
	return g_malloc0((nelts + 1) * sizeof(FlashPage*));
}

/**
 * tifiles_fp_resize_array:
 * @array: address of array
 * @nelts: size of NULL-terminated array (number of FlashPage structures).
 *
 * Re-allocate a NULL-terminated array of FlashPage structures. You have to allocate
 * each element of the array by yourself.
 *
 * Return value: the array or NULL if error.
 **/
TIEXPORT2 FlashPage** TICALL tifiles_fp_resize_array(FlashPage** array, int nelts)
{
	FlashPage ** ptr = g_realloc(array, (nelts + 1) * sizeof(FlashPage *));
	if (ptr != NULL)
	{
		ptr[nelts] = NULL;
	}
	return ptr;
}

/**
 * tifiles_fp_delete:
 * @ve: var entry.
 *
 * Free data buffer and the structure itself.
 *
 * Return value: none.
 **/
TIEXPORT2 void TICALL tifiles_fp_delete(FlashPage* fp)
{
	if (fp != NULL)
	{
		g_free(fp->data);
		g_free(fp);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}
}

/**
 * tifiles_fp_delete_array:
 * @array: an NULL-terminated array of FlashPage structures.
 *
 * Free the whole array (data buffer, FlashPage structure and array itself).
 *
 * Return value: none.
 **/
TIEXPORT2 void TICALL tifiles_fp_delete_array(FlashPage** array)
{
	FlashPage** ptr;

	if (array != NULL)
	{
		for (ptr = array; *ptr; ptr++)
		{
			tifiles_fp_delete(*ptr);
		}
		g_free(array);
	}
	else
	{
		tifiles_critical("%s(NULL)", __FUNCTION__);
	}
}
