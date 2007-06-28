# Microsoft Developer Studio Project File - Name="tifiles2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tifiles2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tifiles2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tifiles2.mak" CFG="tifiles2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tifiles2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tifiles2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tifiles2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "C:\msvc\Output\tifiles2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /w /W0 /GX /I "." /I "../../../ticonv/src" /I "C:\Gtk2Dev\include" /I "c:\gtk2dev\include\glib-2.0" /I "C:\GTK2DEV\lib\glib-2.0\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "TIFILES_EXPORTS" /D "__WIN32__" /D "HAVE_LIBZ" /D "ENABLE_NLS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\tests\libtifiles2-4.dll"

!ELSEIF  "$(CFG)" == "tifiles2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\msvc\tilp2\"
# PROP Intermediate_Dir "C:\msvc\Output\tifiles2"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /ZI /Od /I "." /I "../../../ticonv/src" /I "C:\Gtk2Dev\include" /I "c:\gtk2dev\include\glib-2.0" /I "C:\GTK2DEV\lib\glib-2.0\include" /D "_DEBUG" /D "_CONSOLE" /D "ENABLE_NLS" /D "WIN32" /D "TIFILES_EXPORTS" /D "__WIN32__" /D "HAVE_LIBZ" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"C:\msvc\tilp2\libtifiles2-4.dll" /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "tifiles2 - Win32 Release"
# Name "tifiles2 - Win32 Debug"
# Begin Group "TI types"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\types73.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types82.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types83.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types83p.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types84p.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types85.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types86.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types89.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types89t.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types92.c
# End Source File
# Begin Source File

SOURCE=..\..\src\types92p.c
# End Source File
# Begin Source File

SOURCE=..\..\src\typesv2.c
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\files8x.h
# End Source File
# Begin Source File

SOURCE=..\..\src\files9x.h
# End Source File
# End Group
# Begin Group "TI files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\comments.c
# End Source File
# Begin Source File

SOURCE=..\..\src\files8X.c
# End Source File
# Begin Source File

SOURCE=..\..\src\files9X.c
# End Source File
# Begin Source File

SOURCE=..\..\src\filesxx.c
# End Source File
# Begin Source File

SOURCE=..\..\src\grouped.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ve_fp.c
# End Source File
# End Group
# Begin Group "Others"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\intelhex.c
# End Source File
# Begin Source File

SOURCE=..\..\src\logging.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rwfile.c
# End Source File
# End Group
# Begin Group "API"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\error.c
# End Source File
# Begin Source File

SOURCE=..\..\src\filetypes.c
# End Source File
# Begin Source File

SOURCE=..\..\src\misc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tifiles.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tifiles.h
# End Source File
# Begin Source File

SOURCE=..\..\src\type2str.c
# End Source File
# Begin Source File

SOURCE=..\..\src\typesXX.c
# End Source File
# End Group
# Begin Group "TI groups"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\minizip\ioapi.c
# End Source File
# Begin Source File

SOURCE=..\..\src\minizip\iowin32.c
# End Source File
# Begin Source File

SOURCE=..\..\src\minizip\miniunz.c
# End Source File
# Begin Source File

SOURCE=..\..\src\minizip\minizip.c
# End Source File
# Begin Source File

SOURCE=..\..\src\tigroup.c
# End Source File
# Begin Source File

SOURCE=..\..\src\minizip\unzip.c
# End Source File
# Begin Source File

SOURCE=..\..\src\minizip\zip.c
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\..\GTK2DEV\lib\glib-2.0.lib"
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\GTK2DEV\lib\z.lib
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\ChangeLog
# End Source File
# Begin Source File

SOURCE=..\mingw\tifiles2.rc
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\GTK2DEV\lib\intl.lib
# End Source File
# End Target
# End Project
