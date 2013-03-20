/*
    Copyright (C) 2012 by Andrei Barsan

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

#ifndef __SPACE_PARTICLEEDITOR_H__
#define __SPACE_PARTICLEEDITOR_H__

#include "cseditor/modifiableeditor.h"
#include "csutil/csbaseeventh.h"
#include "csutil/eventnames.h"
#include "csutil/ref.h"
#include "csutil/scf_implementation.h"
#include "ieditor/editor.h"
#include "ieditor/space.h"
#include "iutil/event.h"
#include "iutil/comp.h"
#include "iutil/modifiable.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  using namespace CS::EditorApp;

  class CSPartEditSpace : public wxPanel, public csBaseEventHandler, public scfImplementation1<CSPartEditSpace, iSpace>
  {
  public:
    CSPartEditSpace (iBase* parent);
    virtual ~CSPartEditSpace ();

    // iSpace
    virtual bool Initialize (iObjectRegistry* obj_reg, iEditor* editor, iSpaceFactory* fact, wxWindow* parent);
    virtual iSpaceFactory* GetFactory () const { return spaceFactory; }
    virtual wxWindow* GetwxWindow ();
    virtual void SetEnabled (bool enabled);
    virtual bool GetEnabled () const;
    virtual void Update ();
    
  private:
    // iEventHandler 
    bool HandleEvent (iEvent &event);

    // Various events
    void OnSize           (wxSizeEvent& event);

    /**
     * Checks the current editor context and, if available, gets the selected
     * particle system, passing it to the modifiable editor and updating the rest of
     * the GUI.
     */
    void Populate ();

    /**
     * Called when the editor should be cleared, e.g. when the selected data isn't
     * a particle system, with the message containing the reason.
     */
    void Empty (const wxString& message);

    void OnButtonAddEmitter (wxCommandEvent &event);
    void OnButtonRemoveEmitter (wxCommandEvent &event);
    void OnButtonAddEffector (wxCommandEvent &event);
    void OnButtonRemoveEffector (wxCommandEvent &event);
    void OnEmitterSelect (wxCommandEvent& event);
    void OnEffectorSelect (wxCommandEvent& event);

    void UpdateEmitterList  ();
    void UpdateEffectorList ();

  private:
    static const int borderWidth = 4; 
    bool enabled;
    wxBoxSizer* mainSizer, *middleSizer, *middleLSizer, *middleRSizer;
    wxListBox *emitterList, *effectorList;
    csRef<iEditor> editor;
    csRef<iEventQueue> queue;

    iObjectRegistry* object_reg;
    csRef<iSpaceFactory> spaceFactory;
    /// The particle system factory currently being edited
    csRef<iParticleSystemFactory> factory;
    /// The corresponding emitter factory
    csRef<iParticleBuiltinEmitterFactory> emitterFactory;
    /// The corresponding effector factory
    csRef<iParticleBuiltinEffectorFactory> effectorFactory;

    /// Used to edit the general PS propertiees
    ModifiableEditor* mainEditor;
    /// Used to edit the selectid emitter/ effector
    ModifiableEditor* secondaryEditor;

    // Various event ids
    iEventNameRegistry* nameRegistry;
    csStringID addObject;
    csStringID clearObjects;
    csStringID activateObject;

    enum {
      idMainEditor = 42,
      idSecondaryEditor,
      idButtonAddEmitter,
      idButtonRemoveEmitter,
      idButtonAddEffector,
      idButtonRemoveEffector,
      idEmitterList,
      idEffectorList
    };

    DECLARE_EVENT_TABLE ();
  };
}
CS_PLUGIN_NAMESPACE_END (CSEditor)

#endif
