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

SCF_USE_STATIC_PLUGIN(ball)
SCF_USE_STATIC_PLUGIN(ballldr)
SCF_USE_STATIC_PLUGIN(bezier)
SCF_USE_STATIC_PLUGIN(bezierldr)
SCF_USE_STATIC_PLUGIN(bruteblock)
SCF_USE_STATIC_PLUGIN(bugplug)
SCF_USE_STATIC_PLUGIN(crossbld)
SCF_USE_STATIC_PLUGIN(csbmpimg)
SCF_USE_STATIC_PLUGIN(csconin)
SCF_USE_STATIC_PLUGIN(csddsimg)
SCF_USE_STATIC_PLUGIN(csfont)
SCF_USE_STATIC_PLUGIN(csgifimg)
SCF_USE_STATIC_PLUGIN(csjpgimg)
SCF_USE_STATIC_PLUGIN(csparser)
SCF_USE_STATIC_PLUGIN(cspngimg)
SCF_USE_STATIC_PLUGIN(cssynldr)
SCF_USE_STATIC_PLUGIN(dynavis)
SCF_USE_STATIC_PLUGIN(emit)
SCF_USE_STATIC_PLUGIN(emitldr)
SCF_USE_STATIC_PLUGIN(engine)
SCF_USE_STATIC_PLUGIN(engseq)
SCF_USE_STATIC_PLUGIN(explo)
SCF_USE_STATIC_PLUGIN(exploldr)
SCF_USE_STATIC_PLUGIN(fire)
SCF_USE_STATIC_PLUGIN(fireldr)
SCF_USE_STATIC_PLUGIN(fountain)
SCF_USE_STATIC_PLUGIN(fountldr)
SCF_USE_STATIC_PLUGIN(frustvis)
SCF_USE_STATIC_PLUGIN(genmesh)
SCF_USE_STATIC_PLUGIN(gl3d)
SCF_USE_STATIC_PLUGIN(glshader_arb)
SCF_USE_STATIC_PLUGIN(glshader_fixed)
SCF_USE_STATIC_PLUGIN(gmeshanim)
SCF_USE_STATIC_PLUGIN(gmeshldr)
SCF_USE_STATIC_PLUGIN(gmeshskelanim)
SCF_USE_STATIC_PLUGIN(haze)
SCF_USE_STATIC_PLUGIN(hazeldr)
SCF_USE_STATIC_PLUGIN(ieplex)
SCF_USE_STATIC_PLUGIN(imgplex)
SCF_USE_STATIC_PLUGIN(nullmesh)
SCF_USE_STATIC_PLUGIN(opcode)
SCF_USE_STATIC_PLUGIN(protomesh)
SCF_USE_STATIC_PLUGIN(protomeshldr)
SCF_USE_STATIC_PLUGIN(rain)
SCF_USE_STATIC_PLUGIN(rainldr)
SCF_USE_STATIC_PLUGIN(rendloop_loader)
SCF_USE_STATIC_PLUGIN(rendstep_std)
SCF_USE_STATIC_PLUGIN(rendstep_stencil)
SCF_USE_STATIC_PLUGIN(reporter)
SCF_USE_STATIC_PLUGIN(sequence)
SCF_USE_STATIC_PLUGIN(shadermgr)
SCF_USE_STATIC_PLUGIN(simpcon)
SCF_USE_STATIC_PLUGIN(simpleformer)
SCF_USE_STATIC_PLUGIN(simpleformerldr)
SCF_USE_STATIC_PLUGIN(snow)
SCF_USE_STATIC_PLUGIN(snowldr)
SCF_USE_STATIC_PLUGIN(spiral)
SCF_USE_STATIC_PLUGIN(spirldr)
SCF_USE_STATIC_PLUGIN(spr2d)
SCF_USE_STATIC_PLUGIN(spr2dldr)
SCF_USE_STATIC_PLUGIN(spr3d)
SCF_USE_STATIC_PLUGIN(spr3dldr)
SCF_USE_STATIC_PLUGIN(starldr)
SCF_USE_STATIC_PLUGIN(stars)
SCF_USE_STATIC_PLUGIN(stdpt)
SCF_USE_STATIC_PLUGIN(stdrep)
SCF_USE_STATIC_PLUGIN(thing)
SCF_USE_STATIC_PLUGIN(thingldr)
SCF_USE_STATIC_PLUGIN(vfs)
SCF_USE_STATIC_PLUGIN(xmlread)
SCF_USE_STATIC_PLUGIN(xmlshader)
SCF_USE_STATIC_PLUGIN(glwin32)
