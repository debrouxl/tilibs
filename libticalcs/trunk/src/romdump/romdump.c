///* Hey EMACS -*- linux-c -*- */
/* $Id: main.c 347 2004-05-31 09:39:53Z roms $ */

/*  RomDumper - an TI89/92+/V200PLT ROM dumper
 *
 *  Copyright (c) 2004, Romain Liévin for the TiLP and TiEmu projects
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
	Note: this program is compatible with the built-in VTi ROM dumper as well as
	TiLP.
*/

#define USE_TI89              // Compile for TI-89
#define USE_TI92PLUS          // Compile for TI-92 Plus
#define USE_V200              // Compile for V200

#define MIN_AMS 200           // Compile for AMS 2.00 or higher

#define SAVE_SCREEN           // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files

#define VERSION		"1.00"			// Version

/*
	Function: send a byte and check keyboard
	- [in] c : character to send
	- [out] int : 1 if successful, 0 otherwise.
*/
int SendByte(BYTE c)
{
	while(OSWriteLinkBlock(&c, 1))
	{
//		if(kbhit())
//			return 0;
  }
    
  return 1;
}

/*
	Function: receive a byte and check keyboard
	- [out] c : character to receive
	- [out] int : 1 if successful, 0 otherwise.
*/
int GetByte(BYTE *c)
{
    while(1)
    {
        if(OSReadLinkBlock(c, 1))
            return 1;
            
//        if(kbhit())
//        	return 0;
    }
    
    return 0;
}

/*
	Function: send a 1024-bytes block + 2 bytes of checksum
	- [in] ptr : pointer to ROM area
	- [out] int : 1 if successful, 0 otherwise.
*/
int SendBlock(char *ptr)
{
  WORD csum;
  int i;
  BYTE c;
	int retry = 0;

	while(1)
	{
        for (i = 0, csum = 0; i < 1024; i++)
        {
            BYTE ch = ptr[i];
            
            if(!SendByte(ch)) 
            	return 0;
            csum += (WORD)((BYTE)(ch));
        }
        
        if(!SendByte(csum >> 8)) 
        	return 0;
        if(!SendByte(csum & 0xff)) 
        	return 0;
        	
        i = GetByte(&c);
  
        if (!i)
            return 0;
            
        if (c != 0xda)	// chksum error: retry
        {
        	if(retry++ < 3)
            	continue;
            else
            	return 0;	//2;
        }
        
        return 1;
    }
    
    return 0;
}

#define ROM_size (0x200000 << (V200 || ((unsigned long)ROM_base == 0x800000)))

// Main Function
void _main(void)
{
	unsigned long rom_size = ROM_size;
	unsigned long rom_base = (unsigned long)ROM_base;
	
  unsigned long i;
  unsigned char *p;
  char str[30];
  
  ClrScr ();
  FontSetSys (F_8x10);
  
  sprintf(str, "RomDumper v%s", VERSION);
  DrawStr(0, 0, str, A_NORMAL);
  
  sprintf(str, "Type: HW%i", HW_VERSION);
  DrawStr(0, 20, str, A_NORMAL);
  
  sprintf(str, "ROM base: 0x%lx", rom_base);
  DrawStr(0, 40, str, A_NORMAL);  
  
  for(i = 0, p = (char *)rom_base; i < rom_size; i += 1024, p += 1024)
  {  	
		sprintf(str, "Done: %ld/%ldKB", i >> 10, rom_size >> 10);
		
		switch(CALCULATOR)
		{
			case 0: // TI89
				DrawStr(0, 60, str, A_REPLACE	);
			break;
			case 1: // TI92+
				DrawStr(0, 60, str, A_REPLACE	);
			break;
			case 3: // V200
				DrawStr(0, 60, str, A_REPLACE	);
			break;			
		}
		
		if (!SendBlock(p))
      		break;
  }
  
  return;
}

