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

#ifndef __CSEDITOR_CS3DMENU_H__
#define __CSEDITOR_CS3DMENU_H__

#include "csutil/scf_implementation.h"
#include "iutil/comp.h"

#include "ieditor/menu.h"

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class CS3DMenu : public scfImplementation1<CS3DMenu, iMenu>
{
public:
  CS3DMenu (iBase* parent);
  virtual ~CS3DMenu ();

  // iMenu
  virtual bool Poll (iContext* context);
  virtual void Draw (iContext* context, iLayout* layout);
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
