# Microsoft Developer Studio Project File - Name="libcsgfx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcsgfx - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcsgfx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcsgfx.mak" CFG="libcsgfx - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcsgfx - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsgfx - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsgfx - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcsgfx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcsgfx\"
# PROP Intermediate_Dir "..\..\out\release\build\libcsgfx\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGFX_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGFX_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcsgfx\libcsgfx.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGFX_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcsgfx.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsgfx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsgfx\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsgfx\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGFX_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGFX_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsgfx\libcsgfx.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGFX_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsgfx_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsgfx - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsgfx\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsgfx\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGFX_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGFX_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsgfx\libcsgfx.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGFX_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsgfx_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcsgfx - Win32 Release"
# Name "libcsgfx - Win32 Debug"
# Name "libcsgfx - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csgfx\csimgvec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\gradient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\imagemanipulate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\imagetools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\inv_cmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\memimage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\normalmaptools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\quantize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\shaderexp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\shadervar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\shadervarcontext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\staticvarcleanup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\xorpat.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\csimgvec.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\csrgbvct.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\gradient.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\imagebase.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\imagemanipulate.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\imagetools.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\inv_cmap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\memimage.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\normalmaptools.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\packrgb.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\quantize.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\rgbpixel.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\shaderexp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\shadervar.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\shadervarcontext.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\..\..\include\csgfx\xorpat.h
# End Source File
# End Group
# End Target
# End Project
