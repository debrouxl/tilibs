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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "calc_err.h"
#include "calc_ext.h"
#include "defs89.h"
#include "keys89.h"
#include "group.h"
#include "rom89.h"
#include "pause.h"
#include "update.h"

#ifdef HAVE_CURSES_H
#include <curses.h>
#endif

/* Functions used by TI_PC functions */

// The PC indicates that is OK
int PC_replyOK_89(void)
{
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_TI_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer reply OK.\n");

  return 0;
}

int PC_replyCONT_89(void)
{
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_CONTINUE));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer reply OK.\n");

  return 0;
}

// The PC indicates that it is ready or wait data
int PC_waitdata_89(void)
{
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_WAIT_DATA));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer wait data.\n");
  
  return 0;
}

// Check whether the TI reply OK
int ti89_isOK(void)
{
  byte data;

  TRY(cable->get(&data));
  if(data != TI89_PC) 
    {
      return ERR_INVALID_BYTE;
    }
  TRY(cable->get(&data));
  if(data != CMD89_TI_OK)
    { 
      if(data == CMD89_CHK_ERROR) return ERR_CHECKSUM;
      else return ERR_INVALID_BYTE;
    }
  TRY(cable->get(&data));
  if(data != 0x00)
    { 
      // FLASHed calcs reply in a different way
      //return 4;
    }
  TRY(cable->get(&data));
  if((data&1) != 0)
    { 
      return ERR_NOT_REPLY;
    }

  DISPLAY("The calculator reply OK.\n");

  return 0;
}

// The TI indicates that it is ready or wait data
int ti89_waitdata(void)
{
  byte data;

  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD89_WAIT_DATA) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  //if(data != 0x00) return 16;
  TRY(cable->get(&data));
  //if(data != 0x00) return 16;
  DISPLAY("The calculator wait data.\n");

  return 0;
}

// Check whether the TI reply that it is ready
int ti89_isready(void)
{
  TRY(cable->open_port());
  DISPLAY("Is calculator ready ?\n");
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_ISREADY));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  TRY(ti89_isOK());
  DISPLAY("The calculator is ready.\n");
  DISPLAY("\n");
  TRY(cable->close_port());

  return 0;
}

// Send a string of characters to the TI
int ti89_sendstring(char *s, word *checksum)
{
  int i;
  
  for(i=0; i<strlen(s); i++)
    {
      TRY(cable->put(s[i]));
      (*checksum) += (s[i] & 0xff);
    }

  return 0;
}

#define TI89_MAXTYPES 48
const char *TI89_TYPES[TI89_MAXTYPES]=
{ 
"EXPR", "UNKNOWN", "UNKNOWN", "UNKNOWN", "LIST", "UNKNOWN", "MAT", "UNKNOWN", 
"UNKNOW", "UNKNOWN", "DATA", "TEXT", "STR", "GDB", "FIG", "UNKNOWN",
"PIC", "UNKNOWN", "PRGM", "FUNC", "MAC", "UNKNOWN", "UNKNOWN", "UNKNOWN",
"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "ZIP", "BACKUP", "UNKNOWN", "DIR",
"UNKNOWN", "ASM", "IDLIST", "UNKNOWN", "FLASH", "UNKNOWN", "LOCKED", "ARCHIVED",
"UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN", "UNKNOWN"
};
const char *TI89_EXT[TI89_MAXTYPES]=
{
"89e", "unknown", "unknown", "unknown", "89l", "unknown", "89m", "unknown",
"unknown", "unknown", "89c", "89t", "89s", "89d", "89a", "unknown",
"89i", "unknown", "89p", "89f", "89x", "unknown", "unknown", "unknown",
"unknown", "unknown", "unknown", "unknown", "unknown", "89b", "unknown", "unknown",
"unknown", "89z", "unknown", "unknown", "89k", "unknown", "unknown", "unknown",
"unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown", "unknown"
};

// Return the type corresponding to the value
const char *ti89_byte2type(byte data)
{
  if(data>TI89_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return "UNKNOWN";
    }
  else 
    {
      if(!strcmp(TI89_TYPES[data], "UNKNOWN"))
	fprintf(stderr, "Unknown type. Please report it. Type: %02X\n", data);
      return TI89_TYPES[data];
    }
}

// Return the value corresponding to the type
byte ti89_type2byte(char *s)
{
  int i;

  for(i=0; i<TI89_MAXTYPES; i++)
    {
      if(!strcmp(TI89_TYPES[i], s)) break;
    }
  if(i>TI89_MAXTYPES)
    {
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti89_byte2fext(byte data)
{
  if(data>TI89_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.\n");    
      return ".89?";
    }
  else 
  {
    return TI89_EXT[data];
  }
}

// Return the value corresponding to the file extension
byte ti89_fext2byte(char *s)
{
  int i;

  for(i=0; i<TI89_MAXTYPES; i++)
    {
      if(!strcmp(TI89_EXT[i], s)) break;
    }
  if(i > TI89_MAXTYPES)
    {
      printf("Warning: unknown type. It is a bug. Please report this information.\n");
      return 0;
    }

  return i;
}

// General functions

int ti89_send_key(int key)
{
  TRY(cable->open_port());
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_DIRECT_CMD));
  TRY(cable->put(LSB(key)));
  TRY(cable->put(MSB(key)));
  TRY(ti89_isOK());
  TRY(cable->close_port());

  return 0;
}

