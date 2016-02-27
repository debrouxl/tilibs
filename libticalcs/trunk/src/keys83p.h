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

#define KEY83P_OneVar                0xfe41
#define KEY83P_TwoVar                0xfe42
#define KEY83P_LR                    0xfe43
#define KEY83P_LRExp                 0xfe44
#define KEY83P_LRLn                  0xfe45
#define KEY83P_LRPwr                 0xfe46
#define KEY83P_MedMed                0xfe47
#define KEY83P_Quad                  0xfe48
#define KEY83P_ClrLst                0xfe49
#define KEY83P_Hist                  0xfe4a
#define KEY83P_xyLine                0xfe4b
#define KEY83P_Scatter               0xfe4c

#define KEY83P_mRad                  0xfe4d
#define KEY83P_mDeg                  0xfe4e
#define KEY83P_mNormF                0xfe4f
#define KEY83P_mSci                  0xfe50
#define KEY83P_mEng                  0xfe51
#define KEY83P_mFloat                0xfe52

#define KEY83P_Fix                   0xfe53
#define KEY83P_SplitOn               0xfe54
#define KEY83P_FullScreen            0xfe55
#define KEY83P_Stndrd                0xfe56
#define KEY83P_Param                 0xfe57
#define KEY83P_Polar                 0xfe58
#define KEY83P_SeqG                  0xfe59
#define KEY83P_AFillOn               0xfe5a
#define KEY83P_AFillOff              0xfe5b
#define KEY83P_ACalcOn               0xfe5c
#define KEY83P_ACalcOff              0xfe5d
#define KEY83P_FNOn                  0xfe5e
#define KEY83P_FNOff                 0xfe5f

#define KEY83P_PlotsOn               0xfe60
#define KEY83P_PlotsOff              0xfe61

#define KEY83P_PixelChg              0xfe62

#define KEY83P_SendMBL               0xfe63
#define KEY83P_RecvMBL               0xfe64

#define KEY83P_BoxPlot               0xfe65
#define KEY83P_BoxIcon               0xfe66
#define KEY83P_CrossIcon             0xfe67
#define KEY83P_DotIcon               0xfe68

#define kE2BT                  (kE1BT+105)

#define KEY83P_Seqential             0xfe69
#define KEY83P_SimulG                0xfe6a
#define KEY83P_PolarG                0xfe6b
#define KEY83P_RectG                 0xfe6c
#define KEY83P_CoordOn               0xfe6d
#define KEY83P_CoordOff              0xfe6e
#define KEY83P_DrawLine              0xfe6f
#define KEY83P_DrawDot               0xfe70
#define KEY83P_AxisOn                0xfe71
#define KEY83P_AxisOff               0xfe72
#define KEY83P_GridOn                0xfe73
#define KEY83P_GridOff               0xfe74
#define KEY83P_LblOn                 0xfe75
#define KEY83P_LblOff                0xfe76

