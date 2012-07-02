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



// dusb_vpkt.c

void dusb_vtl_pkt_purge(void);



// nsp_vpkt.c

extern uint16_t nsp_src_port;
extern uint16_t nsp_dst_port;
void nsp_vtl_pkt_purge(void);



// cmd73.c

#define ti73_send_VAR(a,b,c,d)		ti73_send_VAR_h(handle,a,b,c,d)
#define ti73_send_VAR2(a,b,c,d,e)	ti73_send_VAR2_h(handle,a,b,c,d,e)
#define ti73_send_CTS()				ti73_send_CTS_h(handle)
#define ti73_send_XDP(a,b)			ti73_send_XDP_h(handle,a,b)
#define ti73_send_FLSH()			ti73_send_FLSH_h(handle)
#define ti73_send_SKP(a)			ti73_send_SKP_h(handle,a)
#define ti73_send_ACK()				ti73_send_ACK_h(handle)
#define ti73_send_ERR()				ti73_send_ERR_h(handle)
#define ti73_send_RDY()				ti73_send_RDY_h(handle)
#define ti73_send_SCR()				ti73_send_SCR_h(handle)
#define ti73_send_KEY(a)			ti73_send_KEY_h(handle,a)
#define ti73_send_EOT()				ti73_send_EOT_h(handle)
#define ti73_send_REQ(a,b,c,d)		ti73_send_REQ_h(handle,a,b,c,d)
#define ti73_send_REQ2(a,b,c,d)		ti73_send_REQ2_h(handle,a,b,c,d)
#define ti73_send_RTS(a,b,c,d)		ti73_send_RTS_h(handle,a,b,c,d)
#define ti73_send_VER()				ti73_send_VER_h(handle)
#define ti73_send_DEL(a,b,c,d)		ti73_send_DEL_h(handle,a,b,c,d)

#define ti73_recv_VAR(a,b,c,d)		ti73_recv_VAR_h(handle,a,b,c,d)
#define ti73_recv_VAR2(a,b,c,d,e)	ti73_recv_VAR2_h(handle,a,b,c,d,e)
#define ti73_recv_CTS(a)			ti73_recv_CTS_h(handle,a)
#define ti73_recv_SKP(a)			ti73_recv_SKP_h(handle,a)
#define ti73_recv_XDP(a,b)			ti73_recv_XDP_h(handle,a,b)
#define ti73_recv_ACK(a)			ti73_recv_ACK_h(handle,a)
#define ti73_recv_RTS(a,b,c,d,e)	ti73_recv_RTS_h(handle,a,b,c,d,e)



// cmd82.c

#define ti82_send_VAR(a,b,c)		ti82_send_VAR_h(handle, a, b, c)
#define ti82_send_CTS()				ti82_send_CTS_h(handle)
#define ti82_send_XDP(a,b)			ti82_send_XDP_h(handle, a, b)
#define ti82_send_SKP(a,b)			ti82_send_SKP_h(handle, a, b)
#define ti82_send_ACK()				ti82_send_ACK_h(handle)
#define ti82_send_ERR()				ti82_send_ERR_h(handle)
#define ti82_send_SCR()				ti82_send_SCR_h(handle)
#define ti83_send_KEY(a)			ti83_send_KEY_h(handle,a)
#define ti82_send_KEY(a)			ti82_send_KEY_h(handle, a)
#define ti82_send_EOT()				ti82_send_EOT_h(handle)
#define ti82_send_REQ(a,b,c)		ti82_send_REQ_h(handle, a, b, c)
#define ti82_send_RTS(a,b,c)		ti82_send_RTS_h(handle, a, b, c)

#define ti82_recv_VAR(a,b,c)		ti82_recv_VAR_h(handle, a, b, c)
#define ti82_recv_CTS()				ti82_recv_CTS_h(handle)
#define ti82_recv_SKP(a)			ti82_recv_SKP_h(handle, a)
#define ti82_recv_XDP(a,b)			ti82_recv_XDP_h(handle, a, b)
#define ti82_recv_ACK(a)			ti82_recv_ACK_h(handle, a)
#define ti82_recv_RTS(a,b,c)		ti82_recv_RTS_h(handle, a, b, c)



// cmd85.c

#define ti85_send_VAR(a,b,c)		ti85_send_VAR_h(handle, a, b, c)
#define ti85_send_CTS()				ti85_send_CTS_h(handle)
#define ti85_send_XDP(a,b)			ti85_send_XDP_h(handle, a, b)
#define ti85_send_SKP(a,b)			ti85_send_SKP_h(handle, a, b)
#define ti85_send_ACK()				ti85_send_ACK_h(handle)
#define ti85_send_ERR()				ti85_send_ERR_h(handle)
#define ti85_send_SCR()				ti85_send_SCR_h(handle)
#define ti85_send_KEY(a)			ti85_send_KEY_h(handle, a)
#define ti85_send_EOT()				ti85_send_EOT_h(handle)
#define ti85_send_REQ(a,b,c)		ti85_send_REQ_h(handle, a, b, c)
#define ti85_send_RTS(a,b,c)		ti85_send_RTS_h(handle, a, b, c)

