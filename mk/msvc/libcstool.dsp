# Microsoft Developer Studio Project File - Name="libcstool" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcstool - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcstool.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcstool.mak" CFG="libcstool - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcstool - Win32 ExtensiveMemDebug_NR" (based on "Win32 (x86) Static Library")
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
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcstool - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcstool\"
# PROP Intermediate_Dir "..\..\out\release\build\libcstool\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcstool\libcstool.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcstool.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcstool - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcstool\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcstool\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcstool\libcstool.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcstool_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcstool - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcstool\"
# PROP Intermediate_Dir "..\..\out\release\build\libcstool\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcstool\libcstool.res" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcstool.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcstool - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcstool\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcstool\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcstool\libcstool.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcstool_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcstool - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcstool\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcstool\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcstool\libcstool.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcstool_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcstool - Win32 ExtensiveMemDebug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcstool\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcstool\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcstool\libcstool.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSTOOL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcstool_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcstool - Win32 Release"
# Name "libcstool - Win32 Debug"
# Name "libcstool - Win32 Release_NR"
# Name "libcstool - Win32 Debug_NR"
# Name "libcstool - Win32 ExtensiveMemDebug"
# Name "libcstool - Win32 ExtensiveMemDebug_NR"
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

SOURCE=..\..\libs\cstool\proctex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\proctxtanim.cpp
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

SOURCE=..\..\libs\cstool\..\..\include\cstool\framedataholder.h
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

SOURCE=..\..\libs\cstool\..\..\include\cstool\proctex.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\..\..\include\cstool\proctxtanim.h
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
# End Target
# End Project
