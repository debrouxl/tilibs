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

#ifndef __TICALCS_KEYS83P__
#define __TICALCS_KEYS83P__

// Scan codes of the TI83+ (TI's ti83plus.inc, line 2666)

#define KEY83P_Right          0x01
#define KEY83P_Left           0x02
#define KEY83P_Up             0x03
#define KEY83P_Down           0x04
#define KEY83P_Enter          0x05
#define KEY83P_AlphaEnter     0x06
#define KEY83P_AlphaUp        0x07
#define KEY83P_AlphaDown      0x08
#define KEY83P_Clear          0x09
#define KEY83P_Del            0x0A
#define KEY83P_Ins            0x0B
#define KEY83P_Recall         0x0C
#define KEY83P_LastEnt        0x0D
#define KEY83P_BOL            0x0E
#define KEY83P_EOL            0x0F
#define KEY83P_SelAll         0x10
#define KEY83P_UnselAll       0x11
#define KEY83P_LtoTI82        0x12
#define KEY83P_Backup         0x13
#define KEY83P_Recieve        0x14
#define KEY83P_LnkQuit        0x15
#define KEY83P_Trans          0x16
#define KEY83P_Rename         0x17
#define KEY83P_Overw          0x18
#define KEY83P_Omit           0x19
#define KEY83P_Cont           0x1A
#define KEY83P_SendID         0x1B
#define KEY83P_SendSW         0x1C
#define KEY83P_Yes            0x1D
#define KEY83P_NoWay          0x1E
#define KEY83P_vSendType      0x1F
#define KEY83P_OverWAll       0x20
#define KEY83P_No             0x25
#define KEY83P_KReset         0x26
#define KEY83P_App            0x27
#define KEY83P_Doug           0x28
#define KEY83P_Listflag       0x29
#define KEY73_menuStart       0x2B
#define KEY83P_AreYouSure     0x2B
#define KEY83P_AppsMenu       0x2C
#define KEY83P_Prgm           0x2D
#define KEY83P_Zoom           0x2E
#define KEY83P_Draw           0x2F
#define KEY83P_SPlot          0x30
#define KEY83P_Stat           0x31
#define KEY83P_Math           0x32
#define KEY83P_Test           0x33
#define KEY83P_Char           0x34
#define KEY83P_Vars           0x35
#define KEY83P_Mem            0x36
#define KEY83P_Matrix         0x37
#define KEY83P_Dist           0x38
#define KEY83P_Angle          0x39
#define KEY83P_List           0x3A
#define KEY83P_Calc           0x3B
#define KEY83P_Fin            0x3C
#define KEY73_menuEnd         0x3C
#define KEY83P_Catalog        0x3E
#define KEY83P_InputDone      0x3F
#define KEY83P_Off            0x3F
#define KEY83P_Quit           0x40
#define KEY73_appStart        0x40
#define KEY83P_LinkIO         0x41
#define KEY83P_MatrixEd       0x42
#define KEY83P_StatEd         0x43
#define KEY83P_Graph          0x44
#define KEY83P_Mode           0x45
#define KEY83P_PrgmEd         0x46
#define KEY83P_PrgmCr         0x47
#define KEY83P_Window         0x48
#define KEY83P_Yequ           0x49
#define KEY83P_Table          0x4A
#define KEY83P_TblSet         0x4B
#define KEY83P_ChkRAM         0x4C
#define KEY83P_DelMem         0x4D
#define KEY83P_ResetMem       0x4E
#define KEY83P_ResetDef		  0x4F
#define KEY83P_PrgmInput      0x50
#define KEY83P_ZFactEd        0x51
#define KEY83P_Error          0x52
#define KEY83P_SolveTVM       0x53
#define KEY83P_SolveRoot	  0x54
#define KEY83P_StatP          0x55
#define KEY83P_InfStat 		  0x56
#define KEY83P_Format         0x57
#define KEY83P_ExtApps        0x58
#define KEY83P_NewApps        0x59
#define KEY83P_Trace          0x5A
#define KEY83P_ZFit           0x5B
#define KEY83P_ZIn            0x5C
#define KEY83P_ZOut           0x5D
#define KEY83P_ZPrev          0x5E
#define KEY83P_Box            0x5F
#define KEY83P_Decml          0x60
#define KEY83P_SetZm          0x61
#define KEY83P_Squar          0x62
#define KEY83P_Std            0x63
#define KEY83P_Trig           0x64
#define KEY83P_UsrZm          0x65
#define KEY83P_ZSto           0x66
#define KEY83P_ZInt           0x67
#define KEY83P_ZStat          0x68
#define KEY73_echoStart2      0x69
#define KEY83P_Select         0x69
#define KEY83P_Circl          0x6A
#define KEY83P_ClDrw          0x6B
#define KEY83P_Line           0x6C
#define KEY83P_Pen            0x6D
#define KEY83P_PtChg          0x6E
#define KEY83P_PtOff          0x6F
#define KEY83P_PtOn           0x70
#define KEY83P_Vert           0x71
#define KEY83P_Horiz          0x72
#define KEY83P_Text           0x73
#define KEY83P_TanLn          0x74
#define KEY83P_Eval           0x75
#define KEY83P_Inters         0x76
#define KEY83P_DYDX           0x77
#define KEY83P_FnIntg         0x78
#define KEY83P_RootG          0x79
#define KEY83P_DYDT           0x7A
#define KEY83P_DXDT           0x7B
#define KEY83P_DRDo           0x7C
#define KEY83P_GFMin          0x7D
#define KEY83P_GFMax          0x7E
#define KEY73_EchoStart       0x7F
#define KEY83P_ListName       0x7F
#define KEY83P_Add            0x80
#define KEY83P_Sub            0x81
#define KEY83P_Mul            0x82
#define KEY83P_Div            0x83
#define KEY83P_Expon          0x84
#define KEY83P_LParen         0x85
#define KEY83P_RParen         0x86
#define KEY83P_LBrack         0x87
#define KEY83P_RBrack         0x88
#define KEY83P_Shade          0x89
#define KEY83P_Store          0x8A
#define KEY83P_Comma          0x8B
#define KEY83P_Chs            0x8C
#define KEY83P_DecPnt         0x8D
#define KEY83P_0              0x8E
#define KEY83P_1              0x8F
#define KEY83P_2              0x90
#define KEY83P_3              0x91
#define KEY83P_4              0x92
#define KEY83P_5              0x93
#define KEY83P_6              0x94
#define KEY83P_7              0x95
#define KEY83P_8              0x96
#define KEY83P_9              0x97
#define KEY83P_EE             0x98
#define KEY83P_Space          0x99
#define KEY83P_CapA           0x9A
#define KEY83P_CapB           0x9B
#define KEY83P_CapC           0x9C
#define KEY83P_CapD           0x9D
#define KEY83P_CapE           0x9E
#define KEY83P_CapF           0x9F
#define KEY83P_CapG           0xA0
#define KEY83P_CapH           0xA1
#define KEY83P_CapI           0xA2
#define KEY83P_CapJ           0xA3
#define KEY83P_CapK           0xA4
#define KEY83P_CapL           0xA5
#define KEY83P_CapM           0xA6
#define KEY83P_CapN           0xA7
#define KEY83P_CapO           0xA8
#define KEY83P_CapP           0xA9
#define KEY83P_CapQ           0xAA
#define KEY83P_CapR           0xAB
#define KEY83P_CapS           0xAC
#define KEY83P_CapT           0xAD
#define KEY83P_CapU           0xAE
#define KEY83P_CapV           0xAF
#define KEY83P_CapW           0xB0
#define KEY83P_CapX           0xB1
#define KEY83P_CapY           0xB2
#define KEY83P_CapZ           0xB3
#define KEY83P_Varx           0xB4
#define KEY83P_Pi             0xB5
#define KEY83P_Inv            0xB6
#define KEY83P_Sin            0xB7
#define KEY83P_ASin           0xB8
#define KEY83P_Cos            0xB9
#define KEY83P_ACos           0xBA
#define KEY83P_Tan            0xBB
#define KEY83P_ATan           0xBC
#define KEY83P_Square         0xBD
#define KEY83P_Sqrt           0xBE
#define KEY83P_Ln             0xBF
#define KEY83P_Exp            0xC0
#define KEY83P_Log            0xC1
#define KEY83P_ALog           0xC2
#define KEY83P_ToABC          0xC3
#define KEY83P_ClrTbl         0xC4
#define KEY83P_Ans            0xC5
#define KEY83P_Colon          0xC6
#define KEY83P_NDeriv         0xC7
#define KEY83P_FnInt          0xC8
#define KEY83P_Root           0xC9
#define KEY83P_Quest          0xCA
#define KEY83P_Quote          0xCB
#define KEY83P_Theta          0xCC
#define KEY83P_If             0xCD
#define KEY83P_Then           0xCE
#define KEY83P_Else           0xCF
#define KEY83P_For            0xD0
#define KEY83P_While          0xD1
#define KEY83P_Repeat         0xD2
#define KEY83P_End            0xD3
#define KEY83P_Pause          0xD4
#define KEY83P_Lbl            0xD5
#define KEY83P_Goto           0xD6
#define KEY83P_ISG            0xD7
#define KEY83P_DSL            0xD8
#define KEY83P_Menu           0xD9
#define KEY83P_Exec           0xDA
#define KEY83P_Return         0xDB
#define KEY83P_Stop           0xDC
#define KEY83P_Input          0xDD
#define KEY83P_Prompt         0xDE
#define KEY83P_Disp           0xDF
#define KEY83P_DispG          0xE0
#define KEY83P_DispT          0xE1
#define KEY83P_Output         0xE2
#define KEY83P_GetKey         0xE3
#define KEY83P_ClrHome        0xE4
#define KEY83P_PrtScr         0xE5
#define KEY83P_SinH           0xE6
#define KEY83P_CosH           0xE7
#define KEY83P_TanH           0xE8
#define KEY83P_ASinH          0xE9
#define KEY83P_ACosH          0xEA
#define KEY83P_ATanH          0xEB
#define KEY83P_LBrace         0xEC
#define KEY83P_RBrace         0xED
#define KEY83P_I              0xEE
#define KEY83P_CONSTeA        0xEF
#define KEY83P_Plot3          0xF0
#define KEY83P_FMin           0xF1
#define KEY83P_FMax           0xF2
#define KEY83P_L1A            0xF3
#define KEY83P_L2A            0xF4
#define KEY83P_L3A            0xF5
#define KEY83P_L4A            0xF6
#define KEY83P_L5A            0xF7
#define KEY83P_L6A            0xF8
#define KEY83P_unA            0xF9
#define KEY83P_vnA            0xFA
#define KEY83P_wnA            0xFB

// Array of keys

extern const CalcKey TI83P_KEYS[];

#endif
