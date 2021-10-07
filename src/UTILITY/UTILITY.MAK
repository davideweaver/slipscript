# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103
# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=utility_test - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to utility_test - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "utility - Win32 Release" && "$(CFG)" !=\
 "utility - Win32 Debug" && "$(CFG)" != "utility_test - Win32 Release" &&\
 "$(CFG)" != "utility_test - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "utility.mak" CFG="utility_test - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "utility - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "utility - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "utility_test - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "utility_test - Win32 Debug" (based on\
 "Win32 (x86) Console Application")
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
# PROP Target_Last_Scanned "utility_test - Win32 Debug"

!IF  "$(CFG)" == "utility - Win32 Release"

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

ALL : "..\..\lib\utility.lib"

CLEAN : 
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\fdebug.obj"
	-@erase "$(INTDIR)\ini_file.obj"
	-@erase "$(INTDIR)\stricmp.obj"
	-@erase "$(INTDIR)\strlwr.obj"
	-@erase "$(INTDIR)\strnicmp.obj"
	-@erase "$(INTDIR)\strstri.obj"
	-@erase "$(INTDIR)\tags.obj"
	-@erase "..\..\lib\utility.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/utility.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

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

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/utility.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\utility.lib"
LIB32_FLAGS=/nologo /out:"..\..\lib\utility.lib" 
LIB32_OBJS= \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\fdebug.obj" \
	"$(INTDIR)\ini_file.obj" \
	"$(INTDIR)\stricmp.obj" \
	"$(INTDIR)\strlwr.obj" \
	"$(INTDIR)\strnicmp.obj" \
	"$(INTDIR)\strstri.obj" \
	"$(INTDIR)\tags.obj"

"..\..\lib\utility.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "utility - Win32 Debug"

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

ALL : "..\..\lib\utility.lib"

CLEAN : 
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\fdebug.obj"
	-@erase "$(INTDIR)\ini_file.obj"
	-@erase "$(INTDIR)\stricmp.obj"
	-@erase "$(INTDIR)\strlwr.obj"
	-@erase "$(INTDIR)\strnicmp.obj"
	-@erase "$(INTDIR)\strstri.obj"
	-@erase "$(INTDIR)\tags.obj"
	-@erase "..\..\lib\utility.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/utility.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

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

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/utility.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\utility.lib"
LIB32_FLAGS=/nologo /out:"..\..\lib\utility.lib" 
LIB32_OBJS= \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\fdebug.obj" \
	"$(INTDIR)\ini_file.obj" \
	"$(INTDIR)\stricmp.obj" \
	"$(INTDIR)\strlwr.obj" \
	"$(INTDIR)\strnicmp.obj" \
	"$(INTDIR)\strstri.obj" \
	"$(INTDIR)\tags.obj"

"..\..\lib\utility.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "utility_test - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "utility_test\Release"
# PROP BASE Intermediate_Dir "utility_test\Release"
# PROP BASE Target_Dir "utility_test"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "utility_test\Release"
# PROP Intermediate_Dir "utility_test\Release"
# PROP Target_Dir "utility_test"
OUTDIR=.\utility_test\Release
INTDIR=.\utility_test\Release

ALL : "$(OUTDIR)\utility_test.exe"

CLEAN : 
	-@erase "$(INTDIR)\test.obj"
	-@erase "$(OUTDIR)\utility_test.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/utility_test.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\utility_test\Release/
CPP_SBRS=.\.

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/utility_test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /profile /machine:I386\
 /out:"$(OUTDIR)/utility_test.exe" 
LINK32_OBJS= \
	"$(INTDIR)\test.obj" \
	"..\..\lib\utility.lib"

"$(OUTDIR)\utility_test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "utility_test - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "utility_test\Debug"
# PROP BASE Intermediate_Dir "utility_test\Debug"
# PROP BASE Target_Dir "utility_test"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "utility_test\Debug"
# PROP Intermediate_Dir "utility_test\Debug"
# PROP Target_Dir "utility_test"
OUTDIR=.\utility_test\Debug
INTDIR=.\utility_test\Debug

ALL : "$(OUTDIR)\utility_test.exe"

CLEAN : 
	-@erase "$(INTDIR)\test.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\utility_test.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/utility_test.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\utility_test\Debug/
CPP_SBRS=.\.

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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/utility_test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /profile /debug /machine:I386\
 /out:"$(OUTDIR)/utility_test.exe" 
LINK32_OBJS= \
	"$(INTDIR)\test.obj" \
	"..\..\lib\utility.lib"

"$(OUTDIR)\utility_test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "utility - Win32 Release"
# Name "utility - Win32 Debug"

!IF  "$(CFG)" == "utility - Win32 Release"

!ELSEIF  "$(CFG)" == "utility - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ini_file.c
DEP_CPP_INI_F=\
	".\utility_test\..\ini_file.h"\
	

"$(INTDIR)\ini_file.obj" : $(SOURCE) $(DEP_CPP_INI_F) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\tags.c
DEP_CPP_TAGS_=\
	".\utility_test\..\str.h"\
	".\utility_test\..\tags.h"\
	

"$(INTDIR)\tags.obj" : $(SOURCE) $(DEP_CPP_TAGS_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\strstri.c
DEP_CPP_STRST=\
	".\utility_test\..\str.h"\
	

"$(INTDIR)\strstri.obj" : $(SOURCE) $(DEP_CPP_STRST) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\strlwr.c
DEP_CPP_STRLW=\
	".\utility_test\..\str.h"\
	

"$(INTDIR)\strlwr.obj" : $(SOURCE) $(DEP_CPP_STRLW) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\stricmp.c
DEP_CPP_STRIC=\
	".\utility_test\..\str.h"\
	

"$(INTDIR)\stricmp.obj" : $(SOURCE) $(DEP_CPP_STRIC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\strnicmp.c
DEP_CPP_STRNI=\
	".\utility_test\..\str.h"\
	

"$(INTDIR)\strnicmp.obj" : $(SOURCE) $(DEP_CPP_STRNI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\fdebug.c
DEP_CPP_FDEBU=\
	".\fdebug.h"\
	

"$(INTDIR)\fdebug.obj" : $(SOURCE) $(DEP_CPP_FDEBU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\buffer.cpp
DEP_CPP_BUFFE=\
	".\buffer.h"\
	

"$(INTDIR)\buffer.obj" : $(SOURCE) $(DEP_CPP_BUFFE) "$(INTDIR)"


# End Source File
# End Target
################################################################################
# Begin Target

# Name "utility_test - Win32 Release"
# Name "utility_test - Win32 Debug"

!IF  "$(CFG)" == "utility_test - Win32 Release"

!ELSEIF  "$(CFG)" == "utility_test - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\utility_test\test.c

!IF  "$(CFG)" == "utility_test - Win32 Release"

DEP_CPP_TEST_=\
	".\utility_test\..\ini_file.h"\
	".\utility_test\..\str.h"\
	".\utility_test\..\tags.h"\
	

"$(INTDIR)\test.obj" : $(SOURCE) $(DEP_CPP_TEST_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "utility_test - Win32 Debug"

DEP_CPP_TEST_=\
	".\utility_test\..\ini_file.h"\
	".\utility_test\..\str.h"\
	".\utility_test\..\tags.h"\
	

"$(INTDIR)\test.obj" : $(SOURCE) $(DEP_CPP_TEST_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Data\Program\lib\utility.lib

!IF  "$(CFG)" == "utility_test - Win32 Release"

!ELSEIF  "$(CFG)" == "utility_test - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
