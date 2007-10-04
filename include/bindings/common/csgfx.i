%rename(asRGBcolor) csRGBpixel::operator csRGBcolor;
%include "csgfx/rgbpixel.h"
%ignore ShaderVarName;
%include "csgfx/shadervar.h"
ARRAY_CHANGE_ALL_TEMPLATE_PTR(csShaderVariable)

%template(csImageBaseBase) scfImplementation1<csImageBase, iImage>;
%include "csgfx/imagebase.h"
%template(csImageMemoryBase) scfImplementationExt0<csImageMemory, csImageBase>;
%include "csgfx/imagememory.h"

/* POST */
#ifndef SWIGIMPORTED
#undef APPLY_FOR_ALL_INTERFACES_POST
#define APPLY_FOR_ALL_INTERFACES_POST CSGFX_APPLY_FOR_EACH_INTERFACE
%include "bindings/common/basepost.i"
cs_lang_include(csgfxpost.i)
#endif
