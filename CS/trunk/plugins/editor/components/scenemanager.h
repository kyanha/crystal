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

#ifndef __COMPONENTS_SCENEMANAGER_H__
#define __COMPONENTS_SCENEMANAGER_H__

#include "csutil/csbaseeventh.h"
#include "csutil/scf_implementation.h"
#include "ieditor/editor.h"
#include "ieditor/space.h"
#include "ivaria/cameramanager.h"
#include "ivaria/reporter.h"

using namespace CS::EditorApp;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{

class SceneManager
  : public scfImplementation1<SceneManager, iEditorComponent>,
    public csBaseEventHandler
{
public:
  SceneManager (iBase* parent);
  virtual ~SceneManager();

  //-- iEditorComponent
  virtual bool Initialize (iEditor* editor);
  virtual void Update ();
  virtual void Save (iDocumentNode* node) const;
  virtual bool Load (iDocumentNode* node);
  
private:
  bool ReportError (const char* description, ...) const
  {
    va_list arg;
    va_start (arg, description);
    csReportV (object_reg, CS_REPORTER_SEVERITY_ERROR,
	       "crystalspace.editor.component.maploader",
	       description, arg);
    va_end (arg);
    return false;
  }

  bool ReportWarning (const char* description, ...) const
  {
    va_list arg;
    va_start (arg, description);
    csReportV (object_reg, CS_REPORTER_SEVERITY_WARNING,
	       "crystalspace.editor.component.maploader",
	       description, arg);
    va_end (arg);
    return false;
  }

  //-- iEventHandler
  bool HandleEvent (iEvent &event);

  void OnSetCamera (iCamera* camera);
  void OnSetActiveObject (iCamera* camera);
  void OnSetCollection (iCamera* camera);

  void PositionCamera (iCamera* camera, csBox3& bbox);
  void CreateViewmeshScene (iMeshFactoryWrapper* meshFactory,
			    iCamera* camera);

private:
  iObjectRegistry* object_reg;
  iEditor* editor;

  csRef<iEngine> engine;
  csRef<CS::Utility::iCameraManager> cameraManager;
  csRef<iSector> meshSector;

  csEventID eventSetActiveObject;
  csEventID eventSetCamera;
  csEventID eventSetCollection;

  bool internalChange;

  class EventListener : public scfImplementation1<EventListener, iEventHandler>
  {
  public:
    EventListener (SceneManager* manager);

    //-- iEventHandler
    bool HandleEvent (iEvent &event);

  private:
    SceneManager* manager;

    CS_EVENTHANDLER_NAMES ("crystalspace.editor.component.scenemanager.mouse");
  
    virtual const csHandlerID * GenericPrec (csRef<iEventHandlerRegistry> &r1, 
					     csRef<iEventNameRegistry> &r2, csEventID event) const 
    {
      static csHandlerID precConstraint[1];
      precConstraint[0] = CS_HANDLERLIST_END;
      return precConstraint;
    }

    virtual const csHandlerID * GenericSucc (csRef<iEventHandlerRegistry> &r1, 
					     csRef<iEventNameRegistry> &r2, csEventID event) const 
    {
      static csHandlerID precConstraint[2];
      precConstraint[0] = r1->GetGenericID("crystalspace.utilities.cameramanager");
      precConstraint[1] = CS_HANDLERLIST_END;
      return precConstraint;
    }

    CS_EVENTHANDLER_DEFAULT_INSTANCE_CONSTRAINTS;
  };
  csRef<EventListener> eventListener;
};

}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif // __COMPONENTS_SCENEMANAGER_H__
