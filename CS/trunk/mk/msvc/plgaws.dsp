# Microsoft Developer Studio Project File - Name="plgaws" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plgaws - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plgaws.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plgaws.mak" CFG="plgaws - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plgaws - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgaws - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgaws - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plgaws - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\plgaws\"
# PROP Intermediate_Dir "..\..\out\release\build\plgaws\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_WINDOWS" /D "YY_NEVER_INTERACTIVE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_WINDOWS" /D "YY_NEVER_INTERACTIVE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\plgaws\plgaws.res" /D "NDEBUG" /D "_WINDOWS" /D "YY_NEVER_INTERACTIVE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib advapi32.lib user32.lib gdi32.lib shell32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /OPT:NOREF /dll /out:"..\..\aws.dll" /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgaws - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\plgaws\"
# PROP Intermediate_Dir "..\..\out\debug\build\plgaws\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "YY_NEVER_INTERACTIVE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "YY_NEVER_INTERACTIVE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\plgaws\plgaws.res" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "YY_NEVER_INTERACTIVE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib advapi32.lib user32.lib gdi32.lib shell32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /dll /out:"..\..\aws.dll" /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgaws - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\plgaws\"
# PROP Intermediate_Dir "..\..\out\debug\build\plgaws\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "YY_NEVER_INTERACTIVE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "YY_NEVER_INTERACTIVE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\plgaws\plgaws.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "YY_NEVER_INTERACTIVE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib advapi32.lib user32.lib gdi32.lib shell32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /dll /out:"..\..\aws.dll" /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "plgaws - Win32 Release"
# Name "plgaws - Win32 Debug"
# Name "plgaws - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\aws\aws.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\aws3dfrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsbarct.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsbl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awschkbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscmdbt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscomp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsControlBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsEngineView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsfparm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgbl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgrpfr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsimgvw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awskcfct.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awskeycn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslabel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslstbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsmled.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsntbk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsparser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsprefs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsradbt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsscr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsscrbr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsslot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsstbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsstdsk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstxtbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awswin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\skinlex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\skinpars.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\aws\aws.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\aws3dfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsbarct.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsbl.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awschkbx.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscmdbt.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscomp.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsControlBar.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscscr.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsEngineView.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsfparm.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgbc.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgbl.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgrpfr.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsimgvw.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awskcfct.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslabel.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslayot.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslstbx.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsmled.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsntbk.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsparser.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsprefs.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsradbt.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsscr.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsscrbr.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsslot.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsstbar.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsstdsk.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsTabCtrl.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstex.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstimer.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstxtbx.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awswin.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\skinpars.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\msvc\plgaws.rc
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\aws.csplugin
# End Source File
# End Group
# End Target
# End Project
