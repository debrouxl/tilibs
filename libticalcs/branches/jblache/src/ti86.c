/*  tilp - link program for TI calculators
 *  Copyright (C) 1999-2000, Romain Lievin, David Kuder
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

#include "calc_err.h"
#include "defs86.h"
#include "calc_ext.h"
#include "pause.h"
#include "rom86.h"
#include "update.h"

/* Functions used by TI_PC functions */

// The PC indicates that is OK
// 03 56 00 00
int PC_replyOK_86(void)
{
  TRY(cable->put(PC_TI86));
  TRY(cable->put(CMD86_TI_OK));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer reply OK.\n");

  return 0;
}

// The PC indicates that it is ready or wait data
// 03 09 00 00
int PC_waitdata_86(void)
{
  TRY(cable->put(PC_TI86));
  TRY(cable->put(CMD86_WAIT_DATA));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  DISPLAY("The computer wait data.\n");
  
  return 0;
}

// Check whether the TI reply OK
// 86 56 00 00
int ti86_isOK(void)
{
  byte data;

  TRY(cable->get(&data));
  if(data != TI86_PC) 
    {
      return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if(data != CMD86_TI_OK)
    { 
      if(data==CMD86_CHK_ERROR) return ERR_CHECKSUM;
      else return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if(data != 0x00)
    { 
      //printf("Debug: %02X\n", data);
      return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  if((data&1) != 0x00)
    { 
      return ERR_NOT_READY;
    }
  DISPLAY("The calculator reply OK.\n");

  return 0;
}

// Check whether the TI reply OK with packet length
// 86 56 LL HH
int ti86_isPacketOK(word length)
{
  byte data;
  word w;

  TRY(cable->get(&data));
  if(data != TI86_PC) return ERR_NOT_REPLY;
  TRY(cable->get(&data));
  if(data != CMD86_TI_OK)
    {
      if(data==CMD86_CHK_ERROR) return ERR_CHECKSUM;
      else return ERR_NOT_REPLY;
    }
  TRY(cable->get(&data));
  w=data;
  TRY(cable->get(&data));
  w|=(data << 8);
  if(w != length) return ERR_PACKET;
  DISPLAY("The calculator reply OK.\n");

  return 0;
}

// The TI indicates that it is ready or wait data
// 86 09 00 00
static int ti86_waitdata(byte *rej_code)
{
  byte data;
  *rej_code = CMD86_REJ_NONE;

  TRY(cable->get(&data));
  if(data != TI86_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD86_WAIT_DATA)
    {
      if(data != CMD86_REFUSED)
        {
          DISPLAY("Data: %02X\n", data);
          return ERR_INVALID_BYTE;
        }
      else
        {
          DISPLAY("Var rejected... ");
          TRY(cable->get(&data));
          TRY(cable->get(&data));
          TRY(cable->get(&data));
          *rej_code = data;
          //DISPLAY("Rejection code: %02X\n", data);
          TRY(cable->get(&data));
          TRY(cable->get(&data));
          return ERR_VAR_REFUSED;
        }
    }
  TRY(cable->get(&data));
  TRY(cable->get(&data));
  DISPLAY("The calculator wait data.\n");

  return 0;
}

// Check whether the TI reply that it is ready
int ti86_isready(void)
{
  /* This function does not exist */
  return ERR_VOID_FUNCTION;
}

// Send a string of 8 characters
static int sendstring(char *s, word *checksum)
{
  int i;
  
  for(i=0; i<(int)strlen(s); i++)
    {
      TRY(cable->put(s[i]));
      (*checksum) += (0xFF & s[i]);      
    }
  
  return 0;    
}

// Send a string of 8 characters but empty chars are replaced by space chars
static int sendstring20(char *s, word *checksum)
{
  int i;
  
  for(i=0; i<(int)strlen(s); i++)
    {
      TRY(cable->put(s[i]));
      (*checksum) += (0xFF & s[i]);      
    }
  for(i=strlen(s); i<8; i++)
    {
      TRY(cable->put(0x20));
      (*checksum)+=0x20;
    }
  
  return 0;
}


/* This part comes from David Kuder, thanks */

#define TI86_MAXTYPES 48
const char *TI86_TYPES[TI86_MAXTYPES]=
{ 
  "REAL", "CPLX", "VECTR", "CVECT", "LIST", "CLIST", "MATRX", "??", 
  "CONS", "CCONS", "EQU", "??", "STRNG", "GDB", "GDB", "GDB", 
  "GDB", "PIC", "PRGM", "RANGE", "SCRN", "DIR", "??", "WIND", 
  "POLAR", "PARAM", "DIFEQ", "ZRCL", "??", "BACKUP", "??", "??",
  "??", "??", "??", "??", "??", "??", "??", "??",
  "??", "??", "EQU", "??", "??", "??", "??", "??"
};

const char *TI86_EXT[TI86_MAXTYPES]=
{
  "86n", "86c", "86v", "86v", "86l", "86l", "86m", "86?",
  "86k", "86k", "86e", "86?", "86s", "86d", "86d", "86d",
  "86d", "86i", "86p", "86r", "86?", "86g", "86?", "86w",
  "86r", "86r", "86r", "86?", "86?", "86b", "86?", "86?"
  "86?", "86?", "86?", "86?", "86?", "86?", "86?", "86?",
  "86?", "86?", "86e", "86?", "86?", "86?", "86?", "86?"
};

// Return the type corresponding to the value
const char *ti86_byte2type(byte data)
{
  if(data>TI86_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.");
      return "??";
    }
  else 
    {
      return TI86_TYPES[data];
    }
}

// Return the value corresponding to the type
byte ti86_type2byte(char *s)
{
  int i;

  for(i=0; i<TI86_MAXTYPES; i++)
    {
      if(!strcmp(TI86_TYPES[i], s)) break;
    }
  if(i>TI86_MAXTYPES)
    {
      printf("Warning: unknown type. It is a bug. Please report this information.");
      return 0;
    }

  return i;
}

// Return the file extension corresponding to the value
const char *ti86_byte2fext(byte data)
{
  if(data>TI86_MAXTYPES)
    {
      printf("Type: %02X\n", data);
      printf("Warning: unknown type. It is a bug. Please report this information.");
      return ".86?";
    }
  else 
    {
      return TI86_EXT[data];
    }
}

// Return the value corresponding to the file extension
byte ti86_fext2byte(char *s)
{
  int i;

  for(i=0; i<TI86_MAXTYPES; i++)
    {
      if(!strcmp(TI86_EXT[i], s)) break;
    }
  if(i > TI86_MAXTYPES)
    {
      printf("Warning: unknown type. It is a bug. Please report this information.");
      return 0;
    }

  return i;
}

/* Oriented packet functions */

static int send_request(word size, byte type, char *string)
{
  byte data;
  word sum;
  word block_size;

  sum=0;
  TRY(cable->put(PC_TI86));
  TRY(cable->put(CMD86_REQUEST));
  block_size=4+strlen(string);
  data=LSB(block_size);
  TRY(cable->put(data));
  data=MSB(block_size);
  TRY(cable->put(data));
  data=0x00;
  TRY(cable->put(data));
  TRY(cable->put(data));
  data=type;
  sum+=data;
  TRY(cable->put(data));
  data=strlen(string);
  sum+=data;
  TRY(cable->put(data));
  sendstring(string, &sum);
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));

  return 0;
}

static int send_var_header(word size, byte type, char *string)
{
  byte data;
  word sum;

  sum=0;
  TRY(cable->put(PC_TI86));
  TRY(cable->put(CMD86_VAR_HEADER2));
  TRY(cable->put(0x0C));
  TRY(cable->put(0x00));
  data=LSB(size);
  sum+=data;
  TRY(cable->put(data));
  data=MSB(size);
  sum+=data;
  TRY(cable->put(data));
  data=type;
  sum+=data;
  TRY(cable->put(data));
  data=strlen(string);
  sum+=data;
  TRY(cable->put(data));
  sendstring20(string, &sum);
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));  

  return 0;
}

