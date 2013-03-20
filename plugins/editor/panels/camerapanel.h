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

#ifndef __CSEDITOR_PANEL_CAMERA_H__
#define __CSEDITOR_PANEL_CAMERA_H__

#include "csutil/scf_implementation.h"
#include "ieditor/panel.h"

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class CameraPanel : public scfImplementation1<CameraPanel, iPanel>
{
public:
  CameraPanel (iBase* parent);
  virtual ~CameraPanel ();
  
  //-- iPanel
  virtual bool Poll (iContext* context);
  virtual void Draw (iContext* context, iLayout* layout);
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif // __CSEDITOR_PANEL_CAMERA_H__