#define KEY83P_L1                    0xfe77
#define KEY83P_L2                    0xfe78
#define KEY83P_L3                    0xfe79
#define KEY83P_L4                    0xfe7a
#define KEY83P_L5                    0xfe7b
#define KEY83P_L6                    0xfe7c

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
#define KEY83P_FinN                  0xfc5a
#define KEY83P_FinI                  0xfc5b
#define KEY83P_FinPV                 0xfc5c
#define KEY83P_FinPMT                0xfc5d
#define KEY83P_FinFV                 0xfc5e
#define KEY83P_FinPY                 0xfc5f
#define KEY83P_FinCY                 0xfc60
#define KEY83P_FinFPMT               0xfc61
#define KEY83P_FinFI                 0xfc62
#define KEY83P_FinFPV                0xfc63
#define KEY83P_FinFN                 0xfc64
#define KEY83P_FinFFV                0xfc65
#define KEY83P_FinNPV                0xfc66
#define KEY83P_FinIRR                0xfc67
#define KEY83P_FinBAL                0xfc68
#define KEY83P_FinPRN                0xfc69
#define KEY83P_FinINT                0xfc6a
#define KEY83P_SumX                  0xfc6b
#define KEY83P_SumX2                 0xfc6c
#define KEY83P_FinToNom              0xfc6d
#define KEY83P_FinToEff              0xfc6e
#define KEY83P_FinDBD                0xfc6f
#define KEY83P_StatVP                0xfc70
#define KEY83P_StatZ                 0xfc71
#define KEY83P_StatT                 0xfc72
#define KEY83P_StatChi               0xfc73
#define KEY83P_StatF                 0xfc74
#define KEY83P_StatDF                0xfc75
#define KEY83P_StatPhat              0xfc76
#define KEY83P_StatPhat1             0xfc77
#define KEY83P_StatPhat2             0xfc78
#define KEY83P_StatMeanX1            0xfc79
#define KEY83P_StatMeanX2            0xfc7a
#define KEY83P_StatStdX1             0xfc7b
#define KEY83P_StatStdX2             0xfc7c
#define KEY83P_StatStdXP             0xfc7d
#define KEY83P_StatN1                0xfc7e
#define KEY83P_StatN2                0xfc7f
#define KEY83P_StatLower             0xfc80
#define KEY83P_StatUpper             0xfc81
#define KEY83P_uw0                   0xfc82
#define KEY83P_Imag                  0xfc83
#define KEY83P_SumY                  0xfc84
#define KEY83P_Xres                  0xfc85
#define KEY83P_Stat_s                0xfc86
#define KEY83P_SumY2                 0xfc87
#define KEY83P_SumXY                 0xfc88
#define KEY83P_uXres                 0xfc89
#define KEY83P_ModBox		        0xfc8a
#define KEY83P_NormProb	            0xfc8b
#define KEY83P_NormalPDF             0xfc8c
#define KEY83P_TPDF                  0xfc8d
#define KEY83P_ChiPDF                0xfc8e
#define KEY83P_FPDF                  0xfc8f
#define KEY83P_MinY                  0xfc90
#define KEY83P_RandBin               0xfc91
#define KEY83P_Ref                   0xfc92
#define KEY83P_RRef                  0xfc93
#define KEY83P_LRSqr                 0xfc94
#define KEY83P_BRSqr                 0xfc95
#define KEY83P_DiagOn                0xfc96
#define KEY83P_DiagOff               0xfc97
#define KEY83P_un1                   0xfc98
#define KEY83P_vn1                   0xfc99

#define k83_00End       		KEY83P_vn1
#define KEY83P_Archive             0xfc9a
#define KEY83P_Unarchive           0xfc9b
#define KEY83P_Asm                 0xfc9c
#define KEY83P_AsmPrgm             0xfc9d
#define KEY83P_AsmComp             0xfc9e

