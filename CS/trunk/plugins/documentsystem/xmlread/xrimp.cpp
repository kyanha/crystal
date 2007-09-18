/*
    Copyright (C) 2002 by Jorrit Tyberghein

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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "csutil/util.h"
#include "xriface.h"
#include "xrpriv.h"
#include "csutil/scfstr.h"
#include "iutil/vfs.h"
#include "iutil/string.h"
#include "iutil/databuff.h"
#include "xr.h"

CS_PLUGIN_NAMESPACE_BEGIN(XMLRead)
{

//------------------------------------------------------------------------

csXmlReadDocumentSystem::csXmlReadDocumentSystem (iBase* parent) :
  scfImplementationType(this), parent(parent)
{
}

csXmlReadDocumentSystem::~csXmlReadDocumentSystem ()
{
}

csRef<iDocument> csXmlReadDocumentSystem::CreateDocument ()
{
  csRef<iDocument> doc (csPtr<iDocument> (new csXmlReadDocument (this)));
  return doc;
}

//------------------------------------------------------------------------

csXmlReadAttributeIterator::csXmlReadAttributeIterator (TrDocumentNode* parent) :
  scfImplementationType(this)
{
  csXmlReadAttributeIterator::parent = parent ? parent->ToElement () : 0;
  if (csXmlReadAttributeIterator::parent == 0)
  {
    current = csArrayItemNotFound;
    return;
  }
  count = csXmlReadAttributeIterator::parent->GetAttributeCount ();
  if (!count) 
  {
    current = csArrayItemNotFound;
    return;
  }
  current = 0;
}

csXmlReadAttributeIterator::~csXmlReadAttributeIterator()
{
}

bool csXmlReadAttributeIterator::HasNext ()
{
  return current != csArrayItemNotFound;
}

csRef<iDocumentAttribute> csXmlReadAttributeIterator::Next ()
{
  csRef<iDocumentAttribute> attr;
  if (current != csArrayItemNotFound)
  {
    attr = csPtr<iDocumentAttribute> (new csXmlReadAttribute (
    	&parent->GetAttribute (current)));
    current++;
    if (current >= count)
      current = csArrayItemNotFound;
  }
  return attr;
}

//------------------------------------------------------------------------

csXmlReadNodeIterator::csXmlReadNodeIterator (
	csXmlReadDocument* doc, TrDocumentNodeChildren* parent,
	const char* value) :
  scfImplementationType(this), currentPos (0), endPos ((size_t)~0)
{
  csXmlReadNodeIterator::doc = doc;
  csXmlReadNodeIterator::parent = parent;
  csXmlReadNodeIterator::value = value ? CS::StrDup (value) : 0;
  use_contents_value = false;
  if (!parent)
    current = 0;
  else if (value)
    current = parent->FirstChild (value);
  else
  {
    if (parent->ToElement () && parent->ToElement ()->GetContentsValue ())
    {
      use_contents_value = true;
      current = parent;
    }
    else
    {
      current = parent->FirstChild ();
    }
  }
}

csXmlReadNodeIterator::~csXmlReadNodeIterator ()
{
  cs_free (value);
}

bool csXmlReadNodeIterator::HasNext ()
{
  return use_contents_value || current != 0;
}

csRef<iDocumentNode> csXmlReadNodeIterator::Next ()
{
  csRef<iDocumentNode> node;
  if (use_contents_value)
  {
    node = csPtr<iDocumentNode> (doc->Alloc (current, true));
    use_contents_value = false;
    current = parent->FirstChild ();
    currentPos++;
  }
  else if (current != 0)
  {
    node = csPtr<iDocumentNode> (doc->Alloc (current, false));
    if (value)
      current = current->NextSibling (value);
    else
      current = current->NextSibling ();
    currentPos++;
  }
  return node;
}

size_t csXmlReadNodeIterator::GetEndPosition ()
{
  if (endPos == (size_t)~0)
  {
    if (use_contents_value)
      endPos = 1;
    else
    {
      endPos = currentPos;
      TrDocumentNode* node = current;
      while (node != 0)
      {
        endPos++;
        node = node->NextSibling ();
      }
    }
  }
  return endPos;
}

//------------------------------------------------------------------------

void csXmlReadNode::DecRef ()
{
  CS_ASSERT_MSG("Refcount decremented for destroyed object", 
    scfRefCount != 0);
  csRefTrackerAccess::TrackDecRef (scfObject, scfRefCount);
  scfRefCount--;
  if (scfRefCount == 0)
  {
    scfRemoveRefOwners ();
    if (scfParent) scfParent->DecRef();
    doc->Free (this);
  }
}

csXmlReadNode::csXmlReadNode (csXmlReadDocument* doc) :
  scfImplementationType(this)
{
  node = 0;
  node_children = 0;
  csXmlReadNode::doc = doc;	// Increase reference.
}

csXmlReadNode::~csXmlReadNode ()
{
}

csRef<iDocumentNode> csXmlReadNode::GetParent ()
{
  csRef<iDocumentNode> child;
  if (use_contents_value)
  {
    // If we use contents value then the parent is actually this object.
    IncRef ();
    return csPtr<iDocumentNode> (this);
  }
  else
  {
    if (!node->Parent ()) return child;
    child = csPtr<iDocumentNode> (doc->Alloc (node->Parent (), false));
    return child;
  }
  
  return 0;
}

csDocumentNodeType csXmlReadNode::GetType ()
{
  if (use_contents_value) return CS_NODE_TEXT;

  switch (node->Type ())
  {
    case TrDocumentNode::DOCUMENT: return CS_NODE_DOCUMENT;
    case TrDocumentNode::ELEMENT: return CS_NODE_ELEMENT;
    case TrDocumentNode::COMMENT: return CS_NODE_COMMENT;
    case TrDocumentNode::CDATA:
    case TrDocumentNode::TEXT:
      return CS_NODE_TEXT;
    case TrDocumentNode::DECLARATION: return CS_NODE_DECLARATION;
    default: return CS_NODE_UNKNOWN;
  }
}

bool csXmlReadNode::Equals (iDocumentNode* other)
{
  csXmlReadNode* other_node = (csXmlReadNode*)other;
  return GetTiNode () == other_node->GetTiNode ()
  	&& use_contents_value == other_node->use_contents_value;
}

const char* csXmlReadNode::GetValue ()
{
  if (use_contents_value)
  {
    return node->ToElement ()->GetContentsValue ();
  }
  else
  {
    return node->Value ();
  }
}

csRef<iDocumentNodeIterator> csXmlReadNode::GetNodes ()
{
  csRef<iDocumentNodeIterator> it;
  it = csPtr<iDocumentNodeIterator> (new csXmlReadNodeIterator (
  	doc, use_contents_value ? 0 : node_children, 0));
  return it;
}

csRef<iDocumentNodeIterator> csXmlReadNode::GetNodes (const char* value)
{
  csRef<iDocumentNodeIterator> it;
  it = csPtr<iDocumentNodeIterator> (new csXmlReadNodeIterator (
  	doc, use_contents_value ? 0 : node_children, value));
  return it;
}

csRef<iDocumentNode> csXmlReadNode::GetNode (const char* value)
{
  if (!node_children || use_contents_value) return 0;
  csRef<iDocumentNode> child;
  TrDocumentNode* c = node_children->FirstChild (value);
  if (!c) return child;
  child = csPtr<iDocumentNode> (doc->Alloc (c, false));
  return child;
}

const char* csXmlReadNode::GetContentsValue ()
{
  if (!node_children || use_contents_value) return 0;
  TrXmlElement* el = node->ToElement ();
  if (el && el->GetContentsValue ())
  {
    return el->GetContentsValue ();
  }

  TrDocumentNode* child = node_children->FirstChild ();
  while (child)
  {
    if (child->Type () == TrDocumentNode::TEXT ||
    	child->Type () == TrDocumentNode::CDATA)
    {
      return child->Value ();
    }
    child = child->NextSibling ();
  } 
  return 0;
}

int csXmlReadNode::GetContentsValueAsInt ()
{
  const char* v = GetContentsValue ();
  if (!v) return 0;
  int val = 0;
  sscanf (v, "%d", &val);
  return val;
}

float csXmlReadNode::GetContentsValueAsFloat ()
{
  const char* v = GetContentsValue ();
  if (!v) return 0;
  float val = 0.0;
  sscanf (v, "%f", &val);
  return val;
}

csRef<iDocumentAttributeIterator> csXmlReadNode::GetAttributes ()
{
  csRef<iDocumentAttributeIterator> it;
  it = csPtr<iDocumentAttributeIterator> (
    new csXmlReadAttributeIterator (use_contents_value ? 0 : node));
  return it;
}

TrDocumentAttribute* csXmlReadNode::GetAttributeInternal (const char* name)
{
  if (use_contents_value) return 0;
  const TrXmlElement* elem = node->ToElement ();
  if (!elem) return 0;
  size_t count = elem->GetAttributeCount ();
  size_t i;
  for (i = 0 ; i < count ; i++)
  {
    TrDocumentAttribute& attrib = node->ToElement ()->GetAttribute (i);
    if (strcmp (name, attrib.Name ()) == 0)
      return &attrib;
  }

  return 0;
}

csRef<iDocumentAttribute> csXmlReadNode::GetAttribute (const char* name)
{
  if (use_contents_value) return 0;
  csRef<iDocumentAttribute> attr;
  TrDocumentAttribute* a = GetAttributeInternal (name);
  if (a)
  {
    attr = csPtr<iDocumentAttribute> (new csXmlReadAttribute (a));
  }
  return attr;
}

const char* csXmlReadNode::GetAttributeValue (const char* name)
{
  if (use_contents_value) return 0;
  TrXmlElement* el = node->ToElement ();
  if (el) return el->Attribute (name);
  else return 0;
}

int csXmlReadNode::GetAttributeValueAsInt (const char* name)
{
  TrDocumentAttribute* a = GetAttributeInternal (name);
  if (!a) return 0;
  return a->IntValue ();
}

float csXmlReadNode::GetAttributeValueAsFloat (const char* name)
{
  TrDocumentAttribute* a = GetAttributeInternal (name);
  if (!a) return 0;
  float f;
  sscanf (a->Value (), "%f", &f);
  return f;
}

bool csXmlReadNode::GetAttributeValueAsBool (const char* name,bool defaultvalue)
{
  TrDocumentAttribute* a = GetAttributeInternal (name);
  if (!a || !a->Value () ) return defaultvalue;
  if (strcasecmp(a->Value(),"true")==0 ||
      strcasecmp(a->Value(),"yes")==0 ||
      atoi(a->Value())!=0)
  {
    return true;
  }
  else
    return false;
}

//------------------------------------------------------------------------

csXmlReadDocument::csXmlReadDocument (csXmlReadDocumentSystem* sys) :
  scfImplementationType(this)
{
  csXmlReadDocument::sys = sys;	// Increase ref.
  pool = 0;
  root = 0;
}

csXmlReadDocument::~csXmlReadDocument ()
{
  Clear ();
  while (pool)
  {
    csXmlReadNode* n = pool->next_pool;
    // The 'sys' member in pool should be 0 here.
    delete pool;
    pool = n;
  }
}

void csXmlReadDocument::Clear ()
{
  if (!root) return;
  delete root;
  root = 0;
}

csRef<iDocumentNode> csXmlReadDocument::CreateRoot (char* buf)
{
  Clear ();
  root = new TrDocument (buf);
  return csPtr<iDocumentNode> (Alloc (root, false));
}

csRef<iDocumentNode> csXmlReadDocument::CreateRoot ()
{
  Clear ();
  root = new TrDocument ();
  return csPtr<iDocumentNode> (Alloc (root, false));
}

csRef<iDocumentNode> csXmlReadDocument::GetRoot ()
{
  return csPtr<iDocumentNode> (Alloc (root, false));
}

const char* csXmlReadDocument::Parse (iFile* file, bool collapse)
{
  size_t want_size = file->GetSize ();
  char *data = (char*)cs_malloc (want_size + 1);
  size_t real_size = file->Read (data, want_size);
  if (want_size != real_size)
  {
    cs_free (data);
    return "Unexpected EOF encountered";
  }
  data[real_size] = '\0';
#ifdef CS_DEBUG
  if (strlen (data) != real_size)
  {
    cs_free (data);
    return "File contains one or more null characters";
  }
#endif
  const char *error = Parse (data, collapse);
  cs_free (data);
  return error;
}

const char* csXmlReadDocument::Parse (iDataBuffer* buf, bool collapse)
{
  return Parse ((const char*)buf->GetData (), collapse);
}

const char* csXmlReadDocument::Parse (iString* str, bool collapse)
{
  return Parse ((const char*)*str, collapse);
}

const char* csXmlReadDocument::Parse (const char* buf, bool collapse)
{
  CreateRoot (CS::StrDup (buf));
  root->SetCondenseWhiteSpace(collapse);
  root->Parse (root->input_data);
  if (root->Error ())
    return root->ErrorDesc ();
  return 0;
}

const char* csXmlReadDocument::ParseInPlace (char* buf, bool collapse)
{
  CreateRoot (buf);
  root->SetCondenseWhiteSpace(collapse);
  root->Parse (root->input_data);
  if (root->Error ())
    return root->ErrorDesc ();
  return 0;
}

csXmlReadNode* csXmlReadDocument::Alloc ()
{
  if (pool)
  {
    csXmlReadNode* n = pool;
    pool = n->next_pool;
    n->scfRefCount = 1;
    n->doc = this;	// Incref.
    return n;
  }
  else
  {
    csXmlReadNode* n = new csXmlReadNode (this);
    return n;
  }
}

csXmlReadNode* csXmlReadDocument::Alloc (TrDocumentNode* node,
	bool use_contents_value)
{
  csXmlReadNode* n = Alloc ();
  n->SetTiNode (node, use_contents_value);
  return n;
}

void csXmlReadDocument::Free (csXmlReadNode* n)
{
  n->next_pool = pool;
  pool = n;
  n->doc = 0;	// Free ref.
}

//------------------------------------------------------------------------


}
CS_PLUGIN_NAMESPACE_END(XMLRead)
