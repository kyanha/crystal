# Microsoft Developer Studio Project File - Name="plgopcode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plgopcode - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plgopcode.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plgopcode.mak" CFG="plgopcode - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plgopcode - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgopcode - Win32 Debug_NR" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgopcode - Win32 Release_NR" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgopcode - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plgopcode - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plgopcode - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\plgopcode"
# PROP Intermediate_Dir "csrelease\temp\plgopcode"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_WINDOWS" /D "WIN32" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csrelease\temp\plgopcode\plgopcode.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /dll /machine:I386 /out:"csrelease\temp\plgopcode\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath=".\csrelease\temp\plgopcode\opcode.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)" ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)" csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgopcode - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\plgopcode"
# PROP Intermediate_Dir "csdebug\temp\plgopcode"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csdebug\temp\plgopcode\plgopcode.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csdebug\temp\plgopcode\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csdebug\temp\plgopcode\opcode.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgopcode - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csmemdbg"
# PROP BASE Intermediate_Dir "csmemdbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csmemdbg\temp\plgopcode"
# PROP Intermediate_Dir "csmemdbg\temp\plgopcode"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_EXTENSIVE_MEMDEBUG" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csmemdbg\temp\plgopcode\plgopcode.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csmemdbg\temp\plgopcode\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csmemdbg\temp\plgopcode\opcode.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csmemdbg\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgopcode - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\temp\plgopcode"
# PROP Intermediate_Dir "csrelease\temp\plgopcode"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_WINDOWS" /D "WIN32" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csrelease\temp\plgopcode\plgopcode.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /dll /machine:I386 /out:"csrelease\temp\plgopcode\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs"  /OPT:NOREF
# Begin Special Build Tool
TargetPath=".\csrelease\temp\plgopcode\opcode.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)" ..\..	echo Moving output to MSVC Release Bin.	copy "$(TargetPath)" csrelease\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgopcode - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\temp\plgopcode"
# PROP Intermediate_Dir "csdebug\temp\plgopcode"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include" /fo".\csdebug\temp\plgopcode\plgopcode.res"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:1.0 /subsystem:windows /dll /profile /debug /machine:I386 /out:"csdebug\temp\plgopcode\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" /libpath:"..\..\libs\cssys\win32\libs" 
# SUBTRACT LINK32 /profile
# Begin Special Build Tool
TargetPath=".\csdebug\temp\plgopcode\opcode.dll"
SOURCE="$(InputPath)"
PostBuild_Cmds=echo Moving output to CS root.	copy "$(TargetPath)"  ..\..	echo Moving output to MSVC Debug Bin.	copy "$(TargetPath)"  csdebug\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "plgopcode - Win32 ExtensiveMemDebug"
# Name "plgopcode - Win32 Debug"
# Name "plgopcode - Win32 Release"
# Name "plgopcode - Win32 Debug_NR"
# Name "plgopcode - Win32 Release_NR"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\CSopcode.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\CSopcodecollider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceAABB.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceAxes.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceBoundingSphere.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceContainer.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceFPU.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceHPoint.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceIndexedTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceLSS.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceMatrix3x3.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceMatrix4x4.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceMemoryMacros.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceOBB.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IcePairs.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IcePlane.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IcePoint.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IcePreprocessor.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceRandom.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceRay.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceRevisitedRadix.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceSegment.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceTriangle.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceTrilist.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\Stdafx.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_AABBCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_AABBTree.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_BaseModel.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_BoxBoxOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_BoxPruning.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_Collider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_Common.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_HybridModel.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_IceHook.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_LSSAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_LSSCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_LSSTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_MeshInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_Model.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_OBBCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_OptimizedTree.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_Picking.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_PlanesAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_PlanesCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_PlanesTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_RayAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_RayCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_RayTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_Settings.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_SphereAABBOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_SphereCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_SphereTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_SweepAndPrune.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_TreeBuilders.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_TreeCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_TriBoxOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_TriTriOverlap.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_VolumeCollider.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Opcode.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Stdafx.h
# End Source File

# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\visualc6\plgopcode.rc
# End Source File

# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\CSopcode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\CSopcodecollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceAABB.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceContainer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceHPoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceIndexedTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceMatrix3x3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceMatrix4x4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceOBB.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IcePlane.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IcePoint.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceRandom.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceRay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceRevisitedRadix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceSegment.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceTriangle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Ice\IceUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_AABBCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_AABBTree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_BaseModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_BoxPruning.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_Collider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_Common.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_HybridModel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_LSSCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_MeshInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_Model.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_OBBCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_OptimizedTree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_Picking.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_PlanesCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_RayCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_SphereCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_SweepAndPrune.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_TreeBuilders.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_TreeCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\OPC_VolumeCollider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\Opcode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\StdAfx.cpp
# End Source File

# End Group

# End Target
# End Project
