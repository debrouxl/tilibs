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
#ifdef __WIN32__
#include <windows.h>
#endif

#include "intl2.h"
#include "calc_ext.h"
#include "tixx.h"
#include "trans.h"
#include "const.h"
#include "group.h"
#include "calc_ver.h"
#include "calc_err.h"

/*****************/
/* Internal data */
/*****************/
struct ticable_link        *cable;
struct ticalc_info_update  *update;
//struct ti_calc           calc;
static int calc_type;

/***********/
/* Methods */
/***********/

/* Return the version number of the library */
DLLEXPORT
const char* DLLEXPORT2 ticalc_get_version()
{
  return LIBTICALCS_VERSION;
}

/* 
   Set up the update functions in order to be independant of the GUI
*/
DLLEXPORT
void DLLEXPORT2 ticalc_set_update(struct ticalc_info_update *iu,
				  void (*f1) (void),
				  void (*f2) (void),
				  void (*f3) (void),
				  void (*f4) (const char *t, char *s),
				  void (*f5) (void),
				  void (*f6) (void),
				  int  (*f7) (char *cur_name, char *new_name))
{
  iu->cancel  = 0;
  iu->start   = f1;
  iu->stop    = f2;
  iu->refresh = f3;
  iu->msg_box = f4;
  iu->pbar    = f5;
  iu->label   = f6;
  iu->choose  = f7;

  update = iu;
}

static void print_informations();
/*
  Set up the calculator functions according to the calculator type
*/
DLLEXPORT
void DLLEXPORT2 ticalc_set_calc(int type,
				struct ticalc_fncts *calc,
				struct ticable_link *lc)
{
  cable = lc;
  calc_type = type;

#ifdef ENABLE_NLS
#if !defined(__WIN32__)
  bindtextdomain(PACKAGE, NULL);
  textdomain(PACKAGE);
#endif
#endif

  print_informations();

  calc->group_file_ext = group_file_ext;
  calc->backup_file_ext = backup_file_ext;
  calc->tixx_dir = tixx_dir;
  calc->pak_name = pak_name;
  calc->flash_app_file_ext = flash_app_file_ext;
  calc->flash_os_file_ext = flash_os_file_ext;
  calc->tixx_flash = tixx_flash;

