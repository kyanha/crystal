/***************************************************************
 * Name:      Graph_behaviourMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2011-02-11
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef GRAPH_BEHAVIOURMAIN_H
#define GRAPH_BEHAVIOURMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <iostream>
#endif

// Main propertygrid header.
#include "wx/propgrid/propgrid.h"

// Needed for implementing custom properties.
#include "wx/propgrid/propdev.h"
#include <wx/panel.h>
// Extra property classes.
#include "wx/propgrid/advprops.h"

// This defines wxPropertyGridManager.
#include "wx/propgrid/manager.h"
//#include "wxgraph.h"
//*********************************
#include <stdarg.h>
#include "csutil/array.h"
#include "csutil/csstring.h"
#include <wx/variant.h>
#include <string>

#include "graphedit/graphNode.h"
#include "graphedit/graphNodeFactory.h"

using namespace std;



#if wxUSE_DATEPICKCTRL
    #include <wx/datectrl.h>
#endif

#include <wx/artprov.h>
class csVariant;

//class GraphNodeFrame

class PropGridManager
{
 public:
  PropGridManager (wxFrame *frame, int pageId);
  ~PropGridManager ();

  void SetGraphNode (GraphNode* node);

  void OnPopulateClick(wxCommandEvent &event);
  void OnPropertyGridChanging(wxPropertyGridEvent& event);
  wxPropertyGridManager* GetManager();	 
	
 private:
  void Populate ();

  wxPropertyGridManager* m_pPropGridManager;
  wxPropertyGridPage* page;
  wxPropertyGridManager* pgMan;

  GraphNodeFactory* nodeFactory;
	wxFrame *frame;		
	     
 protected:
  void OnGetNewValue(wxPGProperty* property);
  GraphNode* node;
	     
};

class wxVector3f
{
public:
    wxVector3f();
     wxVector3f( double x, double y, double z );
   
  
   
	~wxVector3f();
	
	

    double x, y, z;
};

inline bool operator == (const wxVector3f& a, const wxVector3f& b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z);
}

WX_PG_DECLARE_VARIANT_DATA(wxVector3fVariantData, wxVector3f, wxPG_NO_DECL)

class wxVectorProperty : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxVectorProperty)
public:

    wxVectorProperty( const wxString& label = wxPG_LABEL,
                      const wxString& name = wxPG_LABEL,
                      const wxVector3f& value = wxVector3f() );
  
    virtual ~wxVectorProperty();

    WX_PG_DECLARE_PARENTAL_METHODS()
   
protected:
};

///----------------------------------------------------

class wxVector2f
{
public:
    wxVector2f();
     wxVector2f( double x, double y );
   
  
   
	~wxVector2f();
	
	

    double x, y;
};

inline bool operator == (const wxVector2f& a, const wxVector2f& b)
{
    return (a.x == b.x && a.y == b.y );
}

WX_PG_DECLARE_VARIANT_DATA(wxVector2fVariantData, wxVector2f, wxPG_NO_DECL)

class wxVector2Property : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxVector2Property)
public:

		
		wxVector2Property( const wxString& label = wxPG_LABEL,
                      const wxString& name = wxPG_LABEL,
                      const wxVector2f& value = wxVector2f() );
  
    virtual ~wxVector2Property();

    WX_PG_DECLARE_PARENTAL_METHODS()
   
protected:
};

//--------------------------------------------------
class wxVector4f
{
public:
    wxVector4f();
     wxVector4f( double x, double y, double z, double w );
   
  
   
	~wxVector4f();
	
	

    double x, y, z , w;
};

inline bool operator == (const wxVector4f& a, const wxVector4f& b)
{
    return (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w);
}

WX_PG_DECLARE_VARIANT_DATA(wxVector4fVariantData, wxVector4f, wxPG_NO_DECL)

class wxVector4Property : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxVector4Property)
public:

    wxVector4Property( const wxString& label = wxPG_LABEL,
                      const wxString& name = wxPG_LABEL,
                      const wxVector4f& value = wxVector4f() );
  
    virtual ~wxVector4Property();

    WX_PG_DECLARE_PARENTAL_METHODS()
   
protected:
};

///----------------------------------------------



#endif // GRAPH_BEHAVIOURMAIN_H