#define KEY83P_capAAcute	       0xfc9f
#define KEY83P_capAGrave	       0xfca0
#define KEY83P_capACaret	       0xfca1
#define KEY83P_capADier	       0xfca2
#define KEY83P_aAcute		       0xfca3
#define KEY83P_aGrave		       0xfca4
#define KEY83P_aCaret		       0xfca5
#define KEY83P_aDier		       0xfca6
#define KEY83P_capEAcute	       0xfca7
#define KEY83P_capEGrave	       0xfca8
#define KEY83P_capECaret	       0xfca9
#define KEY83P_capEDier	       0xfcaa
#define KEY83P_eAcute		       0xfcab
#define KEY83P_eGrave		       0xfcac
#define KEY83P_eCaret		       0xfcad
#define KEY83P_eDier		       0xfcae
#define KEY83P_capIAcute	       0xfcaf
#define KEY83P_capIGrave	       0xfcb0
#define KEY83P_capICaret	       0xfcb1
#define KEY83P_capIDier	       0xfcb2
#define KEY83P_iAcute		       0xfcb3
#define KEY83P_iGrave		       0xfcb4
#define KEY83P_iCaret		       0xfcb5
#define KEY83P_iDier		       0xfcb6
#define KEY83P_capOAcute	       0xfcb7
#define KEY83P_capOGrave	       0xfcb8
#define KEY83P_capOCaret	       0xfcb9
#define KEY83P_capODier	       0xfcba
#define KEY83P_oAcute		       0xfcbb
#define KEY83P_oGrave		       0xfcbc
#define KEY83P_oCaret		       0xfcbd
#define KEY83P_oDier		       0xfcbe
#define KEY83P_capUAcute	       0xfcbf
#define KEY83P_capUGrave	       0xfcc0
#define KEY83P_capUCaret	       0xfcc1
#define KEY83P_capUDier	       0xfcc2
#define KEY83P_uAcute		       0xfcc3
#define KEY83P_uGrave		       0xfcc4
#define KEY83P_uCaret		       0xfcc5
#define KEY83P_uDier		       0xfcc6
#define KEY83P_capCCed	       0xfcc7
#define KEY83P_cCed		       0xfcc8
#define KEY83P_capNTilde	       0xfcc9
#define KEY83P_nTilde		       0xfcca
#define KEY83P_accent		       0xfccb
#define KEY83P_grave		       0xfccc
#define KEY83P_dieresis	       0xfccd
#define KEY83P_quesDown	       0xfcce
#define KEY83P_exclamDown            0xfccf
#define KEY83P_alpha                 0xfcd0
#define KEY83P_beta                 0xfcd1
#define KEY83P_gamma                0xfcd2
#define KEY83P_capDelta             0xfcd3
#define KEY83P_delta                0xfcd4
#define KEY83P_epsilon              0xfcd5
#define KEY83P_lambda               0xfcd6
#define KEY83P_mu                   0xfcd7
#define KEY83P_pi2                  0xfcd8
#define KEY83P_rho                  0xfcd9
#define KEY83P_capSigma             0xfcda
#define KEY83P_sigma                0xfcdb
#define KEY83P_tau                  0xfcdc
#define KEY83P_phi                  0xfcdd
#define KEY83P_capOmega             0xfcde
#define KEY83P_phat                 0xfcdf
#define KEY83P_chi2                 0xfce0
#define KEY83P_statF2               0xfce1
#define KEY83P_La		     0xfce2
#define KEY83P_Lb		     0xfce3
#define KEY83P_Lc		     0xfce4
#define KEY83P_Ld		     0xfce5
#define KEY83P_Le		     0xfce6
#define KEY83P_Lf		     0xfce7
#define KEY83P_Lg		     0xfce8
#define KEY83P_Lh		     0xfce9
#define KEY83P_Li		     0xfcea
#define KEY83P_Lj		     0xfceb
#define KEY83P_Lk		     0xfcec
#define KEY83P_Ll		     0xfced
#define KEY83P_Lm		     0xfcee
#define KEY83P_Lsmalln	    0xfcef
#define KEY83P_Lo		    0xfcf0
#define KEY83P_Lp		    0xfcf1
#define KEY83P_Lq		    0xfcf2
#define KEY83P_Lsmallr	   0xfcf3
#define KEY83P_Ls		    0xfcf4
#define KEY83P_Lt		    0xfcf5
#define KEY83P_Lu		    0xfcf6
#define KEY83P_Lv		    0xfcf7
#define KEY83P_Lw		    0xfcf8
#define KEY83P_Lx		    0xfcf9
#define KEY83P_Ly		    0xfcfa
#define KEY83P_Lz		    0xfcfb
#define KEY83P_GarbageC	  0xfcfc

// Key codes added in OS 1.15

#define KEY83P_Backspace     0x21

