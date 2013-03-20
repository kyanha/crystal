/*
  Re-implemented wxSlider for wxPropertyGrid
  
*/

#ifndef WXPGSLIDER_H
#define WXPGSLIDER_H

#include "cssysdef.h"

// common wx stuff
#include <wx/slider.h>
#include <wx/wx.h>

// for wxPGEditor
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/propdev.h"
#include "wx/propgrid/advprops.h"
#include "wx/propgrid/editors.h"

// the right csVariant
#include "apps/varedittest/pluginconfig.h"

//----------------- wxPGSliderEditor ---------------------

class WXDLLIMPEXP_PG wxPGSliderEditor : public wxPGEditor 
{
#ifndef SWIG
  WX_PG_DECLARE_EDITOR_CLASS(wxPGSliderEditor)
#endif

public:
  wxPGSliderEditor (int p = 10000)
    : precision(p)
  {}

  ~wxPGSliderEditor ()
  {}

  // Macro for the CreateControls method stub
  wxPG_DECLARE_CREATECONTROLS

  void UpdateControl ( wxPGProperty* property, wxWindow* wnd) const;
  bool OnEvent ( wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* wnd, wxEvent& event) const;
  bool GetValueFromControl ( wxVariant& variant, wxPGProperty* property, wxWindow* wnd) const;
  void SetValueToUnspecified ( wxPGProperty* property, wxWindow* wnd) const;
  void DrawValue ( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text) const;

private:
  int precision;
};

//----------------- wxPGSliderProperty ---------------------
/// The double property corresponding to the slider.
class wxPGSliderProperty : public wxPGProperty
{
  WX_PG_DECLARE_PROPERTY_CLASS(wxPGSliderProperty)

public:
  wxPGSliderProperty( const wxString& label = wxPG_LABEL,
                      const wxString& name  = wxPG_LABEL,
                      const double&   value = 0.0,
                      const int&      min   = 0,
                      const int&      max   = 10000 );

  virtual ~wxPGSliderProperty ();

  /// Initializes the property by creating it's controller, the slider.
  void Init ();

  /// Gets the property's corresponding slider.
  wxSlider* GetSlider () const;

  /// Gets the property slider's bounding rectangle.
  wxRect GetRect (wxPropertyGrid* propgrid) const;

  wxString GetValueAsString ( int argflags ) const;
  wxVariant GetValue() const;

private:
  wxSlider* ctrl;
  int minVal, maxVal;
};

#endif