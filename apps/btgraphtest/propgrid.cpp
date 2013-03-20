/***************************************************************
 * Name:      Graph_behaviourMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2011-02-11
 * Copyright:  ()
 * License:
 **************************************************************/
//#define wxPG_BOOL_USE_CHECKBOX     wxS("UseCheckbox")
#include "cssysdef.h"

#include "propgrid.h"

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
//*********************************

//---------
#include <iostream>
using namespace std;
//----------------



#if wxUSE_DATEPICKCTRL
    #include "wx/datectrl.h"
#endif

#include "wx/artprov.h"
//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}
//---------------------------------------------------

PropGridManager::PropGridManager (wxFrame* frame, int pageId): frame (frame)
{
  pgMan = new wxPropertyGridManager(frame, pageId,
				   wxDefaultPosition, wxDefaultSize,
				    // These and other similar styles are automatically
				    // passed to the embedded wxPropertyGrid.
				    wxPG_BOLD_MODIFIED|wxPG_SPLITTER_AUTO_CENTER|
				    // Include toolbar.
				    wxPG_TOOLBAR |
                                    // Include description box.
				    wxPG_DESCRIPTION |
				    // Plus defaults.
				    wxPG_EX_HELP_AS_TOOLTIPS);
  //--------------------------------------------------------
} 

void PropGridManager::SetGraphNode (GraphNode* node)
{
  printf("ok");
  this->node = node;
  Populate ();
}

wxPropertyGridManager*PropGridManager:: GetManager(){

		return pgMan;
}
//_---------------------------------
WX_PG_IMPLEMENT_VARIANT_DATA(wxVector3fVariantData, wxVector3f)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxVectorProperty,wxPGProperty,
                               wxVector3f,const wxVector3f&,TextCtrl)

wxVector3f::wxVector3f()
{
        x = y = z = 0.0;
}
wxVector3f::wxVector3f( double x, double y, double z ): x (x), y (y), z (z)
    {
    }

wxVector3f::~wxVector3f()
{
		
}

wxVectorProperty::wxVectorProperty( const wxString& label,
                                              const wxString& name,
                                              const wxVector3f& value )
    : wxPGProperty(label,name)
{

						wxVariant variant;
            variant << value;
            SetValue(variant);
    //SetValue( wxVector3fToVariant(value) );
    AddPrivateChild( new wxFloatProperty(wxT("X"),wxPG_LABEL,value.x) );

    AddPrivateChild( new wxFloatProperty(wxT("Y"),wxPG_LABEL,value.y) );
    AddPrivateChild( new wxFloatProperty(wxT("Z"),wxPG_LABEL,value.z) );
}

void wxVectorProperty::RefreshChildren()
{
    if ( !GetCount() ) return;
    wxVector3f& vector = wxVector3fFromVariant(m_value);
    Item(0)->SetValue( vector.x );
    Item(1)->SetValue( vector.y );
    Item(2)->SetValue( vector.z );
}

void wxVectorProperty::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{
		
    wxVector3f& vector = wxVector3fFromVariant(thisValue);
    switch ( childIndex )
    {
        case 0: vector.x = childValue.GetDouble();										
				break;
				case 1: vector.y = childValue.GetDouble(); 	 
				break;
        case 2: vector.z = childValue.GetDouble();				 				
				break;
    }

}

wxVectorProperty::~wxVectorProperty() { }
//__----------------------------------------------------

WX_PG_IMPLEMENT_VARIANT_DATA(wxVector2fVariantData, wxVector2f)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxVector2Property,wxPGProperty,
                               wxVector2f,const wxVector2f&,TextCtrl)

wxVector2f::wxVector2f()
{
        x = y = 0.0;
}
wxVector2f::wxVector2f( double x, double y ): x (x), y (y)
    {
    }
wxVector2f::~wxVector2f()
{
		
}

wxVector2Property::wxVector2Property( const wxString& label,
                                              const wxString& name,
                                              const wxVector2f& value )
    : wxPGProperty(label,name)
{

		wxVariant variant;
    variant << value;
    SetValue(variant);
    //SetValue( wxVector3fToVariant(value) );
    AddPrivateChild( new wxFloatProperty(wxT("X"),wxPG_LABEL,value.x) );
    AddPrivateChild( new wxFloatProperty(wxT("Y"),wxPG_LABEL,value.y) );
    
}

