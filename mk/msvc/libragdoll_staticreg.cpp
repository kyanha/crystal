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
static char const metainfo_ragdoll[] =
"<?xml version=\"1.0\"?>"
"<!-- ragdoll.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>crystalspace.mesh.animesh.animnode.ragdoll</name>"
"        <implementation>RagdollNodeManager</implementation>"
"        <description>Crystal Space ragdoll animation node of an animated Mesh</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef RagdollNodeManager_FACTORY_REGISTER_DEFINED 
  #define RagdollNodeManager_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(RagdollNodeManager) 
  #endif

class ragdoll
{
SCF_REGISTER_STATIC_LIBRARY(ragdoll,metainfo_ragdoll)
  #ifndef RagdollNodeManager_FACTORY_REGISTERED 
  #define RagdollNodeManager_FACTORY_REGISTERED 
    RagdollNodeManager_StaticInit RagdollNodeManager_static_init__; 
  #endif
public:
 ragdoll();
};
ragdoll::ragdoll() {}

}