# Microsoft Developer Studio Project File - Name="plgopcode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plgopcode - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "plgopcode.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "plgopcode.mak" CFG="plgopcode - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plgopcode - Win32 ExtensiveMemDebug_NR" (based on "Win32 (x86) Dynamic-Link Library")
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
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\plgopcode\"
# PROP Intermediate_Dir "..\..\out\release\build\plgopcode\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\plgopcode\plgopcode.res" /D "NDEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /machine:I386 /OPT:NOREF /dll /out:"..\..\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgopcode - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\plgopcode\"
# PROP Intermediate_Dir "..\..\out\debug\build\plgopcode\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\plgopcode\plgopcode.res" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /machine:I386 /debug /pdbtype:sept /dll /out:"..\..\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgopcode - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\plgopcode\"
# PROP Intermediate_Dir "..\..\out\release\build\plgopcode\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\plgopcode\plgopcode.res" /D "NDEBUG" /D "CS_USE_NEW_RENDERER" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /machine:I386 /OPT:NOREF /dll /out:"..\..\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgopcode - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\plgopcode\"
# PROP Intermediate_Dir "..\..\out\debug\build\plgopcode\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\plgopcode\plgopcode.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /machine:I386 /debug /pdbtype:sept /dll /out:"..\..\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgopcode - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 
# PROP BASE Output_Dir "memdebug"
# PROP BASE Intermediate_Dir "memdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 
# PROP Output_Dir "..\..\out\memdebug\build\plgopcode\"
# PROP Intermediate_Dir "..\..\out\memdebug\build\plgopcode\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c  /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\memdebug\build\plgopcode\plgopcode.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /machine:I386 /dll /out:"..\..\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "plgopcode - Win32 ExtensiveMemDebug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 
# PROP BASE Output_Dir "memdebug"
# PROP BASE Intermediate_Dir "memdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 
# PROP Output_Dir "..\..\out\memdebug\build\plgopcode\"
# PROP Intermediate_Dir "..\..\out\memdebug\build\plgopcode\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c  /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\memdebug\build\plgopcode\plgopcode.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_USE_NEW_RENDERER" /D "CS_EXTENSIVE_MEMDEBUG" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib delayimp.lib zlib.lib  /nologo /version:4.0 /subsystem:windows /machine:I386 /dll /out:"..\..\opcode.dll" /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "plgopcode - Win32 Release"
# Name "plgopcode - Win32 Debug"
# Name "plgopcode - Win32 Release_NR"
# Name "plgopcode - Win32 Debug_NR"
# Name "plgopcode - Win32 ExtensiveMemDebug"
# Name "plgopcode - Win32 ExtensiveMemDebug_NR"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\CSopcode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\CSopcodecollider.cpp
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
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\CSopcode.h
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\CSopcodecollider.h
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
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\msvc\plgopcode.rc
# End Source File
# Begin Source File

SOURCE=..\..\plugins\collide\opcode\opcode.csplugin
# End Source File
# End Group
# End Target
# End Project
