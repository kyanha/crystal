/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter <resqu@gmx.ch>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"

#include "cssys/csendian.h"
#include "iutil/document.h"
#include "iutil/databuff.h"
#include "iutil/vfs.h"
#include "iutil/string.h"
#include "csutil/databuf.h"
#include "csutil/csstring.h"
#include "csutil/util.h"
#include "csutil/memfile.h"
#include "csutil/hashmap.h"
#include "csutil/hashmapr.h"
#include "csutil/snprintf.h"

#include "bindoc.h"

// =================================================
//  csBinaryDocAttributeIterator
// =================================================


SCF_IMPLEMENT_IBASE(csBinaryDocAttributeIterator)
  SCF_IMPLEMENTS_INTERFACE(iDocumentAttributeIterator)
SCF_IMPLEMENT_IBASE_END

csBinaryDocAttributeIterator::csBinaryDocAttributeIterator ()
{
  SCF_CONSTRUCT_IBASE (NULL);
}

void csBinaryDocAttributeIterator::SetTo (csBdNode* node,
					  csBinaryDocNode* parent)
{
  if (!(node->flags & BD_NODE_HAS_ATTR))
  {
    iteratedNode = NULL;
  }
  else
  {
    iteratedNode = node;
  }
  parentNode = parent;
  pos = 0;
}

csBinaryDocAttributeIterator::~csBinaryDocAttributeIterator()
{
}

bool csBinaryDocAttributeIterator::HasNext ()
{
  return ((iteratedNode) && (pos < iteratedNode->atNum()));
}

csRef<iDocumentAttribute> csBinaryDocAttributeIterator::Next ()
{
  csBdAttr* attrPtr = iteratedNode->atGetItem (pos++);
  csBinaryDocAttribute* attr = parentNode->doc->GetPoolAttr();
  attr->SetTo (attrPtr, parentNode);
  return csPtr<iDocumentAttribute> (attr);
}

// =================================================
//  csBdAttr
// =================================================

csBdAttr::csBdAttr (const char* name)
{
  flags = BD_ATTR_MODIFIED;
  value = 0;
  vstr = NULL;
  nstr = csStrNew (name);
}

csBdAttr::~csBdAttr ()
{
  if (flags & BD_ATTR_MODIFIED)
  {
    delete[] nstr;
    delete[] vstr;
  }
}

const char* csBdAttr::GetNameStr (csBinaryDocument* doc)
{
  if (flags & BD_ATTR_MODIFIED)
  {
    return nstr;
  }
  else
  {
    if (flags & BD_ATTR_NAME_IMMEDIATE)
    {
      return (char*)&nameID;
    }
    else
    {
      return doc->GetInIDString (
	little_endian_long (nameID));
    }
  }
}

const char* csBdAttr::GetValueStr (csBinaryDocument* doc)
{
  if (flags & BD_ATTR_MODIFIED)
  {
    return vstr;
  }
  else
  {
    if (flags & BD_VALUE_STR_IMMEDIATE)
    {
      return (char*)&value;
    }
    else
    {
      return doc->GetInIDString (
	little_endian_long (value));
    }
  }
}

// =================================================
//  csBinaryDocAttribute
// =================================================

void csBinaryDocAttribute::IncRef ()
{
  scfRefCount++;
}

void csBinaryDocAttribute::DecRef ()
{
  if (scfRefCount == 1)
  {
    node->doc->RecyclePoolAttr (this);
    return;
  }
  scfRefCount--;
}

SCF_IMPLEMENT_IBASE_GETREFCOUNT(csBinaryDocAttribute)
SCF_IMPLEMENT_IBASE_QUERY(csBinaryDocAttribute)
  SCF_IMPLEMENTS_INTERFACE(iDocumentAttribute)
SCF_IMPLEMENT_IBASE_END

csBinaryDocAttribute::csBinaryDocAttribute ()
{
  SCF_CONSTRUCT_IBASE (NULL);

  vstr = NULL;
  vsptr = NULL;
};

csBinaryDocAttribute::~csBinaryDocAttribute ()
{
  CleanData ();
  delete vstr; 
}

void csBinaryDocAttribute::CleanData ()
{
}

void csBinaryDocAttribute::SetTo (csBdAttr* ptr,
				  csBinaryDocNode* owner)
{
  CleanData();
  node = owner;
  attrPtr = ptr;
}

const char* csBinaryDocAttribute::GetName ()
{
  return attrPtr->GetNameStr (node->doc);
}

const char* csBinaryDocAttribute::GetValue ()
{
  switch (attrPtr->flags & BD_VALUE_TYPE_MASK)
  {
    case BD_VALUE_TYPE_STR:
      {
	return attrPtr->GetValueStr (node->doc);
      }
    case BD_VALUE_TYPE_INT:
      {
	if (vsptr != attrPtr)
	{
  	  char buf[50];
	  cs_snprintf (buf, sizeof (buf) - 1, "%d", 
	    little_endian_long (attrPtr->value));
	  delete[] vstr; 
	  vstr = csStrNew (buf);
	  vsptr = attrPtr;
	}
	return vstr;
      }
    case BD_VALUE_TYPE_FLOAT:
      {
	if (vsptr != attrPtr)
	{
  	  char buf[50];
	  cs_snprintf (buf, sizeof (buf) - 1,
	    "%g", long2float (little_endian_long (attrPtr->value)));
	  delete[] vstr; 
	  vstr = csStrNew (buf);
	  vsptr = attrPtr;
	}
	return vstr;
      }
    default:
      return NULL;
  }
}

int csBinaryDocAttribute::GetValueAsInt ()
{
  switch (attrPtr->flags & BD_VALUE_TYPE_MASK)
  {
    case BD_VALUE_TYPE_STR:
      {
	if (!attrPtr->GetValueStr(node->doc)) return 0;
	int val = 0;
	sscanf (attrPtr->GetValueStr(node->doc), "%d", &val);
	return val;
      }
    case BD_VALUE_TYPE_INT:
      {
	return little_endian_long (attrPtr->value);
      }
    case BD_VALUE_TYPE_FLOAT:
      {
	return (int)long2float (little_endian_long (attrPtr->value));
      }
    default:
      return 0;
  }
}

