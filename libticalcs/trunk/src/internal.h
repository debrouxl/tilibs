/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2012  Romain Liévin
 *  Copyright (C) 2012       Lionel Debroux
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * \file internal.h
 * \brief Definitions for internal (libticalcs) usage.
 */

#ifndef __TICALCS_INTERNAL__
#define __TICALCS_INTERNAL__

// dbus_pkt.c

int dbus_recv_2(CalcHandle* cable, uint8_t* host, uint8_t* cmd, uint16_t* length, uint8_t* data);

void pad_buffer_to_8_chars(uint8_t *varname, uint8_t value);

#ifndef WORDS_BIGENDIAN
# define fixup(x) (x &= 0x0000ffff)
#else
# define fixup(x) (x >>= 16)
#endif /* !WORDS_BIGENDIAN */



// cmd82.c

int ti82_send_asm_exec(CalcHandle*, VarEntry * var);



// dusb_vpkt.c

void dusb_vtl_pkt_purge(void);



// nsp_vpkt.c

extern uint16_t nsp_src_port;
extern uint16_t nsp_dst_port;
void nsp_vtl_pkt_purge(void);



// dusb_cmd.c

#define CA(x)   (const DUSBCalcAttr **)(x)
#define CP(x)   (const DUSBCalcParam **)(x)



#endif // __TICALCS_INTERNAL__
