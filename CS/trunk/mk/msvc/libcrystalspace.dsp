# Microsoft Developer Studio Project File - Name="libcrystalspace" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libcrystalspace - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libcrystalspace.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libcrystalspace.mak" CFG="libcrystalspace - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libcrystalspace - Win32 ExtensiveMemDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcrystalspace - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libcrystalspace - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libcrystalspace - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libcrystalspace\"
# PROP Intermediate_Dir "..\..\out\release\build\libcrystalspace\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CRYSTALSPACE_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CRYSTALSPACE_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libcrystalspace\libcrystalspace.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CRYSTALSPACE_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libcrystalspace.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcrystalspace - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcrystalspace\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcrystalspace\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CRYSTALSPACE_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CRYSTALSPACE_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcrystalspace\libcrystalspace.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CRYSTALSPACE_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcrystalspace_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libcrystalspace - Win32 ExtensiveMemDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libcrystalspace\"
# PROP Intermediate_Dir "..\..\out\debug\build\libcrystalspace\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CRYSTALSPACE_LIB" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\include" /I "..\..\include\csutil\win32"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CRYSTALSPACE_LIB"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libcrystalspace\libcrystalspace.res" /D "_DEBUG" /D "CS_DEBUG" /D "CS_EXTENSIVE_MEMDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_CRYSTALSPACE_LIB" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\include" /i "..\..\include\csutil\win32"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libcrystalspace_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib zlib.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows /libpath:"..\..\libs\csutil\win32\libs" 
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libcrystalspace - Win32 Release"
# Name "libcrystalspace - Win32 Debug"
# Name "libcrystalspace - Win32 ExtensiveMemDebug"
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

SOURCE=..\..\libs\csgeom\csrect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\csrectrg.cpp
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

SOURCE=..\..\libs\csgeom\tritri.cpp
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
# Begin Source File

SOURCE=..\..\libs\csgfx\bakekeycolor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\csimgvec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\gradient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\imagecubemapmaker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\imagemanipulate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\imagetools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\imagevolumemaker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\inv_cmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\lightsvcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\memimage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\normalmaptools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\quantize.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\renderbuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\shaderexp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\shaderexpaccessor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\shadervar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\shadervarcontext.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\shadervarframeholder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgfx\xorpat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\cursorconvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\fontcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\graph2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\scrshot.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\softfontcache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\canvas\x11-keys.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\imageloader\commonimagefile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\imageloader\optionsparser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\particlesys\partgen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\particlesys\particle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\render3d\normalizationcube.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\render3d\txtmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\renderstep\basesteploader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\renderstep\basesteptype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\renderstep\parserenderstep.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\shader\shaderprogram.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\sndsys\convert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\sndsys\cyclicbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\soundloader\sndload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\soundloader\soundraw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\soundrenderer\shdl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csplugincommon\soundrenderer\slstn.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\basetexfact.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\bitmasktostr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\collider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\csanim2d.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\csapplicationframework.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\csfxscr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\cspixmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\csview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\enginetools.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\gentrtex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\importkit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\importkit_glue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\initapp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\keyval.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\mapnode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\mdldata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\mdltool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\meshobjtmpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\pen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\proctex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\proctxtanim.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\rendermeshholder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\rendermeshlist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\sndwrap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\sprbuild.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\userrndbuf.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\vidprefs.cpp
# End Source File
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

SOURCE=..\..\libs\csutil\callstack.cpp
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

SOURCE=..\..\libs\csutil\csassert.cpp
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

SOURCE=..\..\libs\csutil\documentcommon.cpp
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

SOURCE=..\..\libs\csutil\inputdef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\memdebug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\memfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\mempool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\mmapio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\nulcache.cpp
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

SOURCE=..\..\libs\csutil\syspath.cpp
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

SOURCE=..\..\libs\csutil\verbosity.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\vfscache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\virtclk.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\callstack-dbghelp.cpp
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

SOURCE=..\..\libs\csutil\win32\setenv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\syminit.cpp
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