  switch(type)
    {
    case CALC_TI92P:
      calc->byte2type = ti92p_byte2type;
      calc->type2byte = ti92p_type2byte;
      calc->byte2fext = ti92p_byte2fext;
      calc->fext2byte = ti92p_fext2byte;
      
      calc->isready=ti92p_isready;
      calc->send_key=ti92p_send_key;
      calc->remote_control=ti92p_remote_control;
      calc->screendump=ti92p_screendump;
      calc->receive_backup=ti92p_receive_backup;
      calc->send_backup=ti92p_send_backup;
      calc->directorylist=ti92p_directorylist;
      calc->receive_var=ti92p_receive_var;
      calc->send_var=ti92p_send_var;
      calc->dump_rom=ti92p_dump_rom;
      calc->get_rom_version=ti92p_get_rom_version;
      calc->send_flash=ti92p_send_flash;

      calc->translate_varname = ti92p_translate_varname;
      calc->ascii2ti_key = ti92_keys;
      calc->generate_single_file_header 
	= generate_89_92_92p_single_file_header;
      calc->generate_group_file_header 
	= generate_89_92_92p_group_file_header_from_varlist;

      break;
    case CALC_TI92:
      calc->byte2type = ti92_byte2type;
      calc->type2byte = ti92_type2byte;
      calc->byte2fext = ti92_byte2fext;
      calc->fext2byte = ti92_fext2byte;

      calc->isready=ti92_isready;
      calc->send_key=ti92_send_key;
      calc->remote_control=ti92_remote_control;
      calc->screendump=ti92_screendump;
      calc->receive_backup=ti92_receive_backup;
      calc->send_backup=ti92_send_backup;
		calc->directorylist=ti92_directorylist;
      calc->receive_var=ti92_receive_var;
      calc->send_var=ti92_send_var;
      calc->dump_rom=ti92_dump_rom;
      calc->get_rom_version=ti92_get_rom_version;
      calc->send_flash=ti92_send_flash;

      calc->translate_varname = ti92_translate_varname;
      calc->ascii2ti_key = ti92_keys;
	  calc->generate_single_file_header
        = generate_89_92_92p_single_file_header;
      calc->generate_group_file_header
        = generate_89_92_92p_group_file_header_from_varlist;

      break;
    case CALC_TI89:
      calc->byte2type = ti89_byte2type;
      calc->type2byte = ti89_type2byte;
      calc->byte2fext = ti89_byte2fext;
      calc->fext2byte = ti89_fext2byte;

      calc->isready=ti89_isready;
      calc->send_key=ti89_send_key;
      calc->remote_control=ti89_remote_control;
      calc->screendump=ti89_screendump;
      calc->receive_backup=ti89_receive_backup;
      calc->send_backup=ti89_send_backup;
      calc->directorylist=ti89_directorylist;
		calc->receive_var=ti89_receive_var;
      calc->send_var=ti89_send_var;
      calc->dump_rom=ti89_dump_rom;
      calc->get_rom_version=ti89_get_rom_version;
      calc->send_flash=ti89_send_flash;

      calc->translate_varname = ti89_translate_varname;
      calc->ascii2ti_key = ti89_keys;
      calc->ascii2ti_key = ti92_keys;
	  calc->generate_single_file_header
        = generate_89_92_92p_single_file_header;
      calc->generate_group_file_header
        = generate_89_92_92p_group_file_header_from_varlist;

      break;
    case CALC_TI86:
      calc->byte2type = ti86_byte2type;
      calc->type2byte = ti86_type2byte;
      calc->byte2fext = ti86_byte2fext;
      calc->fext2byte = ti86_fext2byte;

      calc->isready=ti86_isready;
      calc->send_key=ti86_send_key;
      calc->remote_control=ti86_remote_control;
      calc->screendump=ti86_screendump;
      calc->receive_backup=ti86_receive_backup;
      calc->send_backup=ti86_send_backup;
      calc->directorylist=ti86_directorylist;
		calc->receive_var=ti86_receive_var;
      calc->send_var=ti86_send_var;
      calc->dump_rom=ti86_dump_rom;
      calc->get_rom_version=ti86_get_rom_version;
      calc->send_flash=ti86_send_flash;

      calc->translate_varname = ti86_translate_varname;
      calc->ascii2ti_key = ti92_keys;
      calc->ascii2ti_key = ti92_keys;
      calc->generate_single_file_header
	= generate_82_83_85_86_single_file_header;
      calc->generate_group_file_header
        = generate_82_83_85_86_group_file_header_from_varlist;

      break;
    case CALC_TI85:
      calc->byte2type = ti85_byte2type;
      calc->type2byte = ti85_type2byte;
      calc->byte2fext = ti85_byte2fext;
      calc->fext2byte = ti85_fext2byte;

      calc->isready=ti85_isready;
      calc->send_key=ti85_send_key;
      calc->remote_control=ti85_remote_control;
      calc->screendump=ti85_screendump;
      calc->receive_backup=ti85_receive_backup;
      calc->send_backup=ti85_send_backup;
      calc->directorylist=ti85_directorylist;
		calc->receive_var=ti85_receive_var;
      calc->send_var=ti85_send_var;
      calc->dump_rom=ti85_dump_rom;
      calc->get_rom_version=ti85_get_rom_version;
      calc->send_flash=ti85_send_flash;

      calc->translate_varname = ti85_translate_varname;
      calc->ascii2ti_key = ti92_keys;
      calc->generate_single_file_header
        = generate_82_83_85_86_single_file_header;
      calc->generate_group_file_header
        = generate_82_83_85_86_group_file_header_from_varlist;
      break;
  case CALC_TI83P:
    calc->byte2type = ti83p_byte2type;
    calc->type2byte = ti83p_type2byte;
    calc->byte2fext = ti83p_byte2fext;
    calc->fext2byte = ti83p_fext2byte;
    
    calc->isready=ti83p_isready;
    calc->send_key=ti83p_send_key;
    calc->remote_control=ti83p_remote_control;
    calc->screendump=ti83p_screendump;
    calc->receive_backup=ti83p_receive_backup;
    calc->send_backup=ti83p_send_backup;
    calc->directorylist=ti83p_directorylist;
    calc->receive_var=ti83p_receive_var;
    calc->send_var=ti83p_send_var;
	 calc->dump_rom=ti83p_dump_rom;
    calc->get_rom_version=ti83p_get_rom_version;
    calc->send_flash=ti83p_send_flash;
    
    calc->translate_varname = ti83_translate_varname;
    calc->ascii2ti_key = ti92_keys;
    calc->generate_single_file_header
      = generate_82_83_85_86_single_file_header;
    calc->generate_group_file_header
      = generate_82_83_85_86_group_file_header_from_varlist;
    break;
    case CALC_TI83:
      calc->byte2type = ti83_byte2type;
      calc->type2byte = ti83_type2byte;
      calc->byte2fext = ti83_byte2fext;
      calc->fext2byte = ti83_fext2byte;

      calc->isready=ti83_isready;
      calc->send_key=ti83_send_key;
      calc->remote_control=ti83_remote_control;
      calc->screendump=ti83_screendump;
      calc->receive_backup=ti83_receive_backup;
      calc->send_backup=ti83_send_backup;
      calc->directorylist=ti83_directorylist;
      calc->receive_var=ti83_receive_var;
      calc->send_var=ti83_send_var;
      calc->dump_rom=ti83_dump_rom;
      calc->get_rom_version=ti83_get_rom_version;
		calc->send_flash=ti83_send_flash;

      calc->translate_varname = ti83_translate_varname;
      calc->ascii2ti_key = ti92_keys;
      calc->generate_single_file_header
        = generate_82_83_85_86_single_file_header;
      calc->generate_group_file_header
        = generate_82_83_85_86_group_file_header_from_varlist;
      break;
    case CALC_TI82:
      calc->byte2type = ti82_byte2type;
      calc->type2byte = ti82_type2byte;
      calc->byte2fext = ti82_byte2fext;
      calc->fext2byte = ti82_fext2byte;

      calc->isready=ti82_isready;
      calc->send_key=ti82_send_key;
      calc->remote_control=ti82_remote_control;
      calc->screendump=ti82_screendump;
      calc->receive_backup=ti82_receive_backup;
      calc->send_backup=ti82_send_backup;
      calc->directorylist=ti82_directorylist;
      calc->receive_var=ti82_receive_var;
      calc->send_var=ti82_send_var;
      calc->dump_rom=ti82_dump_rom;
      calc->get_rom_version=ti82_get_rom_version;
      calc->send_flash=ti82_send_flash;
      
		calc->translate_varname = ti82_translate_varname;
      calc->ascii2ti_key = ti92_keys;
      calc->generate_single_file_header
        = generate_82_83_85_86_single_file_header;
      calc->generate_group_file_header
        = generate_82_83_85_86_group_file_header_from_varlist;
      break;
  default:
    fprintf(stderr, "Function not implemented. This is a bug. Please report it.");
    fprintf(stderr, "Informations:\n");
    fprintf(stderr, "Calc: %i\n", type);
    fprintf(stderr, "Program halted before crashing...\n");
    exit(-1);
    break;
  }  
}