int ti89_remote_control(void)
{
#if defined(HAVE_CURSES_H) && defined(HAVE_LIBCURSES)
  int c;
  word d;
  int sp_key;
  char skey[10];
  int b;

  TRY(cable->open_port());
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
	  sp_key=KEY89_CTRL;
	  strcpy(skey, "DIAMOND ");
	  b=1;
	  c=getch();
	}
      if(c==178)
        {
          sp_key=KEY89_2ND;
	  strcpy(skey, "2nd ");
	  b=1;
	  c=getch();
        }
      if(c>31 && c<128)
	{
	  DISPLAY("Sending <%s%c>\n", skey, c);
	  TRY(cable->put(PC_TI89));
	  TRY(cable->put(CMD89_DIRECT_CMD));
	  c+=sp_key;
	  DISPLAY("%i\n", c);
	  TRY(cable->put(LSB(c)));
	  TRY(cable->put(MSB(c)));
	  TRY(ti89_isOK());
	  refresh();
	}
      else
	{
	  if(c==ESC)
            {
              d=KEY89_ESC;
              DISPLAY("Sending %sESC key\n", skey);
            }
	  if(c==BS)
            {
              d=KEY89_BS;
              DISPLAY("Sending %s<- key\n", skey);
            }
	  if(c==F1) 
	    {
	      d=KEY89_F1;
	      DISPLAY("Sending %sF1 key\n", skey);	      
	    }
	  if(c==F2) 
	    {
	      d=KEY89_F2;
	      DISPLAY("Sending %sF2 key\n", skey);
	    }
          if(c==F3) 
	    {
	      d=KEY89_F3;
	      DISPLAY("Sending %sF3 key\n", skey);
	    }
          if(c==F4) 
	    {
	      d=KEY89_F4;
	      DISPLAY("Sending %sF4 key\n", skey);
	    }
          if(c==F5) 
	    {
	      d=KEY89_F5;
	      DISPLAY("Sending %sF5 key\n", skey);
	    }
          if(c==F6) 
	    {
	      d=KEY89_F6;
	      DISPLAY("Sending %sF6 key\n", skey);
	    }
	  if(c==F7) 
	    {
	      d=KEY89_F7;
	      DISPLAY("Sending %sF7 key\n", skey);
	    }
          if(c==F8) 
	    {
	      d=KEY89_F8;
	      DISPLAY("Sending %sF8 key\n", skey);
	    }
          if(c==F9) 
	    {
	      d=KEY89_APPS;
	      DISPLAY("Sending %sAPPS key\n", skey);
	    }
          if(c==F10) 
	    {
	      d=KEY89_STO;
	      DISPLAY("Sending %sSTO key\n", skey);
	    }
          if(c==F11) 
	    {
	      d=KEY89_MODE;
	      DISPLAY("Sending %sMODE key\n", skey);
	    }
          if(c==F12) 
	    {
	      d=KEY89_CLEAR;
	      DISPLAY("Sending %sCLEAR key\n", skey);
	    }
	  if(c==CALC_CR || c==CALC_LF) 
	    {
	      d=KEY89_CR;
	      DISPLAY("Sending %sENTER key\n", skey);
	    }
	  d+=sp_key;
	  TRY(cable->put(PC_TI89));
          TRY(cable->put(CMD89_DIRECT_CMD));
          TRY(cable->put(LSB(d)));
          TRY(cable->put(MSB(d)));
          TRY(ti89_isOK());
	  refresh();
	}
    }
  while(c!=END);
  noraw();
  endwin();
  TRY(cable->close_port());

  return 0;
#else
  return 25;
#endif
}

int ti89_screendump(byte **bitmap, int mask_mode,
                         struct screen_coord *sc)
{
  byte data;
  word max_cnt;
  word sum;
  word checksum;
  int i, j, k;

  TRY(cable->open_port());
  update_start();
  sc->width=TI89_COLS;
  sc->height=TI89_ROWS;
  sc->clipped_width = TI89_COLS_VISIBLE;
  sc->clipped_height = TI89_ROWS_VISIBLE;
  if(*bitmap != NULL)
    free(*bitmap);
  (*bitmap)=(byte *)malloc(TI89_COLS*TI89_ROWS*sizeof(byte)/8);
  if((*bitmap) == NULL)
    {
      fprintf(stderr, "Unable to allocate memory.\n");
      exit(0);
    }

  sum=0;
  DISPLAY("Requesting screendump.\n");
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  TRY(ti89_isOK());

  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD89_DATA_PART) return ERR_INVALID_BYTE;  
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
      sum += data;

      update->count = i;
      update->percentage = (float)i/max_cnt;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
  
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  //if(sum != checksum) return ERR_CHECKSUM;

  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_PC_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("PC reply OK.\n");
  DISPLAY("\n");
  (update->percentage)=0.0;
  TRY(cable->close_port());
  
  /* Clip the unused part of the screen (nethertheless useable via ASM) */
  
  if(mask_mode == CLIPPED_SCREEN)
    {
      i=0;
      for(j=0; j<TI89_ROWS_VISIBLE; j++)
	{
	  for(k=0; k<(TI89_COLS_VISIBLE >> 3); k++)
	    {
	      (*bitmap)[i++] = (*bitmap)[j*(TI89_COLS >> 3)+k];
	    }
	}
    }
  
  return 0;
}