void wxVector2Property::RefreshChildren()
{
    if ( !GetCount() ) return;
    wxVector2f& vector = wxVector2fFromVariant(m_value);
    Item(0)->SetValue( vector.x );
    Item(1)->SetValue( vector.y );
    
}

void wxVector2Property::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{
		
    wxVector2f& vector = wxVector2fFromVariant(thisValue);
    switch ( childIndex )
    {
        case 0: vector.x = childValue.GetDouble();
				break;
      	case 1: vector.y = childValue.GetDouble(); 
				break;
   
    }

}

wxVector2Property::~wxVector2Property() { }
//--------------------------------------------------------
WX_PG_IMPLEMENT_VARIANT_DATA(wxVector4fVariantData, wxVector4f)

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxVector4Property,wxPGProperty,
                               wxVector4f,const wxVector4f&,TextCtrl)

wxVector4f::wxVector4f()
{
        x = y = z = w = 0.0;
}
wxVector4f::wxVector4f( double x, double y, double z , double w ): x (x), y (y), z (z), w (w)
    {
    }

wxVector4f::~wxVector4f()
{
		
}

wxVector4Property::wxVector4Property( const wxString& label,
                                              const wxString& name,
                                              const wxVector4f& value )
    : wxPGProperty(label,name)
{

						wxVariant variant;
            variant << value;
            SetValue(variant);
    
    AddPrivateChild( new wxFloatProperty(wxT("X"),wxPG_LABEL,value.x) );

    AddPrivateChild( new wxFloatProperty(wxT("Y"),wxPG_LABEL,value.y) );
    AddPrivateChild( new wxFloatProperty(wxT("Z"),wxPG_LABEL,value.z) );
		 AddPrivateChild( new wxFloatProperty(wxT("W"),wxPG_LABEL,value.w) );
}

void wxVector4Property::RefreshChildren()
{
    if ( !GetCount() ) return;
    wxVector4f& vector = wxVector4fFromVariant(m_value);
    Item(0)->SetValue( vector.x );
    Item(1)->SetValue( vector.y );
    Item(2)->SetValue( vector.z );
		Item(3)->SetValue( vector.w );
}

void wxVector4Property::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{
		
    wxVector4f& vector = wxVector4fFromVariant(thisValue);
    switch ( childIndex )
    {
        case 0: vector.x = childValue.GetDouble();										
				break;
				case 1: vector.y = childValue.GetDouble(); 	 
				break;
        case 2: vector.z = childValue.GetDouble();				 				
				break;
				case 3: vector.w = childValue.GetDouble();				 				
				break;
    }

}

wxVector4Property::~wxVector4Property() { }
//--------------------------------------------------------

