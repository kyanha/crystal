/*
    Copyright (C) 2011 by Jelle Hellemans

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
#include "csutil/scf.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"

#include "ieditor/space.h"
#include "ieditor/layout.h"

#include "cs3dmenu.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  
SCF_IMPLEMENT_FACTORY (CS3DMenu)

CS3DMenu::CS3DMenu (iBase* parent) : scfImplementationType (this, parent)
{
}

CS3DMenu::~CS3DMenu ()
{
}

bool CS3DMenu::Poll (iContext* context)
{
  return true;
}

void CS3DMenu::Draw (iContext* context, iLayout* layout)
{
  layout->AppendOperator ("crystalspace.editor.operator.select", "Select", "S");
  //layout->AppendSeparator ();
  //layout->AppendOperator ("crystalspace.editor.operator.move", "Move", "G");
}

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
