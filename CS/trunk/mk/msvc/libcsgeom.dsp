# Microsoft Developer Studio Project File - Name="libcsgeom" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcsgeom - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcsgeom.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcsgeom.mak" CFG="libcsgeom - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcsgeom - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsgeom - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsgeom - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcsgeom - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcsgeom\"
# PROP Intermediate_Dir "..\..\out\release\build\libcsgeom\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGEOM_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGEOM_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcsgeom\libcsgeom.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGEOM_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcsgeom.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsgeom - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsgeom\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsgeom\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGEOM_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGEOM_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsgeom\libcsgeom.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGEOM_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsgeom_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsgeom - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsgeom\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsgeom\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGEOM_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGEOM_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsgeom\libcsgeom.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSGEOM_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsgeom_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcsgeom - Win32 Release"
# Name "libcsgeom - Win32 Debug"
# Name "libcsgeom - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csgeom\box.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\bsptree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\chainhull2d.cpp
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

SOURCE=..\..\libs\csgeom\quaterni.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\solidspace.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\sphere.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\spline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\staticvarcleanup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\subrec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\subrec2.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\tcovbuf.cpp
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
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\box.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\bsptree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\chainhull2d.h
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

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\math.h
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

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\quaterni.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\segment.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\solidspace.h
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

SOURCE=..\..\libs\csgeom\..\..\include\csgeom\tcovbuf.h
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
# End Target
# End Project
