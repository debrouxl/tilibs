# Microsoft Developer Studio Project File - Name="ticables2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ticables2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ticables2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ticables2.mak" CFG="ticables2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ticables2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ticables2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ticables2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "C:\msvc\Output\ticables2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /w /W0 /GX /I "C:\Gtk2Dev\include" /I "c:\gtk2dev\include\glib-2.0" /I "C:\GTK2DEV\lib\glib-2.0\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "TICABLES_EXPORTS" /D "__WIN32__" /D "__I386__" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\tests\ticables2.dll"

!ELSEIF  "$(CFG)" == "ticables2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\msvc\tilp2"
# PROP Intermediate_Dir "C:\msvc\Output\ticables2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /ML /W3 /GX /ZI /Od /I "C:\Gtk2Dev\include" /I "c:\gtk2dev\include\glib-2.0" /I "C:\GTK2DEV\lib\glib-2.0\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "TICABLES_EXPORTS" /D "__WIN32__" /D "__I386__" /D "ENABLE_LOGGING" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /debugtype:both /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "ticables2 - Win32 Release"
# Name "ticables2 - Win32 Debug"
# Begin Group "Link cables"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\link_blk.c
# End Source File
# Begin Source File

SOURCE=..\..\src\link_gry.c
# End Source File
# Begin Source File

SOURCE=..\..\src\link_nul.c
# End Source File
# Begin Source File

SOURCE=..\..\src\link_par.c
# End Source File
# Begin Source File

SOURCE=..\..\src\link_slv.c
# End Source File
# Begin Source File

SOURCE=..\..\src\link_tie.c
# End Source File
# Begin Source File

SOURCE=..\..\src\link_vti.c
# End Source File
# Begin Source File

SOURCE=..\..\src\link_xxx.c
# End Source File
# End Group
# Begin Group "Others"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\win32\detect.c
# End Source File
# Begin Source File

SOURCE=..\..\src\win32\ioports.c
# End Source File
# Begin Source File

SOURCE=..\..\src\logging.c
# End Source File
# Begin Source File

SOURCE=..\..\src\win32\porttalk.c
# End Source File
# Begin Source File

SOURCE=..\..\src\probe.c
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\data_log.h
# End Source File
# Begin Source File

SOURCE=..\..\src\error.h
# End Source File
# Begin Source File

SOURCE=..\..\src\export.h
# End Source File
# Begin Source File

SOURCE=..\..\src\win32\tiglusb.h
# End Source File
# Begin Source File

SOURCE=..\..\src\timeout.h
# End Source File
# End Group
# Begin Group "API"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\error.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ticables.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ticables.h
# End Source File
# Begin Source File

SOURCE=..\..\src\type2str.c
# End Source File
# End Group
# Begin Group "Data Logger"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\data_log.c
# End Source File
# Begin Source File

SOURCE=..\..\src\log_hex.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\ChangeLog
# End Source File
# Begin Source File

SOURCE=.\ticables2.rc
# End Source File
# Begin Source File

SOURCE="..\..\..\..\..\GTK2DEV\lib\glib-2.0.lib"
# End Source File
# End Target
# End Project
