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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "cssysdef.h"
#include "csutil/util.h"
#include "csutil/xmltiny.h"
#include "csutil/scfstr.h"
#include "xmltinyp.h"
#include "iutil/vfs.h"
#include "iutil/string.h"
#include "iutil/databuff.h"
#include "tinyxml.h"

//------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csTinyDocumentSystem)
  SCF_IMPLEMENTS_INTERFACE (iDocumentSystem)
SCF_IMPLEMENT_IBASE_END

csTinyDocumentSystem::csTinyDocumentSystem ()
{
  SCF_CONSTRUCT_IBASE (NULL);
}

csTinyDocumentSystem::~csTinyDocumentSystem ()
{
}

csRef<iDocument> csTinyDocumentSystem::CreateDocument ()
{
  csRef<iDocument> doc (csPtr<iDocument> (new csTinyXmlDocument (this)));
  return doc;
}

//------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csTinyXmlAttributeIterator)
  SCF_IMPLEMENTS_INTERFACE (iDocumentAttributeIterator)
SCF_IMPLEMENT_IBASE_END

csTinyXmlAttributeIterator::csTinyXmlAttributeIterator (TiDocumentNode* parent)
{
  SCF_CONSTRUCT_IBASE (NULL);
  csTinyXmlAttributeIterator::parent = parent->ToElement ();
  if (csTinyXmlAttributeIterator::parent == NULL)
  {
    current = -1;
    return;
  }
  count = csTinyXmlAttributeIterator::parent->GetAttributeCount ();
  if (!count) 
  {
    current = -1;
    return;
  }
  current = 0;
}

bool csTinyXmlAttributeIterator::HasNext ()
{
  return current != -1;
}

csRef<iDocumentAttribute> csTinyXmlAttributeIterator::Next ()
{
  csRef<iDocumentAttribute> attr;
  if (current != -1)
  {
    attr = csPtr<iDocumentAttribute> (new csTinyXmlAttribute (
    	&parent->GetAttribute (current)));
    current++;
    if (current >= count)
      current = -1;
  }
  return attr;
}

//------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csTinyXmlNodeIterator)
  SCF_IMPLEMENTS_INTERFACE (iDocumentNodeIterator)
SCF_IMPLEMENT_IBASE_END

csTinyXmlNodeIterator::csTinyXmlNodeIterator (
	csTinyXmlDocument* doc, TiDocumentNodeChildren* parent,
	const char* value)
{
  SCF_CONSTRUCT_IBASE (NULL);
  csTinyXmlNodeIterator::doc = doc;
  csTinyXmlNodeIterator::parent = parent;
  csTinyXmlNodeIterator::value = value ? csStrNew (value) : NULL;
  if (!parent)
    current = NULL;
  else if (value)
    current = parent->FirstChild (value);
  else
    current = parent->FirstChild ();
}

bool csTinyXmlNodeIterator::HasNext ()
{
  return current != NULL;
}

csRef<iDocumentNode> csTinyXmlNodeIterator::Next ()
{
  csRef<iDocumentNode> node;
  if (current != NULL)
  {
    node = csPtr<iDocumentNode> (doc->Alloc (current));
    if (value)
      current = current->NextSibling (value);
    else
      current = current->NextSibling ();
  }
  return node;
}

//------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csTinyXmlAttribute)
  SCF_IMPLEMENTS_INTERFACE (iDocumentAttribute)
SCF_IMPLEMENT_IBASE_END

//------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE_INCREF(csTinyXmlNode)
void csTinyXmlNode::DecRef ()
{
  scfRefCount--;
  if (scfRefCount <= 0)
  {
    if (scfParent) scfParent->DecRef ();
    doc->Free (this);
  }
}
SCF_IMPLEMENT_IBASE_GETREFCOUNT(csTinyXmlNode)
SCF_IMPLEMENT_IBASE_QUERY(csTinyXmlNode)
  SCF_IMPLEMENTS_INTERFACE (iDocumentNode)
SCF_IMPLEMENT_IBASE_END

csTinyXmlNode::csTinyXmlNode (csTinyXmlDocument* doc)
{
  SCF_CONSTRUCT_IBASE (NULL);
  node = NULL;
  node_children = NULL;
  csTinyXmlNode::doc = doc;	// Increase reference.
}

