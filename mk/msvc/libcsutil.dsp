# Microsoft Developer Studio Project File - Name="libcsutil" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcsutil - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcsutil.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcsutil.mak" CFG="libcsutil - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcsutil - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsutil - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcsutil - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcsutil - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcsutil\"
# PROP Intermediate_Dir "..\..\out\release\build\libcsutil\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSUTIL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSUTIL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcsutil\libcsutil.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSUTIL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcsutil.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsutil - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsutil\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsutil\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSUTIL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSUTIL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsutil\libcsutil.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSUTIL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsutil_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcsutil - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcsutil\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcsutil\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSUTIL_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSUTIL_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcsutil\libcsutil.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CSUTIL_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcsutil_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcsutil - Win32 Release"
# Name "libcsutil - Win32 Debug"
# Name "libcsutil - Win32 ExtensiveMemDebug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csutil\ansiparse.cpp
# End Source File
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

SOURCE=..\..\libs\csutil\cseventflattener.cpp
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

SOURCE=..\..\libs\csutil\csuctransform.cpp
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

SOURCE=..\..\libs\csutil\floatrand.cpp
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

SOURCE=..\..\libs\csutil\inputdef.cpp
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

SOURCE=..\..\libs\csutil\parasiticdatabuffer.cpp
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

SOURCE=..\..\libs\csutil\profile.cpp
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

SOURCE=..\..\libs\csutil\staticvarcleanup.cpp
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

SOURCE=..\..\libs\csutil\threadjobqueue.cpp
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

SOURCE=..\..\libs\csutil\verbosity.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\vfscache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\virtclk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\callstack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\DbgHelpAPI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\dir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\expandname.cpp
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
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\ansicolor.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\ansiparse.h
# End Source File
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

SOURCE=..\..\libs\csutil\..\..\include\csutil\cseventflattener.h
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

SOURCE=..\..\libs\csutil\..\..\include\csutil\dirtyaccessarray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\event.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\evoutlet.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\fifo.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\flags.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\floatrand.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\formatter.h
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

SOURCE=..\..\libs\csutil\..\..\include\csutil\inputdef.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\leakguard.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\list.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\macosx\csosdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\macosx\OSXAssistant.h
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

SOURCE=..\..\libs\csutil\..\..\include\csutil\parasiticdatabuffer.h
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

SOURCE=..\..\libs\csutil\..\..\include\csutil\pooledscfclass.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\prfxcfg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\profile.h
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

SOURCE=..\..\libs\csutil\..\..\include\csutil\threadjobqueue.h
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

SOURCE=..\..\libs\csutil\..\..\include\csutil\verbosity.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\vfscache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\vfsplat.h
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

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\callstack.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\csconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\csosdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\DbgHelpAPI.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\minidump.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\..\..\include\csutil\win32\registrycfg.h
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

SOURCE=..\..\libs\csutil\cskey_identstrs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csucmappings.h
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

SOURCE=..\..\libs\csutil\win32\win32kbd_identstrs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\winthread.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\xmltinyp.h
# End Source File
# End Group
# End Target
# End Project
