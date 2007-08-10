/******************************************************************************
 * rwports.c: ioports hardware access under Windows NT/2000/XP and 64-bits
 * Developped from MPlayer's DhaHelper driver and modified by:
 * Copyright (c) 2007 Romain Liévin <roms@tilp.info>.
 * Patched to compile with MinGW by Kevin Kofler:
 * Copyright (c) 2007 Kevin Kofler
 *
 * This file is part of the TiLP/TiEmu projects.
 *
 * rwports is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * rwports is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rwports; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *****************************************************************************/


#if defined(_MSC_VER)
# include <ntddk.h>
# ifndef STDCALL
#  define STDCALL /* nothing */
# endif
#elif defined(__MINGW32__)
# include <ddk/ntddk.h>
#else
# error Unsupported compiler. This driver requires MSVC+DDK or MinGW to build.
#endif

#include "rwports.h"

#define OutputDebugString DbgPrint

static STDCALL NTSTATUS RwPortsDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp);
static STDCALL void		RwPortsUnload(IN PDRIVER_OBJECT DriverObject);

//
// Driver entry point
//
STDCALL NTSTATUS DriverEntry (IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
  UNICODE_STRING  DeviceNameUnicodeString;
  UNICODE_STRING  DeviceLinkUnicodeString;
  NTSTATUS        ntStatus;
  PDEVICE_OBJECT  DeviceObject = NULL;

  OutputDebugString ("rwports: entering DriverEntry");

  RtlInitUnicodeString (&DeviceNameUnicodeString, L"\\Device\\rwports");

  // Create an EXCLUSIVE device object (only 1 thread at a time
  // can make requests to this device).

  ntStatus = IoCreateDevice(DriverObject,0,&DeviceNameUnicodeString,FILE_DEVICE_RWPORTS,0,TRUE,&DeviceObject);
	
  if (NT_SUCCESS(ntStatus))
  {
    // Create dispatch points for device control, create, close.
    DriverObject->MajorFunction[IRP_MJ_CREATE] =
    DriverObject->MajorFunction[IRP_MJ_CLOSE] =
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = RwPortsDispatch;
    DriverObject->DriverUnload = RwPortsUnload;

    // Create a symbolic link, e.g. a name that a Win32 app can specify
    // to open the device.

    RtlInitUnicodeString (&DeviceLinkUnicodeString, L"\\DosDevices\\rwports");

    ntStatus = IoCreateSymbolicLink(&DeviceLinkUnicodeString,&DeviceNameUnicodeString);

    if (!NT_SUCCESS(ntStatus)){
      // Symbolic link creation failed- note this & then delete the
      // device object (it's useless if a Win32 app can't get at it).
      OutputDebugString ("rwports: IoCreateSymbolicLink failed");
      IoDeleteDevice (DeviceObject);
    }
  }
  else
  {
    OutputDebugString ("rwports: IoCreateDevice failed");
  }
  
  OutputDebugString ("rwports: leaving DriverEntry");
  return ntStatus;
}

//
// Dispatch: process the IRPs sent to this device
//
static STDCALL NTSTATUS RwPortsDispatch(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
{
  PIO_STACK_LOCATION IrpStack;
  ULONG              dwInputBufferLength;
  ULONG              dwOutputBufferLength;
  ULONG              dwIoControlCode;
  PVOID              pvIoBuffer;
  NTSTATUS           ntStatus;

  PUCHAR			 p8IoBuffer;
  PUSHORT			 p16IoBuffer;
  PULONG			 p32IoBuffer;

  OutputDebugString ("rwports: entering RwPortsDispatch");

  // Init to default settings

  Irp->IoStatus.Status      = STATUS_SUCCESS;
  Irp->IoStatus.Information = 0;
  
  IrpStack = IoGetCurrentIrpStackLocation(Irp);

  // Get the pointer to the input/output buffer and it's length

  pvIoBuffer           = Irp->AssociatedIrp.SystemBuffer;
  dwInputBufferLength  = IrpStack->Parameters.DeviceIoControl.InputBufferLength;
  dwOutputBufferLength = IrpStack->Parameters.DeviceIoControl.OutputBufferLength;

  p8IoBuffer  = (PUCHAR)pvIoBuffer;
  p16IoBuffer = (PUSHORT)pvIoBuffer;
  p32IoBuffer = (PULONG)pvIoBuffer;

  switch (IrpStack->MajorFunction)
  {
    case IRP_MJ_CREATE:
      OutputDebugString("rwports: IRP_MJ_CREATE");
      break;
    case IRP_MJ_CLOSE:
      OutputDebugString("rwports: IRP_MJ_CLOSE");
      break;
    case IRP_MJ_DEVICE_CONTROL:
      OutputDebugString("rwports: IRP_MJ_DEVICE_CONTROL");
      dwIoControlCode = IrpStack->Parameters.DeviceIoControl.IoControlCode;

      switch (dwIoControlCode)
	  {
        case IOCTL_RWPORTS_READ_BYTE:
          OutputDebugString("rwports: IOCTL_RWPORTS_READ_BYTE");

		  if ((dwInputBufferLength >= 2) && (dwOutputBufferLength >= 1)) 
		  {
			  OutputDebugString("rwports: read at 0x%x", p16IoBuffer[0]);

			  *p8IoBuffer = READ_PORT_UCHAR((PUCHAR)p16IoBuffer[0]);

			  Irp->IoStatus.Information = 1;	// 1 byte written
          } 
		  else 
			  ntStatus = STATUS_BUFFER_TOO_SMALL;        
          break;

        case IOCTL_RWPORTS_WRITE_BYTE:
          OutputDebugString("rwports: IOCTL_RWPORTS_WRITE_BYTE");

		  if (dwInputBufferLength >= 3) 
		  {
			  OutputDebugString("rwports: write at 0x%x", p16IoBuffer[0]);

			  WRITE_PORT_UCHAR((PUCHAR)p16IoBuffer[0], p8IoBuffer[2]);

			  Irp->IoStatus.Information = 0;	// 0 bytes written
          } 
		  else 
			  ntStatus = STATUS_BUFFER_TOO_SMALL;
          break;

         default:
          OutputDebugString("rwports: unknown IRP_MJ_DEVICE_CONTROL");
          Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
          break;
      }
      break;
  }

  // DON'T get cute and try to use the status field of the irp in the
  // return status.  That IRP IS GONE as soon as you call IoCompleteRequest.

  ntStatus = Irp->IoStatus.Status;
  IoCompleteRequest (Irp, IO_NO_INCREMENT);

  // We never have pending operation so always return the status code.

  OutputDebugString("rwports: leaving RwPortsDispatch");

  return ntStatus;
}

//
// Delete the associated device and return
//
static STDCALL void RwPortsUnload(IN PDRIVER_OBJECT DriverObject)
{
  UNICODE_STRING DeviceLinkUnicodeString;
  NTSTATUS ntStatus=STATUS_SUCCESS;
  OutputDebugString ("rwports: entering RwPortsUnload");
  OutputDebugString ("rwports: unmapping remaining memory");
  
  RtlInitUnicodeString (&DeviceLinkUnicodeString, L"\\DosDevices\\rwports");
  ntStatus = IoDeleteSymbolicLink (&DeviceLinkUnicodeString);

  if (NT_SUCCESS(ntStatus))
  {
    IoDeleteDevice (DriverObject->DeviceObject);
  }
  else 
  {
    OutputDebugString ("rwports: IoDeleteSymbolicLink failed");
  }
  OutputDebugString ("rwports: leaving RwPortsUnload");
}

