/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "calc_err.h"
#include "calc_ext.h"
#include "defs92.h"
//#include "keys92.h"
#include "group.h"
#include "rom92f2.h"
#include "pause.h"
#include "update.h"

#ifdef HAVE_CURSES_H
#include <curses.h>
#endif

/* Functions used by TI_PC functions */

// The PC indicates that is OK
static int PC_replyOK_92(void)
{
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_TI_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer reply OK.\n");

  return 0;
}

// The PC indicates that it is ready or wait data
static int PC_waitdata_92(void)
{
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_WAIT_DATA));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer wait data.\n");
  
  return 0;
}

// Check whether the TI reply OK
static int ti92_isOK(void)
{
  byte data;

  TRY(cable->get(&data));
  if(data != TI92_PC) 
    {
      //DISPLAY("Debug1: %02X\n", data);
      return 4;
    }
  TRY(cable->get(&data));
  if(data != CMD92_TI_OK)
    { 
      //DISPLAY("Debug2: %02X\n", data);
      if(data==CMD92_CHK_ERROR) return ERR_CHECKSUM;
      else return 4;
    }
  TRY(cable->get(&data));
  if(data != 0x00)
    { 
      //DISPLAY("Debug3: %02X\n", data);
      return 4;
    }
  TRY(cable->get(&data));
  if((data&1) != 0)
    { 
      //DISPLAY("Debug4: %02X\n", data);
      return ERR_NOT_READY;
    }
  DISPLAY("The calculator reply OK.\n");

  return 0;
}

// The TI indicates that it is ready or wait data
static int ti92_waitdata(void)
{
  byte data;

  TRY(cable->get(&data));
  if(data != TI92_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD92_WAIT_DATA) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != 0x00) return ERR_DISCONTINUE;
  TRY(cable->get(&data));
  if(data != 0x00) return ERR_DISCONTINUE;
  DISPLAY("The calculator wait data.\n");

  return 0;
}

// Check whether the TI reply that it is ready
int ti92_isready(void)
{
  TRY(cable->open());
  DISPLAY("Is calculator ready ?\n");
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_ISREADY));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  TRY(ti92_isOK());
  DISPLAY("The calculator is ready.\n");
  TRY(cable->close());

  return 0;
}

// Send a string of characters to the TI
static int ti92_sendstring(char *s, word *checksum)
{
  int i;

  for(i=0; i<(int)strlen(s); i++)
    {
      TRY(cable->put(s[i]));
      (*checksum) += (s[i] & 0xff); //0xFF allows special characters to be received !
    }

  return 0;
}

#define TI92_MAXTYPES 32
const char *TI92_TYPES[TI92_MAXTYPES]=
{ 
"EXPR", "UNKNOWN", "UNKNOWN", "UNKNOWN", "LIST", "UNKNOWN", "MAT", "UNKNOWN", 
"UNKNOW", "UNKNOWN", "DATA", "TEXT", "STR", "GDB", "FIG", "UNKNOWN",
"PIC", "UNKNOWN", "PRGM", "FUNC", "MAC", "UNKNOWN", "UNKNOWN", "UNKNOWN",
"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "BACKUP", "UNKNOWN", "DIR"
};
const char *TI92_EXT[TI92_MAXTYPES]=
{
"92e", "unknown", "unknown", "unknown", "92l", "unknown", "92m", "unknown",
"unknown", "unknown", "92c", "92t", "92s", "92d", "92a", "unknown",
"92i", "unknown", "92p", "92f", "92x", "unknown", "unknown", "unknown",
"unknown", "unknown", "unknown", "unknown", "unknown", "92b", "unknown", "unknown"
};

// Return the type corresponding to the value
const char *ti92_byte2type(byte data)
{
  if(data>TI92_MAXTYPES)
    {
      DISPLAY("Type: %02X\n", data);
      DISPLAY("Warning: unknown type. It is a bug. Please report this information.\n");
      return "UNKNOWN";
    }
  else 
    {
      return TI92_TYPES[data];
    }
}

