# Microsoft Developer Studio Project File - Name="plgcsvosa3dl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plgcsvosa3dl - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plgcsvosa3dl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plgcsvosa3dl.mak" CFG="plgcsvosa3dl - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plgcsvosa3dl - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgcsvosa3dl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgcsvosa3dl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plgcsvosa3dl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\plgcsvosa3dl\"
# PROP Intermediate_Dir "..\..\out\release\build\plgcsvosa3dl\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\plgcsvosa3dl\plgcsvosa3dl.res" /D "NDEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib advapi32.lib user32.lib gdi32.lib shell32.lib zlib.lib metaobject_a3dl.lib metaobject_misc.lib  /nologo /version:4.0 /machine:I386 /OPT:NOREF /dll /out:"..\..\csvosa3dl.dll" /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgcsvosa3dl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\plgcsvosa3dl\"
# PROP Intermediate_Dir "..\..\out\debug\build\plgcsvosa3dl\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\plgcsvosa3dl\plgcsvosa3dl.res" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib advapi32.lib user32.lib gdi32.lib shell32.lib zlib.lib metaobject_a3dl.lib metaobject_misc.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /dll /out:"..\..\csvosa3dl.dll" /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgcsvosa3dl - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\plgcsvosa3dl\"
# PROP Intermediate_Dir "..\..\out\debug\build\plgcsvosa3dl\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\plgcsvosa3dl\plgcsvosa3dl.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib advapi32.lib user32.lib gdi32.lib shell32.lib zlib.lib metaobject_a3dl.lib metaobject_misc.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /dll /out:"..\..\csvosa3dl.dll" /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "plgcsvosa3dl - Win32 Release"
# Name "plgcsvosa3dl - Win32 Debug"
# Name "plgcsvosa3dl - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\net\vos\csvosa3dl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosbillboard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosclone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voscone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voscube.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voscylinder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voslight.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voslightmapcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosmaterial.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosmodel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosobject3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vospolygonmesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vossector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vossphere.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vostexture.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\net\vos\csvosa3dl.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosbillboard.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosclone.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voscone.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voscube.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voscylinder.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voslight.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\voslightmapcache.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosmaterial.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosmodel.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vosobject3d.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vospolygonmesh.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vossector.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vossphere.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\vostexture.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\msvc\plgcsvosa3dl.rc
# End Source File
# Begin Source File

SOURCE=..\..\plugins\net\vos\csvosa3dl.csplugin
# End Source File
# End Group
# End Target
# End Project
