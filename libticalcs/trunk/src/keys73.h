/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __TICALCS_KEYS73__
#define __TICALCS_KEYS73__

// Scan codes of the TI73 (TI's ti73.inc, line 2167)

#define KEY73_Right                 0x01
#define KEY73_Left                  0x02
#define KEY73_Up                    0x03
#define KEY73_Down                  0x04
#define KEY73_Enter                 0x05
#define KEY73_AlphaEnter            0x06
#define KEY73_AlphaUp               0x07
#define KEY73_AlphaDown             0x08
#define KEY73_Clear                 0x09
#define KEY73_Del                   0x0A
#define KEY73_Ins                   0x0B
#define KEY73_Recall                0x0C
#define KEY73_LastEnt               0x0D
#define KEY73_BOL                   0x0E
#define KEY73_EOL                   0x0F
#define KEY73_SelAll                0x10
#define KEY73_UnselAll              0x11
#define KEY73_LtoTI82               0x12
#define KEY73_Backup                0x13
#define KEY73_Recieve               0x14
#define KEY73_LnkQuit               0x15
#define KEY73_Trans                 0x16
#define KEY73_Rename                0x17
#define KEY73_Overw                 0x18
#define KEY73_Omit                  0x19
#define KEY73_Cont                  0x1A
#define KEY73_LtoTI83               0x1B
#define KEY73_SendApp               0x1C
#define KEY73_vApps                 0x1C
#define KEY73_SendId                0x1D
#define KEY73_SendSW                0x1E
#define KEY73_vALL                  0x1F
#define KEY73_vSendType             0x20
#define KEY73_NoWay                 0x21
#define KEY73_Yes                   0x22
#define KEY73_About                 0x23
#define KEY73_App                   0x24
#define KEY73_ProgExec              0x25
#define KEY73_Listflag              0x26
#define KEY73_menustart             0x2c
#define KEY73_Prgm                  0x2D
#define KEY73_Zoom                  0x2E
#define KEY73_Draw                  0x2F
#define KEY73_SPlot                 0x30
#define KEY73_Stat                  0x31
#define KEY73_Math                  0x32
#define KEY73_Test                  0x33
#define KEY73_AppsMenu              0x34
#define KEY73_Vars                  0x35
#define KEY73_Mem                   0x36
#define KEY73_ConstSetmenu          0x37
#define KEY73_ConvMenu              0x38
#define KEY73_Trigmenu              0x39
#define KEY73_LIST                  0x3A
#define KEY73_ConstMenu             0x3B
#define KEY73_Const                 0x3C
#define KEY73_Alpha                 0x3d
#define KEY73_Catalog               0x3e
#define KEY73_InputDone             0x3f
#define KEY73_Quit                  0x40
#define KEY73_LinkIO                0x41
#define KEY73_ChkApps               0x42
#define KEY73_StatEd                0x43
#define KEY73_Graph                 0x44
#define KEY73_Mode                  0x45
#define KEY73_PrgmEd                0x46
#define KEY73_PrgmCr                0x47
#define KEY73_Window                0x48
#define KEY73_Y                     0x49
#define KEY73_Table                 0x4A
#define KEY73_TblSet                0x4B
#define KEY73_ChkRAM                0x4C
#define KEY73_DelMem                0x4D
#define KEY73_ResetMem              0x4E
#define KEY73_ResetDef				0x4F
#define KEY73_PrgmInput             0x50
#define KEY73_ZFactEd               0x51
#define KEY73_Error                 0x52
#define KEY73_ExtApps               0x53
#define KEY73_SolveRoot				0x54
#define KEY73_StatP                 0x55
#define KEY73_ConstEd               0x56
#define KEY73_PrgmSetVar            0x57
#define KEY73_Format                0x58
#define echoStart1					0x5a
#define KEY73_Trace                 0x5a
#define KEY73_ZFit                  0x5b
#define KEY73_ZIn                   0x5c
#define KEY73_ZOut                  0x5d
#define KEY73_ZPrev                 0x5e
#define KEY73_ZBox                  0x5f
#define KEY73_ZDecml                0x60
#define KEY73_SetZm                 0x61
#define KEY73_ZSquar                0x62
#define KEY73_ZStd                  0x63
#define KEY73_ZTrig                 0x64
#define KEY73_ZQuad1                0x65
#define KEY73_ZInt                  0x67
#define KEY73_ZStat                 0x68
#define echoStart2					0x69
#define KEY73_Select                0x69
#define KEY73_Circl                 0x6a
#define KEY73_ClDrw                 0x6b
#define KEY73_Line                  0x6c
#define KEY73_Pen                   0x6d
#define KEY73_PtChg                 0x6e
#define KEY73_PtOff                 0x6f
#define KEY73_PtOn                  0x70
#define KEY73_Vert                  0x71
#define KEY73_Horiz                 0x72
#define KEY73_Text                  0x73
#define KEY73_TanLn                 0x74
#define KEY73_ManFit                0x75
#define KEY73_Eval                  0x76
#define echoStart					0x7d
#define KEY73_Remain                0x7d
#define KEY73_Plot3                 0x7e
#define KEY73_ListName              0x7f
#define KEY73_Add                   0x80
#define KEY73_Sub                   0x81
#define KEY73_Mul                   0x82
#define KEY73_Div                   0x83
#define KEY73_Expon                 0x84
#define KEY73_LParen                0x85
#define KEY73_RParen                0x86
#define KEY73_LBrack                0x87
#define KEY73_RBrack                0x88
#define KEY73_Shade                 0x89
#define KEY73_Store                 0x8a
#define KEY73_Comma                 0x8b
#define KEY73_Chs                   0x8c
#define KEY73_DecPnt                0x8d
#define KEY73_0                     0x8e
#define KEY73_1                     0x8f
#define KEY73_2                     0x90
#define KEY73_3                     0x91
#define KEY73_4                     0x92
#define KEY73_5                     0x93
#define KEY73_6                     0x94
#define KEY73_7                     0x95
#define KEY73_8                     0x96
#define KEY73_9                     0x97
#define KEY73_EE                    0x98
#define KEY73_Space                 0x99
#define KEY73_CapA                  0x9A
#define KEY73_CapB                  0x9B
#define KEY73_CapC                  0x9C
#define KEY73_CapD                  0x9D
#define KEY73_CapE                  0x9E
#define KEY73_CapF                  0x9F
#define KEY73_CapG                  0xA0
#define KEY73_CapH                  0xA1
#define KEY73_CapI                  0xA2
#define KEY73_CapJ                  0xA3
#define KEY73_CapK                  0xA4
#define KEY73_CapL                  0xA5
#define KEY73_CapM                  0xA6
#define KEY73_CapN                  0xA7
#define KEY73_CapO                  0xA8
#define KEY73_CapP                  0xA9
#define KEY73_CapQ                  0xAA
#define KEY73_CapR                  0xAB
#define KEY73_CapS                  0xAC
#define KEY73_CapT                  0xAD
#define KEY73_CapU                  0xAE
#define KEY73_CapV                  0xAF
#define KEY73_CapW                  0xB0
#define KEY73_CapX                  0xB1
#define KEY73_CapY                  0xB2
#define KEY73_CapZ                  0xB3
#define KEY73_Varx                  0xB4
#define KEY73_Pi                    0xB5
#define KEY73_Inv                   0xB6
#define KEY73_Sin                   0xB7
#define KEY73_ASin                  0xB8
#define KEY73_Cos                   0xB9
#define KEY73_ACos                  0xBA
#define KEY73_Tan                   0xBB
#define KEY73_ATan                  0xBC
#define KEY73_Square                0xBD
#define KEY73_Sqrt                  0xBE
#define KEY73_Ln                    0xBF
#define KEY73_Exp                   0xC0
#define KEY73_Log                   0xC1
#define KEY73_ALog                  0xC2
#define KEY73_ToABC                 0xC3
#define KEY73_Ans                   0xc5
#define KEY73_Colon                 0xc6
#define KEY73_Unit                  0xc7
#define KEY73_FracSlash             0xc8
#define KEY73_Root                  0xc9
#define KEY73_Quest                 0xca
#define KEY73_Quote                 0xcb
#define KEY73_Theta                 0xcc
#define KEY73_If                    0xcd
#define KEY73_Then                  0xce
#define KEY73_Else                  0xcf
#define KEY73_For                   0xd0
#define KEY73_While                 0xd1
#define KEY73_Repeat                0xd2
#define KEY73_End                   0xd3
#define KEY73_Pause                 0xd4
#define KEY73_Lbl                   0xd5
#define KEY73_Goto                  0xd6
#define KEY73_ISG                   0xd7
#define KEY73_DSL                   0xd8
#define KEY73_Menu                  0xd9
#define KEY73_Exec                  0xda
#define KEY73_Return                0xdb
#define KEY73_Stop                  0xdc
#define KEY73_Input                 0xdd
#define KEY73_Prompt                0xde
#define KEY73_Disp                  0xdf
#define KEY73_DispG                 0xe0
#define KEY73_DispT                 0xe1
#define KEY73_Output                0xe2
#define KEY73_GetKey                0xe3
#define KEY73_ClrHome               0xe4
#define KEY73_prtscr				0xe5
#define KEY73_Percent               0xe6
#define KEY73_MixSimp               0xe7
#define KEY73_FracDec               0xe8
#define KEY73_Simp                  0xe9
#define KEY73_UnredF                0xea
#define KEY73_IDiv                  0xeb
#define KEY73_Convert               0xec
#define KEY73_LBrace                0xed
#define KEY73_RBrace                0xee
#define KEY73_L1A                   0xef
#define KEY73_L2A                   0xf0
#define KEY73_L3A                   0xf1
#define KEY73_L4A                   0xf2
#define KEY73_L5A                   0xf3
#define KEY73_L6A                   0xf4
#define KEY73_ConvLength            0xf5
#define KEY73_ConvArea              0xf6
#define KEY73_ConvVolume            0xf7
#define KEY73_ConvTime              0xf8
#define KEY73_ConvTemp              0xf9
#define KEY73_ConvMass              0xfa
#define KEY73_ConvSpeed             0xfb

// Array of keys

extern const CalcKey TI73_KEYS[];

#endif

