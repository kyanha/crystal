# Microsoft Developer Studio Project File - Name="libcsws" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcsws - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcsws.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcsws.mak" CFG="libcsws - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcsws - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsws - Win32 Debug_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsws - Win32 Release_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsws - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsws - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcsws - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\bin\libs"
# PROP Intermediate_Dir "csrelease\temp\libcsws"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsws - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\bin\libs"
# PROP Intermediate_Dir "csdebug\temp\libcsws"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"csdebug\bin\libs\libcsws_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsws - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csmemdbg"
# PROP BASE Intermediate_Dir "csmemdbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csmemdbg\bin\libs"
# PROP Intermediate_Dir "csmemdbg\temp\libcsws"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_EXTENSIVE_MEMDEBUG" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"csmemdbg\bin\libs\libcsws_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsws - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\bin\libs"
# PROP Intermediate_Dir "csrelease\temp\libcsws"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsws - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\bin\libs"
# PROP Intermediate_Dir "csdebug\temp\libcsws"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"csdebug\bin\libs\libcsws_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcsws - Win32 ExtensiveMemDebug"
# Name "libcsws - Win32 Debug"
# Name "libcsws - Win32 Release"
# Name "libcsws - Win32 Debug_NR"
# Name "libcsws - Win32 Release_NR"
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

# End Group

# End Target
# End Project
