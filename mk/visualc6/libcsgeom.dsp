# Microsoft Developer Studio Project File - Name="libcsgeom" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcsgeom - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcsgeom.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcsgeom.mak" CFG="libcsgeom - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcsgeom - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsgeom - Win32 Debug_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsgeom - Win32 Release_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsgeom - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsgeom - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcsgeom - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\bin\libs"
# PROP Intermediate_Dir "csrelease\temp\libcsgeom"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsgeom - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\bin\libs"
# PROP Intermediate_Dir "csdebug\temp\libcsgeom"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"csdebug\bin\libs\libcsgeom_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsgeom - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csmemdbg"
# PROP BASE Intermediate_Dir "csmemdbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csmemdbg\bin\libs"
# PROP Intermediate_Dir "csmemdbg\temp\libcsgeom"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_EXTENSIVE_MEMDEBUG" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"csmemdbg\bin\libs\libcsgeom_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsgeom - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\bin\libs"
# PROP Intermediate_Dir "csrelease\temp\libcsgeom"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /Gy /GF /W3 /Gm /G5 /Ob2 /Og /Oi /Ot /Oy /D "WIN32" /D "NDEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /Gy /GF /MD /W3 /Gm /GX /G5 /Ob2 /Og /Oi /Ot /Oy /I "..\..\include\csutil\win32" /D "NDEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" 
# ADD RSC /l 0x409 /d "NDEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsgeom - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\bin\libs"
# PROP Intermediate_Dir "csdebug\temp\libcsgeom"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /vmb /vms /MDd /W3 /Gm /GR /GX /G5 /ZI /Od /I "..\..\include\csutil\win32" /D "_DEBUG" /D "_MT" /D "_LIB" /D "WIN32" /D "_WINDOWS" /D "WIN32_VOLATILE" /D "__CRYSTAL_SPACE__" /D "CS_DEBUG" /D "CS_STRICT_SMART_POINTERS" /D "CS_USE_NEW_RENDERER" /FD /I "..\.." /I "..\..\include" /I "..\..\plugins" /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" 
# ADD RSC /l 0x409 /d "_DEBUG" /d "CS_DEBUG" /i "..\..\include\csutil\win32" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"csdebug\bin\libs\libcsgeom_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcsgeom - Win32 ExtensiveMemDebug"
# Name "libcsgeom - Win32 Debug"
# Name "libcsgeom - Win32 Release"
# Name "libcsgeom - Win32 Debug_NR"
# Name "libcsgeom - Win32 Release_NR"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\box.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\bsptree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\crysball.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\cspoint.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\csrect.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\csrectrg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\fastsqrt.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\frustum.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\kdtree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\math2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\math3d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\math3d_d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\matrix2.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\matrix3.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\obb.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\objmodel.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\path.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\plane2.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\plane3.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\pmtools.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\poly2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\poly3d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\polyaa.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\polyclip.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\polyedge.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\polyidx.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\polymesh.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\polypool.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\polytree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\quaterni.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\segment.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\sphere.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\spline.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\subrec.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\subrec2.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\textrans.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\transfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\tri.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\trimesh.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\trimeshlod.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\vector2.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\vector3.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\vector4.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\obb_priv.h
# End Source File

# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csgeom\box.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\bsptree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\crysball.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\csrect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\csrectrg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\fastsqrt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\frustum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\kdtree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\math2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\math3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\math3d_d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\matrix2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\matrix3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\obb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\path.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\plane3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\pmtools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\poly2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\poly3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\polyaa.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\polyclip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\polyedge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\polyidx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\polymesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\polytree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\quaterni.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\sphere.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\spline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\subrec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\subrec2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\textrans.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\transfrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\trimesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\trimeshlod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\vector2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\vector3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\vector4.cpp
# End Source File

# End Group

# End Target
# End Project