int ti89_directorylist(struct varinfo *list, int *n_elts)
{
  byte data;
  word sum;
  word checksum;
  int i, j;
  word block_size;
  byte var_type;
  byte locked;
  longword var_size;
  byte name_length;
  char var_name[9];
  struct varinfo *p, *f;
  struct varinfo *q, *tmp;
  byte num_var;

  TRY(cable->open_port());
  update_start();
  *n_elts=0;
  p=list;
  p->next=NULL;
  f=NULL;
  p->folder=f;
  p->is_folder = VARIABLE;
  sum=0;
  DISPLAY("Requesting directory list (dir)...\n");
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_REQUEST));
  TRY(cable->put(0x06));
  TRY(cable->put(0x00));
  for(i=0; i<3; i++)
    {
      TRY(cable->put(0x00));
    }
  data=TI89_DIR;
  sum+=data;
  TRY(cable->put(data));
  data=TI89_DIRL;
  sum+=data;
  TRY(cable->put(data));
  TRY(cable->put(0x00));
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  TRY(ti89_isOK());
  sum=0;
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD89_VAR_HEADER) return ERR_INVALID_BYTE;
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
  //  DISPLAY("Size of the var in memory: %08X.\n", var_size);
  TRY(cable->get(&data));
  list->vartype=data;
  sum+=data;
  //  DISPLAY("Ty: %02X\n", data);
  TRY(cable->get(&data));
  name_length=data;
  sum+=data;
  //  DISPLAY("Current directory: ");
  for(i=0; i<name_length; i++)
    {
      TRY(cable->get(&data));
      var_name[i]=data;
      sum+=data;
      //DISPLAY("%c", data);
    }
  var_name[i]='\0';
  //  DISPLAY("\n");
  strcpy(list->varname, var_name);
  strncpy(list->translate, list->varname, 9);
  list->folder=NULL;
  TRY(cable->get(&data));
  sum+=data;
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;
  TRY(PC_replyOK_89());
  TRY(PC_waitdata_89());

  TRY(ti89_isOK());
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD89_DATA_PART) return ERR_INVALID_BYTE;
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
  block_size=block_size-4;
  for(j=0; j<block_size/14; j++)
    {
      if( (p->next=(struct varinfo *)malloc(sizeof(struct varinfo))) == NULL)
        {
          fprintf(stderr, "Unable to allocate memory.\n");
          exit(0);
        }
      p=p->next;
      p->next=NULL;
      (*n_elts)++;
      //strcpy(p->varname, "");
      p->is_folder = FOLDER;

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
      // See the doc: PROTOCOL.89
      //      var_size |= (data << 24);
      sum+=data;
      p->varsize=var_size;
      DISPLAY("Name: %8s | ", var_name);
      DISPLAY("Type: %8s\n", ti89_byte2type(var_type));
      //      DISPLAY("Type: %8s | ", ti89_byte2type(var_type));
      //      DISPLAY("Locked: %i | ", locked);
      //      DISPLAY("Size: %08X\n", var_size);
      p->folder=p;
    }
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  TRY(PC_replyOK_89());
  sprintf(update->label_text, "Reading of directory: TI89/%s", 
	   p->translate);
  update->label();
  if(update->cancel) return ERR_ABORT;
  
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  DISPLAY("The calculator do not want continue.\n");

  TRY(PC_replyOK_89());

  ////////

  q=list->next;
  do
    {
      tmp=q->next;
      DISPLAY("Requesting local directory list in %8s...\n", q->varname);
      p=q;
      num_var=0;
      sum=0;
      TRY(cable->put(PC_TI89));
      TRY(cable->put(CMD89_REQUEST));
      block_size=4+2+strlen(q->varname);
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
      for(i=0; i<3; i++)
	{
	  TRY(cable->put(0x00));
	}
      data=TI89_LDIR;
      sum+=data;
      TRY(cable->put(data));
      data=TI89_DIRL;
      sum+=data;
      TRY(cable->put(data));
      data=strlen(q->varname);
      sum+=data;
      TRY(cable->put(data));
      TRY(ti89_sendstring(q->varname, &sum));
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));

      TRY(ti89_isOK());

      sum=0;
      TRY(cable->get(&data));
      if(data != TI89_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD89_VAR_HEADER) return ERR_INVALID_BYTE;
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
      TRY(cable->get(&data));
      sum+=data;
      TRY(cable->get(&data));
      name_length=data;
      sum+=data;
      for(i=0; i<name_length; i++)
	{
	  TRY(cable->get(&data));
	  var_name[i]=data;
	  sum+=data;
	}
      var_name[i]='\0';
      TRY(cable->get(&data));
      sum+=data;
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(checksum != sum) return ERR_CHECKSUM;
      TRY(PC_replyOK_89());
      TRY(PC_waitdata_89());

      TRY(ti89_isOK());
      TRY(cable->get(&data));
      if(data != TI89_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD89_DATA_PART) return ERR_INVALID_BYTE;
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
      block_size-=4;
      for(i=0; i<14; i++)
        {
          TRY(cable->get(&data));
          sum+=data;
        }
      block_size-=14;
      for(j=0; j<block_size/14; j++)
	{ 
	  if( (q->next=(struct varinfo *)malloc(sizeof(struct varinfo))) == NULL)
	    {
	      fprintf(stderr, "Unable to allocate memory.\n");
	      exit(0);
	    }
	  q=q->next;
	  (*n_elts)++;
	  num_var++;
	  //strcpy(p->varname, "");
	  q->is_folder = VARIABLE;
	  
	  for(i=0; i<8; i++)
	    {
	      TRY(cable->get(&data));
	      sum+=data;
	      var_name[i]=data;
	    }
	  var_name[i]='\0';
	  strcpy(q->varname, var_name);
	  strncpy(q->translate, q->varname, 9);
	  TRY(cable->get(&data));
	  var_type=data;
	  sum+=data;
	  q->vartype=var_type;
	  TRY(cable->get(&data));
	  locked=data;
	  sum+=data;
	  q->varlocked=locked;
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
	  // See the doc: PROTOCOL.89
	  //	  var_size |= (data << 24);
	  sum+=data;
	  q->varsize=var_size;
	  DISPLAY("Name: %8s | ", var_name);
	  DISPLAY("Type: %8s | ", ti89_byte2type(var_type));
	  DISPLAY("Locked: %i | ", locked);
	  DISPLAY("Size: %08X\n", var_size);
	  q->folder=p;
	  sprintf(update->label_text, "Reading of: TI89/%s/%s", 
		   (q->folder)->translate, q->translate);
	  update->label();	  
	  if(update->cancel) return ERR_ABORT;
	}
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(checksum != sum) return ERR_CHECKSUM;
      
      TRY(PC_replyOK_89());
      
      TRY(cable->get(&data));
      if(data != TI89_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      TRY(cable->get(&data));
      TRY(cable->get(&data));
      DISPLAY("The calculator do not want continue.\n");
      
      TRY(PC_replyOK_89());

      q->next=tmp;
      q=tmp;
      p->varsize=num_var;
    }
  while(q != NULL);
  DISPLAY("\n");
  (update->percentage)=0.0;
  TRY(cable->close_port());

  return 0;
}