float csBinaryDocAttribute::GetValueAsFloat ()
{
  switch (attrPtr->flags & BD_VALUE_TYPE_MASK)
  {
    case BD_VALUE_TYPE_STR:
      {
	if (!attrPtr->GetValueStr(node->doc)) return 0.0f;
	float val = 0.0f;
	sscanf (attrPtr->GetValueStr(node->doc), "%g", &val);
	return val;
      }
    case BD_VALUE_TYPE_INT:
      {
	return (float)little_endian_long (attrPtr->value);
      }
    case BD_VALUE_TYPE_FLOAT:
      {
	return long2float (little_endian_long (attrPtr->value));
      }
    default:
      return 0.0f;
  }
}

void csBinaryDocAttribute::SetName (const char* name)
{
  if (attrPtr->flags & BD_NODE_MODIFIED)
  {
    delete[] attrPtr->nstr;
    attrPtr->nstr = csStrNew (name);
    node->ResortAttrs();
  }
}

static inline bool checkInt (const char* str, int &v)
{
  const char *c = str;
  if (*c == 0) return false;
  while (*c)
  {
    if (!(isdigit (*c)) && (*c != '-')) return false;
    c++;
  }
  sscanf (str, "%d", &v);
  return true;
}

static inline bool checkFloat (const char* str, float &v)
{
  const char *c = str;
  if (*c == 0) return false;
  bool firstchar = true;
  while (*c)
  {
    if (!(isdigit (*c)) && (*c != '-') && (*c != '.') && 
      ((firstchar && (*c == 'e' )) || (*c != 'e')))
      return false;
    c++;
    firstchar = false;
  }
  sscanf (str, "%f", &v);
  return true;
}

void csBinaryDocAttribute::SetValue (const char* val)
{
  if (attrPtr->flags & BD_NODE_MODIFIED)
  {
    delete[] attrPtr->vstr; attrPtr->vstr = NULL;
    delete[] vstr; vstr = NULL;
    int v;
    float f;
    if (checkInt (val, v))
    {
      attrPtr->flags = (attrPtr->flags & ~BD_VALUE_TYPE_MASK) | 
	BD_VALUE_TYPE_INT;
      attrPtr->value = little_endian_long (v);
    }
    else if (checkFloat (val, f))
    {
      attrPtr->flags = (attrPtr->flags & ~BD_VALUE_TYPE_MASK) | 
	BD_VALUE_TYPE_FLOAT;
      attrPtr->value = little_endian_long (float2long (f));
    }
    else 
    {
      attrPtr->flags = (attrPtr->flags & ~BD_VALUE_TYPE_MASK) | 
	BD_VALUE_TYPE_STR;
      attrPtr->vstr = csStrNew (val);
    }
  }
}

void csBinaryDocAttribute::SetValueAsInt (int v)
{
  if (attrPtr->flags & BD_NODE_MODIFIED)
  {
    delete[] attrPtr->vstr; attrPtr->vstr = NULL;
    delete[] vstr; vstr = NULL;
    attrPtr->flags = (attrPtr->flags & ~BD_VALUE_TYPE_MASK) | 
      BD_VALUE_TYPE_INT;
    attrPtr->value = little_endian_long (v);
  }
}

void csBinaryDocAttribute::SetValueAsFloat (float f)
{
  if (attrPtr->flags & BD_NODE_MODIFIED)
  {
    delete[] attrPtr->vstr;	attrPtr->vstr = NULL;
    delete[] vstr; vstr = NULL;
    attrPtr->flags = (attrPtr->flags & ~BD_VALUE_TYPE_MASK) | 
      BD_VALUE_TYPE_FLOAT;
    attrPtr->value = little_endian_long (float2long (f));
  }
}

void csBinaryDocAttribute::Store (csMemFile* nodesFile)
{
  bdNodeAttribute diskAttr;

  diskAttr.flags = attrPtr->flags & BD_VALUE_TYPE_MASK;
  if ((diskAttr.flags & BD_VALUE_TYPE_MASK) == BD_VALUE_TYPE_STR)
  {
    if (strlen (attrPtr->GetValueStr (node->doc)) < MAX_IMM_ATTR_VALUE_STR)
    {
      diskAttr.flags |= BD_VALUE_STR_IMMEDIATE;
      diskAttr.value = 0;
      strcpy ((char*)&diskAttr.value, attrPtr->GetValueStr (node->doc));
    }
    else
    {
      diskAttr.value = little_endian_long (
	node->doc->GetOutStringID (attrPtr->GetValueStr (node->doc)));
    }
  }
  else
    diskAttr.value = attrPtr->value;

  if (strlen (attrPtr->GetNameStr (node->doc)) < MAX_IMM_ATTR_NAME_STR)
  {
    diskAttr.flags |= BD_ATTR_NAME_IMMEDIATE;
    diskAttr.nameID = 0;
    strcpy ((char*)&diskAttr.nameID, attrPtr->GetNameStr (node->doc));
  }
  else
  {
    diskAttr.nameID = little_endian_long (
      node->doc->GetOutStringID (attrPtr->GetNameStr (node->doc)));
  }
  nodesFile->Write ((char*)&diskAttr, sizeof (diskAttr));
}

// =================================================
//  csBinaryDocNodeIterator
// =================================================

SCF_IMPLEMENT_IBASE(csBinaryDocNodeIterator)
  SCF_IMPLEMENTS_INTERFACE(iDocumentNodeIterator)
SCF_IMPLEMENT_IBASE_END

csBinaryDocNodeIterator::csBinaryDocNodeIterator ()
{
  SCF_CONSTRUCT_IBASE (NULL);

  value = NULL;
}

