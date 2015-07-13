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
static char const metainfo_an_ikccd[] =
"<?xml version=\"1.0\"?>"
"<!-- ikccd.csplugin -->"
"<plugin>"
"  <scf>"
"    <classes>"
"      <class>"
"        <name>crystalspace.mesh.animesh.animnode.ik.ccd</name>"
"        <implementation>IKCCDNodeManager</implementation>"
"        <description>Cyclic Coordinate Descent implementation of the inverse kinematics animation node</description>"
"      </class>"
"    </classes>"
"  </scf>"
"</plugin>"
;
  #ifndef IKCCDNodeManager_FACTORY_REGISTER_DEFINED 
  #define IKCCDNodeManager_FACTORY_REGISTER_DEFINED 
    SCF_DEFINE_FACTORY_FUNC_REGISTRATION(IKCCDNodeManager) 
  #endif

class an_ikccd
{
SCF_REGISTER_STATIC_LIBRARY(an_ikccd,metainfo_an_ikccd)
  #ifndef IKCCDNodeManager_FACTORY_REGISTERED 
  #define IKCCDNodeManager_FACTORY_REGISTERED 
    IKCCDNodeManager_StaticInit IKCCDNodeManager_static_init__; 
  #endif
public:
 an_ikccd();
};
an_ikccd::an_ikccd() {}

}