static int receive_var_header(word *size, byte *type, char *string)
{
  byte data;
  word sum;
  word checksum;
  int i;
  int j;
  byte nl;

  sum=0;
  TRY(cable->get(&data));
  if(data != TI86_PC) 
    return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data == CMD86_EOT) return ERR_ABORT;
  else if(data != CMD86_VAR_HEADER) 
    return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  j=data;
  TRY(cable->get(&data));
  j|=data << 8;
  TRY(cable->get(&data));
  (*size)=data;
  sum+=data;
  TRY(cable->get(&data));
  (*size) |= (data << 8);
  sum+=data;
  TRY(cable->get(&data));
  (*type)=data;
  sum+=data;
  TRY(cable->get(&data));
  nl=data;
  sum+=data;
  for(i=0; i<nl; i++)
    {
      TRY(cable->get(&data));
      string[i]=data;
      //DISPLAY("%c %02X", data, data);
      sum+=data;
    }
  for(i=nl; i<8; i++)
    {
      TRY(cable->get(&data));
      sum+=data;
      string[i]='\0';
    }
  string[i]='\0';
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  return 0;
}

/* General functions */

int ti86_send_key(int key)
{
  return ERR_VOID_FUNCTION;
}

int ti86_remote_control(void)
{
  /* This function does not exist */
  return ERR_VOID_FUNCTION;
}

