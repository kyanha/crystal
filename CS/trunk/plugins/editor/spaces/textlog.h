/*
    Copyright (C) 2012 by Christian Van Brussel

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

#ifndef __SPACE_TEXTLOG_H__
#define __SPACE_TEXTLOG_H__

#include "csutil/scf_implementation.h"
#include "ieditor/components.h"
#include "ieditor/space.h"
#include "iutil/objreg.h"
#include "ivaria/reporter.h"

#include <wx/log.h>
#include <wx/panel.h>
#include <wx/textctrl.h>

class wxWindow;

using namespace CS::EditorApp;
using namespace CS::EditorApp::Component;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class TextLog : public scfImplementation1<TextLog, iSpace>
{
public:
  TextLog (iBase* parent);
  virtual ~TextLog ();

  //-- iSpace
  virtual bool Initialize (iObjectRegistry* obj_reg, iEditor* editor,
			   iSpaceFactory* fact, wxWindow* parent);
  virtual iSpaceFactory* GetFactory () const { return factory; }
  virtual wxWindow* GetwxWindow ();
  virtual void SetEnabled (bool enabled) { this->enabled = enabled; }
  virtual bool GetEnabled () const { return enabled; }
  virtual void Update ();
  
private:
  bool ReportError (const char* description, ...) const
  {
    va_list arg;
    va_start (arg, description);
    csReportV (object_reg, CS_REPORTER_SEVERITY_ERROR,
	       "crystalspace.editor.space.textlog",
	       description, arg);
    va_end (arg);
    return false;
  }

private:
  iObjectRegistry* object_reg;
  csRef<iSpaceFactory> factory;
  wxWindow* window;
  bool enabled;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif // __SPACE_TEXTLOG_H__
