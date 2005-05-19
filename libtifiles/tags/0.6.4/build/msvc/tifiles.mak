# Microsoft Developer Studio Generated NMAKE File, Based on tifiles.dsp
!IF "$(CFG)" == ""
CFG=tifiles - Win32 Debug
!MESSAGE No configuration specified. Defaulting to tifiles - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "tifiles - Win32 Release" && "$(CFG)" != "tifiles - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tifiles.mak" CFG="tifiles - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tifiles - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tifiles - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "tifiles - Win32 Release"

OUTDIR=.
INTDIR=C:\msvc\Output\tifiles

ALL : "..\..\tests\tifiles.dll"


CLEAN :
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\files8X.obj"
	-@erase "$(INTDIR)\files9X.obj"
	-@erase "$(INTDIR)\filesxx.obj"
	-@erase "$(INTDIR)\grp_ops.obj"
	-@erase "$(INTDIR)\intelhex.obj"
	-@erase "$(INTDIR)\intrface.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\printl.obj"
	-@erase "$(INTDIR)\tifiles.res"
	-@erase "$(INTDIR)\trans.obj"
	-@erase "$(INTDIR)\type2str.obj"
	-@erase "$(INTDIR)\types73.obj"
	-@erase "$(INTDIR)\types82.obj"
	-@erase "$(INTDIR)\types83.obj"
	-@erase "$(INTDIR)\types83p.obj"
	-@erase "$(INTDIR)\types85.obj"
	-@erase "$(INTDIR)\types86.obj"
	-@erase "$(INTDIR)\types89.obj"
	-@erase "$(INTDIR)\types92.obj"
	-@erase "$(INTDIR)\types92p.obj"
	-@erase "$(INTDIR)\typesv2.obj"
	-@erase "$(INTDIR)\typesXX.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\tifiles.exp"
	-@erase "$(OUTDIR)\tifiles.lib"
	-@erase "..\..\tests\tifiles.dll"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /w /W0 /GX /I "C:\Gtk2Dev\include" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "TIFILES_EXPORTS" /D "__WIN32__" /Fp"$(INTDIR)\tifiles.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\tifiles.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tifiles.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\tifiles.pdb" /machine:I386 /out:"..\..\tests\tifiles.dll" /implib:"$(OUTDIR)\tifiles.lib" 
LINK32_OBJS= \
	"$(INTDIR)\types73.obj" \
	"$(INTDIR)\types82.obj" \
	"$(INTDIR)\types83.obj" \
	"$(INTDIR)\types83p.obj" \
	"$(INTDIR)\types85.obj" \
	"$(INTDIR)\types86.obj" \
	"$(INTDIR)\types89.obj" \
	"$(INTDIR)\types92.obj" \
	"$(INTDIR)\types92p.obj" \
	"$(INTDIR)\typesv2.obj" \
	"$(INTDIR)\typesXX.obj" \
	"$(INTDIR)\files8X.obj" \
	"$(INTDIR)\files9X.obj" \
	"$(INTDIR)\filesxx.obj" \
	"$(INTDIR)\grp_ops.obj" \
	"$(INTDIR)\intelhex.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\intrface.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\printl.obj" \
	"$(INTDIR)\trans.obj" \
	"$(INTDIR)\type2str.obj" \
	"$(INTDIR)\tifiles.res"

"..\..\tests\tifiles.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tifiles - Win32 Debug"

OUTDIR=C:\msvc\tilp
INTDIR=C:\msvc\Output\tifiles
# Begin Custom Macros
OutDir=C:\msvc\tilp\ 
# End Custom Macros

ALL : "$(OUTDIR)\tifiles.dll"


