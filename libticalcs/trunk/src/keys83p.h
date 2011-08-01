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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __TICALCS_KEYS83P__
#define __TICALCS_KEYS83P__

// Scan codes of the TI83+ (TI's ti83plus.inc, line 1798)

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
#define KEY83P_menuStart       0x2B

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

#define KEY83P_menuEnd         0x3C

#define KEY83P_Catalog        0x3E
#define KEY83P_InputDone      0x3F
#define KEY83P_Off            0x3F

#define KEY83P_Quit           0x40
#define KEY83P_appStart        0x40


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



#define KEY83P_echoStart1	  0x5A

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

#define KEY83P_echoStart2      0x69

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


#define KEY83P_EchoStart       0x7F

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

// FE (ExtendEcho) prefixed keys:

#define kE1BT					0xfe00

#define KEY83P_DrawInv          0xfe00
#define KEY83P_DrawF            0xfe01
#define KEY83P_PixelOn          0xfe02
#define KEY83P_PixelOff         0xfe03
#define KEY83P_PxlTest          0xfe04
#define KEY83P_RCGDB            0xfe05
#define KEY83P_RCPic            0xfe06
#define KEY83P_STGDB            0xfe07
#define KEY83P_STPic            0xfe08
#define KEY83P_Abs              0xfe09
#define KEY83P_TEqu             0xfe0a
#define KEY83P_TNoteQ           0xfe0b
#define KEY83P_TGT              0xfe0c
#define KEY83P_TGTE             0xfe0d
#define KEY83P_TLT              0xfe0e
#define KEY83P_TLTE             0xfe0f

#define KEY83P_And              0xfe10
#define KEY83P_Or               0xfe11
#define KEY83P_Xor              0xfe12
#define KEY83P_Not              0xfe13

#define KEY83P_LR1              0xfe14

#define KEY83P_XRoot            0xfe15
#define KEY83P_Cube             0xfe16
#define KEY83P_CbRt             0xfe17
#define KEY83P_ToDec            0xfe18

#define KEY83P_CubicR           0xfe19
#define KEY83P_QuartR           0xfe1a

#define KEY83P_Plot1            0xfe1b
#define KEY83P_Plot2            0xfe1c


#define KEY83P_Round            0xfe1d
#define KEY83P_IPart            0xfe1e
#define KEY83P_FPart            0xfe1f
#define KEY83P_Int              0xfe20

#define KEY83P_Rand             0xfe21
#define KEY83P_NPR              0xfe22
#define KEY83P_NCR              0xfe23
#define KEY83P_XFactorial       0xfe24

#define KEY83P_Rad              0xfe25
#define KEY83P_Degr             0xfe26
#define KEY83P_APost            0xfe27
#define KEY83P_ToDMS            0xfe28
#define KEY83P_RToPo            0xfe29
#define KEY83P_RToPr            0xfe2a
#define KEY83P_PToRx            0xfe2b
#define KEY83P_PToRy            0xfe2c

#define KEY83P_RowSwap          0xfe2d
#define KEY83P_RowPlus          0xfe2e
#define KEY83P_TimRow           0xfe2f
#define KEY83P_TRowP            0xfe30

#define KEY83P_SortA            0xfe31
#define KEY83P_SortD            0xfe32
#define KEY83P_Seq              0xfe33

#define KEY83P_Min              0xfe34
#define KEY83P_Max              0xfe35
#define KEY83P_Mean             0xfe36
#define KEY83P_Median           0xfe37
#define KEY83P_Sum              0xfe38
#define KEY83P_Prod             0xfe39

#define KEY83P_Det              0xfe3a
#define KEY83P_Transp           0xfe3b
#define KEY83P_Dim              0xfe3c
#define KEY83P_Fill             0xfe3d
#define KEY83P_Ident            0xfe3e
#define KEY83P_Randm            0xfe3f
#define KEY83P_Aug              0xfe40

