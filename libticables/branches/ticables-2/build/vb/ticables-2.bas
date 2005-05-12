Attribute VB_Name = "Module1"
'****************************************************************************
'*  @doc INTERNAL
'*  @module ticables-2.bas
'*
'*  TiCables-2 Interface
'*  <cp> Copyright 1999-2005 The TiLP Team<nl>
'*
'*  Win32 Prototypes for TiCables-2
'*
'*  @comm
'*  Author: Romain Liévin<nl>
'*  Date:   12/05/05 10:09:34
'*
'*  @group Revision History
'*  @comm
'*  $Revision: 1 $
'*  <nl>
'*  $Id$
'*  <nl>
'****************************************************************************

Public Declare Function TicablesLibraryInit Lib "ticables-2.dll" Alias "ticables_library_init" () As Integer
Public Declare Function TicablesLibraryExit Lib "ticables-2.dll" Alias "ticables_library_exit" () As Integer

Public Declare Function TicablesVersionGet Lib "ticables-2.dll" Alias "ticables_version_get" () As String

Public Declare Function TicablesHandleNew Lib "ticables-2.dll" Alias "ticables_handle_new" (ByVal Model As Integer, ByVal Port As Integer) As Integer
Public Declare Function TicablesHandleDel Lib "ticables-2.dll" Alias "ticables_handle_del" (ByRef Handle As Integer)

'Public Declare Function DlPortReadPortUchar Lib "dlportio.dll" (ByVal Port As Long) As Byte
'Public Declare Function DlPortReadPortUshort Lib "dlportio.dll" (ByVal Port As Long) As Integer
'Public Declare Function DlPortReadPortUlong Lib "dlportio.dll" (ByVal Port As Long) As Long