csTinyXmlNode::~csTinyXmlNode ()
{
}

csRef<iDocumentNode> csTinyXmlNode::GetParent ()
{
  csRef<iDocumentNode> child;
  if (!node->Parent ()) return child;
  child = csPtr<iDocumentNode> (doc->Alloc (node->Parent ()));
  return child;
}

csDocumentNodeType csTinyXmlNode::GetType ()
{
  switch (node->Type ())
  {
    case TiDocumentNode::DOCUMENT: return CS_NODE_DOCUMENT;
    case TiDocumentNode::ELEMENT: return CS_NODE_ELEMENT;
    case TiDocumentNode::COMMENT: return CS_NODE_COMMENT;
    case TiDocumentNode::CDATA:
    case TiDocumentNode::TEXT:
      return CS_NODE_TEXT;
    case TiDocumentNode::DECLARATION: return CS_NODE_DECLARATION;
    default: return CS_NODE_UNKNOWN;
  }
}

bool csTinyXmlNode::Equals (iDocumentNode* other)
{
  csTinyXmlNode* other_node = (csTinyXmlNode*)other;
  return GetTiNode () == other_node->GetTiNode ();
}

const char* csTinyXmlNode::GetValue ()
{
  return node->Value ();
}

void csTinyXmlNode::SetValue (const char* value)
{
  node->SetValue (value);
}

void csTinyXmlNode::SetValueAsInt (int value)
{
  char buf[40];
  sprintf (buf, "%d", value);
  node->SetValue (buf);
}

void csTinyXmlNode::SetValueAsFloat (float value)
{
  char buf[40];
  sprintf (buf, "%g", value);
  node->SetValue (buf);
}

csRef<iDocumentNodeIterator> csTinyXmlNode::GetNodes ()
{
  csRef<iDocumentNodeIterator> it;
  it = csPtr<iDocumentNodeIterator> (new csTinyXmlNodeIterator (
  	doc, node_children, NULL));
  return it;
}

csRef<iDocumentNodeIterator> csTinyXmlNode::GetNodes (const char* value)
{
  csRef<iDocumentNodeIterator> it;
  it = csPtr<iDocumentNodeIterator> (new csTinyXmlNodeIterator (
  	doc, node_children, value));
  return it;
}

csRef<iDocumentNode> csTinyXmlNode::GetNode (const char* value)
{
  if (!node_children) return NULL;
  csRef<iDocumentNode> child;
  TiDocumentNode* c = node_children->FirstChild (value);
  if (!c) return child;
  child = csPtr<iDocumentNode> (doc->Alloc (c));
  return child;
}

void csTinyXmlNode::RemoveNode (const csRef<iDocumentNode>& child)
{
  //CS_ASSERT (child.IsValid ());
  if (node_children)
    node_children->RemoveChild (
  	((csTinyXmlNode*)(iDocumentNode*)child)->GetTiNode ());
}

void csTinyXmlNode::RemoveNodes ()
{
  if (node_children) node_children->Clear ();
}

csRef<iDocumentNode> csTinyXmlNode::CreateNodeBefore (csDocumentNodeType type,
	iDocumentNode* before)
{
  if (!node_children) return NULL;
  csRef<iDocumentNode> n;
  TiDocumentNode* child = NULL;
  switch (type)
  {
    case CS_NODE_DOCUMENT:
      break;
    case CS_NODE_ELEMENT:
      {
        TiXmlElement el;
	if (before)
	  child = node_children->InsertBeforeChild (
	  	((csTinyXmlNode*)(iDocumentNode*)before)->GetTiNode (),
		el);
        else
	  child = node_children->InsertEndChild (el);
        //CS_ASSERT (child != NULL);
      }
      break;
    case CS_NODE_COMMENT:
      {
        TiXmlComment el;
	if (before)
	  child = node_children->InsertBeforeChild (
	  	((csTinyXmlNode*)(iDocumentNode*)before)->GetTiNode (),
		el);
        else
	  child = node_children->InsertEndChild (el);
        //CS_ASSERT (child != NULL);
      }
      break;
    case CS_NODE_TEXT:
      {
        TiXmlText el;
	if (before)
	  child = node_children->InsertBeforeChild (
	  	((csTinyXmlNode*)(iDocumentNode*)before)->GetTiNode (),
		el);
        else
	  child = node_children->InsertEndChild (el);
        //CS_ASSERT (child != NULL);
      }
      break;
    case CS_NODE_DECLARATION:
      {
        TiXmlDeclaration el;
	if (before)
	  child = node_children->InsertBeforeChild (
	  	((csTinyXmlNode*)(iDocumentNode*)before)->GetTiNode (),
		el);
        else
	  child = node_children->InsertEndChild (el);
        //CS_ASSERT (child != NULL);
      }
      break;
    case CS_NODE_UNKNOWN:
      {
        TiXmlUnknown el;
	if (before)
	  child = node_children->InsertBeforeChild (
	  	((csTinyXmlNode*)(iDocumentNode*)before)->GetTiNode (),
		el);
        else
	  child = node_children->InsertEndChild (el);
        //CS_ASSERT (child != NULL);
      }
      break;
    default:
      break;
  }
  if (child)
    n = csPtr<iDocumentNode> (doc->Alloc (child));
  return n;
}

