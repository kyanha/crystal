/*
    Copyright (C) 1998 by Jorrit Tyberghein
    Based on CSScript module created by Brandon Ehle
    Copyright (C) 2002 by W.C.A. Wijngaards
  
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
  
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/**\file
 * The queen of all include files.  Good for being a precompiled header, by
 * utilizing the NO_* macros, while stop a complete build
 */

#ifndef __CS_CSS_H__
#define __CS_CSS_H__

#ifndef CS_INCLUDE_NOTHING

// SysDef
#ifndef NO_CSSYSDEF
// make it safe so no double cssysdef.h includes
#ifndef __CS_CSSYSDEFS_H__

#define CS_SYSDEF_PROVIDE_MKDIR
#define CS_SYSDEF_PROVIDE_GETCWD
#define CS_SYSDEF_PROVIDE_TEMP
#define CS_SYSDEF_PROVIDE_DIR
#define CS_SYSDEF_PROVIDE_UNLINK
#define CS_SYSDEF_PROVIDE_ACCESS
#define CS_SYSDEF_PROVIDE_GETOPT
#define CS_SYSDEF_PROVIDE_SELECT
#include "cssysdef.h"
#endif
#endif 

// CS Version
#ifndef NO_CSVER
#include "csver.h"
#endif

// quick int and sqrt
#ifndef NO_CSQINTQSQRT
#include "qint.h"
#include "qsqrt.h"
#endif

// SCF
#ifndef NO_CSSCF
#include "csutil/scf.h"
#endif

// CS Sys
#ifndef NO_CSSYS
#include "cssys/csendian.h"
#include "cssys/csshlib.h"
#include "cssys/getopt.h"
#include "cssys/sysfunc.h"
#include "cssys/sockets.h"
#endif

// CS Interfaces
#ifndef NO_CSINTERFACE
#include "iaws/aws.h"
#include "iaws/awscnvs.h"
#include "iaws/awsdefs.h"
//#include "iaws/awsecomp.h"
#include "iaws/awsparm.h"
#include "iengine/camera.h"
#include "iengine/campos.h"
#include "iengine/collectn.h"
#include "iengine/dynlight.h"
#include "iengine/engine.h"
#include "iengine/fview.h"
#include "iengine/halo.h"
#include "iengine/light.h"
#include "iengine/lod.h"
#include "iengine/material.h"
#include "iengine/mesh.h"
#include "iengine/motion.h"
#include "iengine/movable.h"
#include "iengine/region.h"
#include "iengine/rview.h"
#include "iengine/sector.h"
#include "iengine/shadcast.h"
#include "iengine/shadows.h"
#include "iengine/skelbone.h"
#include "iengine/statlght.h"
#include "iengine/texture.h"
#include "iengine/viscull.h"
#include "iengine/portal.h"
#include "igeom/clip2d.h"
#include "igeom/polymesh.h"
#include "igeom/objmodel.h"
#include "igraphic/image.h"
#include "igraphic/imageio.h"
#include "imap/parser.h"
#include "imap/reader.h"
#include "imap/services.h"
#include "imap/writer.h"
#include "imesh/ball.h"
#include "imesh/crossbld.h"
#include "imesh/emit.h"
#include "imesh/explode.h"
#include "imesh/fire.h"
#include "imesh/fountain.h"
#include "imesh/genmesh.h"
#include "imesh/haze.h"
#include "imesh/lighting.h"
#include "imesh/mdlconv.h"
#include "imesh/mdldata.h"
#include "imesh/metaball.h"
#include "imesh/metagen.h"
#include "imesh/object.h"
#include "imesh/particle.h"
#include "imesh/partsys.h"
#include "imesh/rain.h"
#include "imesh/skeleton.h"
#include "imesh/snow.h"
#include "imesh/spiral.h"
#include "imesh/sprite2d.h"
#include "imesh/sprite3d.h"
#include "imesh/stars.h"
#include "imesh/terrfunc.h"
#include "imesh/bezier.h"
#include "imesh/thing/lightmap.h"
#include "imesh/thing/polygon.h"
#include "imesh/thing/thing.h"
#include "inetwork/driver.h"
#include "inetwork/socket.h"
#include "isound/data.h"
#include "isound/driver.h"
#include "isound/handle.h"
#include "isound/listener.h"
#include "isound/loader.h"
#include "isound/renderer.h"
#include "isound/source.h"
#include "isound/wrapper.h"
#include "iutil/cfgfile.h"
#include "iutil/cfgmgr.h"
#include "iutil/cmdline.h"
#include "iutil/comp.h"
#include "iutil/config.h"
#include "iutil/csinput.h"
#include "iutil/databuff.h"
#include "iutil/evdefs.h"
#include "iutil/event.h"
#include "iutil/eventh.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "iutil/objref.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"
#include "iutil/string.h"
#include "iutil/strvec.h"
#include "iutil/vfs.h"
#include "iutil/virtclk.h"
#include "ivaria/collider.h"
#include "ivaria/conin.h"
#include "ivaria/conout.h"
#include "ivaria/iso.h"
#include "ivaria/keyval.h"
#include "ivaria/mapnode.h"
#include "ivaria/perfstat.h"
#include "ivaria/pmeter.h"
#include "ivaria/reporter.h"
#include "ivaria/script.h"
#include "ivaria/sequence.h"
#include "ivaria/stdrep.h"
#include "ivaria/view.h"
#include "ivideo/codec.h"
#include "ivideo/cursor.h"
#include "ivideo/fontserv.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/halo.h"
#include "ivideo/material.h"
#include "ivideo/natwin.h"
#include "ivideo/texture.h"
#include "ivideo/txtmgr.h"
#include "ivideo/vbufmgr.h"
//#include "ivideo/xextf86vm.h"
//#include "ivideo/xextshm.h"
//#include "ivideo/xwindow.h"
#endif

