# Microsoft Developer Studio Project File - Name="ticalcs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ticalcs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ticalcs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ticalcs.mak" CFG="ticalcs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ticalcs - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ticalcs - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ticalcs - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "C:\msvc\Output\ticalcs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /w /W0 /GX /I "../../files/src" /I "../../cables/src/" /I "../../../files/src" /I "../../../cables/src/" /D "NDEBUG" /D "TICALCS_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /D "ENABLE_NLS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\tests\ticalcs.dll"

!ELSEIF  "$(CFG)" == "ticalcs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\msvc\tilp\"
# PROP Intermediate_Dir "C:\msvc\Output\ticalcs"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /w /W0 /GX /ZI /Od /I "../../files/src" /I "../../cables/src/" /I "../../../libticables/src" /I "../../../libtifiles/src" /D "_DEBUG" /D "TICALCS_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "__WIN32__" /D "ENABLE_NLS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "ticalcs - Win32 Release"
# Name "ticalcs - Win32 Debug"
# Begin Group "TI calcs"

# PROP Default_Filter ""
# Begin Group "cmds"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\cmd73.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd82.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd85.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd89.c
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd92.c
# End Source File
# End Group
# Begin Group "calcs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\ti73.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti82.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti83.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti85.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti86.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti89.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti92.c
# End Source File
# End Group
# Begin Group "keys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\keys73.c
# End Source File
# Begin Source File

SOURCE=..\..\src\keys83p.c
# End Source File
# Begin Source File

SOURCE=..\..\src\keys89.c
# End Source File
# Begin Source File

SOURCE=..\..\src\keys92p.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tikeys.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\packets.c
# End Source File
# End Group
# Begin Group "Others"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\clock.c
# End Source File
# Begin Source File

SOURCE=..\..\src\dirlist.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Error.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Intrface.c
# End Source File
# Begin Source File

SOURCE=..\..\src\Probe.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tnode.c
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\Calc_def.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Calc_err.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Calc_int.h
# End Source File
# Begin Source File

SOURCE=..\..\src\clock.h
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd73.h
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd82.h
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd85.h
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd89.h
# End Source File
# Begin Source File

SOURCE=..\..\src\cmd92.h
# End Source File
# Begin Source File

SOURCE=..\..\src\dirlist.h
# End Source File
# Begin Source File

SOURCE=..\..\src\export.h
# End Source File
# Begin Source File

SOURCE=..\..\src\externs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\headers.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Intl2.h
# End Source File
# Begin Source File

SOURCE=..\..\src\keys73.h
# End Source File
# Begin Source File

SOURCE=..\..\src\keys83p.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Keys89.h
# End Source File
# Begin Source File

SOURCE=..\..\src\keys92p.h
# End Source File
# Begin Source File

SOURCE=..\..\src\packets.h
# End Source File
# Begin Source File

SOURCE=..\..\src\pause.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ti73.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti82.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti83.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti83p.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti85.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti86.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti89.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Ti92.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ticalcs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\tikeys.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Tixx.h
# End Source File
# Begin Source File

SOURCE=..\..\src\tnode.h
# End Source File
# Begin Source File

SOURCE=..\..\src\update.h
# End Source File
# End Group
# Begin Group "ROMdumping"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\rom82.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rom83.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rom83p.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rom85u.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rom85z.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rom86.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Rom89.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Rom92f2.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\Calc_ver.h
# End Source File
# Begin Source File

SOURCE=..\..\ChangeLog
# End Source File
# Begin Source File

SOURCE=.\ticalcs.rc
# End Source File
# End Target
# End Project