const char* csTinyXmlNode::GetContentsValue ()
{
  if (!node_children) return NULL;
  TiDocumentNode* child = node_children->FirstChild ();
  while (child)
  {
    if ((child->Type () == TiDocumentNode::TEXT)
      || (child->Type () == TiDocumentNode::CDATA))
    {
      return child->Value ();
    }
    child = child->NextSibling ();
  } 
  return NULL;
}

int csTinyXmlNode::GetContentsValueAsInt ()
{
  const char* v = GetContentsValue ();
  if (!v) return 0;
  int val = 0;
  sscanf (v, "%d", &val);
  return val;
}

float csTinyXmlNode::GetContentsValueAsFloat ()
{
  const char* v = GetContentsValue ();
  if (!v) return 0;
  float val = 0.0;
  sscanf (v, "%f", &val);
  return val;
}

csRef<iDocumentAttributeIterator> csTinyXmlNode::GetAttributes ()
{
  csRef<iDocumentAttributeIterator> it;
  it = csPtr<iDocumentAttributeIterator> (
  	new csTinyXmlAttributeIterator (node));
  return it;
}

TiDocumentAttribute* csTinyXmlNode::GetAttributeInternal (const char* name)
{
  int count = node->ToElement ()->GetAttributeCount ();
  int i;
  for (i = 0 ; i < count ; i++)
  {
    TiDocumentAttribute& attrib = node->ToElement ()->GetAttribute (i);
    if (strcmp (name, attrib.Name ()) == 0)
      return &attrib;
  }

  return NULL;
}

csRef<iDocumentAttribute> csTinyXmlNode::GetAttribute (const char* name)
{
  csRef<iDocumentAttribute> attr;
  TiDocumentAttribute* a = GetAttributeInternal (name);
  if (a)
  {
    attr = csPtr<iDocumentAttribute> (new csTinyXmlAttribute (a));
  }
  return attr;
}

const char* csTinyXmlNode::GetAttributeValue (const char* name)
{
  TiXmlElement* el = node->ToElement ();
  if (el) return el->Attribute (name);
  else return NULL;
}

int csTinyXmlNode::GetAttributeValueAsInt (const char* name)
{
  TiDocumentAttribute* a = GetAttributeInternal (name);
  if (!a) return 0;
  return a->IntValue ();
}

float csTinyXmlNode::GetAttributeValueAsFloat (const char* name)
{
  TiDocumentAttribute* a = GetAttributeInternal (name);
  if (!a) return 0;
  float f;
  sscanf (a->Value (), "%f", &f);
  return f;
}

void csTinyXmlNode::RemoveAttribute (const csRef<iDocumentAttribute>&)
{
  // @@@ TODO
}

void csTinyXmlNode::RemoveAttributes ()
{
  // @@@ TODO
}

void csTinyXmlNode::SetAttribute (const char* name, const char* value)
{
  TiXmlElement* el = node->ToElement ();
  if (el) el->SetAttribute (el->GetDocument (), name, value);
}

void csTinyXmlNode::SetAttributeAsInt (const char* name, int value)
{
  TiXmlElement* el = node->ToElement ();
  if (el) el->SetAttribute (el->GetDocument (), name, value);
}