// Return the value corresponding to the type
byte ti92_type2byte(char *s)
{
  int i;

  for(i=0; i<TI92_MAXTYPES; i++)
    {
      if(!strcmp(TI92_TYPES[i], s)) break;
    }
  if(i>TI92_MAXTYPES)
    {
      DISPLAY("Warning: unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti92_byte2fext(byte data)
{
  if(data>TI92_MAXTYPES)
    {
      DISPLAY("Type: %02X\n", data);
      DISPLAY("Warning: unknown type. It is a bug. Please report this information.\n");    
      return ".92?";
    }
  else 
  {
    return TI92_EXT[data];
  }
}

// Return the value corresponding to the file extension
byte ti92_fext2byte(char *s)
{
  int i;

  for(i=0; i<TI92_MAXTYPES; i++)
    {
      if(!strcmp(TI92_EXT[i], s)) break;
    }
  if(i > TI92_MAXTYPES)
    {
      DISPLAY("Warning: unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// General functions
int ti92_send_key(word key)
{
  TRY(cable->open());
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_DIRECT_CMD));
  TRY(cable->put(LSB(key)));
  TRY(cable->put(MSB(key)));
  TRY(ti92_isOK());
  TRY(cable->close());

  return 0;
}

int ti92_remote_control(void)
{
#if defined(HAVE_CURSES_H) && defined(HAVE_LIBCURSES)
  int c;
  word d;
  int sp_key;
  char skey[10];
  int b;

  TRY(cable->open());
  d=0;
  DISPLAY("\n");
  DISPLAY("Remote control: press any key but for:\n");
  DISPLAY("2nd, press the square key\n");
  DISPLAY("diamond, press the tab key\n");
  DISPLAY("APPS, press F9\n");
  DISPLAY("STO, press F10\n");
  DISPLAY("MODE, press F11\n");
  DISPLAY("CLEAR, press F12\n");
  DISPLAY("Press End to quit the remote control mode.\n");
  getchar();
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  raw();
  do
    {
      sp_key=0;
      b=0;
      strcpy(skey, "");
      c=getch();
      if(c==9)
	{
	  sp_key=KEY92_CTRL;
	  strcpy(skey, "DIAMOND ");
	  b=1;
	  c=getch();
	}
      if(c==178)
        {
          sp_key=KEY92_2ND;
	  strcpy(skey, "2nd ");
	  b=1;
	  c=getch();
        }
      if(c>31 && c<128)
	{
	  DISPLAY("Sending <%s%c>\n", skey, c);
	  TRY(cable->put(PC_TI92));
	  TRY(cable->put(CMD92_DIRECT_CMD));
	  c+=sp_key;
	  DISPLAY("%i\n", c);
	  TRY(cable->put(LSB(c)));
	  TRY(cable->put(MSB(c)));
	  TRY(ti92_isOK());
	  refresh();
	}
      else
	{
	  if(c==ESC)
            {
              d=KEY92_ESC;
              DISPLAY("Sending %sESC key\n", skey);
            }
	  if(c==BS)
            {
              d=KEY92_BS;
              DISPLAY("Sending %s<- key\n", skey);
            }
	  if(c==F1) 
	    {
	      d=KEY92_F1;
	      DISPLAY("Sending %sF1 key\n", skey);	      
	    }
	  if(c==F2) 
	    {
	      d=KEY92_F2;
	      DISPLAY("Sending %sF2 key\n", skey);
	    }
          if(c==F3) 
	    {
	      d=KEY92_F3;
	      DISPLAY("Sending %sF3 key\n", skey);
	    }
          if(c==F4) 
	    {
	      d=KEY92_F4;
	      DISPLAY("Sending %sF4 key\n", skey);
	    }
          if(c==F5) 
	    {
	      d=KEY92_F5;
	      DISPLAY("Sending %sF5 key\n", skey);
	    }
          if(c==F6) 
	    {
	      d=KEY92_F6;
	      DISPLAY("Sending %sF6 key\n", skey);
	    }
	  if(c==F7) 
	    {
	      d=KEY92_F7;
	      DISPLAY("Sending %sF7 key\n", skey);
	    }
          if(c==F8) 
	    {
	      d=KEY92_F8;
	      DISPLAY("Sending %sF8 key\n", skey);
	    }
          if(c==F9) 
	    {
	      d=KEY92_APPS;
	      DISPLAY("Sending %sAPPS key\n", skey);
	    }
          if(c==F10) 
	    {
	      d=KEY92_STO;
	      DISPLAY("Sending %sSTO key\n", skey);
	    }
          if(c==F11) 
	    {
	      d=KEY92_MODE;
	      DISPLAY("Sending %sMODE key\n", skey);
	    }
          if(c==F12) 
	    {
	      d=KEY92_CLEAR;
	      DISPLAY("Sending %sCLEAR key\n", skey);
	    }
	  if(c==CALC_CR || c==CALC_LF) 
	    {
	      d=KEY92_CR;
	      DISPLAY("Sending %sENTER key\n", skey);
	    }
	  d+=sp_key;
	  TRY(cable->put(PC_TI92));
          TRY(cable->put(CMD92_DIRECT_CMD));
          TRY(cable->put(LSB(d)));
          TRY(cable->put(MSB(d)));
          TRY(ti92_isOK());
	  refresh();
	}
    }
  while(c!=END);
  noraw();
  endwin();
  DISPLAY("Remote control finished.\n");
  DISPLAY("\n");
  TRY(cable->close());
  
  return 0;
#else
  return ERR_VOID_FUNCTION;
#endif
}

int ti92_screendump(byte **bitmap, int mask_mode,
                         struct screen_coord *sc)
{
  byte data;
  word max_cnt;
  word sum;
  word checksum;
  int i;

  TRY(cable->open());
  update_start();
  update->prev_percentage = update->percentage = 0.0;
  sc->width=TI92_COLS;
  sc->height=TI92_ROWS;
  sc->clipped_width=TI92_COLS;
  sc->clipped_height=TI92_ROWS;
  if(*bitmap != NULL)
    free(*bitmap);
  (*bitmap)=(byte *)malloc(TI92_COLS*TI92_ROWS*sizeof(byte)/8);
  if((*bitmap) == NULL)
    {
      fprintf(stderr, "Unable to allocate memory.\n");
      exit(0);
    }

  sum=0;
  DISPLAY("Request screendump.\n");
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  TRY(ti92_isOK());

  TRY(cable->get(&data));
  if(data != TI92_PC) return 6;
  TRY(cable->get(&data));
  if(data != CMD92_DATA_PART) return 6;  
  TRY(cable->get(&data));
  max_cnt=data;
  TRY(cable->get(&data));
  max_cnt += (data << 8);
  DISPLAY("0x%04X = %i bytes to receive\n", max_cnt, max_cnt);
  DISPLAY("Screendump in progress...\n");

  update->total = max_cnt;
  for(i=0; i<max_cnt; i++)
    {
      TRY(cable->get(&data));
      (*bitmap)[i]=~data;
      sum+=data;
    
      update->count = i;
      update->percentage = (float)i/max_cnt;
      update_pbar();
      if(update->cancel) return -1;
    }
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  //if(sum != checksum) return 7; // PC & TI checksum are different. Why ?
  //DISPLAY("Ckechsum: %04X\n", checksum);
  //DISPLAY("Sum: %04X\n", sum);
  //DISPLAY("Checksum OK.\n");

  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_PC_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("PC reply OK.\n");
  update_stop();
  DISPLAY("\n");
  TRY(cable->close());

  return 0;
}

int ti92_recv_backup(FILE *file, int mask_mode, longword *version)
{
  byte data;
  word sum;
  word checksum;
  int i;
  int b;
  word block_size;
  word num_bytes;
  long offset1, offset2;
  char desc[43]="Backup file received by tilp";
  word file_checksum;
  longword index;
  int block;

  TRY(cable->open());
  update_start();
  fprintf(file, "**TI92**");
  fprintf(file, "%c%c", 0x01, 0x00);
  for(i=0; i<8; i++) fprintf(file, "%c", 0x00);
  for(i=0; i<40; i++) fprintf(file, "%c", desc[i]);
  fprintf(file, "%c%c", 0x01, 0x00);
  index=0x52;
  fprintf(file, "%c%c%c%c", (index & 0xFF),(index & 0x0000FF00)>>8, (index & 0x00FF0000)>>16, 
	  (index & 0xFF000000)>>24);
  offset1=ftell(file);
  for(i=0; i<8; i++) fprintf(file, "%c", 0x00);
  fprintf(file, "%c%c", TI92_BKUP, 0x00); 
  fprintf(file, "%c%c", 0x00, 0x00);
  offset2=ftell(file);
  fprintf(file, "%c%c%c%c", 0x00, 0x00, 0x00, 0x00);
  fprintf(file, "%c%c", 0xA5, 0x5A);

  sum=0;
  file_checksum=0;
  num_bytes=0;
  DISPLAY("Request backup...\n");
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_REQUEST));
  TRY(cable->put(0x11));
  TRY(cable->put(0x00));
  for(i=0; i<4; i++) { TRY(cable->put(0x00)); }
  TRY(cable->put(TI92_BKUP));
  sum+=0x1D;
  TRY(cable->put(0x0B));
  sum+=0x0B;
  TRY(ti92_sendstring("main\\backup", &sum));
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  TRY(ti92_isOK());

  for(block = 0; ; block++)
    {
      sprintf(update->label_text, "Receiving block %2i", block);      
      update_label(); //update_label();
      if(update->cancel) return -1;
      sum=0;
      TRY(cable->get(&data));
      if(data != TI92_PC) return 8;
      TRY(cable->get(&data));

      if(data == CMD92_EOT) break;
      else if(data != CMD92_VAR_HEADER) return 8;
      DISPLAY("The calculator is going to send block %i\n", block);

      TRY(cable->get(&data));
      if(data == 0x09) { b=0; } else { b=1; }
      TRY(cable->get(&data));
      TRY(cable->get(&data));
      block_size=data;
      sum+=data;
      TRY(cable->get(&data));
      block_size += (data << 8);
      sum+=data;
      for(i=0; i<4; i++)
	{
	  TRY(cable->get(&data));
	  sum+=data;
	}
      TRY(cable->get(&data));
      (*version)|=data;
      (*version) <<= 8;
      sum+=data;
      TRY(cable->get(&data));
      (*version)|=data;
      (*version) <<= 8;
      sum+=data;
      TRY(cable->get(&data));
      (*version)|=data;
      (*version) <<= 8;
      sum+=data;
      if(b == 1)
	{
	  TRY(cable->get(&data));
	  (*version)|=data;
	  sum+=data;
	}
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(checksum != sum) return 9;
      
      TRY(PC_replyOK_92());
      TRY(PC_waitdata_92());

      TRY(ti92_isOK());
      TRY(cable->get(&data));
      if(data != TI92_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD92_DATA_PART) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      block_size=data;
      TRY(cable->get(&data));
      block_size+=(data << 8);
      sum=0;
      TRY(cable->get(&data));
      sum+=data;
      TRY(cable->get(&data));
      sum+=data;
      TRY(cable->get(&data));
      sum+=data;
      TRY(cable->get(&data));
      sum+=data;
      DISPLAY("The computer is receiving block %i (%i bytes)\n", 
	      block, 
	      block_size-4);
      num_bytes+=block_size-4;
      for(i=0; i<block_size-4; i++)
	{
	  TRY(cable->get(&data));
	  sum+=data;
	  fprintf(file, "%c", data);
	  file_checksum+=data;
	}
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(sum != checksum) return 9;

      TRY(PC_replyOK_92());
    }
  DISPLAY("%i bytes of backup.\n", num_bytes);
  DISPLAY("ROM version %c%c%c%c\n", (*version & 0xFF000000)>>24, (*version & 0x00FF0000)>>16, (*version & 0x0000FF00)>>8, (*version & 0xFF));

  fprintf(file, "%c%c", LSB(file_checksum), MSB(file_checksum));
  fseek(file, offset1, SEEK_SET);
  fprintf(file, "%c%c%c%c", (*version & 0xFF000000)>>24, (*version & 0x00FF0000)>>16, (*version & 0x0000FF00)>>8, (*version & 0xFF));
  fseek(file, 0L, SEEK_END);
  fseek(file, offset2, SEEK_SET);
  num_bytes+=index+2; //  2 bytes of checksum
  fprintf(file, "%c%c%c%c", (num_bytes & 0xFF),(num_bytes & 0x0000FF00)>>8, (num_bytes & 0x00FF0000)>>16, (num_bytes & 0xFF000000)>>24);
  fseek(file, 0L, SEEK_END);
  DISPLAY("\n");
  update_stop();
  TRY(cable->close());

  return 0;
}

int ti92_send_backup(FILE *file, int mask_mode)
{
  byte data;
  word sum;
  char str[128];
  longword backup_size;
  longword block_size;
  int i, j;
  int num_blocks;
  char version[9];
  word last_block;
  longword index;

  TRY(cable->open());
  update_start();
  fgets(str, 9, file);
  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
	{
	  if( strcmp(str, "**TI92P*") && strcmp(str, "**TI89**") &&
	      strcmp(str, "**TI92**") )
	    { 
	      return ERR_INVALID_TIXX_FILE;
	    }
	}
      else if(mask_mode & MODE_FILE_CHK_ALL)
	{
	  if( strcmp(str, "**TI92**"))
	    {
	      return ERR_INVALID_TI92_FILE;
	    }
	}
    }
 
  for(i=0; i<2; i++) fgetc(file);
  for(i=0; i<8; i++) fgetc(file);
  for(i=0; i<40; i++) fgetc(file);
  for(i=0; i<2; i++) fgetc(file);
  index=(LSB(fgetc(file)))+(LSB(fgetc(file)) << 8)+
    (LSB(fgetc(file)) << 16)+(LSB(fgetc(file)) << 24);
  for(i=0; i<8; i++) version[i]=fgetc(file);
  version[8]='\0';
  for(i=0; i<2; i++) fgetc(file);
  for(i=0; i<2; i++) fgetc(file);
  backup_size=(LSB(fgetc(file)))+(LSB(fgetc(file)) << 8)+
    (LSB(fgetc(file)) << 16)+(LSB(fgetc(file)) << 24);
  for(i=0; i<2; i++) fgetc(file);
  backup_size-=index+2;
  DISPLAY("Sending backup...\n");
  DISPLAY("ROM version %s\n", version);
  DISPLAY("Size of backup: %i bytes.\n", backup_size);

  sum=0;
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_VAR_HEADER));
  block_size=4+2+strlen(version);
  TRY(cable->put(LSB(block_size)));
  TRY(cable->put(MSB(block_size)));
  data=backup_size & 0x000000FF;
  sum+=data;
  TRY(cable->put(data));
  data=(backup_size & 0x0000FF00) >> 8;
  sum+=data;
  TRY(cable->put(data));
  data=(backup_size & 0x00FF0000) >> 16;
  sum+=data;
  TRY(cable->put(data));
  data=(backup_size & 0xFF000000) >> 24;
  sum+=data;
  TRY(cable->put(data));
  data=TI92_BKUP;
  sum+=data;
  TRY(cable->put(data));
  data=strlen(version);
  sum+=data;
  TRY(cable->put(data));
  ti92_sendstring(version, &sum);
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  TRY(ti92_isOK());

  num_blocks=backup_size/1024;
  
  for(i=0; i<num_blocks; i++ )
    {
      DISPLAY("Sending block %i.\n", i);
      sum=0;
      TRY(cable->put(PC_TI92));
      TRY(cable->put(CMD92_VAR_HEADER));
      block_size=4+2+strlen(version);
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
      data=0x00;
      sum+=data;
      TRY(cable->put(data));
      data=0x04;
      sum+=data;
      TRY(cable->put(data));
      data=0x00;
      sum+=data;
      TRY(cable->put(data));
      data=0x00;
      sum+=data;
      TRY(cable->put(data));
      data=TI92_BKUP;
      sum+=data;
      TRY(cable->put(data));
      data=strlen(version);
      sum+=data;
      TRY(cable->put(data));
      ti92_sendstring(version, &sum);
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti92_isOK());
      TRY(ti92_waitdata());

      sum=0;
      TRY(PC_replyOK_92());
      TRY(cable->put(PC_TI92));
      TRY(cable->put(CMD92_DATA_PART));
      block_size=1024;
      update->total = 1024;
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
      DISPLAY("Transmitting data.\n");
      update->total = 1024;
      for(j=0; j<1024; j++)
 	{
	  data=fgetc(file);
	  sum+=data;
	  TRY(cable->put(data));
	  
	  update->count = j;
	  update->percentage = (float)j/1024;
	  update_pbar();
	  if(update->cancel) return -1;
	}
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));

      TRY(ti92_isOK());
      
      ((update->main_percentage))=(float)i/num_blocks;
      update_pbar();
      if(update->cancel) return -1;
    }

  DISPLAY("Sending the last block.\n");
  sum=0;
  last_block=backup_size%1024;
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_VAR_HEADER));
  block_size=4+2+strlen(version);
  TRY(cable->put(LSB(block_size)));
  TRY(cable->put(MSB(block_size)));
  data=LSB(last_block);
  sum+=data;
  TRY(cable->put(data));
  data=MSB(last_block);
  sum+=data;
  TRY(cable->put(data));
  data=0x00;
  sum+=data;
  TRY(cable->put(data));
  data=0x00;
  sum+=data;
  TRY(cable->put(data));
  data=TI92_BKUP;
  sum+=data;
  TRY(cable->put(data));
  data=strlen(version);
  sum+=data;
  TRY(cable->put(data));
  ti92_sendstring(version, &sum);
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  
  TRY(ti92_isOK());
  TRY(ti92_waitdata());
  
  sum=0;
  TRY(PC_replyOK_92());
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_DATA_PART));
  TRY(cable->put(LSB(last_block)));
  TRY(cable->put(MSB(last_block)));
  DISPLAY("Transmitting data.\n");
  update->total = last_block;
  for(j=0; j<last_block; j++)
    {
      data=fgetc(file);
      sum+=data;
      TRY(cable->put(data));
      update->count = j;
      update->percentage = (float)j/1024;
      update_pbar();
      if(update->cancel) return -1;
    }
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  
  TRY(ti92_isOK());

  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_EOT));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("Backup sent completely.\n");
  DISPLAY("\n");
  update_stop();
  TRY(cable->close());

  return 0;
}

