# Microsoft Developer Studio Project File - Name="libcsws" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcsws - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcsws.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcsws.mak" CFG="libcsws - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcsws - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsws - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsws - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcsws - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcsws\"
# PROP Intermediate_Dir "..\..\out\release\build\libcsws\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSWS_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSWS_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcsws\libcsws.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSWS_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcsws.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsws - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsws\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsws\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSWS_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSWS_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsws\libcsws.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSWS_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsws_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsws - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsws\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsws\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSWS_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSWS_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsws\libcsws.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSWS_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsws_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcsws - Win32 Release"
# Name "libcsws - Win32 Debug"
# Name "libcsws - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csws\csabslay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csapp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csbackgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csbaglay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csbdrlay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csboxlay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cschkbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cscomp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cscompol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cscwheel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csdialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csflwlay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csgfxppl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csgrdlay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csgrid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cshints.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csiline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cskeyacc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cslayout.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cslistbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csmenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csmouse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csnotebk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csradbut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csscrbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csskin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csspinbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cssplit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csstatic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csstddlg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cstimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cstree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\csttlbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cswindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cswspal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cswstex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\cswsutil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\skins\default\sdbutton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\skins\default\sddialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\skins\default\sdlistbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\skins\default\sdscrbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\skins\default\sdttlbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\skins\default\sdwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\staticvarcleanup.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csabslay.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csapp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csbackgr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csbaglay.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csbdrlay.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csboxlay.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csbutton.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cschkbox.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cscomp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cscwheel.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csdialog.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csflwlay.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csgfxppl.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csgrdlay.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csgrid.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cshints.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csiline.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cskeyacc.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cslayout.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cslistbx.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csmenu.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csmouse.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csnotebk.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csradbut.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csscrbar.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csskin.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csspinbx.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cssplit.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csstatic.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csstddlg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cstimer.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cstree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csttlbar.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cswindow.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\csws.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cswsaux.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cswspal.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cswstex.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\cswsutil.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csws\..\..\include\csws\sdefault.h
# End Source File
# End Group
# End Target
# End Project
