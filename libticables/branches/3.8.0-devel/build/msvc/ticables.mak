# Microsoft Developer Studio Generated NMAKE File, Based on ticables.dsp
!IF "$(CFG)" == ""
CFG=ticables - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ticables - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ticables - Win32 Release" && "$(CFG)" != "ticables - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ticables.mak" CFG="ticables - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ticables - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ticables - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ticables - Win32 Release"

OUTDIR=.
INTDIR=C:\msvc\Output\ticables

ALL : "..\..\tests\ticables.dll"


CLEAN :
	-@erase "$(INTDIR)\detect.obj"
	-@erase "$(INTDIR)\direct.obj"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\intrface.obj"
	-@erase "$(INTDIR)\ioports.obj"
	-@erase "$(INTDIR)\link_avr.obj"
	-@erase "$(INTDIR)\link_dev.obj"
	-@erase "$(INTDIR)\link_dfl.obj"
	-@erase "$(INTDIR)\link_par.obj"
	-@erase "$(INTDIR)\link_ser.obj"
	-@erase "$(INTDIR)\link_slv.obj"
	-@erase "$(INTDIR)\link_tie.obj"
	-@erase "$(INTDIR)\link_tig.obj"
	-@erase "$(INTDIR)\link_vti.obj"
	-@erase "$(INTDIR)\link_vtl.obj"
	-@erase "$(INTDIR)\logging.obj"
	-@erase "$(INTDIR)\mapping.obj"
	-@erase "$(INTDIR)\porttalk.obj"
	-@erase "$(INTDIR)\ticables.res"
	-@erase "$(INTDIR)\type2str.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\verbose.obj"
	-@erase "$(INTDIR)\win32_detect.obj"
	-@erase "$(INTDIR)\win32_mapping.obj"
	-@erase "$(OUTDIR)\ticables.exp"
	-@erase "$(OUTDIR)\ticables.lib"
	-@erase "..\..\tests\ticables.dll"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /w /W0 /GX /I "./tiglusb" /D "NDEBUG" /D "TICABLES_EXPORTS" /D "__WIN32__" /D "WIN32" /D "_WINDOWS" /D "__I386__" /D "ENABLE_NLS" /Fp"$(INTDIR)\ticables.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o /win32 "NUL" 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\ticables.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ticables.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=intl.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ticables.pdb" /machine:I386 /out:"..\..\tests\ticables.dll" /implib:"$(OUTDIR)\ticables.lib" 
LINK32_OBJS= \
	"$(INTDIR)\ioports.obj" \
	"$(INTDIR)\link_avr.obj" \
	"$(INTDIR)\link_dev.obj" \
	"$(INTDIR)\link_dfl.obj" \
	"$(INTDIR)\link_par.obj" \
	"$(INTDIR)\link_ser.obj" \
	"$(INTDIR)\link_slv.obj" \
	"$(INTDIR)\link_tie.obj" \
	"$(INTDIR)\link_tig.obj" \
	"$(INTDIR)\link_vti.obj" \
	"$(INTDIR)\link_vtl.obj" \
	"$(INTDIR)\porttalk.obj" \
	"$(INTDIR)\detect.obj" \
	"$(INTDIR)\direct.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\intrface.obj" \
	"$(INTDIR)\logging.obj" \
	"$(INTDIR)\mapping.obj" \
	"$(INTDIR)\type2str.obj" \
	"$(INTDIR)\verbose.obj" \
	"$(INTDIR)\win32_detect.obj" \
	"$(INTDIR)\win32_mapping.obj" \
	"$(INTDIR)\ticables.res"

"..\..\tests\ticables.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ticables - Win32 Debug"

OUTDIR=C:\msvc\tilp
INTDIR=C:\msvc\Output\ticables
# Begin Custom Macros
OutDir=C:\msvc\tilp
# End Custom Macros

ALL : "$(OUTDIR)\ticables.dll"


