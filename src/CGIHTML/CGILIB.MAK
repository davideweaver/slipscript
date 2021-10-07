# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=cgilib - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to cgilib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cgilib - Win32 Release" && "$(CFG)" != "cgilib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "cgilib.mak" CFG="cgilib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cgilib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cgilib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "cgilib - Win32 Debug"
CPP=cl.exe

!IF  "$(CFG)" == "cgilib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\lib\cgi-lib.lib"

CLEAN : 
	-@erase "..\..\lib\cgi-lib.lib"
	-@erase ".\Release\cgi-lib.obj"
	-@erase ".\Release\cgi-llist.obj"
	-@erase ".\Release\html-lib.obj"
	-@erase ".\Release\is_cgi.obj"
	-@erase ".\Release\string-lib.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/cgilib.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cgilib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib/cgi-lib.lib"
LIB32_FLAGS=/nologo /out:"../../lib/cgi-lib.lib" 
LIB32_OBJS= \
	".\Release\cgi-lib.obj" \
	".\Release\cgi-llist.obj" \
	".\Release\html-lib.obj" \
	".\Release\is_cgi.obj" \
	".\Release\string-lib.obj"

"..\..\lib\cgi-lib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cgilib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\lib\cgi-lib.lib"

CLEAN : 
	-@erase "..\..\lib\cgi-lib.lib"
	-@erase ".\Debug\cgi-lib.obj"
	-@erase ".\Debug\cgi-llist.obj"
	-@erase ".\Debug\html-lib.obj"
	-@erase ".\Debug\is_cgi.obj"
	-@erase ".\Debug\string-lib.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/cgilib.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/cgilib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../lib/cgi-lib.lib"
LIB32_FLAGS=/nologo /out:"../../lib/cgi-lib.lib" 
LIB32_OBJS= \
	".\Debug\cgi-lib.obj" \
	".\Debug\cgi-llist.obj" \
	".\Debug\html-lib.obj" \
	".\Debug\is_cgi.obj" \
	".\Debug\string-lib.obj"

"..\..\lib\cgi-lib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "cgilib - Win32 Release"
# Name "cgilib - Win32 Debug"

!IF  "$(CFG)" == "cgilib - Win32 Release"

!ELSEIF  "$(CFG)" == "cgilib - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=".\html-lib.c"
DEP_CPP_HTML_=\
	".\html-lib.h"\
	

!IF  "$(CFG)" == "cgilib - Win32 Release"


".\Release\html-lib.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cgilib - Win32 Debug"


".\Debug\html-lib.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=".\cgi-llist.c"
DEP_CPP_CGI_L=\
	".\cgi-llist.h"\
	".\string-lib.h"\
	

!IF  "$(CFG)" == "cgilib - Win32 Release"


".\Release\cgi-llist.obj" : $(SOURCE) $(DEP_CPP_CGI_L) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cgilib - Win32 Debug"


".\Debug\cgi-llist.obj" : $(SOURCE) $(DEP_CPP_CGI_L) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=".\cgi-lib.c"

!IF  "$(CFG)" == "cgilib - Win32 Release"

DEP_CPP_CGI_LI=\
	".\cgi-lib.h"\
	".\cgi-llist.h"\
	".\html-lib.h"\
	".\string-lib.h"\
	

".\Release\cgi-lib.obj" : $(SOURCE) $(DEP_CPP_CGI_LI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cgilib - Win32 Debug"

DEP_CPP_CGI_LI=\
	".\cgi-lib.h"\
	".\cgi-llist.h"\
	".\html-lib.h"\
	".\string-lib.h"\
	

".\Debug\cgi-lib.obj" : $(SOURCE) $(DEP_CPP_CGI_LI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=".\string-lib.c"
DEP_CPP_STRIN=\
	".\string-lib.h"\
	

!IF  "$(CFG)" == "cgilib - Win32 Release"


".\Release\string-lib.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cgilib - Win32 Debug"


".\Debug\string-lib.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\is_cgi.c
DEP_CPP_IS_CG=\
	".\cgi-lib.h"\
	".\cgi-llist.h"\
	".\string-lib.h"\
	

!IF  "$(CFG)" == "cgilib - Win32 Release"


".\Release\is_cgi.obj" : $(SOURCE) $(DEP_CPP_IS_CG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "cgilib - Win32 Debug"


".\Debug\is_cgi.obj" : $(SOURCE) $(DEP_CPP_IS_CG) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