void csBinaryDocNodeIterator::SetTo (csBdNode* node,
 				     csBinaryDocNode* parent,
				     const char* onlyval)
{
  parentNode = parent; 
  pos = 0;
  delete[] value;
  if (onlyval) 
  {
    value = csStrNew (onlyval);
  }
  else
  {
    value = NULL;
  }
  if (!(node->flags & BD_NODE_HAS_CHILDREN))
  {
    iteratedNode = NULL;
  }
  else
  {
    iteratedNode = node;
  }
  FastForward();
}

csBinaryDocNodeIterator::~csBinaryDocNodeIterator ()
{
  delete[] value;
}

void csBinaryDocNodeIterator::FastForward()
{
  if (value && iteratedNode)
  {
    while (pos < iteratedNode->ctNum())
    {
      const char* vstr = iteratedNode->ctGetItem (pos)->
        GetValueStr (parentNode->doc); 
      if (strcmp (vstr, value) != 0)
      {
        pos++;
      }
      else
      {
	break;
      }
    }
  }
}

bool csBinaryDocNodeIterator::HasNext ()
{
  return ((iteratedNode) && 
    (pos < iteratedNode->ctNum()));
}

csRef<iDocumentNode> csBinaryDocNodeIterator::Next ()
{
  csBdNode* nodeData = iteratedNode->ctGetItem (pos++);
  csBinaryDocNode* retNode = parentNode->doc->GetPoolNode();
  retNode->SetTo (nodeData, parentNode);
  FastForward();
  return csPtr<iDocumentNode> (retNode);
}

// =================================================
//  csBdNode
// =================================================

csBdNode::csBdNode (uint32 newType)
{
  flags = newType | BD_NODE_MODIFIED;
  value = 0;
  vstr = NULL;

  attrs = new csPDelArray<csBdAttr>;
  nodes = new csPDelArray<csBdNode>;
}

csBdNode::~csBdNode ()
{
  if (flags & BD_NODE_MODIFIED)
  {
    delete[] vstr;
    delete attrs;
    delete nodes;
  }
}

bdNodeChildTab* csBdNode::GetChildTab ()
{
  if ((flags & (BD_NODE_HAS_CHILDREN | BD_NODE_MODIFIED))
    == BD_NODE_HAS_CHILDREN)
  {
    const int offsetnum = big_endian_long (flags & BD_NODE_HAS_ATTR);
    return (bdNodeChildTab*)GetFromOffset 
      (little_endian_long (offsets[offsetnum]));
  }
  else
    return NULL;
}

bdNodeAttrTab* csBdNode::GetAttrTab ()
{
  if ((flags & (BD_NODE_HAS_ATTR | BD_NODE_MODIFIED))
    == BD_NODE_HAS_ATTR)
  {
    return (bdNodeAttrTab*)GetFromOffset 
      (little_endian_long (offsets[0]));
  }
  else
    return NULL;
}

const char* csBdNode::GetValueStr (csBinaryDocument* doc)
{
  if (flags & BD_NODE_MODIFIED)
  {
    return (char*)value;
  }
  else
  {
    if (flags & BD_VALUE_STR_IMMEDIATE)
    {
      return (char*)&value;
    }
    else
    {
      return doc->GetInIDString (value);
    }
  }
}

csBdAttr* csBdNode::atGetItem(int pos)
{
  if (flags & BD_NODE_MODIFIED)
  {
    return (*attrs)[pos];
  }
  else
  {
    void* at =  (void*)GetAttrTab();
    return (csBdAttr*)((uint8*)at +
      little_endian_long (*((uint32*)at + pos + 1)));
  }
}

void csBdNode::atSetItem (csBdAttr* item, int pos)
{
  if (flags & BD_NODE_MODIFIED)
  {
    attrs->Put (pos, item);
    if (attrs->Length() != 0)
      flags |= BD_NODE_HAS_ATTR;
  }
}

int csBdNode::atItemPos (csBdAttr* item)
{
  int i;
  for (i = 0; i < atNum(); i++)
  {
    if (atGetItem (i) == item) 
    {
      return i;
    }
  }
  return -1;
}

void csBdNode::atInsertBefore (csBdAttr* item, int pos)
{
  if (flags & BD_NODE_MODIFIED)
  {
    attrs->Insert (pos, item);
    if (attrs->Length() != 0)
      flags |= BD_NODE_HAS_ATTR;
  }
}

void csBdNode::atRemove (int pos)
{
  if (flags & BD_NODE_MODIFIED)
  {
    attrs->Delete (pos);
    if (attrs->Length() == 0)
      flags &= ~BD_NODE_HAS_ATTR;
  }
}

int csBdNode::atNum () 
{ 
  if (flags & BD_NODE_MODIFIED)
  {
    return attrs->Length();
  }
  else
  {
    return GetAttrTab()->num; 
  }
}

csBdNode* csBdNode::ctGetItem(int pos)
{
  if (flags & BD_NODE_MODIFIED)
  {
    return (*nodes)[pos];
  }
  else
  {
    void* ct = (void*)GetChildTab();
    return (csBdNode*)((uint8*)ct +
      little_endian_long (*((uint32*)ct + pos + 1)));
  }
}

void csBdNode::ctSetItem (csBdNode* item, int pos)
{
  if (flags & BD_NODE_MODIFIED)
  {
    nodes->Put (pos, item);
    if (nodes->Length() != 0)
      flags |= BD_NODE_HAS_CHILDREN;
  }
}

int csBdNode::ctItemPos (csBdNode* item)
{
  int i;
  for (i = 0; i < ctNum(); i++)
  {
    if (ctGetItem (i) == item) 
    {
      return i;
    }
  }
  return -1;
}

void csBdNode::ctInsertBefore (csBdNode* item, int pos)
{
  if (flags & BD_NODE_MODIFIED)
  {
    nodes->Insert (pos, item);
    if (nodes->Length() != 0)
      flags |= BD_NODE_HAS_CHILDREN;
  }
}
  
void csBdNode::ctRemove (int pos)
{
  if (flags & BD_NODE_MODIFIED)
  {
    nodes->Delete (pos);
    if (nodes->Length() == 0)
      flags &= ~BD_NODE_HAS_CHILDREN;
  }
}