#define KEY83P_OneVar                kE1BT+65
#define KEY83P_TwoVar                kE1BT+66
#define KEY83P_LR                    kE1BT+67
#define KEY83P_LRExp                 kE1BT+68
#define KEY83P_LRLn                  kE1BT+69
#define KEY83P_LRPwr                 kE1BT+70
#define KEY83P_MedMed                kE1BT+71
#define KEY83P_Quad                  kE1BT+72
#define KEY83P_ClrLst                kE1BT+73
#define KEY83P_Hist                  kE1BT+74
#define KEY83P_xyLine                kE1BT+75
#define KEY83P_Scatter               kE1BT+76

#define KEY83P_mRad                  kE1BT+77
#define KEY83P_mDeg                  kE1BT+78
#define KEY83P_mNormF                kE1BT+79
#define KEY83P_mSci                  kE1BT+80
#define KEY83P_mEng                  kE1BT+81
#define KEY83P_mFloat                kE1BT+82

#define KEY83P_Fix                   kE1BT+83
#define KEY83P_SplitOn               kE1BT+84
#define KEY83P_FullScreen            kE1BT+85
#define KEY83P_Stndrd                kE1BT+86
#define KEY83P_Param                 kE1BT+87
#define KEY83P_Polar                 kE1BT+88
#define KEY83P_SeqG                  kE1BT+89
#define KEY83P_AFillOn               kE1BT+90
#define KEY83P_AFillOff              kE1BT+91
#define KEY83P_ACalcOn               kE1BT+92
#define KEY83P_ACalcOff              kE1BT+93
#define KEY83P_FNOn                  kE1BT+94
#define KEY83P_FNOff                 kE1BT+95

#define KEY83P_PlotsOn               kE1BT+96
#define KEY83P_PlotsOff              kE1BT+97

#define KEY83P_PixelChg              kE1BT+98

#define KEY83P_SendMBL               kE1BT+99
#define KEY83P_RecvMBL               kE1BT+100

#define KEY83P_BoxPlot               kE1BT+101
#define KEY83P_BoxIcon               kE1BT+102
#define KEY83P_CrossIcon             kE1BT+103
#define KEY83P_DotIcon               kE1BT+104

#define kE2BT                  (kE1BT+105)

#define KEY83P_Seqential             kE2BT
#define KEY83P_SimulG                kE2BT+1
#define KEY83P_PolarG                kE2BT+2
#define KEY83P_RectG                 kE2BT+3
#define KEY83P_CoordOn               kE2BT+4
#define KEY83P_CoordOff              kE2BT+5
#define KEY83P_DrawLine              kE2BT+6
#define KEY83P_DrawDot               kE2BT+7
#define KEY83P_AxisOn                kE2BT+8
#define KEY83P_AxisOff               kE2BT+9
#define KEY83P_GridOn                kE2BT+10
#define KEY83P_GridOff               kE2BT+11
#define KEY83P_LblOn                 kE2BT+12
#define KEY83P_LblOff                kE2BT+13

#define KEY83P_L1                    kE2BT+14
#define KEY83P_L2                    kE2BT+15
#define KEY83P_L3                    kE2BT+16
#define KEY83P_L4                    kE2BT+17
#define KEY83P_L5                    kE2BT+18
#define KEY83P_L6                    kE2BT+19

// FC (ExtendEcho2) prefixed keys:

#define kE2BT2					0xfc00