int ti86_screendump(byte **bitmap, int mask_mode,
                         struct screen_coord *sc)
{
  byte data;
  word max_cnt;
  word sum;
  word checksum;
  int i;

  TRY(cable->open_port());
  update_start();
  sc->width=TI86_COLS;
  sc->height=TI86_ROWS;
  sc->clipped_width=TI86_COLS;
  sc->clipped_height=TI86_ROWS;
  if(*bitmap != NULL)
    free(*bitmap);
  (*bitmap)=(byte *)malloc(TI86_COLS*TI86_ROWS*sizeof(byte)/8);
  if((*bitmap) == NULL)
    {
      fprintf(stderr, "Unable to allocate memory.\n");
      exit(0);
    }

  sum=0;
  DISPLAY("Request screendump.\n");
  TRY(cable->put(PC_TI86));
  TRY(cable->put(CMD86_SCREEN_DUMP));
  TRY(cable->put(0x00));
  TRY(cable->put(0x00));
  
  TRY(ti86_isOK());
  TRY(cable->get(&data));
  if(data != TI86_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD86_DATA_PART) return ERR_INVALID_BYTE;
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
      if(update->cancel) return ERR_ABORT;
    }
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(sum != checksum) return ERR_CHECKSUM;

  TRY(PC_replyOK_86());

  update_start();
  TRY(cable->close_port());
  DISPLAY("\n");

  return 0;
}

