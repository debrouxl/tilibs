/******************************************************************************
 * dhahelper.c: direct hardware access under Windows NT/2000/XP
 * Copyright (c) 2004 Sascha Sommer <saschasommer@freenet.de>.
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MPlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *****************************************************************************/

#ifndef _RWPORTS_H
#define _RWPORTS_H 1

// Define the various device type values.  Note that values used by Microsoft
// Corporation are in the range 0-32767, and 32768-65535 are reserved for use
// by customers.

#define FILE_DEVICE_RWPORTS 0x00008011

// Macro definition for defining IOCTL and FSCTL function control codes.
// Note that function codes 0-2047 are reserved for Microsoft Corporation,
// and 2048-4095 are reserved for customers.

#define RWPORTS_IOCTL_INDEX 0x811



#define IOCTL_RWPORTS_READ_BYTE        CTL_CODE(FILE_DEVICE_RWPORTS,     \
                                                  RWPORTS_IOCTL_INDEX + 0, \
                                                  METHOD_BUFFERED,           \
                                                  FILE_ANY_ACCESS)

#define IOCTL_RWPORTS_WRITE_BYTE       CTL_CODE(FILE_DEVICE_RWPORTS,     \
                                                  RWPORTS_IOCTL_INDEX + 1, \
                                                  METHOD_BUFFERED,           \
                                                  FILE_ANY_ACCESS)

#define IOCTL_RWPORTS_READ_BYTES       CTL_CODE(FILE_DEVICE_RWPORTS,     \
                                                  RWPORTS_IOCTL_INDEX + 2, \
                                                  METHOD_BUFFERED,           \
                                                  FILE_ANY_ACCESS)

#define IOCTL_RWPORTS_WRITE_BYTES      CTL_CODE(FILE_DEVICE_RWPORTS,     \
                                                  RWPORTS_IOCTL_INDEX + 3, \
                                                  METHOD_BUFFERED,           \
                                                  FILE_ANY_ACCESS)

#endif
