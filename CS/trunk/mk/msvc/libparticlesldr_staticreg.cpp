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
static char const metainfo_particlesldr[] =
"<?xml version=\"1.0\"?>"
"<!-- particlesldr.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>crystalspace.mesh.loader.factory.particles</name>"
"        <implementation>csParticlesFactoryLoader</implementation>"
"        <description>Crystal Space Particles Factory Loader</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.mesh.loader.particles</name>"
"        <implementation>csParticlesObjectLoader</implementation>"
"        <description>Crystal Space Particles Mesh Loader</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.mesh.saver.factory.particles</name>"
"        <implementation>csParticlesFactorySaver</implementation>"
"        <description>Crystal Space Particles Factory Saver</description>"
"      </class>"
"      <class>"
"        <name>crystalspace.mesh.saver.particles</name>"
"        <implementation>csParticlesObjectSaver</implementation>"
"        <description>Crystal Space Particles Mesh Saver</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef csParticlesFactoryLoader_FACTORY_REGISTER_DEFINED 
  #define csParticlesFactoryLoader_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(csParticlesFactoryLoader) 
  #endif
  #ifndef csParticlesObjectLoader_FACTORY_REGISTER_DEFINED 
  #define csParticlesObjectLoader_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(csParticlesObjectLoader) 
  #endif
  #ifndef csParticlesFactorySaver_FACTORY_REGISTER_DEFINED 
  #define csParticlesFactorySaver_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(csParticlesFactorySaver) 
  #endif
  #ifndef csParticlesObjectSaver_FACTORY_REGISTER_DEFINED 
  #define csParticlesObjectSaver_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(csParticlesObjectSaver) 
  #endif

class particlesldr
{
SCF_REGISTER_STATIC_LIBRARY(particlesldr,metainfo_particlesldr)
  #ifndef csParticlesFactoryLoader_FACTORY_REGISTERED 
  #define csParticlesFactoryLoader_FACTORY_REGISTERED 
    csParticlesFactoryLoader_StaticInit csParticlesFactoryLoader_static_init__; 
  #endif
  #ifndef csParticlesObjectLoader_FACTORY_REGISTERED 
  #define csParticlesObjectLoader_FACTORY_REGISTERED 
    csParticlesObjectLoader_StaticInit csParticlesObjectLoader_static_init__; 
  #endif
  #ifndef csParticlesFactorySaver_FACTORY_REGISTERED 
  #define csParticlesFactorySaver_FACTORY_REGISTERED 
    csParticlesFactorySaver_StaticInit csParticlesFactorySaver_static_init__; 
  #endif
  #ifndef csParticlesObjectSaver_FACTORY_REGISTERED 
  #define csParticlesObjectSaver_FACTORY_REGISTERED 
    csParticlesObjectSaver_StaticInit csParticlesObjectSaver_static_init__; 
  #endif
public:
 particlesldr();
};
particlesldr::particlesldr() {}

}