int ti89_receive_IDlist(char *id);

int ti89_receive_var(FILE *file, int mask_mode, 
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

  if(mask_mode & MODE_IDLIST) 
    {
      TRY(ti89_receive_IDlist(varname));
      return 0;
    }

  TRY(cable->open_port());
  update_start();
  sprintf(update->label_text, "Variable: %s", varname);
  update->label();
  (update->percentage)=0.0;
  sum=0;
  DISPLAY("Request variable: %s\n", varname);
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_REQUEST));
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
  TRY(ti89_sendstring(varname, &sum));
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  
  TRY(ti89_isOK());
  sum=0;
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD89_VAR_HEADER) return ERR_INVALID_BYTE;
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
  DISPLAY("Type of the variable: %s\n", ti89_byte2type(data));
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
  TRY(cable->get(&data)); // It's the only difference with the 92
  sum+=data;
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  TRY(PC_replyOK_89());
  TRY(PC_waitdata_89());
  DISPLAY("The calculator want continue.\n");

  TRY(ti89_isOK());
  DISPLAY("Receiving variable...\n");
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD89_DATA_PART) return ERR_INVALID_BYTE;
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
      if(update->cancel) return ERR_ABORT;
    }
  TRY(cable->get(&data));
  checksum=data;
  fprintf(file, "%c", data);
  TRY(cable->get(&data));
  checksum += (data << 8);
  fprintf(file, "%c", data);
  if(checksum != sum) return ERR_CHECKSUM;
  TRY(PC_replyOK_89());
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  DISPLAY("The calculator do not want continue.\n");
  TRY(cable->get(&data));
  TRY(cable->get(&data));

  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_TI_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("\n");
  
  update_start();
  TRY(cable->close_port());
  PAUSE(pause_between_vars);

  return 0;
}

