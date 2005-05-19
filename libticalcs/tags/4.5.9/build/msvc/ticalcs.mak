# Microsoft Developer Studio Generated NMAKE File, Based on ticalcs.dsp
!IF "$(CFG)" == ""
CFG=ticalcs - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ticalcs - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ticalcs - Win32 Release" && "$(CFG)" != "ticalcs - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ticalcs - Win32 Release"

OUTDIR=.
INTDIR=C:\msvc\Output\ticalcs

!IF "$(RECURSE)" == "0" 

ALL : "..\..\tests\ticalcs.dll"

!ELSE 

ALL : "tifiles - Win32 Release" "ticables - Win32 Release" "..\..\tests\ticalcs.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ticables - Win32 ReleaseCLEAN" "tifiles - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\clock.obj"
	-@erase "$(INTDIR)\cmd73.obj"
	-@erase "$(INTDIR)\cmd82.obj"
	-@erase "$(INTDIR)\cmd85.obj"
	-@erase "$(INTDIR)\cmd89.obj"
	-@erase "$(INTDIR)\cmd92.obj"
	-@erase "$(INTDIR)\dirlist.obj"
	-@erase "$(INTDIR)\Error.obj"
	-@erase "$(INTDIR)\Intrface.obj"
	-@erase "$(INTDIR)\keys73.obj"
	-@erase "$(INTDIR)\keys83p.obj"
	-@erase "$(INTDIR)\keys89.obj"
	-@erase "$(INTDIR)\keys92p.obj"
	-@erase "$(INTDIR)\packets.obj"
	-@erase "$(INTDIR)\printl.obj"
	-@erase "$(INTDIR)\Probe.obj"
	-@erase "$(INTDIR)\ti73.obj"
	-@erase "$(INTDIR)\Ti82.obj"
	-@erase "$(INTDIR)\Ti83.obj"
	-@erase "$(INTDIR)\Ti85.obj"
	-@erase "$(INTDIR)\Ti86.obj"
	-@erase "$(INTDIR)\Ti89.obj"
	-@erase "$(INTDIR)\Ti92.obj"
	-@erase "$(INTDIR)\ticalcs.res"
	-@erase "$(INTDIR)\tikeys.obj"
	-@erase "$(INTDIR)\tnode.obj"
	-@erase "$(INTDIR)\type2str.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ticalcs.exp"
	-@erase "$(OUTDIR)\ticalcs.lib"
	-@erase "..\..\tests\ticalcs.dll"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /w /W0 /GX /I "../../../cables/src" /I "../../../files/src" /I "../../../ticables/src" /I "../../../tifiles/src" /I "C:\Gtk2Dev\Include" /I "." /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "TICALCS_EXPORTS" /D "__WIN32__" /Fp"$(INTDIR)\ticalcs.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL" 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\ticalcs.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ticalcs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ticalcs.pdb" /machine:I386 /out:"..\..\tests\ticalcs.dll" /implib:"$(OUTDIR)\ticalcs.lib" 
LINK32_OBJS= \
	"$(INTDIR)\cmd73.obj" \
	"$(INTDIR)\cmd82.obj" \
	"$(INTDIR)\cmd85.obj" \
	"$(INTDIR)\cmd89.obj" \
	"$(INTDIR)\cmd92.obj" \
	"$(INTDIR)\ti73.obj" \
	"$(INTDIR)\Ti82.obj" \
	"$(INTDIR)\Ti83.obj" \
	"$(INTDIR)\Ti85.obj" \
	"$(INTDIR)\Ti86.obj" \
	"$(INTDIR)\Ti89.obj" \
	"$(INTDIR)\Ti92.obj" \
	"$(INTDIR)\keys73.obj" \
	"$(INTDIR)\keys83p.obj" \
	"$(INTDIR)\keys89.obj" \
	"$(INTDIR)\keys92p.obj" \
	"$(INTDIR)\tikeys.obj" \
	"$(INTDIR)\packets.obj" \
	"$(INTDIR)\clock.obj" \
	"$(INTDIR)\dirlist.obj" \
	"$(INTDIR)\Error.obj" \
	"$(INTDIR)\Intrface.obj" \
	"$(INTDIR)\printl.obj" \
	"$(INTDIR)\Probe.obj" \
	"$(INTDIR)\tnode.obj" \
	"$(INTDIR)\type2str.obj" \
	"$(INTDIR)\ticalcs.res" \
	"..\..\..\..\..\Gtk2Dev\lib\intl.lib" \
	"..\..\..\ticables\build\msvc\ticables.lib" \
	"..\..\..\tifiles\build\msvc\tifiles.lib"

