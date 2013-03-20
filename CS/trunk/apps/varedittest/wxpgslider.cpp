#include "wxpgslider.h"

//----------------- wxPGSliderEditor ---------------------

WX_PG_IMPLEMENT_EDITOR_CLASS(SliderEditor, wxPGSliderEditor, wxPGEditor)

wxPGWindowList wxPGSliderEditor::CreateControls( wxPropertyGrid*  propgrid,
                                                 wxPGProperty*    property,
                                                 const wxPoint&   pos,
                                                 const wxSize&    size ) const
{
  wxSlider* slider = ((wxPGSliderProperty*) property)->GetSlider ();
  ((wxWindow*) slider)->Hide ();

  double v_d = property->GetValue().GetDouble();
  if ( v_d < 0 )
    v_d = 0;
  else if ( v_d > 1 )
    v_d = 1;

  wxSlider* ctrl = new wxSlider ();
  ctrl->Create ( propgrid->GetPanel(),
                 wxPG_SUBID1,
                 (int)(v_d * precision),
                 0,
                 precision,
                 pos,
                 size,
                 wxSL_HORIZONTAL );  

  // Setup event handler
  propgrid->Connect ( wxPG_SUBID1, wxEVT_SCROLL_THUMBTRACK, 
                      (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                      &wxPropertyGrid::OnCustomEditorEvent, NULL, propgrid );

  return ctrl;
}

void wxPGSliderEditor::UpdateControl ( wxPGProperty* property, wxWindow* wnd ) const
{
  wxSlider* ctrl = wxDynamicCast ( wnd, wxSlider );
  if ( ctrl )
  {
    double val;
    if (wxPGVariantToDouble (property->DoGetValue(), &val))
    {
      if ( val < 0 )
        val = 0;
      else if ( val > 1 )
        val = 1;
      ctrl->SetValue ( (int)(val * precision) );
    }
  }
}

bool wxPGSliderEditor::OnEvent ( wxPropertyGrid*  WXUNUSED(propgrid), 
                                 wxPGProperty*    WXUNUSED(property),
                                 wxWindow*        WXUNUSED(wnd),
                                 wxEvent&         event ) const
{
  return event.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK;
}

bool wxPGSliderEditor::GetValueFromControl ( wxVariant&     variant,
                                             wxPGProperty*  property,
                                             wxWindow*      wnd ) const
{
  wxSlider* ctrl = wxDynamicCast ( wnd, wxSlider );
  if ( ctrl )
  {
    variant = wxVariant ( (double)(ctrl->GetValue())/(double)(precision) );
    property->SetValue ( variant );
  }

  return true;
}

void wxPGSliderEditor::SetValueToUnspecified ( wxPGProperty* property, wxWindow* wnd) const
{
  // TODO
  wxSlider* ctrl = wxDynamicCast ( wnd, wxSlider );
  if ( ctrl )
    ctrl->SetValue (0);  
}

void wxPGSliderEditor::DrawValue ( wxDC&           dc,
                                   const wxRect&   rect,
                                   wxPGProperty*   property,
                                   const wxString& WXUNUSED(text) ) const
{
  wxSlider* slider = ( (wxPGSliderProperty*)property )->GetSlider();
  int curr = (int) ((double) property->GetValue() * precision);
  slider-> SetValue (curr);
  wxWindow* win = (wxWindow*) slider;
  if (!win-> IsShown ())
    win-> Show ();
}

//----------------- wxPGSliderProperty ---------------------

WX_PG_IMPLEMENT_PROPERTY_CLASS( wxPGSliderProperty, wxPGProperty,
                                double, const double&, SliderEditor )

wxPGSliderProperty::wxPGSliderProperty ( const wxString&  label,
                                         const wxString&  name,
                                         const double&    value,
                                         const int&       min,
                                         const int&       max )
  : wxPGProperty  ( label, name ),
    ctrl          (NULL),
    minVal        (min),
    maxVal        (max)
{
  SetValue ( wxVariant(value) );

  // Calls DoRegisterEditorClass; mandatory for custom properties
  wxPGEditor* editorPointer = wxPropertyGrid::RegisterEditorClass( new wxPGSliderEditor (max), wxT("SliderEditor") );
  SetEditor ( editorPointer );
}

wxPGSliderProperty::~wxPGSliderProperty () 
{}

void wxPGSliderProperty::Init ()
{
  ctrl = new wxSlider();
  double v_d = GetValue().GetDouble();
  wxPropertyGrid* propgrid = GetGrid();
  wxRect rect = GetRect(propgrid);
  ctrl->Create( propgrid->GetPanel(),
                wxPG_SUBID1,
                (int)(v_d * maxVal),
                minVal,
                maxVal,
                rect.GetPosition(),
                rect.GetSize(),
                wxSL_HORIZONTAL);
}

wxSlider* wxPGSliderProperty::GetSlider () const
{
  return ctrl;
}

wxRect wxPGSliderProperty::GetRect (wxPropertyGrid* propgrid) const
{
  wxRect rect = propgrid->GetPropertyRect(  dynamic_cast<const wxPGProperty*>(this), 
                                            dynamic_cast<const wxPGProperty*>(this) );
  wxSize size = rect.GetSize();
  wxPoint pos = rect.GetPosition();
  int splitterPos = propgrid->GetSplitterPosition() + 2;
  pos.x += splitterPos;
  size.x -= splitterPos;
  size.y -= 1;

  return wxRect (pos, size);
}

wxString wxPGSliderProperty::GetValueAsString ( int argflags ) const
{
  return wxString("", wxConvUTF8);
}

wxVariant wxPGSliderProperty::GetValue() const {
  return wxVariant(GetSlider()->GetValue() * maxVal);
}