int ti89_send_var(FILE *file, int mask_mode)
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
  byte varattr;
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
      TRY(ti89_directorylist(&dirlist, &n));
      do_dirlist=0;
    } 

  if((mask_mode & MODE_SEND_LAST_VAR) ||
     (mask_mode & MODE_SEND_ONE_VAR)) do_dirlist=1;

  // it seems that TIGLv2.00 use always 0xC9
  //mask_mode |= MODE_USE_2ND_HEADER;
  if(t_vartype != NULL) { free(t_vartype); t_vartype=NULL; }
  if(t_varname != NULL)
    {
      for(i=0; i<num_vars; i++) free(t_varname[i]); 
      t_varname=NULL;
      num_vars=0;
    }
  num_vars=0;
  var_index=0;
  TRY(cable->open_port());
  update_start();
  fgets(str, 9, file);
  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
	{
	  if( strcmp(str, "**TI92P*") && strcmp(str, "**TI92p*") &&
	      strcmp(str, "**TI89**") && strcmp(str, "**TI92**") )
	    { 
	      return ERR_INVALID_TIXX_FILE;
	    }
	}
      else if(mask_mode & MODE_FILE_CHK_ALL)
	{
	  fprintf(stderr, "MODE_FILE_CHK_ALL\n");
	  if( strcmp(str, "**TI89**"))
	    {
	      return ERR_INVALID_TI89_FILE;
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
      if(t_varname == NULL) return 40; // 40, I don't remind me what's that ?! 
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
	    t_vartype[var_index]=vartype;
	    varattr=fgetc(file);
	    if( (mask_mode & MODE_USE_2ND_HEADER)    // if backup
		&& (mask_mode & MODE_KEEP_ARCH_ATTRIB) ) // and if we use the extended file format
	      {
		switch(varattr)
		  {
		  case TI89_VNONE: t_vartype[var_index]=TI89_BKUP;
		    break;
		  case TI89_VLOCK: t_vartype[var_index]=0x26;
		    break;
		  case TI89_VARCH: t_vartype[var_index]=0x27;
		    break;
		  }
	      }
	    for(i=0; i<2; i++) fgetc(file);
	    if(! (mask_mode & MODE_LOCAL_PATH))
	      {
		strcpy(t_varname[var_index], foldname);
		strcat(t_varname[var_index], "\\");
		strcat(t_varname[var_index], varname); 	    
	      }
	    else
	      strcpy(t_varname[var_index], varname);
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
      printf("-> <%8s> <%02X>\n", t_varname[i], t_vartype[i]);
    }
  */
  for(j=0; j<num_vars; j++, var_index++)
    {
      if(mask_mode & MODE_USE_2ND_HEADER)
	{
	  (update->main_percentage)=(float)j/num_vars;
	  update_pbar();
	}
      for(i=0; i<4; i++) fgetc(file);
      varsize=fgetc(file) << 8;
      varsize+=fgetc(file);
      varsize+=2;
      
      sprintf(update->label_text, "Variable: %s", 
	       t_varname[var_index]);
      update->label();
      DISPLAY("Sending variable...\n");
      DISPLAY("Name: %s\n", t_varname[var_index]);
      DISPLAY("Size: %08X\n", varsize-2);
      DISPLAY("Type: %s\n", ti89_byte2type(t_vartype[var_index]));

      /**/
      exist=check_if_var_exist(&dirlist, t_varname[var_index]);
      if(exist && (mask_mode & MODE_DIRLIST))
	{
	  action=update->choose(t_varname[var_index], varname);
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
      TRY(cable->put(PC_TI89));
      if((mask_mode & MODE_USE_2ND_HEADER))
	{ // used for sending backup
	  TRY(cable->put(CMD89_VAR_HEADER2));
	}
      else
	{ // used for normal files
	  TRY(cable->put(CMD89_VAR_HEADER));
	}
      block_size=4+2+strlen(t_varname[var_index])+1; // difference: see comment below
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
      TRY(ti89_sendstring(t_varname[var_index], &sum));
      data=0x00;
      TRY(cable->put(data)); // The unique difference in comparison with the 92
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti89_isOK());
      TRY(ti89_waitdata());
      
      sum=0;
      TRY(PC_replyOK_89());
      TRY(cable->put(PC_TI89));
      TRY(cable->put(CMD89_DATA_PART));
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
	  if(update->cancel) return ERR_ABORT;
	}
      fgetc(file);
      fgetc(file);
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti89_isOK());
      TRY(cable->put(PC_TI89));
      TRY(cable->put(CMD89_EOT));
      TRY(cable->put(0x00));
      TRY(cable->put(0x00));
      DISPLAY("The computer does not want continue.\n");
      err = ti89_isOK();
      if(err) 
	{
	  DISPLAY("Variable has been rejected by calc.\n");
	  sprintf(update->label_text, _("Variable rejected"));
	  update->label();
	}
      DISPLAY("\n");
      PAUSE(pause_between_vars);
    }

  update_start();
  TRY(cable->close_port());

  return 0;
}