"..\..\tests\ticalcs.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ticalcs - Win32 Debug"

OUTDIR=C:\msvc\tilp
INTDIR=C:\msvc\Output\ticalcs
# Begin Custom Macros
OutDir=C:\msvc\tilp\ 
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ticalcs.dll"

!ELSE 

ALL : "tifiles - Win32 Debug" "ticables - Win32 Debug" "$(OUTDIR)\ticalcs.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"ticables - Win32 DebugCLEAN" "tifiles - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\clock.obj"
	-@erase "$(INTDIR)\cmd73.obj"
	-@erase "$(INTDIR)\cmd82.obj"
	-@erase "$(INTDIR)\cmd85.obj"
	-@erase "$(INTDIR)\cmd89.obj"
	-@erase "$(INTDIR)\cmd92.obj"
	-@erase "$(INTDIR)\dirlist.obj"
	-@erase "$(INTDIR)\Error.obj"
	-@erase "$(INTDIR)\Intrface.obj"
	-@erase "$(INTDIR)\keys73.obj"
	-@erase "$(INTDIR)\keys83p.obj"
	-@erase "$(INTDIR)\keys89.obj"
	-@erase "$(INTDIR)\keys92p.obj"
	-@erase "$(INTDIR)\packets.obj"
	-@erase "$(INTDIR)\printl.obj"
	-@erase "$(INTDIR)\Probe.obj"
	-@erase "$(INTDIR)\ti73.obj"
	-@erase "$(INTDIR)\Ti82.obj"
	-@erase "$(INTDIR)\Ti83.obj"
	-@erase "$(INTDIR)\Ti85.obj"
	-@erase "$(INTDIR)\Ti86.obj"
	-@erase "$(INTDIR)\Ti89.obj"
	-@erase "$(INTDIR)\Ti92.obj"
	-@erase "$(INTDIR)\ticalcs.res"
	-@erase "$(INTDIR)\tikeys.obj"
	-@erase "$(INTDIR)\tnode.obj"
	-@erase "$(INTDIR)\type2str.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ticalcs.dll"
	-@erase "$(OUTDIR)\ticalcs.exp"
	-@erase "$(OUTDIR)\ticalcs.ilk"
	-@erase "$(OUTDIR)\ticalcs.lib"
	-@erase "$(OUTDIR)\ticalcs.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /w /W0 /GX /ZI /Od /I "../../../ticables/src" /I "../../../tifiles/src" /I "C:\Gtk2Dev\Include" /I "." /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /D "TICALCS_EXPORTS" /D "__WIN32__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL" 
RSC=rc.exe
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\ticalcs.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ticalcs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ticalcs.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ticalcs.dll" /implib:"$(OUTDIR)\ticalcs.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\cmd73.obj" \
	"$(INTDIR)\cmd82.obj" \
	"$(INTDIR)\cmd85.obj" \
	"$(INTDIR)\cmd89.obj" \
	"$(INTDIR)\cmd92.obj" \
	"$(INTDIR)\ti73.obj" \
	"$(INTDIR)\Ti82.obj" \
	"$(INTDIR)\Ti83.obj" \
	"$(INTDIR)\Ti85.obj" \
	"$(INTDIR)\Ti86.obj" \
	"$(INTDIR)\Ti89.obj" \
	"$(INTDIR)\Ti92.obj" \
	"$(INTDIR)\keys73.obj" \
	"$(INTDIR)\keys83p.obj" \
	"$(INTDIR)\keys89.obj" \
	"$(INTDIR)\keys92p.obj" \
	"$(INTDIR)\tikeys.obj" \
	"$(INTDIR)\packets.obj" \
	"$(INTDIR)\clock.obj" \
	"$(INTDIR)\dirlist.obj" \
	"$(INTDIR)\Error.obj" \
	"$(INTDIR)\Intrface.obj" \
	"$(INTDIR)\printl.obj" \
	"$(INTDIR)\Probe.obj" \
	"$(INTDIR)\tnode.obj" \
	"$(INTDIR)\type2str.obj" \
	"$(INTDIR)\ticalcs.res" \
	"..\..\..\..\..\Gtk2Dev\lib\intl.lib" \
	"$(OUTDIR)\ticables.lib" \
	"$(OUTDIR)\tifiles.lib"

