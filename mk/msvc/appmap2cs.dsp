# Microsoft Developer Studio Project File - Name="appmap2cs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=appmap2cs - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "appmap2cs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "appmap2cs.mak" CFG="appmap2cs - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "appmap2cs - Win32 ExtensiveMemDebug_NR" (based on "Win32 (x86) Console Application")
!MESSAGE "appmap2cs - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Console Application")
!MESSAGE "appmap2cs - Win32 Debug_NR" (based on "Win32 (x86) Console Application")
!MESSAGE "appmap2cs - Win32 Release_NR" (based on "Win32 (x86) Console Application")
!MESSAGE "appmap2cs - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "appmap2cs - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "appmap2cs - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\appmap2cs\"
# PROP Intermediate_Dir "..\..\out\release\build\appmap2cs\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\appmap2cs\appmap2cs.res" /D "NDEBUG" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /subsystem:console /machine:I386 /OPT:NOREF /out:"..\..\map2cs.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appmap2cs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\appmap2cs\"
# PROP Intermediate_Dir "..\..\out\debug\build\appmap2cs\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\appmap2cs\appmap2cs.res" /D "_DEBUG" /D "CS_DEBUG" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /subsystem:console /machine:I386 /debug /pdbtype:sept /out:"..\..\map2cs.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appmap2cs - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\appmap2cs\"
# PROP Intermediate_Dir "..\..\out\release\build\appmap2cs\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\appmap2cs\appmap2cs.res" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /subsystem:console /machine:I386 /OPT:NOREF /out:"..\..\map2cs.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appmap2cs - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\appmap2cs\"
# PROP Intermediate_Dir "..\..\out\debug\build\appmap2cs\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\appmap2cs\appmap2cs.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /subsystem:console /machine:I386 /debug /pdbtype:sept /out:"..\..\map2cs.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appmap2cs - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 
# PROP BASE Output_Dir "memdebug"
# PROP BASE Intermediate_Dir "memdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 
# PROP Output_Dir "..\..\out\memdebug\build\appmap2cs\"
# PROP Intermediate_Dir "..\..\out\memdebug\build\appmap2cs\"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c  /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\memdebug\build\appmap2cs\appmap2cs.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /subsystem:console /machine:I386  /out:"..\..\map2cs.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appmap2cs - Win32 ExtensiveMemDebug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 
# PROP BASE Output_Dir "memdebug"
# PROP BASE Intermediate_Dir "memdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 
# PROP Output_Dir "..\..\out\memdebug\build\appmap2cs\"
# PROP Intermediate_Dir "..\..\out\memdebug\build\appmap2cs\"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c  /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\memdebug\build\appmap2cs\appmap2cs.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /subsystem:console /machine:I386  /out:"..\..\map2cs.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "appmap2cs - Win32 Release"
# Name "appmap2cs - Win32 Debug"
# Name "appmap2cs - Win32 Release_NR"
# Name "appmap2cs - Win32 Debug_NR"
# Name "appmap2cs - Win32 ExtensiveMemDebug"
# Name "appmap2cs - Win32 ExtensiveMemDebug_NR"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\apps\import\map2cs\bindata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\brush.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\csector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\cthing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\cworld.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\dmath3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\entity.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\iportal.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\isector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\ithing.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\iworld.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\map.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\map2cs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\mcurve.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\mparser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\mpoly.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\mpolyset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\pk3file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\sparser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\texarch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\texfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\texman.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\texplane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\vertbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\wad3file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\ziparch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\zipfile.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\apps\import\map2cs\bindata.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\brush.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\contain.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\csector.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\cthing.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\cworld.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\dmath3d.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\entity.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\iportal.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\isector.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\ithing.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\iworld.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\map.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\mapstd.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\mcurve.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\mparser.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\mpoly.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\mpolyset.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\pk3file.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\sparser.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\texarch.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\texfile.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\texman.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\texplane.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\vertbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\wad3file.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\ziparch.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\zipfile.h
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\map2cs\dochelp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\msvc\appmap2cs.rc
# End Source File
# End Group
# End Target
# End Project
