# Microsoft Developer Studio Project File - Name="appvsh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=appvsh - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "appvsh.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "appvsh.mak" CFG="appvsh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "appvsh - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Console Application")
!MESSAGE "appvsh - Win32 Debug_NR" (based on "Win32 (x86) Console Application")
!MESSAGE "appvsh - Win32 Release_NR" (based on "Win32 (x86) Console Application")
!MESSAGE "appvsh - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "appvsh - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "appvsh - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\appvsh"
# PROP Intermediate_Dir "csrelease\temp\appvsh"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /nologo /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\.." /I "..\..\include\csutil\win32" /I "..\..\include\cssys\win32" /I "..\..\include" /I "..\..\libs" /I "..\..\support" /I "..\..\apps" /I "..\..\plugins" /D "NDEBUG" /D "_MT" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /FD  /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include\cssys\win32" /i "..\..\include" /fo".\csrelease\temp\appvsh\appvsh.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib gdi32.lib user32.lib advapi32.lib zlib.lib /nologo /subsystem:console /machine:I386 /out:"csrelease\temp\appvsh\vsh.exe" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath".\csrelease\temp\appvsh\vsh.exe"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)"  csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appvsh - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\appvsh"
# PROP Intermediate_Dir "csdebug\temp\appvsh"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\plugins" /I "..\.." /I "..\..\include\csutil\win32" /I "..\..\include\cssys\win32" /I "..\..\include" /I "..\..\libs" /I "..\..\support" /I "..\..\apps" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /FD  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include\cssys\win32" /i "..\..\include" /fo".\csdebug\temp\appvsh\appvsh.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib gdi32.lib user32.lib advapi32.lib zlib.lib /nologo /subsystem:console /debug /machine:I386 /out:"csdebug\temp\appvsh\vsh.exe" /pdbtype:sept /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# Begin Special Build Tool
TargetPath=".\csdebug\temp\appvsh\vsh.exe"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appvsh - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csmemdbg"
# PROP BASE Intermediate_Dir "csmemdbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csmemdbg\temp\appvsh"
# PROP Intermediate_Dir "csmemdbg\temp\appvsh"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\plugins" /I "..\.." /I "..\..\include\csutil\win32" /I "..\..\include\cssys\win32" /I "..\..\include" /I "..\..\libs" /I "..\..\support" /I "..\..\apps" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_EXTENSIVE_MEMDEBUG" /FD  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include\cssys\win32" /i "..\..\include" /fo".\csmemdbg\temp\appvsh\appvsh.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib gdi32.lib user32.lib advapi32.lib zlib.lib /nologo /subsystem:console /debug /machine:I386 /out:"csmemdbg\temp\appvsh\vsh.exe" /pdbtype:sept /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# Begin Special Build Tool
TargetPath=".\csmemdbg\temp\appvsh\vsh.exe"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csmemdbg\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appvsh - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\appvsh"
# PROP Intermediate_Dir "csrelease\temp\appvsh"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /nologo /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\.." /I "..\..\include\csutil\win32" /I "..\..\include\cssys\win32" /I "..\..\include" /I "..\..\libs" /I "..\..\support" /I "..\..\apps" /I "..\..\plugins" /D "NDEBUG" /D "_MT" /D "_CONSOLE" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD  /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include\cssys\win32" /i "..\..\include" /fo".\csrelease\temp\appvsh\appvsh.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 shell32.lib gdi32.lib user32.lib advapi32.lib zlib.lib /nologo /subsystem:console /machine:I386 /out:"csrelease\temp\appvsh\vsh.exe" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath".\csrelease\temp\appvsh\vsh.exe"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)"  csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appvsh - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\appvsh"
# PROP Intermediate_Dir "csdebug\temp\appvsh"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\plugins" /I "..\.." /I "..\..\include\csutil\win32" /I "..\..\include\cssys\win32" /I "..\..\include" /I "..\..\libs" /I "..\..\support" /I "..\..\apps" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include\cssys\win32" /i "..\..\include" /fo".\csdebug\temp\appvsh\appvsh.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib advapi32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib gdi32.lib user32.lib advapi32.lib zlib.lib /nologo /subsystem:console /debug /machine:I386 /out:"csdebug\temp\appvsh\vsh.exe" /pdbtype:sept /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# Begin Special Build Tool
TargetPath=".\csdebug\temp\appvsh\vsh.exe"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "appvsh - Win32 ExtensiveMemDebug"
# Name "appvsh - Win32 Debug"
# Name "appvsh - Win32 Release"
# Name "appvsh - Win32 Debug_NR"
# Name "appvsh - Win32 Release_NR"
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\visualc6\appvsh.rc
# End Source File

# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\apps\tools\vsh\vsh.cpp
# End Source File

# End Group

# End Target
# End Project