"$(OUTDIR)\ticalcs.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ticalcs.dep")
!INCLUDE "ticalcs.dep"
!ELSE 
!MESSAGE Warning: cannot find "ticalcs.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ticalcs - Win32 Release" || "$(CFG)" == "ticalcs - Win32 Debug"
SOURCE=..\..\src\cmd73.c

"$(INTDIR)\cmd73.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\cmd82.c

"$(INTDIR)\cmd82.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\cmd85.c

"$(INTDIR)\cmd85.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\cmd89.c

"$(INTDIR)\cmd89.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\cmd92.c

"$(INTDIR)\cmd92.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\ti73.c

"$(INTDIR)\ti73.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\Ti82.c

"$(INTDIR)\Ti82.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\Ti83.c

"$(INTDIR)\Ti83.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\Ti85.c

"$(INTDIR)\Ti85.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\Ti86.c

"$(INTDIR)\Ti86.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\Ti89.c

"$(INTDIR)\Ti89.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\Ti92.c

"$(INTDIR)\Ti92.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\keys73.c

"$(INTDIR)\keys73.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\keys83p.c

"$(INTDIR)\keys83p.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\keys89.c

"$(INTDIR)\keys89.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\keys92p.c

"$(INTDIR)\keys92p.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\tikeys.c

"$(INTDIR)\tikeys.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\packets.c

"$(INTDIR)\packets.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\clock.c

"$(INTDIR)\clock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\dirlist.c

"$(INTDIR)\dirlist.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\Error.c

"$(INTDIR)\Error.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\Intrface.c

"$(INTDIR)\Intrface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\printl.c

"$(INTDIR)\printl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\Probe.c

"$(INTDIR)\Probe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\tnode.c

"$(INTDIR)\tnode.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\type2str.c

"$(INTDIR)\type2str.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "ticalcs - Win32 Release"

"ticables - Win32 Release" : 
   cd "\sources\roms\ticables\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F .\ticables.mak CFG="ticables - Win32 Release" 
   cd "..\..\..\ticalcs\build\msvc"

"ticables - Win32 ReleaseCLEAN" : 
   cd "\sources\roms\ticables\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F .\ticables.mak CFG="ticables - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\ticalcs\build\msvc"

!ELSEIF  "$(CFG)" == "ticalcs - Win32 Debug"

"ticables - Win32 Debug" : 
   cd "\sources\roms\ticables\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F .\ticables.mak CFG="ticables - Win32 Debug" 
   cd "..\..\..\ticalcs\build\msvc"

"ticables - Win32 DebugCLEAN" : 
   cd "\sources\roms\ticables\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F .\ticables.mak CFG="ticables - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ticalcs\build\msvc"

!ENDIF 

!IF  "$(CFG)" == "ticalcs - Win32 Release"

"tifiles - Win32 Release" : 
   cd "\sources\roms\tifiles\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F .\tifiles.mak CFG="tifiles - Win32 Release" 
   cd "..\..\..\ticalcs\build\msvc"

"tifiles - Win32 ReleaseCLEAN" : 
   cd "\sources\roms\tifiles\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F .\tifiles.mak CFG="tifiles - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\ticalcs\build\msvc"

!ELSEIF  "$(CFG)" == "ticalcs - Win32 Debug"

"tifiles - Win32 Debug" : 
   cd "\sources\roms\tifiles\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F .\tifiles.mak CFG="tifiles - Win32 Debug" 
   cd "..\..\..\ticalcs\build\msvc"

"tifiles - Win32 DebugCLEAN" : 
   cd "\sources\roms\tifiles\build\msvc"
   $(MAKE) /$(MAKEFLAGS) /F .\tifiles.mak CFG="tifiles - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\ticalcs\build\msvc"

!ENDIF 

SOURCE=.\ticalcs.rc

"$(INTDIR)\ticalcs.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