int csBdNode::ctNum ()
{
  if (flags & BD_NODE_MODIFIED)
  {
    return nodes->Length();
  }
  else
  {
    return GetChildTab()->num; 
  }
}

// =================================================
//  csBinaryDocNode
// =================================================

void csBinaryDocNode::IncRef ()
{
  scfRefCount++;
}

void csBinaryDocNode::DecRef ()
{
  if (scfRefCount == 1)
  {
    doc->RecyclePoolNode (this);
    return;
  }
  scfRefCount--;
}

SCF_IMPLEMENT_IBASE_GETREFCOUNT(csBinaryDocNode)
SCF_IMPLEMENT_IBASE_QUERY(csBinaryDocNode)
  SCF_IMPLEMENTS_INTERFACE(iDocumentNode)
SCF_IMPLEMENT_IBASE_END

csBinaryDocNode::csBinaryDocNode ()
{
  SCF_CONSTRUCT_IBASE (NULL);

  vstr = NULL;
  vsptr = NULL;
}

csBinaryDocNode::~csBinaryDocNode ()
{
  CleanData();
  delete vstr;
}

void csBinaryDocNode::SetTo (csBdNode* ptr,
			     csBinaryDocNode* parent)
{
  CleanData();

  // we have to keep a ref on the parent.
  // RecyclePoolNode() takes care of DecRef()ing.
  PoolNextOrParent = parent;
  if (parent) parent->scfRefCount++;
  nodeData = ptr;
}

csDocumentNodeType csBinaryDocNode::GetType ()
{
  switch (nodeData->flags & BD_NODE_TYPE_MASK)
  {
    case BD_NODE_TYPE_COMMENT:
      return CS_NODE_COMMENT;
    case BD_NODE_TYPE_DECLARATION:
      return CS_NODE_DECLARATION;
    case BD_NODE_TYPE_DOCUMENT:
      return CS_NODE_DOCUMENT;
    case BD_NODE_TYPE_ELEMENT:
      return CS_NODE_ELEMENT;
    case BD_NODE_TYPE_TEXT:
      return CS_NODE_TEXT;
    case BD_NODE_TYPE_UNKNOWN:
      return CS_NODE_UNKNOWN;

    default:
      return CS_NODE_UNKNOWN;
  }
}

bool csBinaryDocNode::Equals (iDocumentNode* other)
{
  return (nodeData == ((csBinaryDocNode*)other)->nodeData);
}

const char* csBinaryDocNode::nodeValueStr (csBdNode* nodeData)
{
  switch (nodeData->flags & BD_VALUE_TYPE_MASK)
  {
    case BD_VALUE_TYPE_STR:
      {
	return nodeData->GetValueStr(doc);
      }
    case BD_VALUE_TYPE_INT:
      {
	if (vsptr != nodeData)
	{
  	  char buf[50];
	  cs_snprintf (buf, sizeof (buf) - 1, "%d", 
	    little_endian_long (nodeData->value));
	  delete[] vstr; 
	  vstr = csStrNew (buf);
	  vsptr = nodeData;
	}
	return vstr;
      }
    case BD_VALUE_TYPE_FLOAT:
      {
	if (vsptr != nodeData)
	{
  	  char buf[50];
	  cs_snprintf (buf, sizeof (buf) - 1, "%g", 
	    long2float (little_endian_long (nodeData->value)));
	  delete[] vstr; 
	  vstr = csStrNew (buf);
	  vsptr = nodeData;
	}
	return vstr;
      }
    default:
      return NULL;
  }
}

int csBinaryDocNode::nodeValueInt (csBdNode* nodeData)
{
  switch (nodeData->flags & BD_VALUE_TYPE_MASK)
  {
    case BD_VALUE_TYPE_STR:
      {
	if (!nodeData->GetValueStr (doc)) return 0;
	int val = 0;
	sscanf (nodeData->GetValueStr (doc), "%d", &val);
	return val;
      }
    case BD_VALUE_TYPE_INT:
      {
	return little_endian_long (nodeData->value);
      }
    case BD_VALUE_TYPE_FLOAT:
      {
	return (int)long2float (little_endian_long (nodeData->value));
      }
    default:
      return 0;
  }
}

float csBinaryDocNode::nodeValueFloat (csBdNode* nodeData)
{
  switch (nodeData->flags & BD_VALUE_TYPE_MASK)
  {
    case BD_VALUE_TYPE_STR:
      {
	if (!nodeData->GetValueStr (doc)) return 0.0f;
	float val = 0.0f;
	sscanf (nodeData->GetValueStr (doc), "%g", &val);
	return val;
      }
    case BD_VALUE_TYPE_INT:
      {
	return (float)little_endian_long (nodeData->value);
      }
    case BD_VALUE_TYPE_FLOAT:
      {
	return long2float (little_endian_long (nodeData->value));
      }
    default:
      return 0.0f;
  }
}

const char* csBinaryDocNode::GetValue ()
{
  return nodeValueStr (nodeData);
}

int csBinaryDocNode::GetValueAsInt ()
{
  return nodeValueInt (nodeData);
}

float csBinaryDocNode::GetValueAsFloat ()
{
  return nodeValueFloat (nodeData);
}

int csBinaryDocNode::IndexOfAttr (const char* attr)
{
  if (!(nodeData->flags & BD_NODE_HAS_ATTR)) return -1;
  int lo = 0, hi = nodeData->atNum() - 1;
  int mid = 0;
  while (lo <= hi)
  {
    mid = (lo + hi) / 2;
    int c = strcmp (attr, nodeData->atGetItem (mid)->GetNameStr (doc));
    if (c == 0)
    {
      return mid;
    }
    else if (c > 0)
    {
      lo = mid + 1;
    }
    else
    {
      hi = mid - 1;
    }
  }
  return -1;
}

void csBinaryDocNode::CleanData ()
{
}

void csBinaryDocNode::ResortAttrs()
{
  ResortAttrs (0, nodeData->atNum());
}