int ti92_directorylist(struct varinfo *list, int *n_elts)
{
  byte data;
  word sum;
  word checksum;
  int i;
  word block_size;
  byte var_type;
  byte locked;
  longword var_size;
  byte name_length;
  char var_name[9];
  struct varinfo *p, *f;

  TRY(cable->open());
  *n_elts=0;
  update_start();
  p=list;
  f=NULL;  
  p->folder=f;
  p->is_folder = VARIABLE;
  p->next=NULL;
  p->folder=NULL;
  strcpy(p->varname, "");
  p->varsize=0;
  p->vartype=0;
  p->varlocked=0;
  strcpy(p->translate, "");
  
  sum=0;
  DISPLAY("Request directory list (dir)...\n");
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_REQUEST));
  TRY(cable->put(0x06));
  TRY(cable->put(0x00));
  for(i=0; i<4; i++)
    {
      TRY(cable->put(0x00));
    }
  TRY(cable->put(0x19));
  sum+=0x19;
  TRY(cable->put(0x00));
  sum+=0x00;
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  TRY(ti92_isOK());
  sum=0;
  TRY(cable->get(&data));
  if(data != TI92_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD92_VAR_HEADER) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  var_size=data;
  sum+=data;
  TRY(cable->get(&data));
  var_size |= (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  var_size |= (data << 16);
  sum+=data;
  TRY(cable->get(&data));
  var_size |= (data << 24);
  sum+=data;
  list->varsize=var_size;
  DISPLAY("Size of the var in memory: %08X.\n", var_size);
  TRY(cable->get(&data));
  list->vartype=data;
  sum+=data;
  DISPLAY("Ty: %02X\n", data);
  TRY(cable->get(&data));
  name_length=data;
  sum+=data;
  DISPLAY("Current directory: ");
  for(i=0; i<name_length; i++)
    {
      TRY(cable->get(&data));
      var_name[i]=data;
      sum+=data;
      DISPLAY("%c", data);
    }
  var_name[i]='\0';
  DISPLAY("\n");
  strcpy(list->varname, var_name);
  strncpy(list->translate, list->varname, 9); 
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;
  list->folder=NULL;
  
  for( ; ; )
    {
      sum=0;
      if( (p->next=(struct varinfo *)malloc(sizeof(struct varinfo))) == NULL)
	{
	  fprintf(stderr, "Unable to allocate memory.\n");
	  exit(0);
	}
      p=p->next;
      p->next=NULL;
      (*n_elts)++;
      strcpy(p->translate, "");
      TRY(PC_replyOK_92());
      TRY(PC_waitdata_92());

      TRY(ti92_isOK());
      TRY(cable->get(&data));
      if(data != TI92_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD92_DATA_PART) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      block_size=data;
      TRY(cable->get(&data));
      block_size += (data << 8);
      sum=0;
      for(i=0; i<4; i++)
        {
          TRY(cable->get(&data));
	  sum+=data;
	}
      for(i=0; i<8; i++)
	{
	  TRY(cable->get(&data));
          sum+=data;
	  var_name[i]=data;
	}
      var_name[i]='\0';      
      strcpy(p->varname, var_name);
      strncpy(p->translate, p->varname, 9);
      TRY(cable->get(&data));
      var_type=data;
      sum+=data;
      p->vartype=var_type;
      if(p->vartype == TI92_DIR)
	p->is_folder = FOLDER;
      else
	p->is_folder = VARIABLE;
      if(p->vartype==TI92_DIR)
	{
	  f=p;
	}
      p->folder=f;
      TRY(cable->get(&data));
      locked=data;
      sum+=data;
      p->varlocked=locked;
      TRY(cable->get(&data));
      var_size=data;
      sum+=data;
      TRY(cable->get(&data));
      var_size |= (data << 8);
      sum+=data;
      TRY(cable->get(&data));
      var_size |= (data << 16);
      sum+=data;
      TRY(cable->get(&data));
      var_size |= (data << 24);
      sum+=data;
      p->varsize=var_size;
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(checksum != sum) return ERR_CHECKSUM;
      DISPLAY("Name: %8s | ", var_name);
      DISPLAY("Type: %8s | ", ti92_byte2type(var_type));
      DISPLAY("Attr: %i | ", locked);
      DISPLAY("Size: %08X\n", var_size);
      if(p->is_folder == VARIABLE)
	list->varsize += var_size;
      
      TRY(PC_replyOK_92());
      TRY(cable->get(&data));
      if(data != TI92_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data == CMD92_EOT) break;
      else if(data != CMD92_CONTINUE) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      TRY(cable->get(&data));
      DISPLAY("The calculator want continue.\n");

      sprintf(update->label_text, "Reading of: TI92/%s/%s", 
	       (p->folder)->translate, p->translate);
      update_label();
      if(update->cancel) return -1;
   }
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  TRY(PC_replyOK_92());
  DISPLAY("\n");
  update_stop();
  TRY(cable->close());

  return 0;
}

