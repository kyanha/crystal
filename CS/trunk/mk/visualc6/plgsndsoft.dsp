# Microsoft Developer Studio Project File - Name="plgsndsoft" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plgsndsoft - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plgsndsoft.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plgsndsoft.mak" CFG="plgsndsoft - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plgsndsoft - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgsndsoft - Win32 Debug_NR" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgsndsoft - Win32 Release_NR" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgsndsoft - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgsndsoft - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plgsndsoft - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\plgsndsoft"
# PROP Intermediate_Dir "csrelease\temp\plgsndsoft"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_WINDOWS" /D "WIN32" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csrelease\temp\plgsndsoft\plgsndsoft.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /dll /machine:I386 /out:"csrelease\temp\plgsndsoft\sndsoft.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath=".\csrelease\temp\plgsndsoft\sndsoft.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)" ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)" csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgsndsoft - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\plgsndsoft"
# PROP Intermediate_Dir "csdebug\temp\plgsndsoft"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csdebug\temp\plgsndsoft\plgsndsoft.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csdebug\temp\plgsndsoft\sndsoft.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csdebug\temp\plgsndsoft\sndsoft.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgsndsoft - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csmemdbg"
# PROP BASE Intermediate_Dir "csmemdbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csmemdbg\temp\plgsndsoft"
# PROP Intermediate_Dir "csmemdbg\temp\plgsndsoft"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_EXTENSIVE_MEMDEBUG" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csmemdbg\temp\plgsndsoft\plgsndsoft.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csmemdbg\temp\plgsndsoft\sndsoft.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csmemdbg\temp\plgsndsoft\sndsoft.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csmemdbg\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgsndsoft - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\plgsndsoft"
# PROP Intermediate_Dir "csrelease\temp\plgsndsoft"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_WINDOWS" /D "WIN32" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csrelease\temp\plgsndsoft\plgsndsoft.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /dll /machine:I386 /out:"csrelease\temp\plgsndsoft\sndsoft.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath=".\csrelease\temp\plgsndsoft\sndsoft.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)" ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)" csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgsndsoft - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\plgsndsoft"
# PROP Intermediate_Dir "csdebug\temp\plgsndsoft"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csdebug\temp\plgsndsoft\plgsndsoft.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csdebug\temp\plgsndsoft\sndsoft.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csdebug\temp\plgsndsoft\sndsoft.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "plgsndsoft - Win32 ExtensiveMemDebug"
# Name "plgsndsoft - Win32 Debug"
# Name "plgsndsoft - Win32 Release"
# Name "plgsndsoft - Win32 Debug_NR"
# Name "plgsndsoft - Win32 Release_NR"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\sound\renderer\software\sndhdl.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\sound\renderer\software\srdrcom.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\sound\renderer\software\srdrsrc.h
# End Source File

# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\visualc6\plgsndsoft.rc
# End Source File

# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\sound\renderer\software\sndhdl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\sound\renderer\software\srdrcom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\sound\renderer\software\srdrsrc.cpp
# End Source File

# End Group

# End Target
# End Project