CLEAN :
	-@erase "$(INTDIR)\detect.obj"
	-@erase "$(INTDIR)\direct.obj"
	-@erase "$(INTDIR)\error.obj"
	-@erase "$(INTDIR)\intrface.obj"
	-@erase "$(INTDIR)\ioports.obj"
	-@erase "$(INTDIR)\link_avr.obj"
	-@erase "$(INTDIR)\link_dev.obj"
	-@erase "$(INTDIR)\link_dfl.obj"
	-@erase "$(INTDIR)\link_par.obj"
	-@erase "$(INTDIR)\link_ser.obj"
	-@erase "$(INTDIR)\link_slv.obj"
	-@erase "$(INTDIR)\link_tie.obj"
	-@erase "$(INTDIR)\link_tig.obj"
	-@erase "$(INTDIR)\link_vti.obj"
	-@erase "$(INTDIR)\link_vtl.obj"
	-@erase "$(INTDIR)\logging.obj"
	-@erase "$(INTDIR)\mapping.obj"
	-@erase "$(INTDIR)\porttalk.obj"
	-@erase "$(INTDIR)\ticables.res"
	-@erase "$(INTDIR)\type2str.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\verbose.obj"
	-@erase "$(INTDIR)\win32_detect.obj"
	-@erase "$(INTDIR)\win32_mapping.obj"
	-@erase "$(OUTDIR)\ticables.dll"
	-@erase "$(OUTDIR)\ticables.exp"
	-@erase "$(OUTDIR)\ticables.ilk"
	-@erase "$(OUTDIR)\ticables.lib"
	-@erase "$(OUTDIR)\ticables.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /GX /ZI /Od /I "./tiglusb" /D "_DEBUG" /D "TICABLES_EXPORTS" /D "__WIN32__" /D "WIN32" /D "_WINDOWS" /D "__I386__" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o /win32 "NUL" 
RSC_PROJ=/l 0x40c /fo"$(INTDIR)\ticables.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/o"$(OUTDIR)\ticables.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib intl.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ticables.pdb" /debug /debugtype:both /machine:I386 /out:"$(OUTDIR)\ticables.dll" /implib:"$(OUTDIR)\ticables.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ioports.obj" \
	"$(INTDIR)\link_avr.obj" \
	"$(INTDIR)\link_dev.obj" \
	"$(INTDIR)\link_dfl.obj" \
	"$(INTDIR)\link_par.obj" \
	"$(INTDIR)\link_ser.obj" \
	"$(INTDIR)\link_slv.obj" \
	"$(INTDIR)\link_tie.obj" \
	"$(INTDIR)\link_tig.obj" \
	"$(INTDIR)\link_vti.obj" \
	"$(INTDIR)\link_vtl.obj" \
	"$(INTDIR)\porttalk.obj" \
	"$(INTDIR)\detect.obj" \
	"$(INTDIR)\direct.obj" \
	"$(INTDIR)\error.obj" \
	"$(INTDIR)\intrface.obj" \
	"$(INTDIR)\logging.obj" \
	"$(INTDIR)\mapping.obj" \
	"$(INTDIR)\type2str.obj" \
	"$(INTDIR)\verbose.obj" \
	"$(INTDIR)\win32_detect.obj" \
	"$(INTDIR)\win32_mapping.obj" \
	"$(INTDIR)\ticables.res"

"$(OUTDIR)\ticables.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ticables.dep")
!INCLUDE "ticables.dep"
!ELSE 
!MESSAGE Warning: cannot find "ticables.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ticables - Win32 Release" || "$(CFG)" == "ticables - Win32 Debug"
SOURCE=..\..\src\win32\ioports.c

"$(INTDIR)\ioports.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_avr.c

"$(INTDIR)\link_avr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_dev.c

"$(INTDIR)\link_dev.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_dfl.c

"$(INTDIR)\link_dfl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_par.c

"$(INTDIR)\link_par.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_ser.c

"$(INTDIR)\link_ser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_slv.c

"$(INTDIR)\link_slv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_tie.c

"$(INTDIR)\link_tie.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_tig.c

"$(INTDIR)\link_tig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_vti.c

"$(INTDIR)\link_vti.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\link_vtl.c

"$(INTDIR)\link_vtl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\win32\porttalk.c

"$(INTDIR)\porttalk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\detect.c

"$(INTDIR)\detect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\direct.c

"$(INTDIR)\direct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\error.c

"$(INTDIR)\error.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\intrface.c

"$(INTDIR)\intrface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\logging.c

"$(INTDIR)\logging.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\mapping.c

"$(INTDIR)\mapping.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\type2str.c

"$(INTDIR)\type2str.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\verbose.c

"$(INTDIR)\verbose.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\win32\win32_detect.c

"$(INTDIR)\win32_detect.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\src\win32\win32_mapping.c

"$(INTDIR)\win32_mapping.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ticables.rc

"$(INTDIR)\ticables.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

