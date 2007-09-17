/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

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
#include "iutil/comp.h"
#include "iutil/databuff.h"
#include "iutil/vfs.h"
#include "csutil/scf.h"
#include "csutil/weakref.h"
#include "xriface.h"
#include "xrpriv.h"

CS_PLUGIN_NAMESPACE_BEGIN(XMLRead)
{

struct csXmlReadDocWrapper :
  public scfImplementation1<csXmlReadDocWrapper, iDocument>
{
private:
  csRef<iDocument> xmlreaddoc;

public:
  csXmlReadDocWrapper (csRef<iDocument> doc);
  virtual ~csXmlReadDocWrapper ();

  virtual void Clear ();
  virtual csRef<iDocumentNode> CreateRoot ();
  virtual csRef<iDocumentNode> GetRoot ();
  virtual const char* Parse (iFile* file,      bool collapse = false);
  virtual const char* Parse (iDataBuffer* buf, bool collapse = false);
  virtual const char* Parse (iString* str,     bool collapse = false);
  virtual const char* Parse (const char* buf,  bool collapse = false);
  const char* ParseInPlace (char* buf, bool collapse = false);
  virtual const char* Write (iFile* file);
  virtual const char* Write (iString* str);
  virtual const char* Write (iVFS* vfs, const char* filename);

  virtual int Changeable ();
};

csXmlReadDocWrapper::csXmlReadDocWrapper (csRef<iDocument> doc) :
  scfImplementationType(this)
{
  xmlreaddoc = doc;
}

csXmlReadDocWrapper::~csXmlReadDocWrapper ()
{
}

void csXmlReadDocWrapper::Clear ()
{
  xmlreaddoc->Clear();
}

csRef<iDocumentNode> csXmlReadDocWrapper::CreateRoot ()
{
  return 0;
}

csRef<iDocumentNode> csXmlReadDocWrapper::GetRoot ()
{
  return xmlreaddoc->GetRoot();
}

const char* csXmlReadDocWrapper::Parse (iFile* file, bool collapse)
{
  char *buf = (char*)cs_malloc (file->GetSize()+1);
  file->Read (buf, file->GetSize());
  buf[file->GetSize ()] = 0;
  const char *ret = ParseInPlace (buf, collapse);
  return ret;
}

const char* csXmlReadDocWrapper::Parse (iDataBuffer* buf, bool collapse)
{
  return Parse ((const char*)buf->GetData(), collapse);
}

const char* csXmlReadDocWrapper::Parse (iString* str, bool collapse)
{
  return Parse ((const char*)str, collapse);
}

const char* csXmlReadDocWrapper::Parse (const char* buf, bool collapse)
{
  const char* b = buf;
  while ((*b == ' ') || (*b == '\n') || (*b == '\t') || 
    (*b == '\r')) b++;
  if (*b == '<')
  {
    return xmlreaddoc->Parse (buf, collapse);
  }
  else
  {
    return "Data does not seem to be XML.";
  }
}

const char* csXmlReadDocWrapper::ParseInPlace (char* buf, bool collapse)
{
  char* b = buf;
  while ((*b == ' ') || (*b == '\n') || (*b == '\t') || 
    (*b == '\r')) b++;
  if (*b == '<')
  {
    return ((csXmlReadDocument*)(iDocument*)xmlreaddoc)->ParseInPlace (
      buf, collapse);
  }
  else
  {
    cs_free (buf);
    return "Data does not seem to be XML.";
  }
}

const char* csXmlReadDocWrapper::Write (iFile*)
{
  return "Writing not supported by this plugin!";
}

const char* csXmlReadDocWrapper::Write (iString*)
{
  return "Writing not supported by this plugin!";
}

const char* csXmlReadDocWrapper::Write (iVFS*, const char*)
{
  return "Writing not supported by this plugin!";
}

int csXmlReadDocWrapper::Changeable ()
{
  return xmlreaddoc->Changeable();
}

class csXmlReadXMLPlugin :
  public scfImplementation2<csXmlReadXMLPlugin,
    iDocumentSystem, iComponent>
{
private:
  csWeakRef<csXmlReadDocumentSystem> xmlread;

public:
  csXmlReadXMLPlugin (iBase* parent = 0);
  virtual ~csXmlReadXMLPlugin ();

  virtual bool Initialize (iObjectRegistry* objreg);

  csRef<iDocument> CreateDocument ();
};

csXmlReadXMLPlugin::csXmlReadXMLPlugin(iBase* parent) :
  scfImplementationType(this, parent)
{
}

csXmlReadXMLPlugin::~csXmlReadXMLPlugin()
{
}

bool csXmlReadXMLPlugin::Initialize (iObjectRegistry* /*objreg*/)
{
  return true;
}

csRef<iDocument> csXmlReadXMLPlugin::CreateDocument ()
{
  csRef<csXmlReadDocumentSystem> xmlread;
  xmlread = csXmlReadXMLPlugin::xmlread;
  if (!xmlread.IsValid())
  {
    xmlread.AttachNew (new csXmlReadDocumentSystem ((iComponent*)this));
    csXmlReadXMLPlugin::xmlread = xmlread;
  }
  return csPtr<iDocument> (new csXmlReadDocWrapper(xmlread->CreateDocument()));
}

SCF_IMPLEMENT_FACTORY (csXmlReadXMLPlugin)

}
CS_PLUGIN_NAMESPACE_END(XMLRead)

CS_IMPLEMENT_PLUGIN
