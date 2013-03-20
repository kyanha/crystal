/*
  Copyright (C) 2012 Christian Van Brussel, Andrei Barsan

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef __VAREDITFRAME_H__
#define __VAREDITFRAME_H__

#include "cseditor/modifiableeditor.h"
#include "cseditor/wxpgslider.h"
#include "cstool/initapp.h"
#include "csutil/array.h"
#include "csutil/csstring.h"
#include "csutil/refarr.h"
#include "csutil/weakref.h"
#include "iutil/document.h"
#include "iutil/objreg.h"
#include "ivaria/translator.h"

#include "vareditapp.h"

#include <wx/variant.h>

struct csVariant;
struct iModifiable;

// We need to implement WeakReferenced for the pump's weakref
class ModifiableTestFrame : public wxFrame,
                            public CS::Utility::WeakReferenced
{
 public:
  ModifiableTestFrame (iObjectRegistry* object_reg);
  ~ModifiableTestFrame ();

  void AddModifiable (CS::Utility::iModifiable* modifiable);
  bool Initialize ();

  void OnPopulateClick (wxCommandEvent &event);
  void OnEsc (wxKeyEvent& event);
  wxPropertyGridManager* GetManager ();	
	
 private:
   /// Generates the GUI based on an iModifiable entity
  void Populate (CS::Utility::iModifiable* dataSource);

  iObjectRegistry* object_reg;

  csRefArray<CS::Utility::iModifiable> modifiableEntities;
  size_t focusedIndex;

  // Main window sizer
  wxBoxSizer* mainsizer;
  // Left/ right sizers
  wxStaticBoxSizer* left_vsizer;
  wxStaticBoxSizer* right_vsizer;
  
  wxButton* btnCycle;
	     
private:

  enum
  {
    idFrame = 1000,
    idMenuQuit = 1001,
    idMenuAbout = 1002,
    idgrid = 1003,
    idBtnTest1 = 1004,
    idBtnTest2 = 2000,
    idMenuEdit = 1006,
    idMenuAddprop = 1007,
    idCombo = 1008,
    pageId = 1
  };

  // Container of the iModifiable editing GUI
  CS::EditorApp::ModifiableEditor* modifiableEditor;

  void OnClose (wxCloseEvent& event);
  void OnQuit (wxCommandEvent& event);
  void OnAbout (wxCommandEvent& event);

  DECLARE_EVENT_TABLE ();
};

#endif // __VAREDITFRAME_H__
