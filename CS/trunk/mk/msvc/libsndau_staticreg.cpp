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
static char const metainfo_sndau[] =
"<?xml version=\"1.0\"?>"
"<!-- sndau.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>crystalspace.sound.loader.au</name>"
"        <implementation>csSoundLoader_AU</implementation>"
"        <description>AU Sound Loader</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef csSoundLoader_AU_FACTORY_REGISTER_DEFINED 
  #define csSoundLoader_AU_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(csSoundLoader_AU) 
  #endif

class sndau
{
SCF_REGISTER_STATIC_LIBRARY(sndau,metainfo_sndau)
  #ifndef csSoundLoader_AU_FACTORY_REGISTERED 
  #define csSoundLoader_AU_FACTORY_REGISTERED 
    csSoundLoader_AU_StaticInit csSoundLoader_AU_static_init__; 
  #endif
public:
 sndau();
};
sndau::sndau() {}

}