void PropGridManager::Populate ()
{
  // all pages are cleared from the page manager
  pgMan->Clear();
	
	
  pgMan->AddPage(wxT("page1"));
  pgMan->SetExtraStyle( wxPG_EX_HELP_AS_TOOLTIPS );

  size_t nPg = pgMan->GetPageCount ();
	
  if (nPg >0)
  { 
    page = pgMan->GetPage (wxT ("page1"));
    pgMan->SetDescription (wxT ("Page Manager"), wxT ("New Page is added"));
    wxString categoryName (node->GetName (), wxConvUTF8);
	
    page ->Append (new wxPropertyCategory (categoryName));

    nodeFactory = node->GetFactory ();
    for (size_t i = 0; i < nodeFactory->GetParameterCount (); i++)
    {
      const csOptionDescription* option = nodeFactory->GetParameterDescription (i);

      switch (option->type)
			{
				case CSVAR_STRING:
	  			{
	    			wxString stringDescription (option->description, wxConvUTF8);
	    			wxString stringName (option->name, wxConvUTF8);					
	    			wxString stringValue (node->GetParameter (i)->GetString (), wxConvUTF8);
	    			wxStringProperty* stringP = new wxStringProperty (stringName);
	    			page->Append (stringP);
	    			stringP->SetValue (stringValue);
						page->SetPropertyHelpString(stringName, stringDescription);
	  			}
	  		break;

				case CSVAR_LONG :
	  			{
	    			wxString longDescription (option->description, wxConvUTF8);
	    			wxString longName (option->name, wxConvUTF8);
	    			wxString longValue = wxString::Format (wxT("%ld"), (int) node->GetParameter (i)->GetLong ());
	    			wxIntProperty* intP = new wxIntProperty(longName);				
	    			page->Append(intP);
	    			intP->SetValue(longValue);					
	    			pgMan->GetGrid ()->SetPropertyValue (intP, longValue);
	    			page->SetPropertyHelpString(longName, longDescription);
	  			}
	  		break;

				case CSVAR_FLOAT:
	  			{ 
	    			wxString floatDescription (option->description, wxConvUTF8);
	    			wxString floatName (option->name, wxConvUTF8);					
	    			wxString floatValue = wxString::Format (wxT("%f"), node->GetParameter (i)->GetFloat ());      
	    			wxFloatProperty* floatP = new wxFloatProperty(floatName);
	    			page->Append (floatP );
	    			floatP->SetValue (floatValue);
						page->SetPropertyHelpString(floatName,floatDescription );
	  			}
	  		break;

				case CSVAR_BOOL:
	  			{
	    			wxString boolDescription (option->description, wxConvUTF8);
	    			wxString boolName (option->name, wxConvUTF8);
						wxBoolProperty* boolP = new wxBoolProperty(boolName);
	    			page->Append (boolP);
	    			pgMan->SetPropertyAttribute (boolP, wxPG_BOOL_USE_CHECKBOX, (long)1, wxPG_RECURSE);	
	    			boolP->SetValue (node->GetParameter (i)->GetBool ());
		   			page->SetPropertyHelpString(boolName,boolDescription );
	  			}
	  		break;
	
				case CSVAR_COLOR :
					{
						wxString colorDescription(option->description, wxConvUTF8);
						wxString colorName (option->name, wxConvUTF8);
						csColor colorValue(node->GetParameter (i)->GetColor ());
						int red  = colorValue[0]*255;
						int blue  = colorValue[1]*255;
						int green = colorValue[2]*255;
						wxColourProperty* colorP = new wxColourProperty (colorName,wxPG_LABEL,wxColour (red,blue,green) );
			 			page->Append (colorP);
						page->SetPropertyHelpString(colorName,colorDescription );

		 			}
				break;
	
				case CSVAR_VECTOR3 :
					{
						wxString vector3Description(option->description, wxConvUTF8);
						wxString vector3Name (option->name, wxConvUTF8);
						csVector3 vector3Value (node->GetParameter (i)->GetVector3());
						double x = vector3Value.x;
						double y = vector3Value.y;
						double z = vector3Value.z;
						wxVectorProperty *vector3P = new wxVectorProperty(vector3Name,wxT("csVector3"),wxVector3f(x,y,z));
						page->Append (vector3P);
						page->SetPropertyHelpString(vector3Name,vector3Description );

					}
				break;

				case CSVAR_VECTOR2 :
					{
						wxString vector2Description(option->description, wxConvUTF8);
						wxString vector2Name (option->name, wxConvUTF8);
						csVector2 vector2Value (node->GetParameter (i)->GetVector2());
						double x = vector2Value.x;
						double y = vector2Value.y;
						wxVector2Property *vector2P = new wxVector2Property(vector2Name,wxT("csVector2"),wxVector2f(x,y));
						page->Append (vector2P);
						page->SetPropertyHelpString(vector2Name,vector2Description );

					}
				break;
				
				case CSVAR_VECTOR4 :
					{
						wxString vector4Description(option->description, wxConvUTF8);
						wxString vector4Name (option->name, wxConvUTF8);
						csVector4 vector4Value (node->GetParameter (i)->GetVector4());
						double x = vector4Value.x;
						double y = vector4Value.y;
						double z = vector4Value.z;
						double w = vector4Value.w;
						wxVector4Property *vector4P = new wxVector4Property(vector4Name,wxT("csVector4"),wxVector4f(x,y,z,w));
						page->Append (vector4P);
						page->SetPropertyHelpString(vector4Name,vector4Description );

					}
				break;
				
	
				case CSVAR_VFSPATH :
					{
						wxString pathDescription (option->description, wxConvUTF8);
						wxString pathName (option->name, wxConvUTF8);
						wxString pathValue (node->GetParameter (i)->GetVFSPath (), wxConvUTF8);
						wxFileProperty* pathP = new wxFileProperty (pathName);
    				page->Append (pathP );
						//pathP->SetAttribute(wxPG_FILE_SHOW_FULL_PATH,false);
						pathP->SetValue (pathValue);
						page->SetPropertyHelpString(pathName,pathDescription );

					}
			default:
	  				pgMan->SetDescription(wxT("Page Manager :"), wxT("Select a property to add a new value"));
			}
    
  	}
	}
  else
  {
    pgMan->SetDescription(wxT("Page Manager :"), wxT("Error page no added"));
  }
}