int ti86_receive_backup(FILE *file, int mask_mode, longword *version)
{
  byte data;
  word sum;
  word checksum;
  word file_checksum;
  word block_size, size;
  int i, j;
  char desc[43]="Backup file received by TiLP";
  long offset;
  byte header[10];
  int three_or_four = 0;

  TRY(cable->open_port());
  update_start();
  sprintf(update->label_text, "Waiting backup...");
  update_label();
  file_checksum=0;
  fprintf(file, "**TI86**");
  fprintf(file, "%c%c%c", 0x1A, 0x0A, 0x00);
  for(i=0; i<42; i++) fprintf(file, "%c", desc[i]);
  offset=ftell(file);
  fprintf(file, "XX");
  size=0;
  sum=0;
  DISPLAY("Receiving backup...\n");
  TRY(cable->get(&data));
  if(data != TI86_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD86_VAR_HEADER) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  file_checksum+=data;
  j=data; /* j should be equal to 0x09 but to be the safe side ... */
  if(data != 0x09) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  file_checksum+=data;
  j|=(data << 8);
  if(data != 0x00) return ERR_INVALID_BYTE;
  DISPLAY("Receiving header.\n");
  sprintf(update->label_text, "Receiving...");
  update_label();
  DISPLAY("Section: ");
  for(i=0; i<j; i++) // raw write the var header
    {
      TRY(cable->get(&data));
      sum+=data;
      file_checksum+=data;
      fprintf(file, "%c", data); 
      header[i] = data;
      DISPLAY("%02X ", data);
    }
  if( (header[5]==0x00) && (header[6]==0x00)) 
    three_or_four=0;
  else 
    three_or_four=1;
  DISPLAY("\n");
  DISPLAY("Backup in %i parts.\n", 3 + three_or_four);
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;

  TRY(PC_replyOK_86());
  TRY(PC_waitdata_86());
  TRY(ti86_isOK());
  for(i=0; i<3+three_or_four; i++)
    {
      (update->main_percentage)=(float)i/(3+three_or_four);
      DISPLAY("Receiving part %i\n", i+1);
      TRY(cable->get(&data));
      if(data != TI86_PC) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      if(data != CMD86_DATA_PART) return ERR_INVALID_BYTE;
      TRY(cable->get(&data));
      block_size=data;
      fprintf(file, "%c", data);
      file_checksum+=data;
      TRY(cable->get(&data));
      block_size+=(data << 8);
      fprintf(file, "%c", data);
      file_checksum+=data;
      j=0;
      sum=0;
      size+=block_size;
      update->total = block_size;
      DISPLAY("Block size: %04x\n", block_size);
      for(j=0; j<block_size; j++)
	{
	  TRY(cable->get(&data));
          sum+=data;
	  file_checksum+=data;
          fprintf(file, "%c", data);
	  
	  update->count = j;
	  update->percentage = (float)j/block_size;
	  update_pbar();
	  if(update->cancel) return ERR_ABORT;
	}
      TRY(cable->get(&data));
      checksum=data;
      TRY(cable->get(&data));
      checksum += (data << 8);
      if(sum != checksum) return ERR_CHECKSUM;

      TRY(PC_replyOK_86());
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
  fprintf(file, "%c%c", LSB(file_checksum), MSB(file_checksum));
  DISPLAY("Backup complete.\n");
  fseek(file, offset, SEEK_SET);
  size+=2;
  fprintf(file, "%c%c", LSB(size+15), MSB(size+15));
  fseek(file, 0L, SEEK_END);
  DISPLAY("\n");

  update_start();
  TRY(cable->close_port());

  return 0;
}

int ti86_send_backup(FILE *file, int mask_mode)
{
  int err = 0;
  byte data;
  char str[128];
  word sum;
  int i;
  int j;
  word block_size;
  byte rej_code = CMD86_REJ_NONE;
  byte header[10];
  int three_or_four = 0;

  TRY(cable->open_port());
  update_start();
  sprintf(update->label_text, "Sending...");
  update_label();
  DISPLAY("Sending backup...\n");
  fgets(str, 9, file);
  if(!(mask_mode & MODE_FILE_CHK_NONE))
    {
      if(mask_mode & MODE_FILE_CHK_MID)
	{
	  if( strcmp(str, "**TI83**") && strcmp(str, "**TI86**") )
	    { 
	      return ERR_INVALID_TIXX_FILE;
	    }
	}
      else if(mask_mode & MODE_FILE_CHK_ALL)
	{
	  fprintf(stderr, "MODE_FILE_CHK_ALL\n");
	  if( strcmp(str, "**TI86**"))
	    {
	      return ERR_INVALID_TI86_FILE;
	    }
	}
    }

  for(i=0; i<3; i++) fgetc(file);
  for(i=0; i<42; i++)
  {
    fgetc(file);
  }
  data=fgetc(file); //skip total size
  //printf("%02X\n", data);
  data=fgetc(file);
  //printf("%02X\n", data);
  sum=0;
  DISPLAY("Sending header.\n");
  TRY(cable->put(PC_TI86));
  TRY(cable->put(CMD86_VAR_HEADER));
  data=fgetc(file);
  TRY(cable->put(data));
  block_size=data;
  data=fgetc(file);
  TRY(cable->put(data));
  block_size+=data;
  for(i=0; i<9; i++)
    {
      data=fgetc(file);
      sum+=data;
      header[i] = data;
      TRY(cable->put(data));
    }
  TRY(cable->put(LSB(sum)));
  TRY(cable->put(MSB(sum)));
  if( (header[5]==0x00) && (header[6]==0x00))
    three_or_four=0;
  else
    three_or_four=1;
  DISPLAY("Backup in %i parts.\n", 3 + three_or_four);
  DISPLAY("The calculator should ask you to continue the transfer.\n");

  TRY(ti86_isPacketOK(block_size));

  /* Here, the calc is waiting until the user confirmed the operation */
  sprintf(update->label_text, "Waiting confirmation on calc...");
  update_label();
  do
    {
      update_refresh();
      if(update->cancel) return ERR_ABORT;
      err=ti86_waitdata(&rej_code); 
      if(err == ERR_VAR_REFUSED)
        {
          //fprintf(stderr, "Rejection code: 0x%02x\n", rej_code);
          switch(rej_code)
            {
            case CMD86_REJ_SKIP:
              fprintf(stderr, "Variable skipped by user\n");
              //for(i=0; i<varsize; i++) fgetc(file); // read file anyway
              TRY(PC_replyOK_86());
              goto label_skip;
              break;
            case CMD86_REJ_EXIT:
              fprintf(stderr, "Transfer cancelled by user\n");
              goto label_exit;
              break;
            case CMD86_REJ_OUTOFMEM:
              fprintf(stderr, "Out of mem\n");
              return ERR_OUT_OF_MEMORY;
              break;
            default:
              return ERR_INVALID_BYTE;
	      break;
            }
        }
    }
  while( (err == 35) || (err == 3) );
  DISPLAY("The calculator continue the transfer.\n");
		
  sprintf(update->label_text, "Transmitting...");
  update_label();
	
  TRY(PC_replyOK_86());
  for(i=0; i<3+three_or_four; i++)
    {
      (update->main_percentage)=(float)i/(3+three_or_four);
      update_pbar();
      if(update->cancel) return ERR_ABORT;
      DISPLAY("Sending part %i...\n", i+1);
      sum=0;
      TRY(cable->put(PC_TI86));
      TRY(cable->put(CMD86_DATA_PART));
      
      
      data=fgetc(file);
      block_size=data;
      TRY(cable->put(data));
      data=fgetc(file);
      block_size+=data << 8;
      TRY(cable->put(data));
      printf("Backup block size: %04X\n", block_size);
      update->total = block_size;
      for(j=0; j<block_size; j++)
        {
          data=fgetc(file);
          sum+=data;
          TRY(cable->put(data));
	  
	  update->count = j;
	  update->percentage = (float)j/block_size;
	  update_pbar();
	  if(update->cancel) return ERR_ABORT;
	}
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti86_isPacketOK(block_size));
    }
  TRY(PC_replyOK_86());
  DISPLAY("\n");
 label_skip:
 label_exit:
  update_start();
  TRY(cable->close_port());

  return 0;
}

