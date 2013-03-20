#ifndef WXPGSLIDER_H
#define WXPGSLIDER_H

#include "cssysdef.h"

// common wx stuff
#include <wx/slider.h>
#include <wx/wx.h>

// for wxPGEditor
#include "cseditor/wx/propgrid/propgrid.h"
#include "cseditor/wx/propgrid/propdev.h"
#include "cseditor/wx/propgrid/advprops.h"
#include "cseditor/wx/propgrid/editors.h"

//----------------- wxPGSliderEditor ---------------------

/**
 * wxSlider for wxPropertyGrid. Contains a value slider.
 */
class WXDLLIMPEXP_PG wxPGSliderEditor : public wxPGEditor 
{
#ifndef SWIG
  WX_PG_DECLARE_EDITOR_CLASS(wxPGSliderEditor)
#endif


public:
  wxPGSliderEditor (float min = 0, float max = 100, int precision = 1)
    : precision(precision), min(min), max(max)        
  {
  }

  virtual ~wxPGSliderEditor ()
  {}

  // Macro for the CreateControls method stub
  wxPG_DECLARE_CREATECONTROLS

  void UpdateControl ( wxPGProperty* property, wxWindow* wnd) const;
  bool OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* wnd, wxEvent& event) const;
  bool GetValueFromControl ( wxVariant& variant, wxPGProperty* property, wxWindow* wnd) const;
  void SetValueToUnspecified ( wxPGProperty* property, wxWindow* wnd) const;
  //void DrawValue ( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text) const;

private:
  int precision;
  float min, max;
};

#endif