#define KEY83P_Reserved      0xFB01
#define KEY83P_AtSign        0xFB02
#define KEY83P_Pound         0xFB03
#define KEY83P_Dollar        0xFB04
#define KEY83P_Ampersand     0xFB05
#define KEY83P_BackQuote     0xFB06
#define KEY83P_Semicolon     0xFB07
#define KEY83P_BackSlash     0xFB08
#define KEY83P_VertSlash     0xFB09
#define KEY83P_Underscore    0xFB0A
#define KEY83P_Tilde         0xFB0B
#define KEY83P_Percent       0xFB0C
#define KEY83P_Tab           0xFB0D
#define KEY83P_ShftTaB       0xFB0E
#define KEY83P_ShftDel       0xFB0F
#define KEY83P_ShftBack      0xFB10
#define KEY83P_ShftPgUp      0xFB11
#define KEY83P_ShftPgDn      0xFB12
#define KEY83P_ShftLeft      0xFB13
#define KEY83P_ShftRight     0xFB14
#define KEY83P_ShftUp        0xFB15
#define KEY83P_ShftDn        0xFB16
#define KEY83P_DiaAdd        0xFB17
#define KEY83P_DiaSub        0xFB18
#define KEY83P_DiaTilde      0xFB19
#define KEY83P_DiaDiv        0xFB1A
#define KEY83P_DiaBkSlash    0xFB1B
#define KEY83P_DiaColon      0xFB1C
#define KEY83P_DiaQuote      0xFB1D
#define KEY83P_DiaLBrack     0xFB1E
#define KEY83P_DiaRBrack     0xFB1F
#define KEY83P_DiaBkSpace    0xFB20
#define KEY83P_DiaEnter      0xFB21
#define KEY83P_DiaComma      0xFB22
#define KEY83P_DiaDel        0xFB23
#define KEY83P_DiaDecPnt     0xFB24
#define KEY83P_Dia0          0xFB25
#define KEY83P_Dia1          0xFB26
#define KEY83P_Dia2          0xFB27
#define KEY83P_Dia3          0xFB28
#define KEY83P_Dia4          0xFB29
#define KEY83P_Dia5          0xFB2A
#define KEY83P_Dia6          0xFB2B
#define KEY83P_Dia7          0xFB2C
#define KEY83P_Dia8          0xFB2D
#define KEY83P_Dia9          0xFB2E
#define KEY83P_DiaTab        0xFB2F
#define KEY83P_DiaSpace      0xFB30
#define KEY83P_DiaA          0xFB31
#define KEY83P_DiaB          0xFB32
#define KEY83P_DiaC          0xFB33
#define KEY83P_DiaD          0xFB34
#define KEY83P_DiaE          0xFB35
#define KEY83P_DiaF          0xFB36
#define KEY83P_DiaG          0xFB37
#define KEY83P_DiaH          0xFB38
#define KEY83P_DiaI          0xFB39
#define KEY83P_DiaJ          0xFB3A
#define KEY83P_DiaK          0xFB3B
#define KEY83P_DiaL          0xFB3C
#define KEY83P_DiaM          0xFB3D
#define KEY83P_DiaN          0xFB3E
#define KEY83P_DiaO          0xFB3F
#define KEY83P_DiaP          0xFB40
#define KEY83P_DiaQ          0xFB41
#define KEY83P_DiaR          0xFB42
#define KEY83P_DiaS          0xFB43
#define KEY83P_DiaT          0xFB44
#define KEY83P_DiaU          0xFB45
#define KEY83P_DiaV          0xFB46
#define KEY83P_DiaW          0xFB47
#define KEY83P_DiaX          0xFB48
#define KEY83P_DiaY          0xFB49
#define KEY83P_DiaZ          0xFB4A
#define KEY83P_DiaPgUp       0xFB4B
#define KEY83P_DiaPgDn       0xFB4C
#define KEY83P_DiaLeft       0xFB4D
#define KEY83P_DiaRight      0xFB4E
#define KEY83P_DiaUp         0xFB4F
#define KEY83P_DiaDn         0xFB50
#define KEY83P_SqrAdd        0xFB51
#define KEY83P_SqrSub        0xFB52
#define KEY83P_SqrTilde      0xFB53
#define KEY83P_SqrDiv        0xFB54
#define KEY83P_SqrBkSlash    0xFB55
#define KEY83P_SqrColon      0xFB56
#define KEY83P_SqrQuote      0xFB57
#define KEY83P_SqrLBrack     0xFB58
#define KEY83P_SqrRBrack     0xFB59
#define KEY83P_SqrBkSpace    0xFB5A
#define KEY83P_SqrEnter      0xFB5B
#define KEY83P_SqrComma      0xFB5C
#define KEY83P_SqrDel        0xFB5D
#define KEY83P_SqrDecPnt     0xFB5E
#define KEY83P_Sqr0          0xFB5F
#define KEY83P_Sqr1          0xFB60
#define KEY83P_Sqr2          0xFB61
#define KEY83P_Sqr3          0xFB62
#define KEY83P_Sqr4          0xFB63
#define KEY83P_Sqr5          0xFB64
#define KEY83P_Sqr6          0xFB65
#define KEY83P_Sqr7          0xFB66
#define KEY83P_Sqr8          0xFB67
#define KEY83P_Sqr9          0xFB68
#define KEY83P_SqrTab        0xFB69
#define KEY83P_SqrSpace      0xFB6A
#define KEY83P_SqrA          0xFB6B
#define KEY83P_SqrB          0xFB6C
#define KEY83P_SqrC          0xFB6D
#define KEY83P_SqrD          0xFB6E
#define KEY83P_SqrE          0xFB6F
#define KEY83P_SqrF          0xFB70
#define KEY83P_SqrG          0xFB71
#define KEY83P_SqrH          0xFB72
#define KEY83P_SqrI          0xFB73
#define KEY83P_SqrJ          0xFB74
#define KEY83P_SqrK          0xFB75
#define KEY83P_SqrL          0xFB76
#define KEY83P_SqrM          0xFB77
#define KEY83P_SqrN          0xFB78
#define KEY83P_SqrO          0xFB79
#define KEY83P_SqrP          0xFB7A
#define KEY83P_SqrQ          0xFB7B
#define KEY83P_SqrR          0xFB7C
#define KEY83P_SqrS          0xFB7D
#define KEY83P_SquareT       0xFB7E
#define KEY83P_SqrU          0xFB7F
#define KEY83P_SqrV          0xFB80
#define KEY83P_SqrW          0xFB81
#define KEY83P_SqrX          0xFB82
#define KEY83P_SqrY          0xFB83
#define KEY83P_SqrZ          0xFB84
#define KEY83P_SqrPgUp       0xFB85
#define KEY83P_SqrPgDn       0xFB86
#define KEY83P_SqrLeft       0xFB87
#define KEY83P_SqrRight      0xFB88
#define KEY83P_SqrUp         0xFB89
#define KEY83P_SqrDn         0xFB8A
#define KEY83P_UnDef         0xFB8B