int ti92_recv_var(FILE *file, int mask_mode, 
		     char *varname, byte vartype, byte varlock)
{
  byte data;
  word sum;
  word checksum;
  word block_size;
  int i;
  longword var_size;
  byte name_length;
  char name[9];

  update_stop();
  TRY(cable->open());
  sprintf(update->label_text, "Variable: %s", varname);
  update_label();
  sum=0;
  DISPLAY("Request variable: %s\n", varname);
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_REQUEST));
  block_size=4+2+strlen(varname);
  data=LSB(block_size);
  TRY(cable->put(data));
  data=MSB(block_size);
  TRY(cable->put(data));
  for(i=0; i<4; i++)
    {
      TRY(cable->put(0x00));
    }
  data=vartype;
  TRY(cable->put(data));
  sum+=data;
  data=strlen(varname);
  TRY(cable->put(data));
  sum+=data;
  TRY(ti92_sendstring(varname, &sum));
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  
  TRY(ti92_isOK());
  sum=0;
  TRY(cable->get(&data));
  if(data != TI92_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD92_VAR_HEADER) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  var_size=data;
  sum+=data;
  TRY(cable->get(&data));
  var_size |= (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  var_size |= (data << 16);
  sum+=data;
  TRY(cable->get(&data));
  var_size |= (data << 24);
  sum+=data;
  DISPLAY("Size of the var in memory: 0x%08X = %i.\n", var_size-2, var_size-2);
  TRY(cable->get(&data));
  DISPLAY("Type of the variable: %s\n", ti92_byte2type(data));
  sum+=data;
  TRY(cable->get(&data));
  name_length=data;
  sum+=data;
  DISPLAY("Variable name: ");
  for(i=0; i<name_length; i++)
    {
      TRY(cable->get(&data));
      name[i]=data;
      DISPLAY("%c", data);
      sum+=data;
    }
  name[i]='\0';
  DISPLAY("\n");
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  TRY(PC_replyOK_92());
  TRY(PC_waitdata_92());
  DISPLAY("The calculator want continue.\n");

  TRY(ti92_isOK());
  DISPLAY("Receiving variable...\n");
  TRY(cable->get(&data));
  if(data != TI92_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD92_DATA_PART) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  sum=0;
  for(i=0; i<4; i++)
    {
      TRY(cable->get(&data));
      sum+=data;
      fprintf(file, "%c", data);
    }
  TRY(cable->get(&data));
  block_size = (data << 8);
  sum+=data;
  fprintf(file, "%c", data);
  TRY(cable->get(&data));
  block_size |= data;
  sum+=data;
  fprintf(file, "%c", data);
  update->total = block_size;
  for(i=0; i<block_size; i++)
    {
      TRY(cable->get(&data));
      sum+=data;
      fprintf(file, "%c", data);

      update->count = i;
      update->percentage = (float)i/block_size;
      update_pbar();
      if(update->cancel) return -1;
    }
  TRY(cable->get(&data));
  checksum=data;
  fprintf(file, "%c", data);
  TRY(cable->get(&data));
  checksum += (data << 8);
  fprintf(file, "%c", data);
  if(checksum != sum) return ERR_CHECKSUM;
  TRY(PC_replyOK_92());
  TRY(cable->get(&data));
  if(data != TI92_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  DISPLAY("The calculator do not want continue.\n");
  TRY(cable->get(&data));
  TRY(cable->get(&data));

  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_TI_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("\n");

  update_stop();
  TRY(cable->close());
  PAUSE(pause_between_vars);

  return 0;
}

int ti92_send_var(FILE *file, int mask_mode)
{
  byte data;
  word sum;
  word block_size = 0;
  int i;
  int j;
  int k;
  longword varsize;
  byte vartype;
  char foldname[18];
  char varname[18];

  char str[128];
  int num_vars_in_folder;
  int num_entries;
  int var_index;
  static int num_vars;
  static char **t_varname=NULL;
  static byte *t_vartype=NULL;
  static struct varinfo dirlist;
  int n;
  int err;
  int exist=0;
  int action=ACTION_NONE;
  static int do_dirlist=1;

  if((mask_mode & MODE_DIRLIST) && do_dirlist) // do dirlist one time
    {
      TRY(ti92_directorylist(&dirlist, &n));
      do_dirlist=0;
    }

  if((mask_mode & MODE_SEND_LAST_VAR) ||
     (mask_mode & MODE_SEND_ONE_VAR)) do_dirlist=1;

  if(t_vartype != NULL) { free(t_vartype); t_vartype=NULL; }
  if(t_varname != NULL)
    {
      for(i=0; i<num_vars; i++) free(t_varname[i]); 
      t_varname=NULL;
      num_vars=0;
    }
  num_vars=0;
  var_index=0;
  TRY(cable->open());
  update_start();
  fgets(str, 9, file);
  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
	{
	  if( strcmp(str, "**TI92P*") && strcmp(str, "**TI89**") &&
	      strcmp(str, "**TI92**") )
	    { 
	      return ERR_INVALID_TIXX_FILE;
	    }
	}
      else if(mask_mode & MODE_FILE_CHK_ALL)
	{
	  if( strcmp(str, "**TI92**"))
	    {
	      return ERR_INVALID_TI92_FILE;
	    }
	}
    }

  for(i=0; i<2; i++) fgetc(file);
  for(i=0; i<8; i++) foldname[i]=fgetc(file);
  foldname[i]='\0';
  for(i=0; i<40; i++) fgetc(file);
  num_entries=fgetc(file);
  num_entries+=fgetc(file) << 8;

  if(num_entries == 1)
    { // normal file, not a group file
      t_varname=(char **)malloc(sizeof(char *));
      if(t_varname == NULL) return 40; 
      t_varname[0]=(char *)malloc(18*sizeof(char));
      if(t_varname[0] == NULL) return 40;
      t_vartype=(byte *)malloc(sizeof(byte));
      if(t_vartype == NULL) return 40; 
      num_vars=1;
      for(i=0; i<4; i++) fgetc(file);
      for(i=0; i<8; i++) t_varname[var_index][i]=fgetc(file);
      t_varname[var_index][i]='\0';
      if(! (mask_mode & MODE_LOCAL_PATH))
	{ // full path
	  strcat(foldname, "\\");
	  strcat(foldname, t_varname[var_index]);
	  strcpy(t_varname[var_index], foldname);
	}
      t_vartype[var_index]=fgetc(file);
      fgetc(file);
      for(i=0; i<2; i++) fgetc(file);
    }
  else
  { // group file
    k=0;
    while(k<num_entries)
      {
	k++;
	for(i=0; i<4; i++) fgetc(file);
	for(i=0; i<8; i++) foldname[i]=fgetc(file);
	foldname[i]='\0';
	vartype=fgetc(file);
	fgetc(file);
	num_vars_in_folder=fgetc(file);
	num_vars_in_folder+=fgetc(file) << 8;
	num_vars+=num_vars_in_folder;
        t_vartype=(byte *)realloc(t_vartype, (num_vars+1)*sizeof(byte));
        if(t_vartype == NULL) return 40;
	t_varname=(char **)realloc(t_varname, (num_vars+1)*sizeof(char *));
	if(t_varname == NULL) return 40;

	for(j=0; j<num_vars_in_folder; j++)
	  { 
	    t_varname[var_index]=(char *)malloc(18*sizeof(char));
	    if(t_varname[var_index] == NULL) return 40; 	
	    k++;
	    for(i=0; i<4; i++) fgetc(file);
	    for(i=0; i<8; i++) varname[i]=fgetc(file);
	    varname[i]='\0';
	    vartype=fgetc(file);
	    fgetc(file);
	    for(i=0; i<2; i++) fgetc(file);
	    if(! (mask_mode & MODE_LOCAL_PATH))
	      {
		strcpy(t_varname[var_index], foldname);
		strcat(t_varname[var_index], "\\");
		strcat(t_varname[var_index], varname); 	    
	      }
	    else
	      strcpy(t_varname[var_index], varname);
	    t_vartype[var_index]=vartype;
	    var_index++;
	  }
      }
    var_index=0;
  }
  for(i=0; i<4; i++) fgetc(file);
  for(i=0; i<2; i++) fgetc(file);
  /*
  for(i=0; i<num_vars; i++)
    {
      DISPLAY("-> <%8s> <%02X>\n", t_varname[i], t_vartype[i]);
    }
  */
  for(j=0; j<num_vars; j++, var_index++)
    {
      for(i=0; i<4; i++) fgetc(file);
      varsize=fgetc(file) << 8;
      varsize+=fgetc(file);
      varsize+=2;
      
      sprintf(update->label_text, "Variable: %s", 
	      t_varname[var_index]);
      update_label();
      DISPLAY("Sending variable...\n");
      DISPLAY("Name: %s\n", t_varname[var_index]);
      DISPLAY("Size: %08X\n", varsize-2);
      DISPLAY("Type: %s\n", ti92_byte2type(t_vartype[var_index]));
      
      /**/
      exist=check_if_var_exist(&dirlist, t_varname[var_index]);
      if(exist && (mask_mode & MODE_DIRLIST))
        {
          action=update_choose(t_varname[var_index], varname);
          if(!strcmp(varname, "") && (action==ACTION_RENAME))
            action=ACTION_SKIP;
          switch(action)
            {
            case ACTION_SKIP: // skip var
              DISPLAY("User action: skip.\n");
	      for(i=0; i<varsize; i++)
                data=fgetc(file);
              continue;
              break;
            case ACTION_OVERWRITE: //try to overwrite it
              DISPLAY("User action: overwrite.\n");
              break;
            case ACTION_RENAME: //rename var
              DISPLAY("User action: rename from %s into %s.\n",
                      t_varname[var_index], varname);
              strcpy(t_varname[var_index], varname);
              break;
            default: // skip
              for(i=0; i<block_size+2; i++)
                data=fgetc(file);
              continue;
              break;
            }
        }
      /**/

      sum=0;
      TRY(cable->put(PC_TI92));
      TRY(cable->put(CMD92_VAR_HEADER));
      block_size=4+2+strlen(t_varname[var_index]);
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
      data=varsize & 0x000000FF;
      sum+=data;
      TRY(cable->put(data));
      data=(varsize & 0x0000FF00) >> 8;
      sum+=data;
      TRY(cable->put(data));
      data=(varsize & 0x00FF0000) >> 16;
      sum+=data;
      TRY(cable->put(data));
      data=(varsize & 0xFF000000) >> 24;
      sum+=data;
      TRY(cable->put(data));
      data=t_vartype[var_index];
      sum+=data;
      TRY(cable->put(data));
      data=strlen(t_varname[var_index]);
      sum+=data;
      TRY(cable->put(data));
      TRY(ti92_sendstring(t_varname[var_index], &sum));
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti92_isOK());
      TRY(ti92_waitdata());
      
      sum=0;
      TRY(PC_replyOK_92());
      TRY(cable->put(PC_TI92));
      TRY(cable->put(CMD92_DATA_PART));
      block_size=4+varsize;
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
      for(i=0; i<4; i++)
	{
	  TRY(cable->put(0x00));
	}
      block_size=varsize-2;
      data=MSB(block_size);
      sum+=data;
      TRY(cable->put(data));
      data=LSB(block_size);
      sum+=data;
      TRY(cable->put(data));
      update->total = block_size;
      for(i=0; i<block_size; i++)
	{
	  data=fgetc(file);
	  TRY(cable->put(data));
	  sum+=data;
	  
	  update->count = i;
	  update->percentage = (float)i/block_size;
	  update_pbar();
	  if(update->cancel) return -1;
	}
      fgetc(file); //skips checksum
      fgetc(file);
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti92_isOK());
      TRY(cable->put(PC_TI92));
      TRY(cable->put(CMD92_EOT));
      TRY(cable->put(0x00));
      TRY(cable->put(0x00));
      DISPLAY("The computer does not want continue.\n");
      err = ti92_isOK();
      if(err)
        {
          DISPLAY("Variable has been rejected by calc.\n");
          sprintf(update->label_text, _("Variable rejected"));
          update_label();
        }
      DISPLAY("\n");
      PAUSE(pause_between_vars);
    }
  update_stop();
  TRY(cable->close());

  return 0;
}