//---------------------------------------------------

void PropGridManager::OnGetNewValue(wxPGProperty* property)
{
  wxVariant newValue = property->GetValue ();
	if (newValue.IsNull ())
    return;

  size_t index = property->GetIndexInParent ();
  csVariant* variant = node->GetParameter (index);
	const csOptionDescription* compareValue = nodeFactory->GetParameterDescription (index);
	csVariantType compareType = compareValue->type;
  csVariant oldValue = *variant;
	csVariant valueVar;

  if (compareType == CSVAR_STRING)
  {
    valueVar.SetString (newValue.GetString ().mb_str ());
    node->UpdateParameter (index, &oldValue, &valueVar);
   
  }

  else if (compareType == CSVAR_LONG)
  {	
    valueVar.SetLong (newValue.GetLong ());
    node->UpdateParameter (index, &oldValue, &valueVar); 
    
  }

  else if (compareType == CSVAR_FLOAT)
  {
    valueVar.SetFloat (newValue.GetDouble ());
    node->UpdateParameter (index, &oldValue, &valueVar); 
    
  }

  else if (compareType == CSVAR_BOOL)
  {
    valueVar.SetBool (newValue.GetBool ());
    node->UpdateParameter (index, &oldValue, &valueVar);
    
	
  }

/// !!!!!le problème se trouve ici !!!!
	else if (compareType == CSVAR_COLOR)
	{		
		//wxVariant v_txcol = page->GetPropertyValue (property);
    //const wxColour *txcol = wxGetVariantCast (v_txcol,wxColour);
		//valueVar.SetColor (csColor(txcol->Red (),txcol->Green (),txcol->Blue ()));
		//node->UpdateParameter (index, &oldValue, &valueVar);
		 
		 

	}
	else if (compareType == CSVAR_VECTOR3)
	{

		float valueX = property->Item(0)->GetValue().GetDouble ();
		float valueY = property->Item(1)->GetValue().GetDouble ();
		float valueZ = property->Item(2)->GetValue().GetDouble ();
		valueVar.SetVector3(csVector3(valueX,valueY,valueZ));
		node->UpdateParameter (index, &oldValue, &valueVar);
				
	}
	else if (compareType == CSVAR_VECTOR4)
	{

		float valueX = property->Item(0)->GetValue().GetDouble ();
		float valueY = property->Item(1)->GetValue().GetDouble ();
		float valueZ = property->Item(2)->GetValue().GetDouble ();
		float valueW = property->Item(3)->GetValue().GetDouble ();
		valueVar.SetVector4(csVector4(valueX,valueY,valueZ,valueW));
		node->UpdateParameter (index, &oldValue, &valueVar);
				
	}
	else if (compareType == CSVAR_VECTOR2)
	{

		float valueX = property->Item(0)->GetValue().GetDouble ();
		float valueY = property->Item(1)->GetValue().GetDouble ();
		valueVar.SetVector2(csVector2(valueX,valueY));
		node->UpdateParameter (index, &oldValue, &valueVar);
				
	}
	else if (compareType == CSVAR_VFSPATH)
	{
		valueVar.SetVFSPath (newValue.GetString ().mb_str ());
    node->UpdateParameter (index, &oldValue, &valueVar);
	}
  else
  {
    pgMan->SetDescription (wxT ("Page Manager :"), wxT ("Message test"));
  }

	
}


void PropGridManager::OnPropertyGridChanging (wxPropertyGridEvent& event)
{
  wxPGProperty* property = event.GetProperty ();
  OnGetNewValue (property);
  //wxVariant newValue = property->GetValue ();
}



PropGridManager::~PropGridManager()
{
}


void PropGridManager::OnPopulateClick (wxCommandEvent &event )
{
	Populate ();
	pgMan->SetFocus ();
}

