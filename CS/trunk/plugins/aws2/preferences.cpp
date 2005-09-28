/*
    Copyright (C) 2005 by Christopher Nelson

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

#include "cssysdef.h"

#include "preferences.h"
#include "xml_def.h"

#include "ivideo/graph3d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/fontserv.h"
#include "ivaria/reporter.h"
#include "iutil/vfs.h"
#include "iutil/databuff.h"

namespace aws
{

#if 0 /* not presently used */
static char *color_names[] = 
{
 "HighlightColor",
 "HighlightColor2",
 "ShadowColor",
 "ShadowColor2",
 "FillColor",
 "DarkFill",
 "BackFill",
 "TextForeColor",
 "TextBackColor",
 "SelectedTextForeColor",
 "SelectedTextBackColor",
 "TextDisabledColor",
 "ButtonTextColor",
 "TransparentColor",
 "BlackColor",
 "WhiteColor",
 "RedColor",
 "GreenColor",
 "BlueColor" 
};
#endif

void preferences::init_default_colors()
{
  sys_colors[AC_HIGHLIGHT] = csColor4(0.75f, 0.75f, 0.75f, 1);
  sys_colors[AC_HIGHLIGHT2] = csColor4(0.65f, 0.65f, 0.65f, 1);
  sys_colors[AC_SHADOW] = csColor4(0.25f, 0.25f, 0.25f, 1);
  sys_colors[AC_SHADOW2] = csColor4(0.35f, 0.35f, 0.35f, 1);
  sys_colors[AC_FILL] = csColor4(0.5f, 0.5f, 0.5f, 1);
  sys_colors[AC_DARKFILL] = csColor4(0.4f, 0.4f, 0.4f, 1);
  sys_colors[AC_BACKFILL] = csColor4(0.5f, 0.5f, 0.5f, 1);
  sys_colors[AC_BLACK] = csColor4(0, 0, 0, 1);
  sys_colors[AC_WHITE] = csColor4(1, 1, 1, 1);
  sys_colors[AC_RED] = csColor4(1, 0, 0, 1);
  sys_colors[AC_GREEN] = csColor4(0, 1, 0, 1);
  sys_colors[AC_BLUE] = csColor4(0, 0, 1, 1);
}

bool preferences::load(iObjectRegistry* objreg, const scfString& filename)
{
	csPrintf("aws: Loading definitions file \"%s\"...\n", filename.GetData());

	csRef<iVFS> vfs (CS_QUERY_REGISTRY (objreg, iVFS));

	if (!vfs)
	{
		csPrintf("aws: Unable to load VFS plugin.\n");
		return false;
	}

	csRef<iFile> input = vfs->Open (filename.GetData(), VFS_FILE_READ);

	if (!input)
	{
		csPrintf("aws: Unable to open file \"%s\".\n", filename.GetData());
		return false;
	}

	// Read the whole file.
	csRef<iDataBuffer> buf = input->GetAllData();

	aws2::defFile df;

	// Parse the definition file into the root registry object.
	df.Parse(buf->GetData(), root);

    return true;
}

}