void csBinaryDocNode::ResortAttrs (int Left, int Right)
{
  int i = Left, j = Right;
  int x = (Left + Right) / 2;
  do
  {
    const char* nxn = nodeData->atGetItem(x)->GetNameStr(doc);
    while ((i != x) && (strcmp (nodeData->atGetItem(i)->GetNameStr(doc),
      nxn) < 0))
      i++;
    while ((j != x) && (strcmp (nodeData->atGetItem(j)->GetNameStr(doc),
      nxn) > 0))
      j--;
    if (i < j)
    {
      csBdAttr* t;
      t = nodeData->atGetItem (i);
      nodeData->atSetItem (nodeData->atGetItem (j), i);
      nodeData->atSetItem (t, j);
      if (x == i)
        x = j;
      else if (x == j)
        x = i;
    }
    if (i <= j)
    {
      i++;
      if (j > Left)
        j--;
    }
  } while (i <= j);

  if (j - Left < Right - i)
  {
    if (Left < j)
      ResortAttrs (Left, j);
    if (i < Right)
    {
      ResortAttrs (i, Right);
    }
  }
  else
  {
    if (i < Right)
      ResortAttrs (i, Right);
    if (Left < j)
    {
      ResortAttrs (Left, j);
    }
  }
}

void csBinaryDocNode::SetValue (const char* value)
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    delete[] vstr; vstr = NULL;
    delete[] nodeData->vstr; nodeData->vstr = NULL;
    int v;
    float f;
    if (checkInt (value, v))
    {
      nodeData->flags = (nodeData->flags & ~BD_VALUE_TYPE_MASK) | 
	BD_VALUE_TYPE_INT;
      nodeData->value = little_endian_long (v);
    }
    else if (checkFloat (value, f))
    {
      nodeData->flags = (nodeData->flags & ~BD_VALUE_TYPE_MASK) | 
	BD_VALUE_TYPE_FLOAT;
      nodeData->value = little_endian_long (float2long (f));
    }
    else 
    {
      nodeData->flags = (nodeData->flags & ~BD_VALUE_TYPE_MASK) | 
	BD_VALUE_TYPE_STR;
      nodeData->vstr = csStrNew (value);
    }
  }
}

void csBinaryDocNode::SetValueAsInt (int value)
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    delete[] vstr; vstr = NULL;
    nodeData->flags = (nodeData->flags & ~BD_VALUE_TYPE_MASK) |
      BD_VALUE_TYPE_INT;
    nodeData->value = little_endian_long (value);
  }
}

void csBinaryDocNode::SetValueAsFloat (float value)
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    delete[] vstr; vstr = NULL;
    nodeData->flags = (nodeData->flags & ~BD_VALUE_TYPE_MASK) |
      BD_VALUE_TYPE_FLOAT;
    nodeData->value = little_endian_long (float2long (value));
  }
}

csRef<iDocumentNode> csBinaryDocNode::GetParent ()
{
  return (iDocumentNode*) (PoolNextOrParent);
}

csRef<iDocumentNodeIterator> csBinaryDocNode::GetNodes ()
{
  csBinaryDocNodeIterator* it = new csBinaryDocNodeIterator ();
  it->SetTo (nodeData, this);
  return csPtr<iDocumentNodeIterator> (it);
}

csRef<iDocumentNodeIterator> csBinaryDocNode::GetNodes (const char* value)
{
  csBinaryDocNodeIterator* it = new csBinaryDocNodeIterator ();
  it->SetTo (nodeData, this, value);
  return csPtr<iDocumentNodeIterator> (it);
}

csRef<iDocumentNode> csBinaryDocNode::GetNode (const char* value)
{
  if (nodeData->flags & BD_NODE_HAS_CHILDREN)
  {
    int i;
    for (i = 0; i < nodeData->ctNum(); i++)
    {
      csBdNode* nodeData = csBinaryDocNode::nodeData->ctGetItem (i);
      if (strcmp (nodeData->GetValueStr (doc), value) == 0)
      {
	csBinaryDocNode* node = doc->GetPoolNode();
	node->SetTo (nodeData, this);
	return csPtr<iDocumentNode> (node);
      }
    }
  }
  return NULL;
}

void csBinaryDocNode::RemoveNode (const csRef<iDocumentNode>& child)
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    csBdNode* nd = ((csRef<csBinaryDocNode>&)child)->nodeData;
    int pos = nodeData->ctItemPos (nd);
    if (pos != -1) nodeData->ctRemove (pos);
  }
}

void csBinaryDocNode::RemoveNodes ()
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    while (nodeData->ctNum() > 0)
    {
      nodeData->ctRemove (0);
    }
  }
}

csRef<iDocumentNode> csBinaryDocNode::CreateNodeBefore (csDocumentNodeType type,
      iDocumentNode* before)
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    uint32 newType;
    switch (type)
    {
      case CS_NODE_COMMENT:
	newType = BD_NODE_TYPE_COMMENT;
	break;
      case CS_NODE_DECLARATION:
	newType = BD_NODE_TYPE_DECLARATION;
	break;
      case CS_NODE_ELEMENT:
	newType = BD_NODE_TYPE_ELEMENT;
	break;
      case CS_NODE_TEXT:
	newType = BD_NODE_TYPE_TEXT;
	break;

      default:
	return NULL;
    }

    csBdNode* newData = new csBdNode (newType);

    int oldChildCount = nodeData->ctNum();

    int newPos = -1;
    if (oldChildCount && before)
    {
      newPos = nodeData->ctItemPos (
	((csBinaryDocNode*)before)->nodeData);
    }
    if (newPos < 0) newPos = oldChildCount;
    nodeData->ctInsertBefore (newData, newPos);

    csBinaryDocNode* newNode = doc->GetPoolNode ();
    newNode->SetTo (newData, this);

    return csPtr<iDocumentNode> (newNode);
  }
  else
  {
    return NULL;
  }
}

