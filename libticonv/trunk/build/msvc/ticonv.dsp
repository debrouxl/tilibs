# Microsoft Developer Studio Project File - Name="ticonv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ticonv - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ticonv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ticonv.mak" CFG="ticonv - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ticonv - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ticonv - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ticonv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "C:\msvc\Output\ticonv"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /w /W0 /GX /I "C:\Gtk2Dev\include" /I "c:\gtk2dev\include\glib-2.0" /I "C:\GTK2DEV\lib\glib-2.0\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "TICONV_EXPORTS" /D "__WIN32__" /D "HAVE_LIBZ" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL"
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\..\tests\libticonv-2.dll"

!ELSEIF  "$(CFG)" == "ticonv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\msvc\tilp2\"
# PROP Intermediate_Dir "C:\msvc\Output\ticonv"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /ZI /Od /I "C:\Gtk2Dev\include" /I "c:\gtk2dev\include\glib-2.0" /I "C:\GTK2DEV\lib\glib-2.0\include" /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /D "TICONV_EXPORTS" /D "__WIN32__" /D "HAVE_LIBZ" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"C:\msvc\tilp2\libticonv-2.dll" /pdbtype:sept
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "ticonv - Win32 Release"
# Name "ticonv - Win32 Debug"
# Begin Group "API"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\charset.c
# End Source File
# Begin Source File

SOURCE=..\..\src\filename.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ticonv.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ticonv.h
# End Source File
# Begin Source File

SOURCE=..\..\src\tokens.c
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\..\..\..\GTK2DEV\lib\glib-2.0.lib"
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\ChangeLog
# End Source File
# Begin Source File

SOURCE=.\ticonv.rc
# End Source File
# End Target
# End Project