int ti86_directorylist(struct varinfo *list, int *n_elts)
{
    byte data;
    int err;
    int i;
    byte var_type;
    char var_name[9];
    struct varinfo *p;
    word size;
    word sum;

    update_start();
    TRY(cable->open_port());
    *n_elts=0;
    p=list;
    p->next=NULL;
    p->folder=list;
    strcpy(p->varname, "");
    p->varsize=0;
    p->vartype=0;
    p->varlocked=0;
    strcpy(p->translate, "");

    DISPLAY("Request directory list (dir)...\n");
    sum=0;
    TRY(cable->put(PC_TI86));
    TRY(cable->put(CMD86_REQUEST));
    data=0x05;
    TRY(cable->put(data));
    data=0x00;
    TRY(cable->put(data));
    for(i=0; i<2; i++)
      {
	data=0x00;
	sum+=data;
	TRY(cable->put(data));
      }
    data=TI86_DIR;
    sum+=data;
    TRY(cable->put(data));
    data=0x01;
    sum+=data;
    TRY(cable->put(data));
    data=0x00;
    sum+=data;
    TRY(cable->put(data));
    TRY(cable->put(LSB(sum)));
    TRY(cable->put(MSB(sum)));

    TRY(ti86_isPacketOK(0x0005));

    TRY(cable->get(&data));
    if(data != TI86_PC) return ERR_INVALID_BYTE;
    TRY(cable->get(&data));
    if(data != CMD86_DATA_PART) return ERR_INVALID_BYTE;
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(cable->get(&data));

    TRY(PC_replyOK_86());

    for( ; ; )
      {
	err=receive_var_header(&size, &var_type, var_name);
	if(err == -1) break;

	if( (p->next=(struct varinfo *)malloc(sizeof(struct varinfo))) == NULL)
	  {
	    fprintf(stderr, "Unable to allocate memory.\n");
	    exit(0);
	  }
	p=p->next;
	p->next=NULL;
	(*n_elts)++;

	strcpy(p->varname, var_name);
	p->vartype=var_type;
	p->varsize=size;
	p->varlocked=0;
	p->folder=list;
	p->is_folder = VARIABLE;
	strncpy(p->translate, p->varname, 9);

	DISPLAY("Name: %8s | ", p->varname);
	DISPLAY("Type: %8s | ", ti86_byte2type(p->vartype));
	DISPLAY("Locked: %i | ", p->varlocked);
	DISPLAY("Size: %08X\n", p->varsize);

	TRY(PC_replyOK_86());
	sprintf(update->label_text, "Reading of: TI86/%s", p->translate);
	update_label();
	if(update->cancel) return ERR_ABORT;
      }
    DISPLAY("The calculator does not want continue.\n");
    TRY(cable->get(&data));
    TRY(cable->get(&data));
    TRY(PC_replyOK_86());
    TRY(cable->close_port());
    DISPLAY("\n");
    
  return 0;
}