CLEAN :
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\files8X.obj"
	-@erase "$(INTDIR)\files9X.obj"
	-@erase "$(INTDIR)\filesxx.obj"
	-@erase "$(INTDIR)\grp_ops.obj"
	-@erase "$(INTDIR)\intelhex.obj"
	-@erase "$(INTDIR)\intrface.obj"
	-@erase "$(INTDIR)\misc.obj"
	-@erase "$(INTDIR)\printl.obj"
	-@erase "$(INTDIR)\tifiles.res"
	-@erase "$(INTDIR)\trans.obj"
	-@erase "$(INTDIR)\type2str.obj"
	-@erase "$(INTDIR)\types73.obj"
	-@erase "$(INTDIR)\types82.obj"
	-@erase "$(INTDIR)\types83.obj"
	-@erase "$(INTDIR)\types83p.obj"
	-@erase "$(INTDIR)\types85.obj"
	-@erase "$(INTDIR)\types86.obj"
	-@erase "$(INTDIR)\types89.obj"
	-@erase "$(INTDIR)\types92.obj"
	-@erase "$(INTDIR)\types92p.obj"
	-@erase "$(INTDIR)\typesv2.obj"
	-@erase "$(INTDIR)\typesXX.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\tifiles.dll"
	-@erase "$(OUTDIR)\tifiles.exp"
	-@erase "$(OUTDIR)\tifiles.ilk"
	-@erase "$(OUTDIR)\tifiles.lib"
	-@erase "$(OUTDIR)\tifiles.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W2 /GX /ZI /Od /I "C:\Gtk2Dev\include" /D "_DEBUG" /D "_CONSOLE" /D "WIN32" /D "TIFILES_EXPORTS" /D "__WIN32__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\tifiles.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tifiles.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\tifiles.pdb" /debug /machine:I386 /out:"$(OUTDIR)\tifiles.dll" /implib:"$(OUTDIR)\tifiles.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\types73.obj" \
	"$(INTDIR)\types82.obj" \
	"$(INTDIR)\types83.obj" \
	"$(INTDIR)\types83p.obj" \
	"$(INTDIR)\types85.obj" \
	"$(INTDIR)\types86.obj" \
	"$(INTDIR)\types89.obj" \
	"$(INTDIR)\types92.obj" \
	"$(INTDIR)\types92p.obj" \
	"$(INTDIR)\typesv2.obj" \
	"$(INTDIR)\typesXX.obj" \
	"$(INTDIR)\files8X.obj" \
	"$(INTDIR)\files9X.obj" \
	"$(INTDIR)\filesxx.obj" \
	"$(INTDIR)\grp_ops.obj" \
	"$(INTDIR)\intelhex.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\intrface.obj" \
	"$(INTDIR)\misc.obj" \
	"$(INTDIR)\printl.obj" \
	"$(INTDIR)\trans.obj" \
	"$(INTDIR)\type2str.obj" \
	"$(INTDIR)\tifiles.res"

"$(OUTDIR)\tifiles.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("tifiles.dep")
!INCLUDE "tifiles.dep"
!ELSE 
!MESSAGE Warning: cannot find "tifiles.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tifiles - Win32 Release" || "$(CFG)" == "tifiles - Win32 Debug"
SOURCE=..\..\src\types73.c

"$(INTDIR)\types73.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\types82.c

"$(INTDIR)\types82.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\types83.c

"$(INTDIR)\types83.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\types83p.c

"$(INTDIR)\types83p.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\types85.c

"$(INTDIR)\types85.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\types86.c

"$(INTDIR)\types86.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\types89.c

"$(INTDIR)\types89.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\types92.c

"$(INTDIR)\types92.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\types92p.c

"$(INTDIR)\types92p.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\typesv2.c

"$(INTDIR)\typesv2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\typesXX.c

"$(INTDIR)\typesXX.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\files8X.c

"$(INTDIR)\files8X.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\files9X.c

"$(INTDIR)\files9X.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\filesxx.c

"$(INTDIR)\filesxx.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\grp_ops.c

"$(INTDIR)\grp_ops.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\intelhex.c

"$(INTDIR)\intelhex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\error.c

"$(INTDIR)\error.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\intrface.c

"$(INTDIR)\intrface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\misc.c

"$(INTDIR)\misc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\printl.c

"$(INTDIR)\printl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\trans.c

"$(INTDIR)\trans.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\type2str.c

"$(INTDIR)\type2str.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\tifiles.rc

"$(INTDIR)\tifiles.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