int ti89_receive_backup(FILE *file, int mask_mode, longword *version)
{
  struct varinfo dirlist, *ptr, *ptr2;
  int n;
  int i=0;
  char varname[20];

  TRY(cable->open_port());
  update_start();

  /* Do a directory list get variables entries */
  TRY(ti89_directorylist(&dirlist, &n));

  /* Generate the header of the group file */
  generate_89_92_92p_group_file_header_from_varlist(file, mask_mode, 
						    "**TI89**", 
						    (&dirlist)->next,
						    CALC_TI89);

  /* Receive all variables */
  ptr=(&dirlist)->next;
  do
    {
      i++;
      (update->main_percentage)=(float)i/n;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
      DISPLAY("-> %8s %i  %02X %08X\r\n", ptr->varname, (int)(ptr->varlocked), ptr->vartype, ptr->varsize);
      if(ptr->vartype == TI89_DIR)
        ptr->is_folder = FOLDER;
      else
        ptr->is_folder = VARIABLE;
      if(ptr->vartype == TI89_DIR)
	{
	  ptr=ptr->next;
	  continue;
	}
      if(ptr->vartype == TI89_FLASH)
        {
          ptr=ptr->next;
          continue;
        }
      TRY(ti89_isready());
      strcpy(varname, (ptr->folder)->varname);
      strcat(varname, "\\");
      strcat(varname, ptr->varname);
      TRY(ti89_receive_var(file, mask_mode, 
			   varname, ptr->vartype, ptr->varlocked));

      ptr=ptr->next;
    }
  while(ptr != NULL);

  /* Free the allocated memory for the linked list */
  ptr=&dirlist;
  ptr=ptr->next;
  do
    {
      ptr2=ptr->next;
      free(ptr);
      ptr=ptr2;
    }
  while(ptr != NULL);  

  update_stop();
  TRY(cable->close_port());

  return 0;
}

int ti89_send_backup(FILE *file, int mask_mode)
{
  byte data;
  word sum;
  word block_size;
  int i;

  TRY(cable->open_port());
  update_start();
  DISPLAY("Sending backup...\n");

  /* Send a header */
  sum=0; 
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_VAR_HEADER));
  block_size=4+2+strlen("main");
  TRY(cable->put(LSB(block_size)));
  TRY(cable->put(MSB(block_size)));
  data=0x00;
  for(i=0; i<4; i++)
    {
      TRY(cable->put(data));
    }
  data=TI89_BKUP;
  sum+=data;
  TRY(cable->put(data));
  data=strlen("main");
  sum+=data;
  TRY(cable->put(data));
  ti89_sendstring("main", &sum);
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  TRY(ti89_isOK());
  TRY(ti89_waitdata());  
  TRY(PC_replyOK_89());  
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_EOT));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer do not want continue.\n");
  TRY(ti89_isOK());  

  /* Send as a group file with some 'forced' options */
  TRY(ti89_send_var(file, (mask_mode | MODE_USE_2ND_HEADER | 
			   MODE_KEEP_ARCH_ATTRIB) & 
		    ~MODE_LOCAL_PATH & ~MODE_DIRLIST));

  update_start();
  TRY(cable->close_port());

  return 0;
}

#define DUMP_ROM89_FILE "dumprom.89z"

int ti89_dump_rom(FILE *file, int mask_mode)
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
  update->label();

  /* Open connection and check */
  TRY(cable->open_port());
  TRY(ti89_isready());
  TRY(cable->close_port());
  sprintf(update->label_text, "Yes !");
  update->label();

  /* Transfer ROM dump program from lib to calc */
  f = fopen(DUMP_ROM89_FILE, "wb");
  if(f == NULL)
    return -1;
  fwrite(romDump89, sizeof(unsigned char),
         romDumpSize89, f);
  fclose(f);
  f = fopen(DUMP_ROM89_FILE, "rb");
  TRY(ti89_send_var(f, MODE_NORMAL));
  fclose(f);
  //unlink(DUMP_ROM89_FILE);
  //exit(-1);
  /* Launch calculator program by remote control */
  sprintf(update->label_text, "Launching...");
  update->label();

  TRY(ti89_send_key(KEY89_m));
  TRY(ti89_send_key(KEY89_a));
  TRY(ti89_send_key(KEY89_i));
  TRY(ti89_send_key(KEY89_n));
  TRY(ti89_send_key(KEY89_BSLASH));
  TRY(ti89_send_key(KEY89_d));
  TRY(ti89_send_key(KEY89_u));
  TRY(ti89_send_key(KEY89_m));
  TRY(ti89_send_key(KEY89_p));
  TRY(ti89_send_key(KEY89_r));
  TRY(ti89_send_key(KEY89_o));
  TRY(ti89_send_key(KEY89_m));
  TRY(ti89_send_key(KEY89_LP));
  TRY(ti89_send_key(KEY89_RP));
  TRY(ti89_send_key(KEY89_ENTER));

  /* Receive it now blocks per blocks (1024 + CHK) */
  update_start();
  sprintf(update->label_text, "Receiving...");
  update->label();
  start = time(NULL);
  total = 2 * 1024 * 1024;
  update->total = total;

  for(i=0; i<2*1024; i++)
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

      update->count = 1024*2;
      update->main_percentage = (float)i/(1024*2);
      if(update->cancel) return -1;

      elapsed = difftime(time(NULL), start);
      estimated = elapsed * (float)(1024*2) / i;
      remaining = difftime(estimated, elapsed);
      sprintf(buffer, "%s", ctime(&remaining));
      sscanf(buffer, "%3s %3s %i %s %i", tmp,
             tmp, &pad, tmp, &pad);
      sprintf(update->label_text, "Remaining (mm:ss): %s", tmp+3);
      update->label();
    }

  /* Close connection */
  TRY(cable->close_port());

  return 0;
}