#define ti85_recv_VAR(a,b,c)		ti85_recv_VAR_h(handle, a, b, c)
#define ti85_recv_CTS()				ti85_recv_CTS_h(handle)
#define ti85_recv_SKP(a)			ti85_recv_SKP_h(handle, a)
#define ti85_recv_XDP(a,b)			ti85_recv_XDP_h(handle, a, b)
#define ti85_recv_ACK(a)			ti85_recv_ACK_h(handle, a)
#define ti85_recv_RTS(a,b,c)		ti85_recv_RTS_h(handle, a, b, c)



// cmd89.c

#define ti89_send_VAR(a,b,c)		ti89_send_VAR_h(handle, a, b, c)
#define ti89_send_CTS()				ti89_send_CTS_h(handle)
#define ti89_send_XDP(a,b)			ti89_send_XDP_h(handle, a, b)
#define ti89_send_SKP(a)			ti89_send_SKP_h(handle, a)
#define ti89_send_ACK()				ti89_send_ACK_h(handle)
#define ti89_send_ERR()				ti89_send_ERR_h(handle)
#define ti89_send_RDY()				ti89_send_RDY_h(handle)
#define ti89_send_SCR()				ti89_send_SCR_h(handle)
#define ti89_send_CNT()				ti89_send_CNT_h(handle)
#define ti89_send_KEY(a)			ti89_send_KEY_h(handle, a)
#define ti89_send_EOT()				ti89_send_EOT_h(handle)
#define ti89_send_REQ(a,b,c)		ti89_send_REQ_h(handle, a, b, c)
#define ti89_send_RTS(a,b,c)		ti89_send_RTS_h(handle, a, b, c)
#define ti89_send_RTS2(a,b,c)		ti89_send_RTS2_h(handle, a, b, c)
#define ti89_send_VER()				ti89_send_VER_h(handle)
#define ti89_send_DEL(a,b,c)		ti89_send_DEL_h(handle, a, b, c)

#define ti89_recv_VAR(a,b,c)		ti89_recv_VAR_h(handle, a, b, c)
#define ti89_recv_CTS()				ti89_recv_CTS_h(handle)
#define ti89_recv_SKP(a)			ti89_recv_SKP_h(handle, a)
#define ti89_recv_XDP(a,b)			ti89_recv_XDP_h(handle, a , b)
#define ti89_recv_ACK(a)			ti89_recv_ACK_h(handle, a)
#define ti89_recv_CNT()				ti89_recv_CNT_h(handle)
#define ti89_recv_EOT()				ti89_recv_EOT_h(handle)
#define ti89_recv_RTS(a,b,c)		ti89_recv_RTS_h(handle, a, b, c)



// cmd92.c

#define ti92_send_VAR(a,b,c)		ti92_send_VAR_h(handle, a, b, c)
#define ti92_send_CTS()				ti92_send_CTS_h(handle)
#define ti92_send_XDP(a,b)			ti92_send_XDP_h(handle, a, b)
#define ti92_send_SKP(a)			ti92_send_SKP_h(handle, a)
#define ti92_send_ACK()				ti92_send_ACK_h(handle)
#define ti92_send_ERR()				ti92_send_ERR_h(handle)
#define ti92_send_RDY()				ti92_send_RDY_h(handle)
#define ti92_send_SCR()				ti92_send_SCR_h(handle)
#define ti92_send_CNT()				ti92_send_CNT(handle)
#define ti92_send_KEY(a)			ti92_send_KEY_h(handle, a)
#define ti92_send_EOT()				ti92_send_EOT_h(handle)
#define ti92_send_REQ(a,b,c)		ti92_send_REQ_h(handle, a, b, c)
#define ti92_send_RTS(a,b,c)		ti92_send_RTS_h(handle, a, b, c)

#define ti92_recv_VAR(a,b,c)		ti92_recv_VAR_h(handle, a, b, c)
#define ti92_recv_CTS()				ti92_recv_CTS_h(handle)
#define ti92_recv_SKP(a)			ti92_recv_SKP_h(handle, a)
#define ti92_recv_XDP(a,b)			ti92_recv_XDP_h(handle, a , b)
#define ti92_recv_ACK(a)			ti92_recv_ACK_h(handle, a)
#define ti92_recv_CNT()				ti92_recv_CNT_h(handle)
#define ti92_recv_EOT()				ti92_recv_EOT_h(handle)
#define ti92_recv_RTS(a,b,c)		ti92_recv_RTS_h(handle, a, b, c)



// dusb_cmd.c

#define CA(x)   (const CalcAttr **)(x)
#define CP(x)   (const CalcParam **)(x)



#endif // __TICALCS_INTERNAL__
