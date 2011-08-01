// USB link routines (taken from Kevin's Backgammon game)
// Copyright (c) 2004-2006 Kevin Kofler

/*  This program is free software; you can redistribute it and/or modify
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

#define XR_stringPtr_addr (_rom_call_addr(293))
#define push_offset_array_addr (_rom_call_addr(3c4))
#define IsSupportedAMS300 IsSupportedUSBAMS /* deprecated */
#define IsSupportedUSBAMS() (IsSupportedUSBAMS30x()||IsSupportedUSBAMS31x())
#define IsSupportedUSBAMS30x() (*(long long*)(XR_stringPtr_addr-116)==0x76021003261f4e75ll)
#define IsSupportedUSBAMS31x() (*(long long*)(push_offset_array_addr-1112)==0x76021003261f4e75ll)
#define DetectLinkInterface30x ((unsigned char(*)(void))(XR_stringPtr_addr-158))
#define DetectReceiveInterface30x ((unsigned char(*)(short))(XR_stringPtr_addr-108))
#define DetectLinkInterface31x ((unsigned char(*)(void))(push_offset_array_addr-1154))
#define DetectReceiveInterface31x ((unsigned char(*)(short))(push_offset_array_addr-1104))
#define DetectLinkInterface (IsSupportedUSBAMS31x()?DetectLinkInterface31x:DetectLinkInterface30x)
#define DetectReceiveInterface (IsSupportedUSBAMS31x()?DetectReceiveInterface31x:DetectReceiveInterface30x)
#define DetectUSB (*(short(**)(void))(DetectLinkInterface+4))
#define USBCheckReceived (*(void(**)(void))(DetectReceiveInterface+16))
#define USBCheckStatus (*(short(**)(void))(DetectReceiveInterface+22))
#define USB_SendData ((short (*)(const void *, size_t, long))(USBCheckStatus-494))
#define USB_RecvData ((short (*)(void *, size_t, long))(USBCheckStatus-246))
#define USBLinkClose ((short(*)(void))(USBCheckStatus-266))
#define USBLinkReset (*(void(**)(void))(DetectLinkInterface+36))