#define KEY83P_GDB1             0xfc00
#define KEY83P_GDB2             0xfc01
#define KEY83P_GDB3             0xfc02
#define KEY83P_Y1               0xfc03
#define KEY83P_Y2               0xfc04
#define KEY83P_Y3               0xfc05
#define KEY83P_Y4               0xfc06
#define KEY83P_Y5               0xfc07
#define KEY83P_Y6               0xfc08
#define KEY83P_Y7               0xfc09
#define KEY83P_Y8               0xfc0a
#define KEY83P_Y9               0xfc0b
#define KEY83P_Y0               0xfc0c
#define KEY83P_X1T              0xfc0d
#define KEY83P_Y1T              0xfc0e
#define KEY83P_X2T              0xfc0f
#define KEY83P_Y2T              0xfc10
#define KEY83P_X3T              0xfc11
#define KEY83P_Y3T              0xfc12
#define KEY83P_X4T              0xfc13
#define KEY83P_Y4T              0xfc14
#define KEY83P_X5T              0xfc15
#define KEY83P_Y5T              0xfc16
#define KEY83P_X6T              0xfc17
#define KEY83P_Y6T              0xfc18
#define KEY83P_R1               0xfc19
#define KEY83P_R2               0xfc1a
#define KEY83P_R3               0xfc1b
#define KEY83P_R4               0xfc1c
#define KEY83P_R5               0xfc1d
#define KEY83P_R6               0xfc1e
#define KEY83P_GDB4             0xfc1f
#define KEY83P_GDB5             0xfc20
#define KEY83P_GDB6             0xfc21
#define KEY83P_Pic4             0xfc22
#define KEY83P_Pic5             0xfc23
#define KEY83P_Pic6             0xfc24
#define KEY83P_GDB7             0xfc25
#define KEY83P_GDB8             0xfc26
#define KEY83P_GDB9             0xfc27
#define KEY83P_GDB0             0xfc28
#define KEY83P_Pic7             0xfc29
#define KEY83P_Pic8             0xfc2a
#define KEY83P_Pic9             0xfc2b
#define KEY83P_Pic0             0xfc2c
#define KEY83P_StatN            0xfc2d
#define KEY83P_XMean            0xfc2e
#define KEY83P_Conj             0xfc2f
#define KEY83P_Real             0xfc30
#define KEY83P_FAngle           0xfc31
#define KEY83P_LCM              0xfc32
#define KEY83P_GCD              0xfc33
#define KEY83P_RandInt          0xfc34
#define KEY83P_RandNorm         0xfc35
#define KEY83P_ToPolar          0xfc36
#define KEY83P_ToRect           0xfc37
#define KEY83P_YMean            0xfc38
#define KEY83P_StdX             0xfc39
#define KEY83P_StdX1            0xfc3a
#define KEY83P_w0               0xfc3b
#define KEY83P_MatF             0xfc3c
#define KEY83P_MatG             0xfc3d
#define KEY83P_MatRH            0xfc3e
#define KEY83P_MatI             0xfc3f
#define KEY83P_MatJ             0xfc40
#define KEY83P_YMean1           0xfc41
#define KEY83P_StdY             0xfc42
#define KEY83P_StdY1            0xfc43
#define KEY83P_MatToLst         0xfc44
#define KEY83P_LstToMat         0xfc45
#define KEY83P_CumSum           0xfc46
#define KEY83P_DeltaLst         0xfc47
#define KEY83P_StdDev           0xfc48
#define KEY83P_Variance         0xfc49
#define KEY83P_Length           0xfc4a
#define KEY83P_EquToStrng       0xfc4b
#define KEY83P_StrngToEqu       0xfc4c
#define KEY83P_Expr             0xfc4d
#define KEY83P_SubStrng         0xfc4e
#define KEY83P_InStrng          0xfc4f
#define KEY83P_Str1             0xfc50
#define KEY83P_Str2             0xfc51
#define KEY83P_Str3             0xfc52
#define KEY83P_Str4             0xfc53
#define KEY83P_Str5             0xfc54
#define KEY83P_Str6             0xfc55
#define KEY83P_Str7             0xfc56
#define KEY83P_Str8             0xfc57
#define KEY83P_Str9             0xfc58
#define KEY83P_Str0             0xfc59
#define KEY83P_FinN                  kE2BT2+90
#define KEY83P_FinI                  kE2BT2+91
#define KEY83P_FinPV                 kE2BT2+92
#define KEY83P_FinPMT                kE2BT2+93
#define KEY83P_FinFV                 kE2BT2+94
#define KEY83P_FinPY                 kE2BT2+95
#define KEY83P_FinCY                 kE2BT2+96
#define KEY83P_FinFPMT               kE2BT2+97
#define KEY83P_FinFI                 kE2BT2+98
#define KEY83P_FinFPV                kE2BT2+99
#define KEY83P_FinFN                 kE2BT2+100
#define KEY83P_FinFFV                kE2BT2+101
#define KEY83P_FinNPV                kE2BT2+102
#define KEY83P_FinIRR                kE2BT2+103
#define KEY83P_FinBAL                kE2BT2+104
#define KEY83P_FinPRN                kE2BT2+105
#define KEY83P_FinINT                kE2BT2+106
#define KEY83P_SumX                  kE2BT2+107
#define KEY83P_SumX2                 kE2BT2+108
#define KEY83P_FinToNom              kE2BT2+109
#define KEY83P_FinToEff              kE2BT2+110
#define KEY83P_FinDBD                kE2BT2+111
#define KEY83P_StatVP                kE2BT2+112
#define KEY83P_StatZ                 kE2BT2+113
#define KEY83P_StatT                 kE2BT2+114
#define KEY83P_StatChi               kE2BT2+115
#define KEY83P_StatF                 kE2BT2+116
#define KEY83P_StatDF                kE2BT2+117
#define KEY83P_StatPhat              kE2BT2+118
#define KEY83P_StatPhat1             kE2BT2+119
#define KEY83P_StatPhat2             kE2BT2+120
#define KEY83P_StatMeanX1            kE2BT2+121
#define KEY83P_StatMeanX2            kE2BT2+122
#define KEY83P_StatStdX1             kE2BT2+123
#define KEY83P_StatStdX2             kE2BT2+124
#define KEY83P_StatStdXP             kE2BT2+125
#define KEY83P_StatN1                kE2BT2+126
#define KEY83P_StatN2                kE2BT2+127
#define KEY83P_StatLower             kE2BT2+128
#define KEY83P_StatUpper             kE2BT2+129
#define KEY83P_uw0                   kE2BT2+130
#define KEY83P_Imag                  kE2BT2+131
#define KEY83P_SumY                  kE2BT2+132
#define KEY83P_Xres                  kE2BT2+133
#define KEY83P_Stat_s                kE2BT2+134
#define KEY83P_SumY2                 kE2BT2+135
#define KEY83P_SumXY                 kE2BT2+136
#define KEY83P_uXres                 kE2BT2+137
#define KEY83P_ModBox		        kE2BT2+138
#define KEY83P_NormProb	            kE2BT2+139
#define KEY83P_NormalPDF             kE2BT2+140
#define KEY83P_TPDF                  kE2BT2+141
#define KEY83P_ChiPDF                kE2BT2+142
#define KEY83P_FPDF                  kE2BT2+143
#define KEY83P_MinY                  kE2BT2+144
#define KEY83P_RandBin               kE2BT2+145
#define KEY83P_Ref                   kE2BT2+146
#define KEY83P_RRef                  kE2BT2+147
#define KEY83P_LRSqr                 kE2BT2+148
#define KEY83P_BRSqr                 kE2BT2+149
#define KEY83P_DiagOn                kE2BT2+150
#define KEY83P_DiagOff               kE2BT2+151
#define KEY83P_un1                   kE2BT2+152
#define KEY83P_vn1                   kE2BT2+153

