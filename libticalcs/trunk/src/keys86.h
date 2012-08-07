/* Hey EMACS -*- linux-c -*- */
/* $Id: keys83p.h 1179 2005-06-06 14:42:32Z roms $ */

/*  libticalcs - Ti Calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2005  Romain Liévin
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

#ifndef __TICALCS_KEYS86__
#define __TICALCS_KEYS86__

// Scan codes of the TI86 (thanks to B. Moody)

#define KEY86_Right            0x01
#define KEY86_Left             0x02
#define KEY86_Up               0x03
#define KEY86_Down             0x04
#define KEY86_Colon            0x05
#define KEY86_Enter            0x06
#define KEY86_Exit             0x07
#define KEY86_Clear            0x08
#define KEY86_Del              0x09
#define KEY86_Ins              0x0A
#define KEY86_Next             0x0B
#define KEY86_Add              0x0C
#define KEY86_Sub              0x0D
#define KEY86_Mul              0x0E
#define KEY86_Div              0x0F
#define KEY86_Expon            0x10
#define KEY86_LParen           0x11
#define KEY86_RParen           0x12
#define KEY86_LBrack           0x13
#define KEY86_RBrack           0x14
#define KEY86_Equal            0x15
#define KEY86_Store            0x16
#define KEY86_Recall           0x17
#define KEY86_Comma            0x18
#define KEY86_Ang              0x19
#define KEY86_Chs              0x1A
#define KEY86_DecPnt           0x1B
#define KEY86_0                0x1C
#define KEY86_1                0x1D
#define KEY86_2                0x1E
#define KEY86_3                0x1F
#define KEY86_4                0x20
#define KEY86_5                0x21
#define KEY86_6                0x22
#define KEY86_7                0x23
#define KEY86_8                0x24
#define KEY86_9                0x25
#define KEY86_EE               0x26
#define KEY86_Space            0x27
#define KEY86_CapA             0x28
#define KEY86_CapB             0x29
#define KEY86_CapC             0x2A
#define KEY86_CapD             0x2B
#define KEY86_CapE             0x2C
#define KEY86_CapF             0x2D
#define KEY86_CapG             0x2E
#define KEY86_CapH             0x2F
#define KEY86_CapI             0x30
#define KEY86_CapJ             0x31
#define KEY86_CapK             0x32
#define KEY86_CapL             0x33
#define KEY86_CapM             0x34
#define KEY86_CapN             0x35
#define KEY86_CapO             0x36
#define KEY86_CapP             0x37
#define KEY86_CapQ             0x38
#define KEY86_CapR             0x39
#define KEY86_CapS             0x3A
#define KEY86_CapT             0x3B
#define KEY86_CapU             0x3C
#define KEY86_CapV             0x3D
#define KEY86_CapW             0x3E
#define KEY86_CapX             0x3F
#define KEY86_CapY             0x40
#define KEY86_CapZ             0x41
#define KEY86_a                0x42
#define KEY86_b                0x43
#define KEY86_c                0x44
#define KEY86_d                0x45
#define KEY86_e                0x46
#define KEY86_f                0x47
#define KEY86_g                0x48
#define KEY86_h                0x49
#define KEY86_i                0x4A
#define KEY86_j                0x4B
#define KEY86_k                0x4C
#define KEY86_l                0x4D
#define KEY86_m                0x4E
#define KEY86_n                0x4F
#define KEY86_o                0x50
#define KEY86_p                0x51
#define KEY86_q                0x52
#define KEY86_r                0x53
#define KEY86_s                0x54
#define KEY86_t                0x55
#define KEY86_u                0x56
#define KEY86_v                0x57
#define KEY86_w                0x58
#define KEY86_x                0x59
#define KEY86_y                0x5A
#define KEY86_z                0x5B
#define KEY86_Varx             0x5C
#define KEY86_Ans              0x5D
#define KEY86_Pi               0x5E
#define KEY86_Inv              0x5F
#define KEY86_Sin              0x60
#define KEY86_ASin             0x61
#define KEY86_Cos              0x62
#define KEY86_ACos             0x63
#define KEY86_Tan              0x64
#define KEY86_ATan             0x65
#define KEY86_Square           0x66
#define KEY86_Sqrt             0x67
#define KEY86_Ln               0x68
#define KEY86_Exp              0x69
#define KEY86_Log              0x6A
#define KEY86_ALog             0x6B
#define KEY86_Math             0x6C
#define KEY86_Cplx             0x6D
#define KEY86_String           0x6E
#define KEY86_Test             0x6F
#define KEY86_Conv             0x70
#define KEY86_Char             0x71
#define KEY86_Base             0x72
#define KEY86_Custom           0x73
#define KEY86_Vars             0x74
#define KEY86_Catalog          0x75
#define KEY86_Quit             0x76
#define KEY86_LastEnt          0x77
#define KEY86_LinkIO           0x78
#define KEY86_Mem              0x79
#define KEY86_List             0x7A
#define KEY86_Vector           0x7B
#define KEY86_Const            0x7C
#define KEY86_Matrix           0x7D
#define KEY86_Poly             0x7E
#define KEY86_Simult           0x7F
#define KEY86_Stat             0x80
#define KEY86_GrMenu           0x81
#define KEY86_Mode             0x82
#define KEY86_Prgm             0x83
#define KEY86_Calcu            0x84
#define KEY86_Solver           0x85
#define KEY86_Table            0x86
#define KEY86_BOL              0x87
#define KEY86_EOL              0x88
#define KEY86_F1               0xC2
#define KEY86_F2               0xC3
#define KEY86_F3               0xC4
#define KEY86_F4               0xC5
#define KEY86_F5               0xC6
#define KEY86_F6               0xC7
#define KEY86_F7               0xC8
#define KEY86_F8               0xC9
#define KEY86_F9               0xCA
#define KEY86_F10              0xCB

#endif
