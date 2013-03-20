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
#include "cssysdef.h"
#include "csgeom/vector3.h"
#include "csutil/cscolor.h"
#include "iutil/modifiable.h"
#include "ivaria/translator.h"

#include "vareditframe.h"

//--------------------

#include <wx/panel.h>
#include <wx/toolbar.h>
#include <wx/button.h>
#include <wx/arrstr.h>
#include <wx/combobox.h>
#include <wx/string.h>
#include <wx/wx.h>
#include <wx/variant.h>
#include <wx/event.h>
#include <wx/colour.h>

//---------------------------------------------------

// Helper methods to get information on the wxWidgets build

enum wxbuildinfoformat {
  short_f, long_f };

wxString wxbuildinfo (wxbuildinfoformat format)
{
  wxString wxbuild (wxVERSION_STRING);

  if (format == long_f )
  {
#if defined (__WXMSW__)
    wxbuild << _T ("-Windows");
#elif defined (__WXMAC__)
    wxbuild << _T ("-Mac");
#elif defined (__UNIX__)
    wxbuild << _T ("-Linux");
#endif

#if wxUSE_UNICODE
    wxbuild << _T ("-Unicode build");
#else
    wxbuild << _T ("-ANSI build");
#endif // wxUSE_UNICODE
  }

  return wxbuild;
}

//---------------------------------------------------

BEGIN_EVENT_TABLE (ModifiableTestFrame, wxFrame)
  EVT_CLOSE (ModifiableTestFrame::OnClose)
  EVT_MENU (idMenuQuit, ModifiableTestFrame::OnQuit)
  EVT_BUTTON (idBtnTest1, ModifiableTestFrame::OnPopulateClick)
  EVT_MENU (idMenuAbout, ModifiableTestFrame::OnAbout)
  EVT_KEY_DOWN (ModifiableTestFrame::OnEsc)
  EVT_KEY_UP (ModifiableTestFrame::OnEsc)
END_EVENT_TABLE ()

// Global frame pointer
csWeakRef<ModifiableTestFrame> frame;

//-------------------------------------------------------------------

ModifiableTestFrame::ModifiableTestFrame (iObjectRegistry* object_reg )
  : wxFrame (NULL, idFrame, wxT ("iModifiable GUI generation test"),
	     wxDefaultPosition, wxSize (600, 600)),
    object_reg (object_reg)
{
  mainsizer = new wxBoxSizer (wxHORIZONTAL);
  
  left_vsizer = new wxStaticBoxSizer (wxHORIZONTAL,this,_T ("Nodes"));
  right_vsizer = new wxStaticBoxSizer (wxHORIZONTAL,this,_T (""));
  mainsizer->Add (left_vsizer,1,wxALL|wxEXPAND,5);

  focusedIndex = 0;

  frame = this;

  //-------------------------------------------

  // create a menu bar
  wxMenuBar* mbar = new wxMenuBar ();
  wxMenu* fileMenu = new wxMenu (_T (""));
  fileMenu->Append (idMenuQuit, _ ("&Quit\tAlt-F4"), _("Quit the application"));

  mbar->Append (fileMenu, _("&File"));
  wxMenu* helpMenu = new wxMenu (_T (""));
  helpMenu->Append (idMenuAbout, _("&About\tF1"), _("Show info about this application"));
  mbar->Append (helpMenu, _("&Help"));

  SetMenuBar (mbar);

  //------------------------------------------

  btnCycle = new wxButton (this, idBtnTest1, wxT ("Cycle through nodes"),
			   wxDefaultPosition, wxDefaultSize, 0);
  left_vsizer->Add (btnCycle);
  SetSizer (mainsizer);

  // Create the modifiable editor
  modifiableEditor = new CS::EditorApp::ModifiableEditor (object_reg,
							  this,
							  wxID_ANY,
							  wxDefaultPosition,
							  wxDefaultSize,
							  0L,
							  wxT ("Modifiable editor") );
  mainsizer->Add (modifiableEditor, 1, wxEXPAND | wxALL, 10);

  // Setup the data path where reside the translations of the description of the
  // modifiable objects
  modifiableEditor->SetResourcePath ("/data/varedittest");
}

ModifiableTestFrame::~ModifiableTestFrame ()
{
}

bool ModifiableTestFrame::Initialize ()
{
  return true;
}

void ModifiableTestFrame::AddModifiable (CS::Utility::iModifiable* modifiable)
{
  modifiableEntities.Push (modifiable);
}

void ModifiableTestFrame::Populate (CS::Utility::iModifiable* dataSource)
{
  modifiableEditor->SetModifiable (dataSource);
}

//---------------------------------------------

void ModifiableTestFrame::OnPopulateClick (wxCommandEvent &event)
{
  // Cycles through the available nodes
  focusedIndex++;
  if (focusedIndex >= modifiableEntities.GetSize ()) 
    focusedIndex = 0;

  Populate (modifiableEntities.Get (focusedIndex));
}

//-----------------------------------------------

void ModifiableTestFrame::OnEsc (wxKeyEvent& event)
{
  if (event.GetKeyCode () == WXK_ESCAPE)
  {
    Destroy ();
  }
}

//--------------------------------------------------------

void ModifiableTestFrame::OnAbout (wxCommandEvent &event)
{
  csString message;
  message.Format (
    "Test application for the automated generation of a Graphical User Interface "
    "from iModifiable objects.\n\n"
    "The translation language to be used for this application is defined system-widely "
    "in the file %s. The list of languages that are available for the application are "
    "defined in %s and are %s, %s and %s.\n\n"
    "This demo has been compiled using the ", CS::Quote::Single ("data/config-plugins/translator.cfg"),
    CS::Quote::Single ("data/varedittest"),
    CS::Quote::Single ("en"), CS::Quote::Single ("de"), CS::Quote::Single ("fr"));

  wxString msg (message, wxConvUTF8);
  msg += wxbuildinfo (long_f);
  msg += wxT (".");
  wxMessageBox (msg, _("varedittest"));
}

//-------------------------------------------------

void ModifiableTestFrame::OnClose (wxCloseEvent &event)
{
  Destroy ();
}

void ModifiableTestFrame::OnQuit (wxCommandEvent &event)
{
  Destroy ();
}