/*
  Set up the calculator functions according to the calculator type
*/
DLLEXPORT
int DLLEXPORT2 ticalc_get_calc(int *type)
{
  *type = calc_type;
  return 0;
}

static FILE* ti_file = NULL;

/*
  Open a file to use with the TI calc general functions
*/
DLLEXPORT int DLLEXPORT2
ticalc_open_ti_file(char *filename, char *mode, FILE **fd)
{
  if((ti_file = fopen(filename, mode)) == NULL)
    {
      fprintf(stderr, "Unable to open this file: <%s>.\n", 
	      filename);
      return ERR_OPEN_FILE;
    }
  *fd = ti_file;
  
  return 0;
}

/*
  Close the file previously opened
*/
DLLEXPORT int DLLEXPORT2
ticalc_close_ti_file()
{
  fclose(ti_file);
  ti_file = NULL;

  return 0;
}


static void print_informations(void)
{
  DISPLAY(_("Libticalcs settings...\n"));
  
  switch(calc_type)
    {
    case CALC_TI92P:
      DISPLAY(_("  Calc type: TI92+\n"));
      break;
    case CALC_TI92:
      DISPLAY(_("  Calc type: TI92\n"));
      break;
    case CALC_TI89:
      DISPLAY(_("  Calc type: TI89\n"));
      break;
    case CALC_TI86:
      DISPLAY(_("  Calc type: TI86\n"));
      break;
    case CALC_TI85:
      DISPLAY(_("  Calc type: TI85\n"));
      break;
    case CALC_TI83P:
      DISPLAY(_("  Calc type: TI83+\n"));
      break;
    case CALC_TI83:
      DISPLAY(_("  Calc type: TI83\n"));
      break;
    case CALC_TI82:
      DISPLAY(_("  Calc type: TI82\n"));
      break;
    case CALC_TI73:
      DISPLAY(_("  Calc type: TI73\n"));
      break;
    default: // error !
      DISPLAY(_("Oops, there is a bug. Unknown calculator.\n"));
      break;
    }
}

/* Initialize i18n support under Win32 platforms */
#if defined(__WIN32__)
BOOL WINAPI DllMain(  HINSTANCE hinstDLL,  // handle to DLL module
		      DWORD fdwReason,     // reason for calling function
		      LPVOID lpvReserved)   // reserved);
{
#ifdef ENABLE_NLS
	char buffer[65536];
	HINSTANCE hDLL = hinstDLL;
	int i=0;

	GetModuleFileName(hinstDLL, (LPTSTR)&buffer, 65535);
    //fprintf(stderr, "libticables, filename: <%s>\n", buffer);

	for(i=strlen(buffer); i>=0; i--) { if(buffer[i]=='\\') break; }
	buffer[i]='\0';
	strcat(buffer, "\\locale\\");
	//fprintf(stderr, "libticalcs, locale_dir: <%s>\n", buffer);

	bindtextdomain (PACKAGE, buffer);	
	textdomain (PACKAGE);
#endif
}
#endif
