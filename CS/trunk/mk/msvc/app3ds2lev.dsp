# Microsoft Developer Studio Project File - Name="app3ds2lev" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=app3ds2lev - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "app3ds2lev.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "app3ds2lev.mak" CFG="app3ds2lev - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "app3ds2lev - Win32 ExtensiveMemDebug_NR" (based on "Win32 (x86) Console Application")
!MESSAGE "app3ds2lev - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Console Application")
!MESSAGE "app3ds2lev - Win32 Debug_NR" (based on "Win32 (x86) Console Application")
!MESSAGE "app3ds2lev - Win32 Release_NR" (based on "Win32 (x86) Console Application")
!MESSAGE "app3ds2lev - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "app3ds2lev - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "app3ds2lev - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\app3ds2lev\"
# PROP Intermediate_Dir "..\..\out\release\build\app3ds2lev\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\app3ds2lev\app3ds2lev.res" /D "NDEBUG" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib lib3ds-120.lib  /nologo /version:4.0 /subsystem:console /machine:I386 /OPT:NOREF /out:"..\..\3ds2lev.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "app3ds2lev - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\app3ds2lev\"
# PROP Intermediate_Dir "..\..\out\debug\build\app3ds2lev\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\app3ds2lev\app3ds2lev.res" /D "_DEBUG" /D "CS_DEBUG" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib lib3ds-120.lib  /nologo /version:4.0 /subsystem:console /machine:I386 /debug /pdbtype:sept /out:"..\..\3ds2lev.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "app3ds2lev - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\app3ds2lev\"
# PROP Intermediate_Dir "..\..\out\release\build\app3ds2lev\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\app3ds2lev\app3ds2lev.res" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib lib3ds-120.lib  /nologo /version:4.0 /subsystem:console /machine:I386 /OPT:NOREF /out:"..\..\3ds2lev.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "app3ds2lev - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\app3ds2lev\"
# PROP Intermediate_Dir "..\..\out\debug\build\app3ds2lev\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\app3ds2lev\app3ds2lev.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib lib3ds-120.lib  /nologo /version:4.0 /subsystem:console /machine:I386 /debug /pdbtype:sept /out:"..\..\3ds2lev.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "app3ds2lev - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 
# PROP BASE Output_Dir "memdebug"
# PROP BASE Intermediate_Dir "memdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 
# PROP Output_Dir "..\..\out\memdebug\build\app3ds2lev\"
# PROP Intermediate_Dir "..\..\out\memdebug\build\app3ds2lev\"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c  /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\memdebug\build\app3ds2lev\app3ds2lev.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib lib3ds-120.lib  /nologo /version:4.0 /subsystem:console /machine:I386  /out:"..\..\3ds2lev.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "app3ds2lev - Win32 ExtensiveMemDebug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 
# PROP BASE Output_Dir "memdebug"
# PROP BASE Intermediate_Dir "memdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 
# PROP Output_Dir "..\..\out\memdebug\build\app3ds2lev\"
# PROP Intermediate_Dir "..\..\out\memdebug\build\app3ds2lev\"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c  /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\memdebug\build\app3ds2lev\app3ds2lev.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_CONSOLE" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib lib3ds-120.lib  /nologo /version:4.0 /subsystem:console /machine:I386  /out:"..\..\3ds2lev.exe" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "app3ds2lev - Win32 Release"
# Name "app3ds2lev - Win32 Debug"
# Name "app3ds2lev - Win32 Release_NR"
# Name "app3ds2lev - Win32 Debug_NR"
# Name "app3ds2lev - Win32 ExtensiveMemDebug"
# Name "app3ds2lev - Win32 ExtensiveMemDebug_NR"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\apps\import\3ds2lev\levelwriter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\apps\import\3ds2lev\3ds2lev.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\apps\import\3ds2lev\levelwriter.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\msvc\app3ds2lev.rc
# End Source File
# End Group
# End Target
# End Project