int ti86_receive_var(FILE *file, int mask_mode, 
		     char *varname, byte vartype, byte varlock)
{
  byte data;
  word sum;
  word checksum;
  static word file_checksum;
  word var_size;
  char name[9];
  byte var_type;
  int i;
  static long offset;
  char desc[43]="File received by tilp";
  static word allvars_size;	// This limits the size of a TIGL file to 64 Kb */
  int k;
  byte name_length;

  update_start();
  TRY(cable->open_port());
  if( (mask_mode & MODE_RECEIVE_FIRST_VAR) ||
      (mask_mode & MODE_RECEIVE_SINGLE_VAR) )
    {
      fprintf(file, "**TI86**");
      fprintf(file, "%c%c%c", 0x1A, 0x0A, 0x00);
      for(i=0; i<42; i++) fprintf(file, "%c", desc[i]);
      offset=ftell(file);
      fprintf(file, "XX");
      file_checksum=0;
      allvars_size=0;
    }
  var_size=0;
  sprintf(update->label_text, "Variable: %s", varname);
  update_label();
  DISPLAY("Request variable: %s\n", varname);
  TRY(send_request(0x0000, vartype, varname));

  TRY(ti86_isPacketOK((word)(strlen(varname)+4)));

  DISPLAY("Receiving variable(s)...\n");  
  sum=0;
  TRY(cable->get(&data));
  if(data != TI86_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data == CMD86_EOT) return -2; //break;
  else if(data != CMD86_VAR_HEADER) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  //if(data != 0x0B) return ERR_INVALID_BYTE;
  fprintf(file, "%c", data);
  file_checksum+=data;
  TRY(cable->get(&data));
  if(data != 0x00) return ERR_INVALID_BYTE;
  fprintf(file, "%c", data);
  file_checksum+=data;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  var_size=data;
  sum+=data;
  if(allvars_size+var_size+16 < allvars_size) 
    return ERR_GRP_SIZE_EXCEEDED;
  allvars_size+=var_size+16;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  var_size |= (data << 8);
  sum+=data;
  DISPLAY("-> Size: 0x%04X = %i.\n", var_size, var_size);
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  var_type=data;
  DISPLAY("-> Type: %s\n", ti86_byte2type(data));
  sum+=data;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  name_length=data;
  sum+=data;
  for(i=0; i<name_length; i++)
    {
      TRY(cable->get(&data));
      fprintf(file, "%c", 0xFF & data);
      sum+=data;
      name[i]=data;
    }
  for(i=name_length; i<8; i++)
    {
      TRY(cable->get(&data));
	  fprintf(file, "%c", 0xFF & data);
      sum+=data;
      name[i]='\0';
    }
  name[i]='\0';  
  strcpy(varname, name);
  DISPLAY("-> Name: %s <", varname);
  for(k=0; k<8; k++) DISPLAY("%02X", varname[k]);
  DISPLAY(">\n");
  file_checksum+=sum;
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;
  
  TRY(PC_replyOK_86());
  TRY(PC_waitdata_86());
  DISPLAY("The calculator want continue.\n");
  
  TRY(ti86_isOK());
  DISPLAY("Receiving variable...\n");
  TRY(cable->get(&data));
  if(data != TI86_PC) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  if(data != CMD86_DATA_PART) return ERR_INVALID_BYTE;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  file_checksum+=data;
  TRY(cable->get(&data));
  fprintf(file, "%c", data);
  file_checksum+=data;
  sum=0;
  update->total = var_size;
  for(i=0; i<var_size; i++)
    {
      TRY(cable->get(&data));
      fprintf(file, "%c", data);
      sum+=data;
      
      update->count = i;
      update->percentage = (float)i/var_size;
      update_pbar();
      if(update->cancel) return ERR_ABORT;
    }
  file_checksum+=sum;
  TRY(cable->get(&data));
  checksum=data;
  TRY(cable->get(&data));
  checksum += (data << 8);
  if(checksum != sum) return ERR_CHECKSUM;
  
  TRY(PC_replyOK_86());

  if( (mask_mode & MODE_RECEIVE_SINGLE_VAR) ||
      (mask_mode & MODE_RECEIVE_LAST_VAR) )
    {
      fprintf(file, "%c%c", LSB(file_checksum), MSB(file_checksum));
      fseek(file, offset, SEEK_SET);
      if(mask_mode & MODE_RECEIVE_SINGLE_VAR)
        fprintf(file, "%c%c", LSB(var_size+16), MSB(var_size+16));
      else
        if(mask_mode & MODE_RECEIVE_LAST_VAR)
          fprintf(file, "%c%c", LSB(allvars_size), MSB(allvars_size));
      fseek(file, 0L, SEEK_END);
      DISPLAY("\n");
    }  

  update_start();
  TRY(cable->close_port());
  PAUSE(pause_between_vars);

  return 0;
}

