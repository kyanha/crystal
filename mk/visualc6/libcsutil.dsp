# Microsoft Developer Studio Project File - Name="libcsutil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcsutil - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcsutil.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcsutil.mak" CFG="libcsutil - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcsutil - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsutil - Win32 Debug_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsutil - Win32 Release_NR" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsutil - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsutil - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcsutil - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\bin\libs"
# PROP Intermediate_Dir "csrelease\temp\libcsutil"
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

!ELSEIF  "$(CFG)" == "libcsutil - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\bin\libs"
# PROP Intermediate_Dir "csdebug\temp\libcsutil"
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
# ADD LIB32 /nologo /out:"csdebug\bin\libs\libcsutil_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsutil - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csmemdbg"
# PROP BASE Intermediate_Dir "csmemdbg"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csmemdbg\bin\libs"
# PROP Intermediate_Dir "csmemdbg\temp\libcsutil"
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
# ADD LIB32 /nologo /out:"csmemdbg\bin\libs\libcsutil_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsutil - Win32 Release_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "csrelease"
# PROP BASE Intermediate_Dir "csrelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "csrelease\bin\libs"
# PROP Intermediate_Dir "csrelease\temp\libcsutil"
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

!ELSEIF  "$(CFG)" == "libcsutil - Win32 Debug_NR"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "csdebug"
# PROP BASE Intermediate_Dir "csdebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "csdebug\bin\libs"
# PROP Intermediate_Dir "csdebug\temp\libcsutil"
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
# ADD LIB32 /nologo /out:"csdebug\bin\libs\libcsutil_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo File is a lib, Copy skipped.
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcsutil - Win32 ExtensiveMemDebug"
# Name "libcsutil - Win32 Debug"
# Name "libcsutil - Win32 Release"
# Name "libcsutil - Win32 Debug_NR"
# Name "libcsutil - Win32 Release_NR"
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\archive.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\array.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\binder.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\bintree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\bitarray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\blockallocator.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\callstack.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\cfgacc.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\cfgdoc.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\cfgfile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\cfgmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\cmdhelp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\cmdline.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csbaseeventh.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\cscolor.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csendian.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csevcord.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csevent.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\cseventq.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csinput.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csmd5.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csmmap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csobject.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csosdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\cspmeter.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csppulse.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csprocessorcap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csshlib.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csstring.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csuctransform.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\csunicode.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\databuf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\datastrm.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\event.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\evoutlet.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\flags.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\fpu80x86.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\garray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\getopt.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\hash.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\hashhandlers.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\hashmap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\hashmapr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\hashr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\indprint.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\inpnames.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\list.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\macosx\OSXAssistant.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\macosx\csosdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\memdebug.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\memfile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\mmapio.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\nobjvec.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\nulcache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\objiter.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\objpool.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\objreg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\parray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\physfile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\plugldr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\plugmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\prfxcfg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\radixsort.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\randomgen.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\ref.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\refarr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\refcount.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\reftrackeraccess.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\regexp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\rng.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\scanstr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\scf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\scfstr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\scfstringarray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\scfstrset.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\schedule.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\scopedmutexlock.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\snprintf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\sockets.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\sparse3d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\strhash.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\stringarray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\stringreader.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\strset.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\sysfunc.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\syspath.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\tree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\unix\csosdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\util.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\utiltest.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\vfscache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\virtclk.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\weakref.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\weakrefarr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\DbgHelpAPI.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\callstack.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\csosdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\minidump.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\registrycfg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\volatile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\win32.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\wintools.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\xmltiny.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\zip.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\reftrack.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\regex_wrapper.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\tinystr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\tinywrap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\tinyxml.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\cachedll.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\callstack.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\shellstuff.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\win32kbd.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\winthread.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\xmltinyp.h
# End Source File

# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csutil\archive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\binder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\cfgacc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\cfgdoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\cfgfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\cfgmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\cmdhelp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\cmdline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csbaseeventh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csevcord.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csevent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\cseventq.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csinput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csmd5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csobject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\cspmeter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csppulse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csstring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\databuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\datastrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\event.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\evoutlet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\generic\appdir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\generic\csprocessorcap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\generic\findlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\generic\getopt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\generic\pluginpaths.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\generic\regex.c
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\generic\resdir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\generic\runloop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\hash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\hashmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\hashmapr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\inpnames.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\memdebug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\memfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\mmapio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\nulcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\objiter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\objreg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\physfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\plugldr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\plugmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\prfxcfg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\radixsort.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\randomgen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\reftrack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\reftrackeraccess.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\regexp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\scanstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\scf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\scfimp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\scfstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\scfstringarray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\scfstrset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\schedule.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\snprintf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\sparse3d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\statvar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\strhash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\stringreader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\strset.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\tinypars.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\util.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\utiltest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\vfscache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\virtclk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\DbgHelpAPI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\callstack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\dir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\filesysconfig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\instpath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\loadlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\minidump.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\mmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\registrycfg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\sysroot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\timing32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\username.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\vfsplat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\win32kbd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\winprintf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\winsyspath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\winthread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\wintools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\xmltiny.cpp
# End Source File

# End Group

# End Target
# End Project