#define DUMP_ROM92_FILE "dumprom.92p"

int ti92_dump_rom(FILE *file, int mask_mode)
{
  int i, j;
  int total;
  byte data;
  time_t start, elapsed, estimated, remaining;
  char buffer[MAXCHARS];
  char tmp[MAXCHARS];
  int pad;
  FILE *f;
  word checksum, sum;

  update_start();
  sprintf(update->label_text, "Ready ?");
  update_label();

  /* Open connection and check */
  TRY(cable->open());
  TRY(ti92_isready());
  TRY(cable->close());
  sprintf(update->label_text, "Yes !");
  update_label();

  /* Transfer ROM dump program from lib to calc */
  f = fopen(DUMP_ROM92_FILE, "wb");
  if(f == NULL)
    return -1;
  fwrite(romDump92f2, sizeof(unsigned char), 
	 romDumpSize92f2, f);
  fclose(f);
  
  f = fopen(DUMP_ROM92_FILE, "rb");
  TRY(ti92_send_var(f, MODE_NORMAL));
  fclose(f);      
  unlink(DUMP_ROM92_FILE);

  /* Launch calculator program by remote control */
  sprintf(update->label_text, "Launching...");
  update_label();

  TRY(ti92_send_key(KEY92_CLEAR));
  PAUSE(50);
  TRY(ti92_send_key(KEY92_CLEAR));
  PAUSE(50);
  TRY(ti92_send_key(KEY92_m));
  TRY(ti92_send_key(KEY92_a));
  TRY(ti92_send_key(KEY92_i));
  TRY(ti92_send_key(KEY92_n));
  TRY(ti92_send_key(KEY92_BSLASH));
  TRY(ti92_send_key(KEY92_d));
  TRY(ti92_send_key(KEY92_u));
  TRY(ti92_send_key(KEY92_m));
  TRY(ti92_send_key(KEY92_p));
  TRY(ti92_send_key(KEY92_r));
  TRY(ti92_send_key(KEY92_o));
  TRY(ti92_send_key(KEY92_m));
  TRY(ti92_send_key(KEY92_LP));
  TRY(ti92_send_key(KEY92_RP));
  TRY(ti92_send_key(KEY92_ENTER));

  /* Receive it now blocks per blocks (1024 + CHK) */
  update_start();
  sprintf(update->label_text, "Receiving...");
  update_label();

  start = time(NULL);
  total = mask_mode * 1024 * 1024;
  update->total = total;

  for(i=0; i<mask_mode*1024; i++)
    {
      sum = 0;
      for (j=0; j<1024; j++)
	{
	  TRY(cable->get(&data));
	  fprintf(file, "%c", data);
	  sum += data;

          update->percentage = (float)j/1024;
          update_pbar();
          if(update->cancel) return -1;
	}
      TRY(cable->get(&data));
      checksum = data << 8;
      TRY(cable->get(&data));
      checksum |= data;
      if(sum != checksum) return ERR_CHECKSUM;
      TRY(cable->put(0xda));

      update->count = 1024*mask_mode;
      update->main_percentage = (float)i/(1024*mask_mode);
      if(update->cancel) return -1;

      elapsed = (long)difftime(time(NULL), start);
      estimated = (long)(elapsed * (float)(1024*mask_mode) / i);
      remaining = (long)difftime(estimated, elapsed);
      sprintf(buffer, "%s", ctime(&remaining));
      sscanf(buffer, "%3s %3s %i %s %i", tmp,
	     tmp, &pad, tmp, &pad);
      sprintf(update->label_text, "Remaining (mm:ss): %s", tmp+3);
      update_label();
    }

  /* Close connection */
  TRY(cable->close());

  return 0;
}