void csTinyXmlNode::SetAttributeAsFloat (const char* name, float value)
{
  TiXmlElement* el = node->ToElement ();
  if (el)
  {
    char v[64];
    sprintf (v, "%g", value);
    el->SetAttribute (el->GetDocument (), name, v);
  }
}

//------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (csTinyXmlDocument)
  SCF_IMPLEMENTS_INTERFACE (iDocument)
SCF_IMPLEMENT_IBASE_END

csTinyXmlDocument::csTinyXmlDocument (csTinyDocumentSystem* sys)
{
  SCF_CONSTRUCT_IBASE (NULL);
  csTinyXmlDocument::sys = sys;	// Increase ref.
  pool = NULL;
  root = NULL;
}

csTinyXmlDocument::~csTinyXmlDocument ()
{
  Clear ();
  while (pool)
  {
    csTinyXmlNode* n = pool->next_pool;
    // The 'sys' member in pool should be NULL here.
    delete pool;
    pool = n;
  }
}

void csTinyXmlDocument::Clear ()
{
  if (!root) return;
  delete root;
  root = NULL;
}

csRef<iDocumentNode> csTinyXmlDocument::CreateRoot ()
{
  Clear ();
  root = new TiDocument ();
  return csPtr<iDocumentNode> (Alloc (root));
}

csRef<iDocumentNode> csTinyXmlDocument::GetRoot ()
{
  return csPtr<iDocumentNode> (Alloc (root));
}

const char* csTinyXmlDocument::Parse (iFile* file)
{
  size_t want_size = file->GetSize ();
  char *data = new char [want_size + 1];
  size_t real_size = file->Read (data, want_size);
  if (want_size != real_size)
  {
    delete[] data;
    return "Unexpected EOF encountered";
  }
  data[real_size] = '\0';
#ifdef CS_DEBUG
  if (strlen (data) != real_size)
  {
    delete[] data;
    return "File contains one or more null characters";
  }
#endif
  const char *error = Parse (data);
  delete[] data;
  return error;
}

const char* csTinyXmlDocument::Parse (iDataBuffer* buf)
{
  return Parse ((const char*)buf->GetData ());
}

const char* csTinyXmlDocument::Parse (iString* str)
{
  return Parse ((const char*)*str);
}

const char* csTinyXmlDocument::Parse (const char* buf)
{
  CreateRoot ();
  root->Parse (root, buf);
  if (root->Error ())
    return root->ErrorDesc ();
  return NULL;
}

const char* csTinyXmlDocument::Write (iFile* file)
{
  scfString str;
  const char* error = Write (&str);
  if (error) return error;
  if (!file->Write (str.GetData (), str.Length ()))
    return "Error writing file!";
  return NULL;
}

const char* csTinyXmlDocument::Write (iString* str)
{
  TiDocument* doc = (TiDocument*)(((csTinyXmlNode*)(iDocumentNode*)root)
  	->GetTiNode ());
  doc->Print (str, 0);
  return NULL;
}

const char* csTinyXmlDocument::Write (iVFS* vfs, const char* filename)
{
  scfString str;
  const char* error = Write (&str);
  if (error) return error;
  if (!vfs->WriteFile (filename, str.GetData (), str.Length ()))
    return "Error writing file!";
  return NULL;
}

int csTinyXmlDocument::Changeable ()
{
  return CS_CHANGEABLE_YES;
}

csTinyXmlNode* csTinyXmlDocument::Alloc ()
{
  if (pool)
  {
    csTinyXmlNode* n = pool;
    pool = n->next_pool;
    n->scfRefCount = 1;
    n->doc = this;	// Incref.
    return n;
  }
  else
  {
    csTinyXmlNode* n = new csTinyXmlNode (this);
    return n;
  }
}

csTinyXmlNode* csTinyXmlDocument::Alloc (TiDocumentNode* node)
{
  csTinyXmlNode* n = Alloc ();
  n->SetTiNode (node);
  return n;
}

void csTinyXmlDocument::Free (csTinyXmlNode* n)
{
  n->next_pool = pool;
  pool = n;
  n->doc = 0;	// Free ref.
}

//------------------------------------------------------------------------