int ti86_send_var(FILE *file, int mask_mode)
{
  byte data;
  word sum;
  word block_size;
  longword varsize;
  char varname[9];
  byte vartype;
  int i;
  byte name_length;
  char trans[9];
  byte rej_code = CMD86_REJ_NONE;

  TRY(cable->open_port());
  update_start();
  fgets(trans, 9, file);
  if(strcmp(trans, "**TI86**")) 
    return ERR_INVALID_TI86_FILE;
  for(i=0; i<3; i++) fgetc(file);
  for(i=0; i<42; i++)
  {
    fgetc(file);
  }
  data=fgetc(file);
  data=fgetc(file);
  while(!feof(file))
    {
      block_size=fgetc(file);
      block_size+=fgetc(file) << 8;
      if(feof(file)) break;
      varsize=fgetc(file);
      if(feof(file)) break;
      varsize+=fgetc(file) << 8;
      vartype=fgetc(file);
      name_length=fgetc(file);
      for(i=0; i<8; i++) varname[i]=fgetc(file);
      varname[name_length]='\0';
      fgetc(file);
      fgetc(file);
      sprintf(update->label_text, "Variable: %s", varname);
      update_label();
      DISPLAY("Sending variable...\n");
      DISPLAY("-> Name: %s\n", varname);
      DISPLAY("-> Size: %08X\n", varsize);
      DISPLAY("-> Type: %s\n", ti86_byte2type(vartype));

      TRY(send_var_header((word)varsize, (byte)vartype, varname));
      
      TRY(ti86_isPacketOK(0x000C));
      TRY(ti86_waitdata(&rej_code));
      
      sum=0;
      TRY(PC_replyOK_86());
      TRY(cable->put(PC_TI86));
      TRY(cable->put(CMD86_DATA_PART));
      block_size=(word)varsize;
      TRY(cable->put(LSB(block_size)));
      TRY(cable->put(MSB(block_size)));
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
      TRY(cable->put(LSB(sum)));
      TRY(cable->put(MSB(sum)));
      
      TRY(ti86_isPacketOK(block_size));
    
      TRY(cable->put(PC_TI86));
      TRY(cable->put(CMD86_EOT));
      TRY(cable->put(0x00));
      TRY(cable->put(0x00));
      PAUSE(750);
    }
  DISPLAY("The computer does not want continue.\n");
  DISPLAY("\n");

  update_start();
  TRY(cable->close_port());
    
  return 0;
}

