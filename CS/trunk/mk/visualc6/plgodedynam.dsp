# Microsoft Developer Studio Project File - Name="plgodedynam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plgodedynam - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plgodedynam.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plgodedynam.mak" CFG="plgodedynam - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plgodedynam - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgodedynam - Win32 Debug_NR" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgodedynam - Win32 Release_NR" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgodedynam - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgodedynam - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plgodedynam - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\plgodedynam"
# PROP Intermediate_Dir "csrelease\temp\plgodedynam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /FD /D "CS_USE_ODE_EXTERN_C" /I "..\.." /I "..\..\include" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csrelease\temp\plgodedynam\plgodedynam.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib ode.lib  /nologo /version:4.0 /subsystem:windows /dll /machine:I386 /out:"csrelease\temp\plgodedynam\odedynam.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath=".\csrelease\temp\plgodedynam\odedynam.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)" ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)" csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgodedynam - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\plgodedynam"
# PROP Intermediate_Dir "csdebug\temp\plgodedynam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /FD /D "CS_USE_ODE_EXTERN_C" /I "..\.." /I "..\..\include" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csdebug\temp\plgodedynam\plgodedynam.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib ode.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csdebug\temp\plgodedynam\odedynam.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csdebug\temp\plgodedynam\odedynam.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgodedynam - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csmemdbg"
# PROP BASE Intermediate_Dir "csmemdbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csmemdbg\temp\plgodedynam"
# PROP Intermediate_Dir "csmemdbg\temp\plgodedynam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_EXTENSIVE_MEMDEBUG" /FD /D "CS_USE_ODE_EXTERN_C" /I "..\.." /I "..\..\include" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csmemdbg\temp\plgodedynam\plgodedynam.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib ode.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csmemdbg\temp\plgodedynam\odedynam.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csmemdbg\temp\plgodedynam\odedynam.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csmemdbg\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgodedynam - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\plgodedynam"
# PROP Intermediate_Dir "csrelease\temp\plgodedynam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /D "CS_USE_ODE_EXTERN_C" /I "..\.." /I "..\..\include" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csrelease\temp\plgodedynam\plgodedynam.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib ode.lib  /nologo /version:4.0 /subsystem:windows /dll /machine:I386 /out:"csrelease\temp\plgodedynam\odedynam.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath=".\csrelease\temp\plgodedynam\odedynam.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)" ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)" csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgodedynam - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\plgodedynam"
# PROP Intermediate_Dir "csdebug\temp\plgodedynam"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /D "CS_USE_ODE_EXTERN_C" /I "..\.." /I "..\..\include" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csdebug\temp\plgodedynam\plgodedynam.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib ode.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csdebug\temp\plgodedynam\odedynam.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csdebug\temp\plgodedynam\odedynam.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "plgodedynam - Win32 ExtensiveMemDebug"
# Name "plgodedynam - Win32 Debug"
# Name "plgodedynam - Win32 Release"
# Name "plgodedynam - Win32 Debug_NR"
# Name "plgodedynam - Win32 Release_NR"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\physics\odedynam\odedynam.h
# End Source File

# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\visualc6\plgodedynam.rc
# End Source File
# Begin Source File

SOURCE=..\..\plugins\physics\odedynam\odedynam.csplugin
# End Source File

# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\physics\odedynam\odedynam.cpp
# End Source File

# End Group

# End Target
# End Project
