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
#include "cssysdef.h"
#include "csgeom/math3d.h"
#include "csgeom/plane3.h"
#include "cstool/csview.h"
#include "cstool/enginetools.h"
#include "cstool/initapp.h"
#include "csutil/event.h"
#include "csutil/ref.h"
#include "csutil/scf.h"
#include "csutil/sysfunc.h"
#include "ieditor/context.h"
#include "ieditor/operator.h"
#include "imap/reader.h"
#include "imesh/object.h"
#include "imesh/objmodel.h"
#include "imesh/particles.h"
#include "iutil/csinput.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "iutil/objreg.h"
#include "iutil/plugin.h"
#include "iutil/virtclk.h"
#include "ivaria/translator.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/natwin.h"
#include "ivideo/wxwin.h"

#include <wx/wx.h>
#include <wx/artprov.h>

#include "particleeditor.h"

using namespace CS::Utility;

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  CSPartEditSpace* editorSpace;

  /// Helper function; apparently this functionality isn't built into wx
  bool ListBoxHasSelection (wxListBox* listBox) {
    wxArrayInt sel;
    listBox->GetSelections (sel);
    return sel.size () != 0;
  }

  // This table triggers a bunch of MFP conversion warnings, but due to
  // the fact that the wxPanel is the first class being inherited, there
  // shouldn't be any problems.
  BEGIN_EVENT_TABLE (CSPartEditSpace, wxPanel)
    EVT_SIZE  (CSPartEditSpace::OnSize)
    EVT_BUTTON (idButtonAddEmitter, CSPartEditSpace::OnButtonAddEmitter)
    EVT_BUTTON (idButtonRemoveEmitter, CSPartEditSpace::OnButtonRemoveEmitter)
    EVT_BUTTON (idButtonAddEffector, CSPartEditSpace::OnButtonAddEffector)
    EVT_BUTTON (idButtonRemoveEffector, CSPartEditSpace::OnButtonRemoveEffector)
    EVT_LISTBOX (idEmitterList, CSPartEditSpace::OnEmitterSelect)
    EVT_LISTBOX (idEffectorList, CSPartEditSpace::OnEffectorSelect)
  END_EVENT_TABLE ()

  SCF_IMPLEMENT_FACTORY (CSPartEditSpace)
  
  CSPartEditSpace::CSPartEditSpace (iBase* parent) 
  : scfImplementationType (this, parent),
    object_reg (0) 
  {
    // Setup namespace-scoped pointer to editor, to be used by the static
    // event handler to reach the space
    editorSpace = this;
  }

  CSPartEditSpace::~CSPartEditSpace () 
  {
    delete mainEditor;
    delete secondaryEditor;
  }

  bool CSPartEditSpace::Initialize (iObjectRegistry* obj_reg, iEditor* editor,
				    iSpaceFactory* fact, wxWindow* parent)
  {
    object_reg = obj_reg;
    this->editor = editor;
    spaceFactory = fact;
    mainSizer = new wxBoxSizer (wxVERTICAL);

    // Initializes the wxPanel part of the space
    Create (parent);

    // Load the particle plugins
    csRef<iPluginManager> pluginManager = csQueryRegistry<iPluginManager> (object_reg);
    emitterFactory = csLoadPlugin<iParticleBuiltinEmitterFactory>
      (pluginManager, "crystalspace.mesh.object.particles.emitter");
    effectorFactory = csLoadPlugin<iParticleBuiltinEffectorFactory>
      (pluginManager, "crystalspace.mesh.object.particles.effector");      

    // Setup the event names
    nameRegistry = csEventNameRegistry::GetRegistry (object_reg);
    //addObject = nameRegistry->GetID ("crystalspace.editor.context.selection.addselectedobject");
    //clearObjects = nameRegistry->GetID ("crystalspace.editor.context.selection.clearselectedobjects");
    activateObject = nameRegistry->GetID ("crystalspace.editor.context.selection.setactiveobject");

    // Respond to context events
    //csEventID contextSelect = nameRegistry->GetID ("crystalspace.editor.context");
    RegisterQueue (editor->GetContext ()->GetEventQueue (), activateObject);
    
    // Prepare modifiable editors
    mainEditor = new ModifiableEditor (object_reg, this, idMainEditor, wxDefaultPosition,
				       parent->GetSize (), 0L, wxT ("Modifiable editor"));
    mainSizer->Add (mainEditor, 1, wxEXPAND | wxALL, borderWidth);
    SetSizer (mainSizer);
    mainSizer->SetSizeHints (this);

    middleSizer = new wxBoxSizer (wxHORIZONTAL);
    middleLSizer = new wxBoxSizer (wxVERTICAL);
    middleRSizer = new wxBoxSizer (wxVERTICAL);

    mainSizer->Add (middleSizer, 1, wxEXPAND, borderWidth );
    middleSizer->Add (middleLSizer, 1, wxEXPAND | wxALL, borderWidth);
    middleSizer->Add (middleRSizer, 1, wxEXPAND | wxALL, borderWidth);

    //-- Emitter GUI
    middleLSizer->Add (new wxStaticText (this, wxID_ANY, wxT ("Emitters")));
    emitterList = new wxListBox (this,idEmitterList);
    middleLSizer->Add ( emitterList,
                        1,
                        wxALL | wxEXPAND,
                        borderWidth );

    wxButton* but = new wxButton (this, idButtonAddEmitter, wxT ("Add"));
    but->SetSize (-1, 32);
    middleLSizer->Add ( but,
                        0,
                        wxALL | wxEXPAND,
                        borderWidth );

    but = new wxButton (this, idButtonRemoveEmitter, wxT ("Remove"));
    but->SetSize (-1, 32);
    middleLSizer->Add ( but,
                        0,
                        wxALL | wxEXPAND,
                        borderWidth );

    //-- Effector GUI
    middleRSizer->Add (new wxStaticText (this, wxID_ANY, wxT ("Effectors")));
    effectorList = new wxListBox (this, idEffectorList);
    middleRSizer->Add (effectorList,
                      1,
                      wxALL | wxEXPAND,
                      borderWidth);

    but = new wxButton (this, idButtonAddEffector, wxT ("Add"));
    but->SetSize (-1, 32);
    middleRSizer->Add (but,
                      0,
                      wxALL | wxEXPAND,
                      borderWidth);

    but = new wxButton (this, idButtonRemoveEffector, wxT ("Remove"));
    but->SetSize (-1, 32);
    middleRSizer->Add (but,
                      0,
                      wxALL | wxEXPAND,
                      borderWidth);

    secondaryEditor = new ModifiableEditor (object_reg, this, idSecondaryEditor,
      wxDefaultPosition, wxDefaultSize, 0L, wxT ("Secondary editor"));

    mainSizer->Add (secondaryEditor, 1, wxALL | wxEXPAND, borderWidth);
    enabled = true;
    printf ("\nInitialized particle editing panel!\n");

    // Populate with the current active object 
    Populate (); 

    return true;
  }

  void CSPartEditSpace::Update () {
  }

  void CSPartEditSpace::OnSize (wxSizeEvent& event)
  {
    mainSizer->SetDimension (0, 0, event.GetSize ().GetWidth (), event.GetSize ().GetHeight ());
    event.Skip ();
  }

  void CSPartEditSpace::Populate ()
  {
    // Get the object from the context
    csRef<iContextObjectSelection> objectSelectionContext =
      scfQueryInterface<iContextObjectSelection> (editor->GetContext ());

    // Search for the iModifiable interface of the particle factory
    iObject* result = objectSelectionContext->GetActiveObject ();
    if (!result)
    {
      Empty (wxT ("No object selected."));
      return;
    }

    //csString entityName (result->GetName ());

    csRef<iMeshFactoryWrapper> fac = scfQueryInterface<iMeshFactoryWrapper> (result);
    if (!fac)
    {
      Empty (wxT ("No mesh factory selected."));
      return;
    }

    csRef<iParticleSystemFactory> partSys =
      scfQueryInterface<iParticleSystemFactory> (fac->GetMeshObjectFactory ()); 
    if (!partSys)
    {
      Empty (wxT ("No particle system selected."));
      return;
    }

    csRef<iModifiable> modifiable = scfQueryInterface<iModifiable> (fac->GetMeshObjectFactory ());  
    if (!modifiable)
    {
      Empty (wxT ("Selected particle system is non-standard and doesn't implement iModifiable. It cannot be edited."));
      return;
    }

    // Caches a casted pointer to the factory
    factory = partSys;
   
    // Updates the GUI
    mainEditor->SetModifiable (modifiable);
    UpdateEmitterList ();
    UpdateEffectorList ();
  }

  void CSPartEditSpace::Empty (const wxString& message)
  {
    mainEditor->Clear ();
    secondaryEditor->Clear ();
    emitterList->Clear ();
    effectorList->Clear ();

    mainEditor->SetMessage (wxString (wxT ("Notice:")), message);
  }

  wxWindow* CSPartEditSpace::GetwxWindow ()
  {
    return this;
  }

  bool CSPartEditSpace::GetEnabled () const {
    return enabled;
  }

  void CSPartEditSpace::SetEnabled (bool enabled) {
    this->enabled = enabled;
  }

  bool CSPartEditSpace::HandleEvent (iEvent& event)
  {
    csRef<iEventNameRegistry> strings = csQueryRegistry<iEventNameRegistry> ( object_reg );

#ifdef CS_DEBUG
    printf ("\tCaught event: %s (ID #%u)\n",
      strings->GetString ( event.GetName () ),
      (unsigned int) event.GetName () );
#endif

    if (event.Name == activateObject) {
      // The user activated (double-clicked) something!
      Populate ();
    }

    return false;
  }

  void CSPartEditSpace::OnButtonAddEmitter ( wxCommandEvent &event )
  {
    // TODO: context menu to pick
    csRef<iParticleBuiltinEmitterBox> em = emitterFactory->CreateBox ();
    factory->AddEmitter (em);
    UpdateEmitterList ();

    // Add the emitter to the actual previewed mesh
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    iMeshWrapper* mesh = engine->FindMeshObject ("viewmesh");
    csRef<iParticleSystem> sys = scfQueryInterface<iParticleSystem> (mesh->GetMeshObject ());
    sys->AddEmitter (em);
  }

  void CSPartEditSpace::OnButtonRemoveEmitter ( wxCommandEvent &event )
  {
    if (!ListBoxHasSelection (emitterList)) return;

    factory->RemoveEmitter (event.GetSelection ());
    UpdateEmitterList ();

    // Remove the emitter from the actual previewed mesh
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    iMeshWrapper* mesh = engine->FindMeshObject ("viewmesh");
    csRef<iParticleSystem> sys = scfQueryInterface<iParticleSystem> (mesh->GetMeshObject ());
    sys->RemoveEmitter (event.GetSelection ());
  }

  void CSPartEditSpace::OnButtonAddEffector ( wxCommandEvent &event )
  {
    // TODO: context menu to pick
    csRef<iParticleBuiltinEffectorForce> eff = effectorFactory->CreateForce ();
    factory->AddEffector (eff);
    UpdateEffectorList ();

    // Add the effector to the actual previewed mesh
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    iMeshWrapper* mesh = engine->FindMeshObject ("viewmesh");
    csRef<iParticleSystem> sys = scfQueryInterface<iParticleSystem> (mesh->GetMeshObject ());
    sys->AddEffector (eff);
  }

  void CSPartEditSpace::OnButtonRemoveEffector ( wxCommandEvent &event )
  {
    if (!ListBoxHasSelection (effectorList)) return;

    factory->RemoveEffector (event.GetSelection ());
    UpdateEffectorList ();

    // Remove the effector from the actual previewed mesh
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    iMeshWrapper* mesh = engine->FindMeshObject ("viewmesh");
    csRef<iParticleSystem> sys = scfQueryInterface<iParticleSystem> (mesh->GetMeshObject ());
    sys->RemoveEffector (event.GetSelection ());
  }

  void CSPartEditSpace::OnEmitterSelect ( wxCommandEvent& event )
  {
    effectorList->DeselectAll ();
    iParticleEmitter* emt = static_cast<iParticleEmitter*> (emitterList->GetClientData (event.GetSelection ()));
    csRef<iModifiable> mod = scfQueryInterface<iModifiable> (emt);

    if (mod.IsValid ()) {
      secondaryEditor->SetModifiable (mod);
    } else {
      csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.editor.space.partedit",
        "Tried to edit emitter not implementing iModifiable.");

      return;
    }
  }

  void CSPartEditSpace::OnEffectorSelect ( wxCommandEvent& event )
  {
    emitterList->DeselectAll ();
    iParticleEffector* eff = static_cast<iParticleEffector*> (effectorList->GetClientData (event.GetSelection ()));
    csRef<iModifiable> mod = scfQueryInterface<iModifiable> (eff);

    if (mod.IsValid ()) {
      secondaryEditor->SetModifiable (mod);
    } else {
      csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.editor.space.partedit",
        "Tried to edit effector not implementing iModifiable.");

      return;
    }
  }

  void CSPartEditSpace::UpdateEmitterList ()
  {
    emitterList->Clear ();
    for (size_t i = 0; i < factory->GetEmitterCount (); i++) {
      iParticleEmitter* em = factory->GetEmitter (i);
      emitterList->Append (wxString::Format (wxT ("Emitter #%u"), i), (void*)em);
    }
  }

  void CSPartEditSpace::UpdateEffectorList ()
  {
    effectorList->Clear ();
    for (size_t i = 0; i < factory->GetEffectorCount (); i++) {
      iParticleEffector* em = factory->GetEffector (i);
      effectorList->Append (wxString::Format (wxT ("Effector #%u"), i), (void*)em);
    }
  }

}
CS_PLUGIN_NAMESPACE_END (CSEditor)