#define DUMP_ROM86_FILE "dumprom.86p"

int ti86_dump_rom(FILE *file, int mask_mode)
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
  int err;
  int b=0;;

  update_start();
  sprintf(update->label_text, "Ready ?");
  update_label();

  /* Open connection and check */
  TRY(cable->open_port());
  //TRY(ti86_isready());
  TRY(cable->close_port());
  sprintf(update->label_text, "Yes !");
  update_label();

  /* Transfer ROM dump program from lib to calc */
  f = fopen(DUMP_ROM86_FILE, "wb");
  if(f == NULL)
    return -1;
  fwrite(romDump86, sizeof(unsigned char),
	 romDumpSize86, f);
  fclose(f);
  f = fopen(DUMP_ROM86_FILE, "rb");
  TRY(ti86_send_var(f, MODE_SEND_ONE_VAR));
  fclose(f);
  //unlink(DUMP_ROM86_FILE);

  /* As we can not launch program by remote control, we wait user do that */
  TRY(cable->open_port());
  sprintf(update->label_text, "Launch from calc...");
  update_label();
  PAUSE(500);

  do
    {
      update_refresh();
      if(update->cancel) return ERR_ABORT;
      err = cable->get(&data);
      sum = data;
    }
  while( (err == 35) || (err == 3) );
  fprintf(file, "%c", data);
  
  /* Receive it now blocks per blocks (1024 + CHK) */
  update_start();
  sprintf(update->label_text, "Receiving...");
  update_label();
  start = time(NULL);
  total = 256 * 1024; // 256KB
  update->total = total;

  for(i=0; i<256; i++)
    {
      if(b) sum = 0;
      for (j=0; j<1023+b; j++)
        {
          TRY(cable->get(&data));
          fprintf(file, "%c", data);
          sum += data;
          update->percentage = (float)j/1024;
          update_pbar();
          if(update->cancel) return -1;
        }
      b = 1;
      TRY(cable->get(&data));
      checksum = data << 8;
      TRY(cable->get(&data));
      checksum |= data;
      if(sum != checksum) return ERR_CHECKSUM;
      TRY(cable->put(0xda));

      update->count = 256;
      update->main_percentage = (float)i/(256);
      if(update->cancel) return -1;

      elapsed = (long)difftime(time(NULL), start);
      estimated = (long)(elapsed * (float)(256) / i);
      remaining = (long)difftime(estimated, elapsed);
      sprintf(buffer, "%s", ctime(&remaining));
      sscanf(buffer, "%3s %3s %i %s %i", tmp,
             tmp, &pad, tmp, &pad);
      sprintf(update->label_text, "Remaining (mm:ss): %s", tmp+3);
      update_label();
    }
  /* Close connection */
  TRY(cable->close_port());

  return 0;
}

int ti86_get_rom_version(char *version)
{
  return ERR_VOID_FUNCTION;
}

int ti86_send_flash(FILE *file, int mask_mode)
{
  return ERR_VOID_FUNCTION;
}