// Key codes added in OS 1.16

#define KEY83P_Lellipsis               0xFB8C
#define KEY83P_Langle                  0xFB8D
#define KEY83P_Lss                     0xFB8E
#define KEY83P_LsupX                   0xFB8F
#define KEY83P_LsubT                   0xFB90
#define KEY83P_Lsub0                   0xFB91
#define KEY83P_Lsub1                   0xFB92
#define KEY83P_Lsub2                   0xFB93
#define KEY83P_Lsub3                   0xFB94
#define KEY83P_Lsub4                   0xFB95
#define KEY83P_Lsub5                   0xFB96
#define KEY83P_Lsub6                   0xFB97
#define KEY83P_Lsub7                   0xFB98
#define KEY83P_Lsub8                   0xFB99
#define KEY83P_Lsub9                   0xFB9A
#define KEY83P_Lten                    0xFB9B
#define KEY83P_Lleft                   0xFB9C
#define KEY83P_Lconvert                0xFB9D
#define KEY83P_LupArrow                0xFB9E
#define KEY83P_LdownArrow              0xFB9F
#define KEY83P_Lcross                  0xFBA0
#define KEY83P_Lintegral               0xFBA1
#define KEY83P_LsqUp                   0xFBA2
#define KEY83P_LsqDown                 0xFBA3
#define KEY83P_Lroot                   0xFBA4
#define KEY83P_LinvEQ                  0xFBA5

// Key codes added in OS 2.21 (TI-84+)

#define KEY84P_SetDate                 0xFBA6
#define KEY84P_SetTime                 0xFBA7
#define KEY84P_CheckTmr                0xFBA8
#define KEY84P_SetDtFmt                0xFBA9
#define KEY84P_SetTmFmt                0xFBAA
#define KEY84P_TimeCnv                 0xFBAB
#define KEY84P_DayOfWk                 0xFBAC
#define KEY84P_GetDtStr                0xFBAD
#define KEY84P_GetTmStr                0xFBAE
#define KEY84P_GetDate                 0xFBAF
#define KEY84P_GetTime                 0xFBB0
#define KEY84P_StartTmr                0xFBB1
#define KEY84P_GetDtFmt                0xFBB2
#define KEY84P_GetTmFmt                0xFBB3
#define KEY84P_IsClockOn               0xFBB4
#define KEY84P_ClockOff                0xFBB5
#define KEY84P_ClockOn                 0xFBB6
#define KEY84P_OpenLib                 0xFBB7
#define KEY84P_ExecLib                 0xFBB8

// Key codes added in OS 2.30

#define KEY84P_InvT                    0xFBB9
#define KEY84P_Chi2GOFTest             0xFBBA
#define KEY84P_LinRegTInt              0xFBBB
// BC: unknown
#define KEY84P_ManualFit               0xFBBD

// Key codes for OS 2.53MP / 2.54MP only
// (note that these conflict with earlier and later OSes)

#define KEY84P_253_LogBASE             0xFBBC
#define KEY84P_253_SumSeq              0xFBBD
// BE: unknown
#define KEY84P_253_FracSlash           0xFBBF
#define KEY84P_253_Unit                0xFBC0
#define KEY84P_253_MixSimp             0xFBC1
#define KEY84P_253_FracDec             0xFBC2
#define KEY84P_253_Remainder           0xFBC3
#define KEY84P_253_RandIntNoRep        0xFBC4
// C5: unknown
#define KEY84P_253_Placeholder         0xFBC6
#define KEY84P_253_MATHPRINT           0xFBC7
#define KEY84P_253_CLASSIC             0xFBC8
#define KEY84P_253_SimpleMode          0xFBC9
#define KEY84P_253_MixedMode           0xFBCA
#define KEY84P_253_AUTO                0xFBCB
#define KEY84P_253_DEC                 0xFBCC
#define KEY84P_253_FRAC                0xFBCD
#define KEY84P_253_ZQuadrant1          0xFBCE
#define KEY84P_253_ZFracOneHalf        0xFBCF
#define KEY84P_253_ZFracOneThird       0xFBD0
#define KEY84P_253_ZFracOneQuarter     0xFBD1
#define KEY84P_253_ZFracOneFifth       0xFBD2
#define KEY84P_253_ZFracOneEighth      0xFBD3
#define KEY84P_253_ZFracOneTenth       0xFBD4
// D5: unknown
#define KEY84P_253_ManualFit           0xFBD6