#define k83_00End       		KEY83P_vn1
#define KEY83P_Archive             k83_00End + 1
#define KEY83P_Unarchive           k83_00End + 2
#define KEY83P_Asm                 k83_00End + 3
#define KEY83P_AsmPrgm             k83_00End + 4
#define KEY83P_AsmComp             k83_00End + 5

#define KEY83P_capAAcute	       k83_00End + 6
#define KEY83P_capAGrave	       k83_00End + 7
#define KEY83P_capACaret	       k83_00End + 8
#define KEY83P_capADier	       k83_00End + 9
#define KEY83P_aAcute		       k83_00End + 10
#define KEY83P_aGrave		       k83_00End + 11
#define KEY83P_aCaret		       k83_00End + 12
#define KEY83P_aDier		       k83_00End + 13
#define KEY83P_capEAcute	       k83_00End + 14
#define KEY83P_capEGrave	       k83_00End + 15
#define KEY83P_capECaret	       k83_00End + 16
#define KEY83P_capEDier	       k83_00End + 17
#define KEY83P_eAcute		       k83_00End + 18
#define KEY83P_eGrave		       k83_00End + 19
#define KEY83P_eCaret		       k83_00End + 20
#define KEY83P_eDier		       k83_00End + 21
#define KEY83P_capIAcute	       k83_00End + 22
#define KEY83P_capIGrave	       k83_00End + 23
#define KEY83P_capICaret	       k83_00End + 24
#define KEY83P_capIDier	       k83_00End + 25
#define KEY83P_iAcute		       k83_00End + 26
#define KEY83P_iGrave		       k83_00End + 27
#define KEY83P_iCaret		       k83_00End + 28
#define KEY83P_iDier		       k83_00End + 29
#define KEY83P_capOAcute	       k83_00End + 30
#define KEY83P_capOGrave	       k83_00End + 31
#define KEY83P_capOCaret	       k83_00End + 32
#define KEY83P_capODier	       k83_00End + 33
#define KEY83P_oAcute		       k83_00End + 34
#define KEY83P_oGrave		       k83_00End + 35
#define KEY83P_oCaret		       k83_00End + 36
#define KEY83P_oDier		       k83_00End + 37
#define KEY83P_capUAcute	       k83_00End + 38
#define KEY83P_capUGrave	       k83_00End + 39
#define KEY83P_capUCaret	       k83_00End + 40
#define KEY83P_capUDier	       k83_00End + 41
#define KEY83P_uAcute		       k83_00End + 42
#define KEY83P_uGrave		       k83_00End + 43
#define KEY83P_uCaret		       k83_00End + 44
#define KEY83P_uDier		       k83_00End + 45
#define KEY83P_capCCed	       k83_00End + 46
#define KEY83P_cCed		       k83_00End + 47
#define KEY83P_capNTilde	       k83_00End + 48
#define KEY83P_nTilde		       k83_00End + 49
#define KEY83P_accent		       k83_00End + 50
#define KEY83P_grave		       k83_00End + 51
#define KEY83P_dieresis	       k83_00End + 52
#define KEY83P_quesDown	       k83_00End + 53
#define KEY83P_exclamDown            k83_00End + 54
#define KEY83P_alpha                 k83_00End + 55
#define KEY83P_beta                 k83_00End +  56
#define KEY83P_gamma                k83_00End +  57
#define KEY83P_capDelta             k83_00End +  58
#define KEY83P_delta                k83_00End +  59
#define KEY83P_epsilon              k83_00End +  60
#define KEY83P_lambda               k83_00End +  61
#define KEY83P_mu                   k83_00End +  62
#define KEY83P_pi2                  k83_00End +  63
#define KEY83P_rho                  k83_00End +  64
#define KEY83P_capSigma             k83_00End +  65
#define KEY83P_sigma                k83_00End +  66
#define KEY83P_tau                  k83_00End +  67
#define KEY83P_phi                  k83_00End +  68
#define KEY83P_capOmega             k83_00End +  69
#define KEY83P_phat                 k83_00End +  70
#define KEY83P_chi2                 k83_00End +  71
#define KEY83P_statF2               k83_00End +  72
#define KEY83P_La		     k83_00End + 73
#define KEY83P_Lb		     k83_00End + 74
#define KEY83P_Lc		     k83_00End + 75
#define KEY83P_Ld		     k83_00End + 76
#define KEY83P_Le		     k83_00End + 77
#define KEY83P_Lf		     k83_00End + 78
#define KEY83P_Lg		     k83_00End + 79
#define KEY83P_Lh		     k83_00End + 80
#define KEY83P_Li		     k83_00End + 81
#define KEY83P_Lj		     k83_00End + 82
#define KEY83P_Lk		     k83_00End + 83
#define KEY83P_Ll		     k83_00End + 84
#define KEY83P_Lm		     k83_00End + 85
#define KEY83P_Lsmalln	    k83_00End +  86
#define KEY83P_Lo		    k83_00End +  87
#define KEY83P_Lp		    k83_00End +  88
#define KEY83P_Lq		    k83_00End +  89
#define KEY83P_Lsmallr	   k83_00End +  90
#define KEY83P_Ls		    k83_00End +  91
#define KEY83P_Lt		    k83_00End +  92
#define KEY83P_Lu		    k83_00End +  93
#define KEY83P_Lv		    k83_00End +  94
#define KEY83P_Lw		    k83_00End +  95
#define KEY83P_Lx		    k83_00End +  96
#define KEY83P_Ly		    k83_00End +  97
#define KEY83P_Lz		    k83_00End +  98
#define KEY83P_GarbageC	  k83_00End +  99 ; GarbageCollect

#endif
