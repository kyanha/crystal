# Microsoft Developer Studio Project File - Name="plgaws" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plgaws - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plgaws.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plgaws.mak" CFG="plgaws - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plgaws - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgaws - Win32 Debug_NR" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgaws - Win32 Release_NR" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgaws - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgaws - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plgaws - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\plgaws"
# PROP Intermediate_Dir "csrelease\temp\plgaws"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_WINDOWS" /D "WIN32" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /FD /D "YY_NEVER_INTERACTIVE" /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csrelease\temp\plgaws\plgaws.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /dll /machine:I386 /out:"csrelease\temp\plgaws\aws.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath=".\csrelease\temp\plgaws\aws.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)" ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)" csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgaws - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\plgaws"
# PROP Intermediate_Dir "csdebug\temp\plgaws"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /FD /D "YY_NEVER_INTERACTIVE" /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csdebug\temp\plgaws\plgaws.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csdebug\temp\plgaws\aws.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csdebug\temp\plgaws\aws.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgaws - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csmemdbg"
# PROP BASE Intermediate_Dir "csmemdbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csmemdbg\temp\plgaws"
# PROP Intermediate_Dir "csmemdbg\temp\plgaws"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_EXTENSIVE_MEMDEBUG" /FD /D "YY_NEVER_INTERACTIVE" /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csmemdbg\temp\plgaws\plgaws.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csmemdbg\temp\plgaws\aws.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csmemdbg\temp\plgaws\aws.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csmemdbg\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgaws - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\plgaws"
# PROP Intermediate_Dir "csrelease\temp\plgaws"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_WINDOWS" /D "WIN32" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /D "YY_NEVER_INTERACTIVE" /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csrelease\temp\plgaws\plgaws.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /dll /machine:I386 /out:"csrelease\temp\plgaws\aws.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath=".\csrelease\temp\plgaws\aws.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)" ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)" csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgaws - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\plgaws"
# PROP Intermediate_Dir "csdebug\temp\plgaws"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /D "YY_NEVER_INTERACTIVE" /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csdebug\temp\plgaws\plgaws.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csdebug\temp\plgaws\aws.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csdebug\temp\plgaws\aws.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "plgaws - Win32 ExtensiveMemDebug"
# Name "plgaws - Win32 Debug"
# Name "plgaws - Win32 Release"
# Name "plgaws - Win32 Debug_NR"
# Name "plgaws - Win32 Release_NR"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\aws\aws.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\aws3dfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsControlBar.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsEngineView.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsMenu.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsPanel.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsTabCtrl.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsadler.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsbarct.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsbl.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awschkbx.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscmdbt.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscomp.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscscr.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsfparm.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgbc.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgbl.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgrpfr.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsimgvw.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awskcfct.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslabel.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslayot.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslstbx.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsmled.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsntbk.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsparser.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsprefs.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsradbt.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsscr.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsscrbr.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsslot.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsstbar.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsstdsk.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstex.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstimer.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstxtbx.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awswin.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\skinpars.hpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\slparse.h
# End Source File

# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\visualc6\plgaws.rc
# End Source File

# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\aws\aws.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\aws3dfrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsControlBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsEngineView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsPanel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsadler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsbarct.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsbl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awschkbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscmdbt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awscomp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsfparm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgbl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsgrpfr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsimgvw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awskcfct.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awskeycn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslabel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awslstbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsmled.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsntbk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsparser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsprefs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsradbt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsscr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsscrbr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsslot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsstbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awsstdsk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstimer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awstxtbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\awswin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\skinlex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\aws\skinpars.cpp
# End Source File

# End Group

# End Target
# End Project
