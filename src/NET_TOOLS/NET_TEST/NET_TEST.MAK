# Microsoft Developer Studio Generated NMAKE File, Format Version 4.10
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103
# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=net_tools - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to net_tools - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "net_test - Win32 Release" && "$(CFG)" !=\
 "net_test - Win32 Debug" && "$(CFG)" != "net_tools - Win32 Release" && "$(CFG)"\
 != "net_tools - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "net_test.mak" CFG="net_tools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "net_test - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "net_test - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "net_tools - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "net_tools - Win32 Debug" (based on "Win32 (x86) Static Library")
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
# PROP Target_Last_Scanned "net_tools - Win32 Debug"

!IF  "$(CFG)" == "net_test - Win32 Release"

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

ALL : "net_tools - Win32 Release" ".\Release\net_test.exe"

CLEAN : 
	-@erase ".\Release\net_test.exe"
	-@erase ".\Release\test.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/net_test.pch" /YX /Fo"$(INTDIR)/" /c 
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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/net_test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/net_test.pdb" /machine:I386 /out:"$(OUTDIR)/net_test.exe" 
LINK32_OBJS= \
	"..\..\..\lib\net_tools.lib" \
	".\Release\test.obj"

".\Release\net_test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "net_test - Win32 Debug"

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

ALL : "net_tools - Win32 Debug" ".\Debug\net_test.exe"

CLEAN : 
	-@erase ".\Debug\net_test.exe"
	-@erase ".\Debug\net_test.ilk"
	-@erase ".\Debug\net_test.pdb"
	-@erase ".\Debug\test.obj"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\vc40.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE"\
 /Fp"$(INTDIR)/net_test.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
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

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/net_test.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib wsock32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/net_test.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/net_test.exe" 
LINK32_OBJS= \
	"..\..\..\lib\net_tools.lib" \
	".\Debug\test.obj"

".\Debug\net_test.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "net_tools - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "net_tools\Release"
# PROP BASE Intermediate_Dir "net_tools\Release"
# PROP BASE Target_Dir "net_tools"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "net_tools\Release"
# PROP Intermediate_Dir "net_tools\Release"
# PROP Target_Dir "net_tools"
OUTDIR=.\net_tools\Release
INTDIR=.\net_tools\Release

ALL : "..\..\..\lib\net_tools.lib"

CLEAN : 
	-@erase "..\..\..\lib\net_tools.lib"
	-@erase ".\net_tools\Release\smsmtp.obj"
	-@erase ".\net_tools\Release\tcpstuff.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/net_tools.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\net_tools\Release/
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/net_tools.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib/net_tools.lib"
LIB32_FLAGS=/nologo /out:"../../../lib/net_tools.lib" 
LIB32_OBJS= \
	".\net_tools\Release\smsmtp.obj" \
	".\net_tools\Release\tcpstuff.obj"

"..\..\..\lib\net_tools.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "net_tools - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "net_tools\Debug"
# PROP BASE Intermediate_Dir "net_tools\Debug"
# PROP BASE Target_Dir "net_tools"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "net_tools\Debug"
# PROP Intermediate_Dir "net_tools\Debug"
# PROP Target_Dir "net_tools"
OUTDIR=.\net_tools\Debug
INTDIR=.\net_tools\Debug

ALL : "..\..\..\lib\net_tools.lib"

CLEAN : 
	-@erase "..\..\..\lib\net_tools.lib"
	-@erase ".\net_tools\Debug\smsmtp.obj"
	-@erase ".\net_tools\Debug\tcpstuff.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/net_tools.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\net_tools\Debug/
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
BSC32_FLAGS=/nologo /o"$(OUTDIR)/net_tools.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../lib/net_tools.lib"
LIB32_FLAGS=/nologo /out:"../../../lib/net_tools.lib" 
LIB32_OBJS= \
	".\net_tools\Debug\smsmtp.obj" \
	".\net_tools\Debug\tcpstuff.obj"

"..\..\..\lib\net_tools.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "net_test - Win32 Release"
# Name "net_test - Win32 Debug"

!IF  "$(CFG)" == "net_test - Win32 Release"

!ELSEIF  "$(CFG)" == "net_test - Win32 Debug"

!ENDIF 

################################################################################
# Begin Project Dependency

# Project_Dep_Name "net_tools"

!IF  "$(CFG)" == "net_test - Win32 Release"

"net_tools - Win32 Release" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\net_test.mak" CFG="net_tools - Win32 Release" 

!ELSEIF  "$(CFG)" == "net_test - Win32 Debug"

"net_tools - Win32 Debug" : 
   $(MAKE) /$(MAKEFLAGS) /F ".\net_test.mak" CFG="net_tools - Win32 Debug" 

!ENDIF 

# End Project Dependency
################################################################################
# Begin Source File

SOURCE=\Data\Program\lib\net_tools.lib

!IF  "$(CFG)" == "net_test - Win32 Release"

!ELSEIF  "$(CFG)" == "net_test - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\test.cpp
DEP_CPP_TEST_=\
	"..\smsmtp.h"\
	"..\tcpstuff.h"\
	{$(INCLUDE)}"\net_tools.h"\
	

!IF  "$(CFG)" == "net_test - Win32 Release"


".\Release\test.obj" : $(SOURCE) $(DEP_CPP_TEST_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "net_test - Win32 Debug"


".\Debug\test.obj" : $(SOURCE) $(DEP_CPP_TEST_) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
################################################################################
# Begin Target

# Name "net_tools - Win32 Release"
# Name "net_tools - Win32 Debug"

!IF  "$(CFG)" == "net_tools - Win32 Release"

!ELSEIF  "$(CFG)" == "net_tools - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=\Data\Program\src\net_tools\tcpstuff.cpp

!IF  "$(CFG)" == "net_tools - Win32 Release"

DEP_CPP_TCPST=\
	"..\tcpstuff.h"\
	

".\net_tools\Release\tcpstuff.obj" : $(SOURCE) $(DEP_CPP_TCPST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "net_tools - Win32 Debug"

DEP_CPP_TCPST=\
	"..\tcpstuff.h"\
	

".\net_tools\Debug\tcpstuff.obj" : $(SOURCE) $(DEP_CPP_TCPST) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=\Data\Program\src\net_tools\smsmtp.cpp

!IF  "$(CFG)" == "net_tools - Win32 Release"

DEP_CPP_SMSMT=\
	"..\smsmtp.h"\
	"..\tcpstuff.h"\
	

".\net_tools\Release\smsmtp.obj" : $(SOURCE) $(DEP_CPP_SMSMT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "net_tools - Win32 Debug"

DEP_CPP_SMSMT=\
	"..\smsmtp.h"\
	"..\tcpstuff.h"\
	

".\net_tools\Debug\smsmtp.obj" : $(SOURCE) $(DEP_CPP_SMSMT) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
