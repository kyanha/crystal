// This file is automatically generated.
#include "cssysdef.h"
#include "csutil/scf.h"

// Put static linking stuff into own section.
// The idea is that this allows the section to be swapped out but not
// swapped in again b/c something else in it was needed.
#if !defined(CS_DEBUG) && defined(CS_COMPILER_MSVC)
#pragma const_seg(".CSmetai")
#pragma comment(linker, "/section:.CSmetai,r")
#pragma code_seg(".CSmeta")
#pragma comment(linker, "/section:.CSmeta,er")
#pragma comment(linker, "/merge:.CSmetai=.CSmeta")
#endif

namespace csStaticPluginInit
{
static char const metainfo_sndds3d[] =
"<?xml version=\"1.0\"?>"
"<!-- sndds3d.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>crystalspace.sound.render.ds3d</name>"
"        <implementation>csSoundRenderDS3D</implementation>"
"        <description>Crystal Space DirectSound3D Sound Render</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef csSoundRenderDS3D_FACTORY_REGISTER_DEFINED 
  #define csSoundRenderDS3D_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(csSoundRenderDS3D) 
  #endif

class sndds3d
{
SCF_REGISTER_STATIC_LIBRARY(sndds3d,metainfo_sndds3d)
  #ifndef csSoundRenderDS3D_FACTORY_REGISTERED 
  #define csSoundRenderDS3D_FACTORY_REGISTERED 
    csSoundRenderDS3D_StaticInit csSoundRenderDS3D_static_init__; 
  #endif
public:
 sndds3d();
};
sndds3d::sndds3d() {}

}
