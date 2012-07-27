// Miscellaneous defines - there are no official TIGCC headers for Fargo yet.
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

#define NULL ((void *) 0) 

// The LIO functions in the TI-92 ROM are not exported by Fargo.
unsigned short (*LIO_SendData) (const void *src, unsigned long size); 
unsigned short (*LIO_RecvData) (void *dest, unsigned long size, unsigned long WaitDelay); 

#define __kb_globals tios__001b
extern void __kb_globals;
#define kbhit() (*(volatile short*)(&__kb_globals+0x1c))
#define sprintf tios__000f
short sprintf (char *buffer, const char *format, ...);
#define DrawStr tios__0010
void DrawStr (short x, short y, const char *str, short Attr);
enum Attrs {A_REVERSE, A_NORMAL, A_XOR, A_SHADED, A_REPLACE, A_OR, A_AND, A_THICK1, A_SHADE_V, A_SHADE_H, A_SHADE_NS, A_SHADE_PS};
#define __ROM_base tios__0025
extern void __attribute__((may_alias)) __ROM_base;
#define ROM_base (&__ROM_base)
// ROM_base + 0xc4 (49*4) contains the used ROM size - 4, round up to full 1 or 2 MB
#define ROM_size ((49[(unsigned long *)ROM_base] + 0xffffful) & 0xfff00000ul)
#define TI92_VERSION ((int)(ROM_size>>20))
#define ERD_dialog tios__002F
short ERD_dialog (short err_no, short prog_flag);
#define OSLinkTxQueueInquire tios__0008
unsigned short OSLinkTxQueueInquire (void);
#define OSReadLinkBlock tios__000A
unsigned short OSReadLinkBlock (char *buffer, unsigned short num);
#define memset tios__0032
void *memset (void *buffer, short c, unsigned long num); 
#define __tios_globals tios__001C
extern void __tios_globals;
#define LCD_MEM (&__tios_globals)
#define ClrScr() memset(LCD_MEM,0,3840);
#define FontSetSys tios__0012
unsigned char FontSetSys (short Font);
enum Fonts {F_4x6, F_6x8, F_8x10};
#define OSLinkReset tios__0006
void OSLinkReset (void);

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