// CS Geom
#ifndef NO_CSGEOM
#include "csgeom/box.h"
#include "csgeom/cspoint.h"
#include "csgeom/csrect.h"
#include "csgeom/csrectrg.h"
#include "csgeom/fastsqrt.h"
#include "csgeom/frustum.h"
#include "csgeom/math2d.h"
#include "csgeom/math3d.h"
#include "csgeom/math3d_d.h"
#include "csgeom/matrix2.h"
#include "csgeom/matrix3.h"
#include "csgeom/path.h"
#include "csgeom/plane2.h"
#include "csgeom/plane3.h"
#include "csgeom/poly2d.h"
#include "csgeom/poly3d.h"
#include "csgeom/polyaa.h"
#include "csgeom/polyclip.h"
#include "csgeom/polyedge.h"
#include "csgeom/polyidx.h"
#include "csgeom/polypool.h"
#include "csgeom/quaterni.h"
#include "csgeom/segment.h"
#include "csgeom/sphere.h"
#include "csgeom/spline.h"
#include "csgeom/subrec.h"
#include "csgeom/tesselat.h"
#include "csgeom/textrans.h"
#include "csgeom/transfrm.h"
#include "csgeom/vector2.h"
#include "csgeom/vector3.h"
#include "csgeom/vtpool.h"
#include "csgeom/crysball.h"
#include "csgeom/objmodel.h"
#endif

// CS Tool
#ifndef NO_CSTOOL
#include "cstool/collider.h"
#include "cstool/csanim2d.h"
#include "cstool/csfxscr.h"
#include "cstool/cspixmap.h"
#include "cstool/csview.h"
#include "cstool/gentrtex.h"
#include "cstool/initapp.h"
#include "cstool/keyval.h"
#include "cstool/mapnode.h"
#include "cstool/mdldata.h"
#include "cstool/mdltool.h"
#include "cstool/prdots.h"
#include "cstool/prfire.h"
#include "cstool/proctex.h"
#include "cstool/prplasma.h"
#include "cstool/prsky.h"
#include "cstool/prwater.h"
#include "cstool/sndwrap.h"
#include "cstool/sprbuild.h"
#endif

// CS Util
#ifndef NO_CSUTIL
#include "csutil/bitarray.h"
#include "csutil/bitset.h"
#include "csutil/cfgacc.h"
#include "csutil/cfgfile.h"
#include "csutil/cfgmgr.h"
#include "csutil/cmdhelp.h"
#include "csutil/cmdline.h"
#include "csutil/cscolor.h"
#include "csutil/csevcord.h"
#include "csutil/csevent.h"
#include "csutil/cseventq.h"
#include "csutil/csinput.h"
#include "csutil/csmd5.h"
#include "csutil/csobject.h"
#include "csutil/cspmeter.h"
#include "csutil/csppulse.h"
#include "csutil/csstring.h"
#include "csutil/csstrvec.h"
#include "csutil/databuf.h"
#include "csutil/dataobj.h"
#include "csutil/datastrm.h"
#include "csutil/debug.h"
#include "csutil/evoutlet.h"
#include "csutil/flags.h"
#include "csutil/garray.h"
#include "csutil/tree.h"
#include "csutil/hashmap.h"
#include "csutil/intarray.h"
#include "csutil/memfile.h"
#include "csutil/mmapio.h"
#include "csutil/nobjvec.h"
#include "csutil/objiter.h"
#include "csutil/objpool.h"
#include "csutil/objreg.h"
#include "csutil/plugldr.h"
#include "csutil/plugmgr.h"
#include "csutil/prfxcfg.h"
#include "csutil/randomgen.h"
#include "csutil/scanstr.h"
#include "csutil/scf.h"
#include "csutil/scfstr.h"
#include "csutil/scfstrv.h"
#include "csutil/snprintf.h"
#include "csutil/sparse3d.h"
#include "csutil/strset.h"
#include "csutil/parray.h"
#include "csutil/refarr.h"
#include "csutil/util.h"
#include "csutil/virtclk.h"
#endif

// CS Gfx
#ifndef NO_CSGFX
#include "csgfx/bumpmap.h"
#include "csgfx/csimage.h"
#include "csgfx/inv_cmap.h"
#include "csgfx/memimage.h"
#include "csgfx/quantize.h"
#include "csgfx/rgbpixel.h"
#include "csgfx/xorpat.h"
#endif

// CSWS
#ifndef NO_CSWS
#include "csws/csws.h"
#endif

#endif // CS_INCLUDE_NOTHING

#endif // __CS_CSS_H__