const char* csBinaryDocNode::GetContentsValue ()
{
  if (nodeData->flags & BD_NODE_HAS_CHILDREN)
  {
    int i;
    for (i = 0; i < nodeData->ctNum(); i++)
    {
      csBdNode* nodeData = csBinaryDocNode::nodeData->ctGetItem (i);
      if ((nodeData->flags & BD_NODE_TYPE_MASK) == 
        BD_NODE_TYPE_TEXT)
      {
	return nodeValueStr (nodeData);
      }
    }
  }
  return NULL;
}

int csBinaryDocNode::GetContentsValueAsInt ()
{
  if (nodeData->flags & BD_NODE_HAS_CHILDREN)
  {
    int i;
    for (i = 0; i < nodeData->ctNum(); i++)
    {
      csBdNode* nodeData = csBinaryDocNode::nodeData->ctGetItem (i);
      if ((nodeData->flags & BD_NODE_TYPE_MASK) == 
        BD_NODE_TYPE_TEXT)
      {
	return nodeValueInt (nodeData);
      }
    }
  }
  return 0;
}

float csBinaryDocNode::GetContentsValueAsFloat ()
{
  if (nodeData->flags & BD_NODE_HAS_CHILDREN)
  {
    int i;
    for (i = 0; i < nodeData->ctNum(); i++)
    {
      csBdNode* nodeData = csBinaryDocNode::nodeData->ctGetItem (i);
      if ((nodeData->flags & BD_NODE_TYPE_MASK) == 
        BD_NODE_TYPE_TEXT)
      {
	return nodeValueFloat (nodeData);
      }
    }
  }
  return 0.0f;
}

csRef<iDocumentAttributeIterator> csBinaryDocNode::GetAttributes ()
{
  csBinaryDocAttributeIterator* it = new csBinaryDocAttributeIterator ();
  it->SetTo (nodeData, this);
  return csPtr<iDocumentAttributeIterator> (it);
}

csRef<iDocumentAttribute> csBinaryDocNode::GetAttribute (const char* name)
{
  int index = IndexOfAttr (name);
  if (index < 0)
  {
    if (nodeData->flags & BD_NODE_MODIFIED)
    {
      csBdAttr* newAttr = new csBdAttr(name);
      int newpos = 0;
      if (nodeData->flags & BD_NODE_HAS_ATTR)
      {
	int lo = 0, hi = nodeData->atNum() - 1;
	int c, mid = 0;
	while (lo <= hi)
	{
	  mid = (lo + hi) / 2;
	  c = strcmp (name, nodeData->atGetItem (mid)->GetNameStr (doc));
	  if (c > 0)
	  {
	    lo = mid + 1;
	  }
	  else
	  {
	    hi = mid - 1;
	  }
	}
	newpos = lo;
      }
      nodeData->atInsertBefore (newAttr, newpos);

      csBinaryDocAttribute* attr = doc->GetPoolAttr ();
      attr->SetTo (newAttr, this);
      return csPtr<iDocumentAttribute> (attr);
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    csBdAttr* ptr = nodeData->atGetItem (index);
    csBinaryDocAttribute* attr = doc->GetPoolAttr ();
    attr->SetTo (ptr, this);
    return csPtr<iDocumentAttribute> (attr);
  }

  return NULL;
}

const char* csBinaryDocNode::GetAttributeValue (const char* name)
{
  csRef<iDocumentAttribute> attr = GetAttribute (name);
  if (attr)
  {
    return attr->GetValue ();
  }
  else
  {
    return NULL;
  }
}

int csBinaryDocNode::GetAttributeValueAsInt (const char* name)
{
  csRef<iDocumentAttribute> attr = GetAttribute (name);
  if (attr)
  {
    return attr->GetValueAsInt ();
  }
  else
  {
    return 0;
  }
}

float csBinaryDocNode::GetAttributeValueAsFloat (const char* name)
{
  csRef<iDocumentAttribute> attr = GetAttribute (name);
  if (attr)
  {
    return attr->GetValueAsFloat ();
  }
  else
  {
    return 0.0f;
  }
}

void csBinaryDocNode::RemoveAttribute (const csRef<iDocumentAttribute>& attr)
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    csBdAttr* ad = ((csRef<csBinaryDocAttribute>&)attr)->attrPtr;
    int pos = nodeData->atItemPos (ad);
    if (pos != -1) nodeData->atRemove (pos);
  }
}

void csBinaryDocNode::RemoveAttributes ()
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    while (nodeData->atNum() > 0)
    {
      nodeData->atRemove (0);
    }
  }
}

void csBinaryDocNode::SetAttribute (const char* name, const char* value)
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    csRef<iDocumentAttribute> attr = GetAttribute (name);
    attr->SetValue (value);
  }
}

void csBinaryDocNode::SetAttributeAsInt (const char* name, int value)
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    csRef<iDocumentAttribute> attr = GetAttribute (name);
    attr->SetValueAsInt (value);
  }
}

void csBinaryDocNode::SetAttributeAsFloat (const char* name, float value)
{
  if (nodeData->flags & BD_NODE_MODIFIED)
  {
    csRef<iDocumentAttribute> attr = GetAttribute (name);
    attr->SetValueAsFloat (value);
  }
}

