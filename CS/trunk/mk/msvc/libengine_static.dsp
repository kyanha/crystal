# Microsoft Developer Studio Project File - Name="libengine_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libengine_static - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libengine_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libengine_static.mak" CFG="libengine_static - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libengine_static - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libengine_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libengine_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libengine_static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libengine_static\"
# PROP Intermediate_Dir "..\..\out\release\build\libengine_static\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_STATIC_LINKED" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_STATIC_LINKED"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libengine_static\libengine_static.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_STATIC_LINKED" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libengine_static.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libengine_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libengine_static\"
# PROP Intermediate_Dir "..\..\out\debug\build\libengine_static\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_STATIC_LINKED" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_STATIC_LINKED"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libengine_static\libengine_static.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_STATIC_LINKED" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libengine_static_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libengine_static - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libengine_static\"
# PROP Intermediate_Dir "..\..\out\debug\build\libengine_static\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_STATIC_LINKED" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_STATIC_LINKED"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libengine_static\libengine_static.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_STATIC_LINKED" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libengine_static_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libengine_static - Win32 Release"
# Name "libengine_static - Win32 Debug"
# Name "libengine_static - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\msvc\libengine_staticreg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\camera.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\campos.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\config.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\cscoll.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\engine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\halogen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\halogen2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\impmesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\impprctx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\lghthalo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\light.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\lightmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\lview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\material.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\meshlod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\meshobj.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\movable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\objwatch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\portal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\portalcontainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\radiosty.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\rdrprior.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\region.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\renderloop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\rview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\sector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\sharevar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\stats.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\texture.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\engine\3d\camera.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\campos.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\cscoll.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\engine.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\halo.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\halogen.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\impmesh.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\impprctx.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\light.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\lightmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\lview.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\material.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\meshlod.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\meshobj.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\movable.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\objwatch.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\portal.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\portalcontainer.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\radiosty.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\rdrprior.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\region.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\renderloop.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\rview.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\sector.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\sharevar.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\stats.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\engine\3d\texture.h
# End Source File
# End Group
# End Target
# End Project