int ti92_get_rom_version(char *version)
{
  byte data;
  word sum;
  word checksum;
  int i;
  int b;
  word block_size;
  word num_bytes;

  TRY(cable->open());
  update_start();

  /* Check if TI is ready*/
  TRY(ti92_isready());  
  
  sum=0;
  num_bytes=0;
  DISPLAY("Request backup...\n");
  /* Request a backup */
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_REQUEST));
  TRY(cable->put(0x12));
  TRY(cable->put(0x00));
  for(i=0; i<4; i++) { TRY(cable->put(0x00)); }
  TRY(cable->put(TI92_BKUP));
  sum+=0x1D;
  TRY(cable->put(0x0C));
  sum+=0x0C;
  TRY(ti92_sendstring("main\\version", &sum));
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  /* Check if TI replies OK */
  TRY(ti92_isOK());
  
  /* Receive the ROM version */
  sum=0;
  TRY(cable->get(&data));
  if(data != TI92_PC) return 8;
  TRY(cable->get(&data));
  
  if(data != CMD92_VAR_HEADER) return 8;
  
  TRY(cable->get(&data));
  if(data == 0x09) { b=0; } else { b=1; }
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  block_size=data;
  sum+=data;
  TRY(cable->get(&data));
  block_size += (data << 8);
  sum+=data;
  for(i=0; i<4; i++)
    {
      TRY(cable->get(&data));
      sum+=data;
    }
  TRY(cable->get(&data));
  version[0]=data;
  sum+=data;
  TRY(cable->get(&data));
  version[1]=data;
  sum+=data;
  TRY(cable->get(&data));
  version[2]=data;
  version[3]='\0';
  sum+=data;
  if(b == 1)
    {
      TRY(cable->get(&data));
      version[3]=data;
      version[4]='\0';
      sum+=data;
    }
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return 9;
  
  /* Abort transfer */  
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_CHK_ERROR));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  TRY(cable->put(PC_TI92));
  TRY(cable->put(CMD92_CHK_ERROR));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("ROM version %s\n", version);
  
  DISPLAY("\n");
  update_stop();
  TRY(cable->close());	
  
  return 0;
}

int ti92_send_flash(FILE *file, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}

int ti92_recv_flash(FILE *file, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}

int ti92_get_idlist(char *id)
{
  return ERR_VOID_FUNCTION;
}