void csBinaryDocNode::Store (csMemFile* nodesFile)
{
  bdNode diskNode;
  size_t nodeSize = sizeof(bdNode) - 2 * sizeof(uint32);

  memcpy (&diskNode, nodeData, nodeSize);
  diskNode.flags &= ~(BD_NODE_HAS_ATTR | 
    BD_NODE_HAS_CHILDREN | BD_VALUE_STR_IMMEDIATE);
  diskNode.flags &= BD_DISK_FLAGS_MASK;

  if ((nodeData->flags & BD_VALUE_TYPE_MASK) == BD_VALUE_TYPE_STR) 
  {
    unsigned int maximmvalue = MAX_IMM_NODE_VALUE_STR;
    // Hack: cram one more byte into value, if possible
    if ((diskNode.flags & BE (0xff00)) == 0) maximmvalue++;
    if (strlen (nodeData->vstr) < maximmvalue)
    {
      diskNode.value = 0;
      strcpy ((char*)&diskNode.value, nodeData->vstr);
      diskNode.flags |= BD_VALUE_STR_IMMEDIATE;
    }
    else
    {
      diskNode.value = little_endian_long (
	doc->GetOutStringID (nodeData->vstr));
    }
  }

  if (nodeData->flags & BD_NODE_HAS_ATTR)
  {
    diskNode.flags |= BD_NODE_HAS_ATTR;
    nodeSize += sizeof (uint32);
  }

  if (nodeData->flags & BD_NODE_HAS_CHILDREN)
  {
    diskNode.flags |= BD_NODE_HAS_CHILDREN;
    nodeSize += sizeof (uint32);
  }

  size_t nodeStart = nodesFile->GetPos ();
  nodesFile->Write ((char*)&diskNode, nodeSize);

  if (nodeData->flags & BD_NODE_HAS_ATTR)
  {
    size_t attrStart = nodesFile->GetPos();
    uint32 attrCount = nodeData->atNum();
    uint32* attrStarts = new uint32[attrCount];
    nodesFile->Write ((char*)&attrCount, sizeof(uint32));
    nodesFile->Write ((char*)attrStarts, sizeof(uint32) * attrCount);

    unsigned int i;
    for (i = 0; i < attrCount; i++)
    {
      attrStarts[i] = little_endian_long (nodesFile->GetPos () - attrStart);
      csBinaryDocAttribute* attr = doc->GetPoolAttr ();
      attr->SetTo (nodeData->atGetItem (i), this);
      attr->Store (nodesFile);
    }
    size_t attrEnd = nodesFile->GetPos ();
    nodesFile->SetPos (attrStart + sizeof(uint32));
    nodesFile->Write ((char*)attrStarts, sizeof(uint32) * attrCount);
    diskNode.offsets[0] = little_endian_long (attrStart - nodeStart);
    nodesFile->SetPos (attrEnd);
    delete[] attrStarts;
  }

  if (nodeData->flags & BD_NODE_HAS_CHILDREN)
  {
    size_t childStart = nodesFile->GetPos();
    uint32 childCount = nodeData->ctNum();
    uint32* childStarts = new uint32[childCount];
    nodesFile->Write ((char*)&childCount, sizeof(uint32));
    nodesFile->Write ((char*)childStarts, sizeof(uint32) * childCount);

    unsigned int i;
    for (i = 0; i < childCount; i++)
    {
      childStarts[i] = little_endian_long (nodesFile->GetPos () - childStart);
      csBinaryDocNode* node = doc->GetPoolNode();
      node->SetTo (nodeData->ctGetItem (i), this);
      node->Store (nodesFile);
    }
    size_t childEnd = nodesFile->GetPos ();
    nodesFile->SetPos (childStart + sizeof(uint32));
    nodesFile->Write ((char*)childStarts, sizeof(uint32) * childCount);
    diskNode.offsets[big_endian_long (diskNode.flags & BD_NODE_HAS_ATTR)] = 
      little_endian_long (childStart - nodeStart);
    nodesFile->SetPos (childEnd);
    delete[] childStarts;
  }    

  if (diskNode.flags & (BD_NODE_HAS_ATTR | BD_NODE_HAS_CHILDREN))
  {
    size_t nodeEnd = nodesFile->GetPos();
    nodesFile->SetPos (nodeStart);
    nodesFile->Write ((char*)&diskNode, nodeSize);
    nodesFile->SetPos (nodeEnd);
  }
}

// =================================================
//  csBinaryDocument
// =================================================

SCF_IMPLEMENT_IBASE(csBinaryDocument)
  SCF_IMPLEMENTS_INTERFACE(iDocument)
SCF_IMPLEMENT_IBASE_END

csBinaryDocument::csBinaryDocument ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  nodePool = NULL;
  attrPool = NULL;
  root = NULL;
  outStrHash = NULL;
}

csBinaryDocument::~csBinaryDocument ()
{
  if (root && (root->flags & BD_NODE_MODIFIED))
    delete root;
  while (nodePool)
  {
    csBinaryDocNode *node = nodePool;
    nodePool = node->PoolNextOrParent;
    delete node;
  }
  while (attrPool)
  {
    csBinaryDocAttribute *attr = attrPool;
    attrPool = attr->pool_next;
    delete attr;
  }
}

csBinaryDocNode* csBinaryDocument::GetPoolNode ()
{
  if (nodePool)
  {
    csBinaryDocNode *node = nodePool;
    nodePool = nodePool->PoolNextOrParent;
    node->doc = this;
    // the node has to keep a ref on us. But to avoid
    // a virtual function call, we increase our refcount
    // 'by hand'.
    scfRefCount++;
    return node;
  }
  else
  {
    csBinaryDocNode *node = new csBinaryDocNode ();
    node->doc = this;
    scfRefCount++;
    return node;
  }
}

void csBinaryDocument::RecyclePoolNode (csBinaryDocNode *node)
{
  csBinaryDocNode *parent;
  parent = node->PoolNextOrParent;
  node->PoolNextOrParent = nodePool;
  nodePool = node;
  if (parent)
  {
    if (parent->scfRefCount == 1)
    {
      RecyclePoolNode (parent);
    }
    else
      parent->scfRefCount--;
  }
  // DecRef us.
  if (scfRefCount == 1)
  {
    delete this;
    return;
  }
  scfRefCount--;
}

csBinaryDocAttribute* csBinaryDocument::GetPoolAttr ()
{
  if (attrPool)
  {
    csBinaryDocAttribute* attr = attrPool;
    attrPool = attrPool->pool_next;
    return attr;
  }
  else
  {
    csBinaryDocAttribute* attr = new csBinaryDocAttribute ();
    return attr;
  }
}

void csBinaryDocument::RecyclePoolAttr (csBinaryDocAttribute *attr)
{
  attr->pool_next = attrPool;
  attrPool = attr;
}

csBinaryDocNode* csBinaryDocument::GetRootNode ()
{
  csBinaryDocNode* rootNode;
  rootNode = GetPoolNode();
  rootNode->SetTo (root, NULL);
  return rootNode;
}