int ti89_receive_IDlist(char *id)
{
  byte data;
  word sum;
  word checksum;
  word block_size;
  int i, j;
  longword var_size;
  byte name_length;
  char name[9];

  TRY(cable->open_port());
  update_start();
  sum=0;
  DISPLAY("Request IDlist...\n");
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_REQUEST));
  block_size=4+2;
  data=LSB(block_size);
  TRY(cable->put(data));
  data=MSB(block_size);
  TRY(cable->put(data));
  for(i=0; i<4; i++)
    {
      TRY(cable->put(0x00));
    }
  data=TI89_IDLIST;
  TRY(cable->put(data));
  sum+=data;
  data=0x00;
  TRY(cable->put(data));
  sum+=data;
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  
  TRY(ti89_isOK());
  sum=0;
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD89_VAR_HEADER) return ERR_INVALID_BYTE;
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
  DISPLAY("Size of the IDlist: 0x%08X = %i.\n", var_size-2, var_size-2);
  TRY(cable->get(&data));
  DISPLAY("Type of the variable: %s\n", ti89_byte2type(data));
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
  TRY(cable->get(&data)); // It's the only difference with the 89
  sum+=data;
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  TRY(PC_replyOK_89());
  TRY(PC_waitdata_89());
  DISPLAY("The calculator want continue.\n");

  TRY(ti89_isOK());
  DISPLAY("Receiving IDlist...\n");
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD89_DATA_PART) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  sum=0;
  for(i=0; i<4; i++)
    {
      TRY(cable->get(&data));
      sum+=data;
    }
  TRY(cable->get(&data));
  block_size = (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  block_size |= data;
  sum+=data;
  DISPLAY("block_size=%x, var_size=%x\n", block_size, var_size);
  block_size=var_size-2; // diff here
  for(i=0, j=0; i<block_size; i++, j++)
    {
      TRY(cable->get(&data));
      sum+=data;
      if( (j == 7) || (j == 13) )
	{
	  id[j]='-';
	  j++;
	}
      id[j]=data;
    }
  id[j]='\0';
  DISPLAY("IDlist: <%s>\n", id+2);
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;
  
  TRY(PC_replyOK_89());
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  DISPLAY("The calculator do not want continue.\n");
  TRY(cable->get(&data));
  TRY(cable->get(&data));

  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_TI_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  (update->percentage)=0.0;
  DISPLAY("\n");
  TRY(cable->close_port());

  return 0;
}

int ti89_get_rom_version(char *version)
{
  byte data;
  word sum;
  word checksum;
  int i;
  int b;
  word block_size;
  word num_bytes;

  TRY(cable->open_port());
  (update->percentage)=0.0;

  /* Check if TI is ready*/
  TRY(ti89_isready());  
  
  sum=0;
  num_bytes=0;
  DISPLAY("Request backup...\n");
  /* Request a backup */
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_REQUEST));
  TRY(cable->put(0x12));
  TRY(cable->put(0x00));
  for(i=0; i<4; i++) { TRY(cable->put(0x00)); }
  TRY(cable->put(TI89_BKUP));
  sum+=0x1D;
  TRY(cable->put(0x0C));
  sum+=0x0C;
  TRY(ti89_sendstring("main\\version", &sum));
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  /* Check if TI replies OK */
  TRY(ti89_isOK());
  
  /* Receive the ROM version */
  sum=0;
  TRY(cable->get(&data));
  if(data != TI89_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD89_VAR_HEADER) return ERR_INVALID_BYTE;
  
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
  if(checksum != sum) return ERR_CHECKSUM;
  
  /* Abort transfer */  
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_CHK_ERROR));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_CHK_ERROR));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("ROM version %s\n", version);
  
  DISPLAY("\n");
  (update->percentage)=0.0;
  TRY(cable->close_port());	
    
  return 0;
}

