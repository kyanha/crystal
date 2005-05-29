# Microsoft Developer Studio Project File - Name="libcrystalspace_opengl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcrystalspace_opengl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcrystalspace_opengl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcrystalspace_opengl.mak" CFG="libcrystalspace_opengl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcrystalspace_opengl - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcrystalspace_opengl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcrystalspace_opengl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcrystalspace_opengl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcrystalspace_opengl\"
# PROP Intermediate_Dir "..\..\out\release\build\libcrystalspace_opengl\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSPLUGINCOMMON_GL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSPLUGINCOMMON_GL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcrystalspace_opengl\libcrystalspace_opengl.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSPLUGINCOMMON_GL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcrystalspace_opengl.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib opengl32.lib  /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcrystalspace_opengl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcrystalspace_opengl\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcrystalspace_opengl\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSPLUGINCOMMON_GL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSPLUGINCOMMON_GL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcrystalspace_opengl\libcrystalspace_opengl.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSPLUGINCOMMON_GL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcrystalspace_opengl_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib opengl32.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcrystalspace_opengl - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcrystalspace_opengl\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcrystalspace_opengl\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSPLUGINCOMMON_GL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSPLUGINCOMMON_GL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcrystalspace_opengl\libcrystalspace_opengl.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSPLUGINCOMMON_GL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcrystalspace_opengl_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib opengl32.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcrystalspace_opengl - Win32 Release"
# Name "libcrystalspace_opengl - Win32 Debug"
# Name "libcrystalspace_opengl - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\driverdb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\glcommon2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\glfontcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\glss.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\csextern_gl.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\driverdb.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\glcommon2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\glenum_identstrs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\glextmanager.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\glfontcache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\glhelper.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\glss.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\glstates.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\opengl\..\..\..\include\csplugincommon\opengl\iogl.h
# End Source File
# End Group
# End Target
# End Project