// Key codes added in OS 2.55MP

// BE: unknown
// BF: unknown
#define KEY84P_ZQuadrant1              0xFBC0
#define KEY84P_ZFracOneHalf            0xFBC1
#define KEY84P_ZFracOneThird           0xFBC2
#define KEY84P_ZFracOneQuarter         0xFBC3
#define KEY84P_ZFracOneFifth           0xFBC4
#define KEY84P_ZFracOneEighth          0xFBC5
#define KEY84P_ZFracOneTenth           0xFBC6
#define KEY84P_LogBASE                 0xFBC7
#define KEY84P_SumSeq                  0xFBC8
// C9: unknown
#define KEY84P_FracSlash               0xFBCA
#define KEY84P_Unit                    0xFBCB
#define KEY84P_MixSimp                 0xFBCC
#define KEY84P_FracDec                 0xFBCD
#define KEY84P_Remainder               0xFBCE
#define KEY84P_RandIntNoRep            0xFBCF
// D0: unknown
#define KEY84P_Placeholder             0xFBD1
#define KEY84P_MATHPRINT               0xFBD2
#define KEY84P_CLASSIC                 0xFBD3
#define KEY84P_SimpleMode              0xFBD4
#define KEY84P_MixedMode               0xFBD5
#define KEY84P_AUTO                    0xFBD6
#define KEY84P_DEC                     0xFBD7
#define KEY84P_FRAC                    0xFBD8
#define KEY84P_STATWIZARD_ON           0xFBD9
#define KEY84P_STATWIZARD_OFF          0xFBDA

// Key codes added in OS 4.0 (TI-84+CSE)

#define KEY84PC_BLUE                   0xFBDB
#define KEY84PC_RED                    0xFBDC
#define KEY84PC_BLACK                  0xFBDD
#define KEY84PC_MAGENTA                0xFBDE
#define KEY84PC_GREEN                  0xFBDF
#define KEY84PC_ORANGE                 0xFBE0
#define KEY84PC_BROWN                  0xFBE1
#define KEY84PC_NAVY                   0xFBE2
#define KEY84PC_LTBLUE                 0xFBE3
#define KEY84PC_YELLOW                 0xFBE4
#define KEY84PC_WHITE                  0xFBE5
#define KEY84PC_LTGRAY                 0xFBE6
#define KEY84PC_MEDGRAY                0xFBE7
#define KEY84PC_GRAY                   0xFBE8
#define KEY84PC_DARKGRAY               0xFBE9
#define KEY84PC_Image1                 0xFBEA
#define KEY84PC_Image2                 0xFBEB
#define KEY84PC_Image3                 0xFBEC
#define KEY84PC_Image4                 0xFBED
#define KEY84PC_Image5                 0xFBEE
#define KEY84PC_Image6                 0xFBEF
#define KEY84PC_Image7                 0xFBF0
#define KEY84PC_Image8                 0xFBF1
#define KEY84PC_Image9                 0xFBF2
#define KEY84PC_Image0                 0xFBF3
#define KEY84PC_GridLine               0xFBF4
#define KEY84PC_BackgroundOn           0xFBF5
#define KEY84PC_BackgroundOff          0xFBF6
#define KEY84PC_GraphColor             0xFBF7
#define KEY84PC_QuickPlotAndFitEq      0xFBF8

#define KEY84PC_TextColor              0xFA01
#define KEY84PC_Asm84PCPrgm            0xFA02
#define KEY84PC_DetectAsymOn           0xFA03
#define KEY84PC_DetectAsymOff          0xFA04
#define KEY84PC_BorderColor            0xFA05
#define KEY84PC_SmallDotIcon           0xFA06
#define KEY84PC_Thin                   0xFA07
#define KEY84PC_DotThin                0xFA08

// Key codes added in OS 5.0 (TI-84+CE)

// 09-14: unknown
#define KEY84PCE_PlySmlt2              0xFA15
#define KEY84PCE_Asm84CEPrgm           0xFA16

#endif
