# Microsoft Developer Studio Project File - Name="plgcsddsimg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plgcsddsimg - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plgcsddsimg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plgcsddsimg.mak" CFG="plgcsddsimg - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plgcsddsimg - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgcsddsimg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgcsddsimg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plgcsddsimg - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\plgcsddsimg\"
# PROP Intermediate_Dir "..\..\out\release\build\plgcsddsimg\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\plgcsddsimg\plgcsddsimg.res" /D "NDEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib advapi32.lib user32.lib gdi32.lib shell32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /OPT:NOREF /dll /out:"..\..\csddsimg.dll" /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgcsddsimg - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\plgcsddsimg\"
# PROP Intermediate_Dir "..\..\out\debug\build\plgcsddsimg\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\plgcsddsimg\plgcsddsimg.res" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib advapi32.lib user32.lib gdi32.lib shell32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /dll /out:"..\..\csddsimg.dll" /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgcsddsimg - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\plgcsddsimg\"
# PROP Intermediate_Dir "..\..\out\debug\build\plgcsddsimg\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\plgcsddsimg\plgcsddsimg.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib advapi32.lib user32.lib gdi32.lib shell32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /dll /out:"..\..\csddsimg.dll" /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "plgcsddsimg - Win32 Release"
# Name "plgcsddsimg - Win32 Debug"
# Name "plgcsddsimg - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\dds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ddsloader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ddssaver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\ccDoubleHeap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\ccHeap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\Cclist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\cfVector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\CodeBook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\DXTCGen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\fCodeBook.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\Image.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\ImageDXTC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\Lloyd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\MedianCut.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\Table.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\dds.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ddsloader.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ddssaver.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ddsutil.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\ccDoubleHeap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\ccHeap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\Cclist.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\cfVector.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\CodeBook.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\common.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\DXTCGen.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\fCodeBook.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\Image.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\ImageDXTC.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\Lloyd.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\MedianCut.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\ImageLib\Table.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\msvc\plgcsddsimg.rc
# End Source File
# Begin Source File

SOURCE=..\..\plugins\video\loader\dds\csddsimg.csplugin
# End Source File
# End Group
# End Target
# End Project
