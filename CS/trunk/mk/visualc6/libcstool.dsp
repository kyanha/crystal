# Microsoft Developer Studio Project File - Name="libcstool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcstool - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcstool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcstool.mak" CFG="libcstool - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcstool - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcstool - Win32 Debug_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcstool - Win32 Release_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcstool - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcstool - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcstool - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\bin\libs"
# PROP Intermediate_Dir "csrelease\temp\libcstool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /c
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

!ELSEIF  "$(CFG)" == "libcstool - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\bin\libs"
# PROP Intermediate_Dir "csdebug\temp\libcstool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"csdebug\bin\libs\libcstool_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcstool - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csmemdbg"
# PROP BASE Intermediate_Dir "csmemdbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csmemdbg\bin\libs"
# PROP Intermediate_Dir "csmemdbg\temp\libcstool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_EXTENSIVE_MEMDEBUG" /FD /I "..\.." /I "..\..\include" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"csmemdbg\bin\libs\libcstool_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcstool - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\bin\libs"
# PROP Intermediate_Dir "csrelease\temp\libcstool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /c
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

!ELSEIF  "$(CFG)" == "libcstool - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\bin\libs"
# PROP Intermediate_Dir "csdebug\temp\libcstool"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"csdebug\bin\libs\libcstool_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcstool - Win32 ExtensiveMemDebug"
# Name "libcstool - Win32 Debug"
# Name "libcstool - Win32 Release"
# Name "libcstool - Win32 Debug_NR"
# Name "libcstool - Win32 Release_NR"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\anonrndbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\basetexfact.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\collider.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\csanim2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\csapplicationframework.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\csfxscr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\cspixmap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\csview.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\gentrtex.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\initapp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\keyval.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\mapnode.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\mdldata.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\mdltool.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\meshobjtmpl.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\prdots.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\prfire.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\proctex.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\prplasma.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\prsky.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\prwater.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\rbuflock.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\rendermeshholder.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\rendermeshlist.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\sndwrap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\sprbuild.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\tokenlist.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\vidprefs.h
# End Source File

# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\cstool\anonrndbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\basetexfact.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\collider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\csanim2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\csapplicationframework.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\csfxscr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\cspixmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\csview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\gentrtex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\initapp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\keyval.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\mapnode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\mdldata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\mdltool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\meshobjtmpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\prdots.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\prfire.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\proctex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\prplasma.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\prsky.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\prwater.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\rendermeshholder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\rendermeshlist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\sndwrap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\sprbuild.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\vidprefs.cpp
# End Source File

# End Group

# End Target
# End Project