uint32 csBinaryDocument::GetOutStringID (const char* str)
{
  CS_ASSERT (outStrStorage);
  CS_ASSERT (outStrHash);
  if (str == NULL) return BD_OFFSET_INVALID;
  uint32 val;
  val = outStrHash->Request (str);
  if (val == csInvalidStringID)
  {
    val = outStrStorage->GetPos() - outStrTabOfs;
    outStrStorage->Write (str, strlen (str) + 1);
    outStrHash->Register (str, val);
  }
  return val;
}


const char* csBinaryDocument::GetInIDString (uint32 ID)
{
  if (ID == BD_OFFSET_INVALID) return NULL;
  return (char*)(dataStart + inStrTabOfs + 
    ID);
}

void csBinaryDocument::Clear ()
{
  data = NULL; 
  dataStart = NULL;
  delete root; root = NULL;
}

csRef<iDocumentNode> csBinaryDocument::CreateRoot ()
{
  Clear();
  root = new csBdNode (BD_NODE_TYPE_DOCUMENT);
  return csPtr<iDocumentNode> (GetRootNode ());
}

csRef<iDocumentNode> csBinaryDocument::GetRoot ()
{
  if (!root) root = new csBdNode (BD_NODE_TYPE_DOCUMENT);
  return csPtr<iDocumentNode> (GetRootNode ());
}

const char* csBinaryDocument::Parse (iFile* file)
{
  csRef<csDataBuffer> newBuffer = csPtr<csDataBuffer> 
    (new csDataBuffer(file->GetSize()));
  file->Read ((char*)newBuffer->GetData(), file->GetSize());
  return Parse (newBuffer);
}

const char* csBinaryDocument::Parse (iDataBuffer* buf)
{
  if (buf->GetSize() < sizeof (bdHeader) + sizeof (bdDocument))
  {
    return "Not enough data";
  }
  bdHeader *head = (bdHeader*)buf->GetData();
  if (head->magic != BD_HEADER_MAGIC)
  {
    return "Not a binary CS document";
  }
  if (head->size != buf->GetSize())
  {
    return "Size mismatch";
  }
  bdDocument *bdDoc = (bdDocument*)(buf->GetUint8() +  sizeof(bdHeader));
  if (bdDoc->ofsRoot == BD_OFFSET_INVALID)
  {
    return "No root node";
  }
  
  Clear();
  root = NULL;
  data = buf;
  dataStart = data->GetUint8();

  inStrTabOfs =  sizeof(bdHeader) + bdDoc->ofsStr;

  root = (csBdNode*)((uint8*)bdDoc + bdDoc->ofsRoot);

  return NULL;
}

const char* csBinaryDocument::Parse (iString* str)
{
  csRef<csDataBuffer> newBuffer = csPtr<csDataBuffer>
    (new csDataBuffer(str->Length()));
  memcpy (newBuffer->GetData(), str->GetData(),
    str->Length());
  return Parse (newBuffer);
}

const char* csBinaryDocument::Parse (const char* buf)
{
  csRef<csDataBuffer> newBuffer = csPtr<csDataBuffer>
    (new csDataBuffer(strlen (buf)));
  memcpy (newBuffer->GetData(), buf, strlen (buf));
  return Parse (newBuffer);
}

const char* csBinaryDocument::Write (csMemFile& out)
{
  bdHeader head;
  head.magic = BD_HEADER_MAGIC;

  out.Write ((char*)&head, sizeof (head));

  size_t docStart = out.GetPos();
  bdDocument doc;
  out.Write ((char*)&doc, sizeof (doc));

  outStrStorage = &out;
  outStrHash = new csStringHash (431);
  outStrHash->Register ("fuck", 0);
  doc.ofsStr = out.GetPos();
  {
    int pad = (4 - doc.ofsStr) & 3;
    if (pad)
    {
      // align to 4 byte boundary, to avoid problems
      char null[4] = {0, 0, 0, 0};
      out.Write (null, pad);
      doc.ofsStr += pad;
    }
    doc.ofsStr -= docStart;
  }
  doc.ofsStr = little_endian_long (doc.ofsStr);
  outStrTabOfs = out.GetPos();

  csMemFile outNodes;
  if (root)
  {
    GetRootNode()->Store (&outNodes);
  }
  else
  {
    doc.ofsRoot = BD_OFFSET_INVALID;
  }
  delete outStrHash; 
  outStrHash = NULL;

  doc.ofsRoot = out.GetPos();
  {
    int pad = (4 - doc.ofsRoot) & 3;
    if (pad)
    {
      // align to 4 byte boundary, to avoid problems
      char null[4] = {0, 0, 0, 0};
      out.Write (null, pad);
      doc.ofsRoot += pad;
    }
    doc.ofsRoot -= docStart;
  }
  doc.ofsRoot = little_endian_long (doc.ofsRoot);
  out.Write (outNodes.GetData(), outNodes.GetSize());

  head.size = out.GetSize();
  out.SetPos (0);
  out.Write ((char*)&head, sizeof (head));
  out.Write ((char*)&doc, sizeof (doc));

  return NULL;
}

const char* csBinaryDocument::Write (iFile* file)
{
  csMemFile out;
  const char* ret = Write(out);
  file->Write (out.GetData(), out.GetSize());

  return ret;
}

const char* csBinaryDocument::Write (iString* str)
{
  csMemFile temp;

  const char* ret = Write (temp);
  str->Clear();
  str->Append (temp.GetData(), temp.GetSize());

  return ret;
}

const char* csBinaryDocument::Write (iVFS* vfs, const char* filename)
{
  csMemFile temp;

  const char* ret = Write (temp);
  vfs->WriteFile (filename, temp.GetData(), temp.GetSize());

  return ret;
}

int csBinaryDocument::Changeable ()
{
  if (!root || (root->flags & BD_NODE_MODIFIED))
  {
    return CS_CHANGEABLE_YES;
  }
  else
  {
    return CS_CHANGEABLE_NEWROOT;
  }
}