int ti89_send_flash(FILE *file, int mask_mode)
{
  byte data;
  word sum;
  char str[128];
  char *flash_name = str;
  longword flash_size;
  longword block_size;
  int i, j;
  int num_blocks;
  word last_block;
  byte str_size;
  char date[5];
  char *signature = "Advanced Mathematics Software";
  int tib = 0;

  //DISPLAY("timeout: %i\n", ticable_get_timeout());  
  /* Read the file header and initialize some variables */
  TRY(cable->open_port());
  update_start();
  fgets(str, 128, file);
  if(strstr(str, "**TIFL**") == NULL) // is a .89u file
    {
      for(i=0, j=0; i<127; i++) // is a .tib file
	{
	  if(str[i] == signature[j])
	    {
	      j++;
	      if(j==strlen(signature))
		{
		  DISPLAY("TIB file.\n");
		  tib = 1;
		  break;
		}
	    }
	}
      if(j < strlen(signature))
	return ERR_INVALID_FLASH_FILE; // not a FLASH file
    }

  rewind(file);
  if(!tib)
    {
      fgets(str, 9, file);
      
      for(i=0; i<4; i++) 
	fgetc(file);
      
      for(i=0; i<4; i++)
	date[i] = fgetc(file);
      DISPLAY("Date of the FLASHapp or License: %02X/%02X/%02X%02X\n", 
	      date[0], date[1], date[2], 0xff & date[3]);
      str_size=fgetc(file);
      for(i=0; i<str_size; i++)
	str[i]=fgetc(file);
      str[i]='\0';
      for(i=16+str_size+1; i<0x4A; i++)
	fgetc(file);
      flash_size=(LSB(fgetc(file)))+(LSB(fgetc(file)) << 8)+
	(LSB(fgetc(file)) << 16)+(LSB(fgetc(file)) << 24);
      
      if(!strcmp(str, "License"))
	{
	  DISPLAY("There is a license header: skipped.\n");
	  for(i=0; i<flash_size; i++)
	    fgetc(file);
	  
	  fgets(str, 9, file);
	  if(strcmp(str, "**TIFL**"))
	    return ERR_INVALID_FLASH_FILE;
	  for(i=0; i<4; i++) fgetc(file);
	  for(i=0; i<4; i++)
	    date[i] = 0xff & fgetc(file);
	  DISPLAY("Date of the FLASHapp or License: %02X/%02X/%02X%02X\n", 
		  date[0], date[1], date[2], 0xff & date[3]);
	  str_size=fgetc(file);
	  for(i=0; i<str_size; i++)
	    str[i]=fgetc(file);
	  str[i]='\0';
	  for(i=16+str_size+1; i<0x4A; i++)
	    fgetc(file);
	  flash_size=(LSB(fgetc(file)))+(LSB(fgetc(file)) << 8)+
	    (LSB(fgetc(file)) << 16)+(LSB(fgetc(file)) << 24);
	} 
    }
  else
    {
      fseek(file, 0, SEEK_END);
      flash_size = ftell(file);
      fseek(file, 0, SEEK_SET);
      strcpy(str, "basecode");
    }      

  DISPLAY("\n");
  DISPLAY("Sending FLASH application...\n");
  DISPLAY("FLASH application name: \"%s\"\n", str);
  DISPLAY("FLASH application/Operating System size: %i bytes.\n", flash_size);

  /* Now, read data from the file and send them by block */
  sum=0;
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_VAR_HEADER2));
  if(mask_mode == MODE_AMS)
    block_size = 4+2;
  else
    block_size=4+2+strlen(flash_name);
  TRY(cable->put(LSB(block_size)));
  TRY(cable->put(MSB(block_size)));
  data=flash_size & 0x000000FF;
  sum+=data;
  TRY(cable->put(data));
  data=(flash_size & 0x0000FF00) >> 8;
  sum+=data;
  TRY(cable->put(data));
  data=(flash_size & 0x00FF0000) >> 16;
  sum+=data;
  TRY(cable->put(data));
  data=(flash_size & 0xFF000000) >> 24;
  sum+=data;
  TRY(cable->put(data));
  if(mask_mode == MODE_AMS)
    data = TI89_AMS;
  else
    data = TI89_FLASH;
  sum+=data;
  TRY(cable->put(data));
  if(mask_mode == MODE_AMS)
    data=0x00;
  else
    data=strlen(flash_name);
  sum+=data;
  TRY(cable->put(data));
  if(mask_mode != MODE_AMS)
    ti89_sendstring(flash_name, &sum);
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  num_blocks = flash_size/65536;
  DISPLAY("Number of blocks: %i\n", num_blocks + 1);
  
  for(i=0; i<num_blocks; i++ )
    {
      DISPLAY("Sending block %i.\n", i);
      TRY(ti89_isOK());
      TRY(ti89_waitdata());
      TRY(PC_replyOK_89());

      sum=0;
      TRY(cable->put(PC_TI89));
      TRY(cable->put(CMD89_DATA_PART));
      block_size=65536;
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
      DISPLAY("Transmitting data.\n");
      update->total = 65536;
      for(j=0; j<65536; j++)
 	{
	  data=fgetc(file);
	  sum+=data;
	  TRY(cable->put(data));

	  update->count = j;
	  update->percentage = (float)j/65536;
	  update_pbar();
	  if(update->cancel) return ERR_ABORT;
	}
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));

      TRY(ti89_isOK());
      TRY(PC_replyCONT_89());
      
      ((update->main_percentage))=(float)i/num_blocks;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }

  DISPLAY("Sending the last block.\n");
  last_block=flash_size % 65536;
  sum=0;
  TRY(ti89_isOK());
  TRY(ti89_waitdata());
  TRY(PC_replyOK_89());
  
  sum=0;
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_DATA_PART));
  block_size=last_block;
  TRY(cable->put(LSB(block_size)));
  TRY(cable->put(MSB(block_size)));
  DISPLAY("Transmitting data.\n");
  update->total = last_block;
  for(j=0; j<last_block; j++)
    {
      data=fgetc(file);
      sum+=data;
      TRY(cable->put(data));

      update->count = j;
      update->percentage = (float)j/last_block;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  
  TRY(ti89_isOK());
  
  TRY(cable->put(PC_TI89));
  TRY(cable->put(CMD89_EOT));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  if(mask_mode != MODE_AMS)
    DISPLAY("Flash application sent completely.\n");
  if(mask_mode == MODE_AMS)
    {
      TRY(ti89_isOK());
      DISPLAY("Operating System sent completely.\n");
    }
  DISPLAY("\n");

  update_start();
  TRY(cable->close_port());

  return 0;
}
