# Microsoft Developer Studio Project File - Name="slipscript" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=slipscript - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "slipscript.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "slipscript.mak" CFG="slipscript - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "slipscript - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "slipscript - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "slipscript - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SLIPSCRIPT_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "INCLUDE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SLIPSCRIPT_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "slipscript - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SLIPSCRIPT_EXPORTS" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "INCLUDE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SLIPSCRIPT_EXPORTS" /YX /FD /GZ  /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "slipscript - Win32 Release"
# Name "slipscript - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\SRC\INTERP\BOBCOM.C
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\BOBDBG.C
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\BOBERR.C
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\BOBODB.C
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\BOBSCN.C
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\BUFFER.CPP
# End Source File
# Begin Source File

SOURCE=".\SRC\CGIHTML\CGI-LIB.C"
# End Source File
# Begin Source File

SOURCE=".\SRC\CGIHTML\Cgi-llist.c"
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\DFILE.C
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\EXECUTE.C
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\FDEBUG.C
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\FUNCTION.CPP
# End Source File
# Begin Source File

SOURCE=".\SRC\CGIHTML\HTML-LIB.C"
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\INI_FILE.C
# End Source File
# Begin Source File

SOURCE=.\SRC\CGIHTML\IS_CGI.C
# End Source File
# Begin Source File

SOURCE=.\IS_SLIP\IS_SLIP.CPP
# End Source File
# Begin Source File

SOURCE=.\IS_SLIP\IS_SLIP.DEF
# End Source File
# Begin Source File

SOURCE=.\IS_SLIP\IS_SLIP.RC
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\LINEBUF.C
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\OBJECTS.C
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\OSINT.C
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\PACKET.C
# End Source File
# Begin Source File

SOURCE=.\IS_SLIP\SLIPSCRIPT.CPP
# End Source File
# Begin Source File

SOURCE=.\SRC\NET_TOOLS\SMSMTP.CPP
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\STREAMS.C
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\STRICMP.C
# End Source File
# Begin Source File

SOURCE=".\SRC\CGIHTML\String-lib.c"
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\STRNICMP.C
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\STRSTRI.C
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\TAGS.C
# End Source File
# Begin Source File

SOURCE=.\SRC\NET_TOOLS\TCPSTUFF.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SRC\INTERP\BOB.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\BOBERR.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\BOBODB.H
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\BUFFER.H
# End Source File
# Begin Source File

SOURCE=".\SRC\CGIHTML\CGI-LIB.H"
# End Source File
# Begin Source File

SOURCE=".\SRC\CGIHTML\Cgi-llist.h"
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\CGI.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\COMPILER.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\DATE_TIME.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\DFILE.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\EXECUTE.H
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\FDEBUG.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\FUNCTION.H
# End Source File
# Begin Source File

SOURCE=".\SRC\CGIHTML\HTML-LIB.H"
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\INI_FILE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\INTERP.H
# End Source File
# Begin Source File

SOURCE=".\SRC\CGIHTML\Is_cgi-lib.h"
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\IS_CGI.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\LINEBUF.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\NET_TOOLS.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\OBJECTS.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\ODBC.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\OSINT.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\PACKET.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\PATH.H
# End Source File
# Begin Source File

SOURCE=.\IS_SLIP\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\SLIPSCRIPT.H
# End Source File
# Begin Source File

SOURCE=.\SRC\NET_TOOLS\SMSMTP.H
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\STR.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\STREAMS.H
# End Source File
# Begin Source File

SOURCE=".\SRC\CGIHTML\String-lib.h"
# End Source File
# Begin Source File

SOURCE=.\SRC\UTILITY\TAGS.H
# End Source File
# Begin Source File

SOURCE=.\SRC\NET_TOOLS\TCPSTUFF.H
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\UTIL.H
# End Source File
# Begin Source File

SOURCE=.\SRC\INTERP\VER_INFO.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\IS_SLIP\IS_SLIP.RC2
# End Source File
# End Group
# End Target
# End Project
