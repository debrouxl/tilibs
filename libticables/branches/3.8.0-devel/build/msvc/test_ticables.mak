# Microsoft Developer Studio Generated NMAKE File, Based on test_ticables.dsp
!IF "$(CFG)" == ""
CFG=test_ticables - Win32 Debug
!MESSAGE No configuration specified. Defaulting to test_ticables - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "test_ticables - Win32 Release" && "$(CFG)" != "test_ticables - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_ticables.mak" CFG="test_ticables - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_ticables - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "test_ticables - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "test_ticables - Win32 Release"

OUTDIR=.
INTDIR=C:\msvc\Output\ticables
# Begin Custom Macros
OutDir=.
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\test_ticables.exe"

!ELSE 

ALL : "ticables - Win32 Release" "$(OUTDIR)\test_ticables.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ticables - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\test_ticables.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\test_ticables.exe"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /w /W0 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__WIN32__" /Fp"$(INTDIR)\test_ticables.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\test_ticables.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\test_ticables.pdb" /machine:I386 /out:"$(OUTDIR)\test_ticables.exe" 
LINK32_OBJS= \
	"$(INTDIR)\test_ticables.obj" \
	"$(OUTDIR)\ticables.lib"

"$(OUTDIR)\test_ticables.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "test_ticables - Win32 Debug"

OUTDIR=C:\msvc\tilp
INTDIR=C:\msvc\Output\ticables
# Begin Custom Macros
OutDir=C:\msvc\tilp
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\test_ticables.exe"

!ELSE 

ALL : "ticables - Win32 Debug" "$(OUTDIR)\test_ticables.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ticables - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\test_ticables.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\test_ticables.exe"
	-@erase "$(OUTDIR)\test_ticables.ilk"
	-@erase "$(OUTDIR)\test_ticables.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /w /W0 /Gm /GX /ZI /Od /I "../../ti_libs" /I "../dep/win32" /D "_DEBUG" /D "__WIN32__" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\test_ticables.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\test_ticables.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\test_ticables.pdb" /debug /machine:I386 /out:"$(OUTDIR)\test_ticables.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\test_ticables.obj" \
	"$(OUTDIR)\ticables.lib"

"$(OUTDIR)\test_ticables.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("test_ticables.dep")
!INCLUDE "test_ticables.dep"
!ELSE 
!MESSAGE Warning: cannot find "test_ticables.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "test_ticables - Win32 Release" || "$(CFG)" == "test_ticables - Win32 Debug"
SOURCE=..\..\tests\test_ticables.c

"$(INTDIR)\test_ticables.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "test_ticables - Win32 Release"

"ticables - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ticables.mak" CFG="ticables - Win32 Release" 
   cd "."

"ticables - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ticables.mak" CFG="ticables - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "test_ticables - Win32 Debug"

"ticables - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ticables.mak" CFG="ticables - Win32 Debug" 
   cd "."

"ticables - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\ticables.mak" CFG="ticables - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

