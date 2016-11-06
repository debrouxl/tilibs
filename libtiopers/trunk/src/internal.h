/*  libtiopers - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 2013-2016  Lionel Debroux
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

/**
 * \file internal.h
 * \brief Definitions for internal (libtiopers) usage.
 */

#ifndef __TIOPERS_INTERNAL__
#define __TIOPERS_INTERNAL__

#include "error.h"

#define VALIDATE_NONNULL(ptr) \
	do \
	{ \
		if (ptr == NULL) \
		{ \
			tiopers_critical("%s: " #ptr " is NULL", __FUNCTION__); \
			return ERR_INVALID_PARAMETER; \
		} \
	} while(0);
#define VALIDATE_HANDLE(handle) \
	do \
	{ \
		if (!tiopers_validate_handle(handle)) \
		{ \
			tiopers_critical("%s: " #handle " is invalid", __FUNCTION__); \
			return ERR_INVALID_HANDLE; \
		} \
	} while(0);

#define RETURN_IF_HANDLE_NOT_ATTACHED(handle) \
	do \
	{ \
		if (!handle->attached) \
		{ \
			return ERR_NO_CABLE; \
		} \
	} while(0);
#define RETURN_IF_HANDLE_NOT_OPEN(handle) \
	do \
	{ \
		if (!handle->open) \
		{ \
			return ERR_NO_CABLE; \
		} \
	} while(0);
#define RETURN_IF_HANDLE_BUSY(handle) \
	do \
	{ \
		if (handle->busy) \
		{ \
			return ERR_BUSY; \
		} \
	} while(0);

static inline int tiopers_validate_handle(OperHandle * handle)
{
	return handle != NULL;
}

#endif // __TIOPERS_INTERNAL__