SOURCE=..\..\libs\..\include\csgeom\box.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\bsptree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\chainhull2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\cspoint.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\csrect.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\csrectrg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\fastsqrt.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\frustum.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\kdtree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\math.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\math2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\math3d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\math3d_d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\matrix2.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\matrix3.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\obb.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\objmodel.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\path.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\plane2.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\plane3.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\pmtools.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\poly2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\poly3d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\polyaa.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\polyclip.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\polyedge.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\polyidx.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\polymesh.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\polypool.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\quaterni.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\segment.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\solidspace.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\sphere.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\spline.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\subrec.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\subrec2.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\tcovbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\textrans.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\transfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\tri.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\trimesh.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\trimeshlod.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\vector2.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\vector3.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgeom\vector4.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\bakekeycolor.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\csimgvec.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\gradient.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\imagebase.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\imagecubemapmaker.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\imagemanipulate.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\imagetools.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\imagevolumemaker.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\inv_cmap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\lightsvcache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\memimage.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\normalmaptools.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\packrgb.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\quantize.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\renderbuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\rgbpixel.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\shaderexp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\shaderexpaccessor.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\shadervar.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\shadervarblockalloc.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\shadervarcontext.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\shadervarframeholder.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\vertexlight.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\vertexlistwalker.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csgfx\xorpat.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\cursorconvert.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\draw_box.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\draw_common.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\draw_line.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\draw_text.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\fontcache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\graph2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\scancode.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\scrshot.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\softfontcache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\canvas\softfontcacheimpl.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\imageloader\commonimagefile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\imageloader\optionsparser.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\particlesys\partgen.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\particlesys\particle.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\render3d\normalizationcube.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\render3d\pixfmt.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\render3d\txtmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\renderstep\basesteploader.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\renderstep\basesteptype.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\renderstep\parserenderstep.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\shader\shaderplugin.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\shader\shaderprogram.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\sndsys\convert.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\sndsys\cyclicbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\soundloader\sndload.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\soundloader\soundraw.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\soundrenderer\shdl.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csplugincommon\soundrenderer\slstn.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\basetexfact.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\bitmasktostr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\collider.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\csanim2d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\csapplicationframework.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\csfxscr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\cspixmap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\csview.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\debugimagewriter.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\enginetools.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\fogmath.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\framedataholder.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\gentrtex.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\identstrings.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\importkit.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\initapp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\keyval.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\mapnode.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\mdldata.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\mdltool.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\meshobjtmpl.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\pen.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\proctex.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\proctxtanim.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\rbuflock.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\rendermeshholder.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\rendermeshlist.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\sndwrap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\sprbuild.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\tokenlist.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\userrndbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\vfsdirchange.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\cstool\vidprefs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\algorithms.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\ansicolor.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\ansiparse.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\archive.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\array.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\binder.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\bintree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\bitarray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\blockallocator.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\callstack.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cfgacc.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cfgdoc.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cfgfile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cfgmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cmdhelp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cmdline.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\comparator.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csbaseeventh.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cscolor.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csendian.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csevcord.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csevent.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cseventflattener.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cseventq.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csinput.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csmd5.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csobject.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csosdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\cspmeter.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csppulse.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csprocessorcap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csshlib.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csstring.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csuctransform.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\csunicode.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\databuf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\datastrm.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\dirtyaccessarray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\documentcommon.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\documenthelper.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\event.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\evoutlet.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\fifo.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\flags.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\floatrand.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\formatter.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\fpu80x86.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\garray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\getopt.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\hash.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\hashhandlers.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\hashr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\inputdef.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\leakguard.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\list.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\memdebug.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\memfile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\mempool.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\mmap_dummy.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\mmap_posix.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\mmapio.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\nobjvec.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\nulcache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\objiter.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\objpool.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\objreg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\parasiticdatabuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\parray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\partialorder.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\physfile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\plugldr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\plugmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\pooledscfclass.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\prfxcfg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\profile.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\radixsort.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\randomgen.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\redblacktree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\ref.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\refarr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\refcount.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\reftrackeraccess.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\regexp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scanstr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scf_impl.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scf_implementation.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scf_implgen.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scf_interface.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scfstr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scfstringarray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scfstrset.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\schedule.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\scopedmutexlock.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\set.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\snprintf.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\sparse3d.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\strhash.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\stringarray.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\stringreader.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\strset.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\sysfunc.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\syspath.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\threadjobqueue.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\timemeasure.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\tree.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\util.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\verbosity.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\vfscache.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\vfsplat.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\virtclk.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\weakref.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\weakrefarr.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\callstack.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\csconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\csosdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\DbgHelpAPI.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\minidump.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\mmap.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\psdk-compat.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\registrycfg.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\win32.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\win32\wintools.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\xmltiny.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\..\include\csutil\zip.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csgeom\obb_priv.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\cstool\importkit_glue.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\callstack.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\cskey_identstrs.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\csucmappings.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\demangle.h
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

SOURCE=..\..\libs\csutil\win32\callstack-dbghelp.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\shellstuff.h
# End Source File
# Begin Source File

SOURCE=..\..\libs\csutil\win32\syminit.h
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
