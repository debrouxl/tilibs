/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2001  Romain Lievin
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

#ifndef __CABLE_ERRCODES__
#define __CABLE_ERRCODES__

/* Error codes: ERR_...  */
#define ERR_ABORT           -1  // Operation aborted
#define ERR_ROOT             1  // Root permissions required
#define ERR_SND_BIT_TIMEOUT  2  // Send bit timeout 
#define ERR_RCV_BIT_TIMEOUT  3  // Receive bit timeout
#define ERR_OPEN_SER_DEV     32 // Error in opening a serial device
#define ERR_SND_BYT          33 // Send byte
#define ERR_RCV_BYT          34 // Receive byte error
#define ERR_SND_BYT_TIMEOUT  36 // Send byte timeout
#define ERR_RCV_BYT_TIMEOUT  35 // Receive byte timeout
#define ERR_CREATE_FILE      41 // Error with the CreateFile function (Win32)
#define ERR_OPEN_COM_PORT    42 // Error in opening a COM port (Win32)
#define ERR_READ_FILE        47 // Error while reading the COM port
#define ERR_OPEN_TIDEV_DEV   48 // Error while opening a 'tidev' char device
#define ERR_VT0_ALREADY_USED 49 // Error when the pipe is already used
#define ERR_VT1_ALREADY_USED 50 // Idem
#define ERR_OPEN_PIPE        51 // Unable to open a pipe
#define ERR_PIPE_FCNTL       52 // Unable to modify the pipe characteristics
#define ERR_OPP_NOT_AVAIL    53 // No answer
#define ERR_CLOSE_PIPE       54 // Unable to close a pipe
#define ERR_BYTE_LOST        55 // A byte might have been lost due to check_port
#define ERR_ILLEGAL_OP       56 // An illegal argument has been used
#define ERR_FLUSH	     57 // Error while flushing the buffer

// New error codes must begin at 100
#define ERR_SET_COMMSTATE	100 // SetCommState function error (Win32)
#define ERR_GET_COMMSTATE	101 // GetCommState function error (Win32)
#define ERR_SETUP_COMM		102 // SetupComm function error (Win32)
#define ERR_GET_COMMTIMEOUT	103 // GetCommTimeouts function error (Win32)
#define ERR_SET_COMMTIMEOUT	104 // SetCommTimeouts function error (Win32)
#define ERR_OPEN_FILE_MAP	105 // OpenFileMapping error (Win32)
#define ERR_USB_DEVICE_CMD      106 // DeviceIoControl function error (Win32)
#define ERR_IPC_KEY             107 // Unable to get an IPC key (Linux)
#define ERR_SHM_GET             108 // Unable to open a shm (Linux) 
#define ERR_SHM_ATT             109 // Unable to attach/create a shm (Linux)
#define ERR_SHM_DTCH            110 // Unable to detach a shm
#define ERR_SHM_RMID            111 // Unable to remove a shm

#define ERR_GETPROCADDRESS	112 // GetProcAddress error (Win32)
#define ERR_DLPORTIO_NOT_FOUND  113 // DLPortIO not found whereas required (NT)
#define ERR_FREELIBRARY         114 // FreeLibrary error (Win32)

#define ERR_USB_OPEN            115 // Unable to open the USB device
#define ERR_IOCTL               116 // Unable to use IOCTL codes

#endif



