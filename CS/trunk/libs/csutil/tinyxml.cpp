/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2002 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "cssysdef.h"
#include <ctype.h>
#include "tinyxml.h"
#include "csutil/scfstr.h"

namespace CS
{
namespace Implementation
{
namespace TinyXml
{

const char* const TiXmlBase::errorString[ TIXML_ERROR_STRING_COUNT ] =
{
  "No error",
  "Error",
  "Failed to open file",
  "Memory allocation failed.",
  "Error parsing Element.",
  "Failed to read Element name",
  "Error reading Element value.",
  "Error reading Attributes.",
  "Error: empty tag.",
  "Error reading end tag.",
  "Error parsing Unknown.",
  "Error parsing Comment.",
  "Error parsing Declaration.",
  "Error document empty."
};

void TiXmlBase::PutString( const TiXmlString& str, TiXmlString* outString )
{
  int i=0;

  while( i<(int)str.length() )
  {
    int c = str[i];

    if (    c == '&' 
         && i < ( (int)str.length() - 2 )
       && str[i+1] == '#'
       && str[i+2] == 'x' )
    {
      // Hexadecimal character reference.
      // Pass through unchanged.
      // &#xA9;  -- copyright symbol, for example.
      while ( i<(int)str.length() )
      {
        outString->append( str.c_str() + i, 1 );
        ++i;
        if ( str[i] == ';' )
          break;
      }
    }
    else if ( c == '&' )
    {
      outString->append( entity[0].str, entity[0].strLength );
      ++i;
    }
    else if ( c == '<' )
    {
      outString->append( entity[1].str, entity[1].strLength );
      ++i;
    }
    else if ( c == '>' )
    {
      outString->append( entity[2].str, entity[2].strLength );
      ++i;
    }
    else if ( c == '\"' )
    {
      outString->append( entity[3].str, entity[3].strLength );
      ++i;
    }
    else if ( c == '\'' )
    {
      outString->append( entity[4].str, entity[4].strLength );
      ++i;
    }
    else if ( c < 32 || c > 126 )
    {
      // Easy pass at non-alpha/numeric/symbol
      // 127 is the delete key. Below 32 is symbolic.
      csString buf;
      buf.Format ("&#x%02X;", (unsigned) ( c & 0xff ) );
      outString->append( buf, strlen( buf ) );
      ++i;
    }
    else
    {
      char realc = (char) c;
      outString->append( &realc, 1 );
      ++i;
    }
  }
}


TiDocumentNode::TiDocumentNode( ) : refcount (1), parent (0)
{
}


TiDocumentNode::~TiDocumentNode()
{
}

void TiDocumentNode::DecRef ()
{
  csRefTrackerAccess::TrackDecRef (this, refcount);
  refcount--;
  if (refcount <= 0)
    GetDocument()->DeleteNode (this);
}

const char* TiDocumentNode::Value() const
{
  switch (type)
  {
    case DOCUMENT:
      return static_cast<const TiDocument*> (this)->Value ();
    case ELEMENT:
      return static_cast<const TiXmlElement*> (this)->Value ();
    case COMMENT:
      return static_cast<const TiXmlComment*> (this)->Value ();
    case UNKNOWN:
      return static_cast<const TiXmlUnknown*> (this)->Value ();
    case TEXT:
      return static_cast<const TiXmlText*> (this)->Value ();
    case CDATA:
      return static_cast<const TiXmlCData*> (this)->Value ();
    case DECLARATION:
      return static_cast<const TiXmlDeclaration*> (this)->Value ();
    default:
      CS_ASSERT(false);
      return 0;
  }
}

void TiDocumentNode::SetValue (const char* v)
{
  switch (type)
  {
    case DOCUMENT:
      static_cast<TiDocument*> (this)->SetValue (v);
      break;
    case ELEMENT:
      static_cast<TiXmlElement*> (this)->SetValue (v);
      break;
    case COMMENT:
      static_cast<TiXmlComment*> (this)->SetValue (v);
      break;
    case UNKNOWN:
      static_cast<TiXmlUnknown*> (this)->SetValue (v);
      break;
    case TEXT:
      static_cast<TiXmlText*> (this)->SetValue (v);
      break;
    case CDATA:
      static_cast<TiXmlCData*> (this)->SetValue (v);
      break;
    case DECLARATION:
      static_cast<TiXmlDeclaration*> (this)->SetValue (v);
      break;
    default:
      CS_ASSERT(false);
  }
}

csPtr<TiDocumentNode> TiDocumentNode::Clone (TiDocument* doc) const
{
  switch (type)
  {
    case DOCUMENT:
      return static_cast<const TiDocument*> (this)->Clone (doc);
    case ELEMENT:
      return static_cast<const TiXmlElement*> (this)->Clone (doc);
    case COMMENT:
      return static_cast<const TiXmlComment*> (this)->Clone (doc);
    case UNKNOWN:
      return static_cast<const TiXmlUnknown*> (this)->Clone (doc);
    case TEXT:
      return static_cast<const TiXmlText*> (this)->Clone (doc);
    case CDATA:
      return static_cast<const TiXmlCData*> (this)->Clone (doc);
    case DECLARATION:
      return static_cast<const TiXmlDeclaration*> (this)->Clone (doc);
    default:
      CS_ASSERT(false);
      return 0;
  }
}

void TiDocumentNode::Print( iString* cfile, int depth ) const
{
  switch (type)
  {
    case DOCUMENT:
      static_cast<const TiDocument*> (this)->Print (cfile, depth);
      break;
    case ELEMENT:
      static_cast<const TiXmlElement*> (this)->Print (cfile, depth);
      break;
    case COMMENT:
      static_cast<const TiXmlComment*> (this)->Print (cfile, depth);
      break;
    case UNKNOWN:
      static_cast<const TiXmlUnknown*> (this)->Print (cfile, depth);
      break;
    case TEXT:
      static_cast<const TiXmlText*> (this)->Print (cfile, depth);
      break;
    case CDATA:
      static_cast<const TiXmlCData*> (this)->Print (cfile, depth);
      break;
    case DECLARATION:
      static_cast<const TiXmlDeclaration*> (this)->Print (cfile, depth);
      break;
    default:
      CS_ASSERT(false);
  }
}

TiDocumentNode* TiDocumentNode::NextSibling( const char * value ) const
{
  TiDocumentNode* node;
  for ( node = next; node; node = node->next )
  {
    const char* node_val = node->Value ();
    if (node_val && strcmp (node_val, value) == 0)
      return node;
  }
  return 0;
}


csPtr<TiDocumentNode> TiDocumentNodeChildren::Identify( ParseInfo& parse,
	const char* p )
{
  TiDocumentNode* returnNode = 0;

  p = SkipWhiteSpace( parse, p );
  if( !p || !*p || *p != '<' )
  {
    return 0;
  }

  p = SkipWhiteSpace( parse, p );

  if ( !p || !*p )
  {
    return 0;
  }

  // What is this thing? 
  // - Elements start with a letter or underscore, but xml is reserved.
  // - Comments: <!--
  // - Decleration: <?xml
  // - Everthing else is unknown to tinyxml.
  //

  const char* xmlHeader = "<?xml";
  const char* commentHeader = "<!--";

  if ( StringEqual( p, xmlHeader) )
  {
    void* ptr = parse.document->docHeap.Alloc (sizeof (TiXmlDeclaration));
    returnNode = new (ptr) TiXmlDeclaration();
  }
  else if (    isalpha( *(p+1) ) || *(p+1) == '_' )
  {
    void* p = parse.document->blk_element.Alloc (sizeof (TiXmlElement));
    returnNode = new (p) TiXmlElement ();
  }
  else if ( StringEqual ( p, commentHeader) )
  {
    void* ptr = parse.document->docHeap.Alloc (sizeof (TiXmlComment));
    returnNode = new (ptr) TiXmlComment();
  }
  else
  {
    void* ptr = parse.document->docHeap.Alloc (sizeof (TiXmlUnknown));
    returnNode = new (ptr) TiXmlUnknown();
  }

  if ( returnNode )
  {
    // Set the parent, so it can report errors
    returnNode->parent = this;
    //p = returnNode->Parse( p );
  }
  else
  {
    parse.document->SetError( TIXML_ERROR_OUT_OF_MEMORY, this, p );
  }
  return returnNode;
}

// -------------------------------------------------------------------------
TiDocumentNodeChildren::TiDocumentNodeChildren ()
{
}

TiDocumentNodeChildren::~TiDocumentNodeChildren()
{
}

void TiDocumentNodeChildren::Clear()
{
  firstChild = 0;
}

void TiDocumentNodeChildren::InsertAfterChild( TiDocumentNode* afterThis,
                                               TiDocumentNode* addThis )
{
  addThis->parent = this;
  addThis->next = 0;

  if ( afterThis )
    afterThis->next = addThis;
  else
    firstChild = addThis;      // it was an empty list.
}


TiDocumentNode* TiDocumentNodeChildren::InsertBeforeChild(
  TiDocumentNode* beforeThis, const TiDocumentNode& addThis )
{  
  if ( !beforeThis || beforeThis->parent != this )
    return 0;

  csRef<TiDocumentNode> node = addThis.Clone(GetDocument ());
  if ( !node )
    return 0;
  node->parent = this;

  node->next = beforeThis;
  TiDocumentNode* beforeThisPrev = Previous (beforeThis);
  if ( beforeThisPrev )
  {
    beforeThisPrev->next = node;
  }
  else
  {
    assert( firstChild == beforeThis );
    firstChild = node;
  }
  return node;
}

TiDocumentNode* TiDocumentNodeChildren::InsertAfterChild(
  TiDocumentNode* afterThis, const TiDocumentNode& addThis )
{
  csRef<TiDocumentNode> node = addThis.Clone(GetDocument ());
  if ( !node )
    return 0;

  InsertAfterChild (afterThis, node);
  return node;
}

bool TiDocumentNodeChildren::RemoveChild( TiDocumentNode* removeThis )
{
  if ( removeThis->parent != this )
  {  
    assert( 0 );
    return false;
  }

  TiDocumentNode* removeThisPrev = Previous (removeThis);
  if ( removeThisPrev )
    removeThisPrev->next = removeThis->next;
  else
    firstChild = removeThis->next;

  // Needed to make freeing work ... but right?
  removeThis->parent = GetDocument();
  removeThis->next = 0;

  return true;
}

TiDocumentNode* TiDocumentNodeChildren::FirstChild( const char * value ) const
{
  TiDocumentNode* node;
  for ( node = firstChild; node; node = node->next )
  {
    const char* node_val = node->Value ();
    if (node_val && strcmp (node_val, value) == 0)
      return node;
  }
  return 0;
}

TiDocumentNode* TiDocumentNodeChildren::Previous (TiDocumentNode* child)
{
  TiDocumentNode* prev = 0;
  TiDocumentNode* node = firstChild;

  while (node != 0)
  {
    if (node == child) return prev;
    prev = node;
    node = node->next;
  }

  return 0;
}

TiDocumentNode* TiDocumentNodeChildren::LastChild ()
{
  TiDocumentNode* node = firstChild;

  while (node != 0)
  {
    TiDocumentNode* next = node->next;
    if (next == 0) return node;
    node = next;
  }

  return 0;
}

// -------------------------------------------------------------------------

void TiXmlElement::RemoveAttribute( const char * name )
{
  size_t nodeidx = attributeSet.Find (name);
  if ( nodeidx != (size_t)-1 )
  {
    attributeSet.set.DeleteIndex (nodeidx);
  }
}

TiXmlElement* TiDocumentNode::NextSiblingElement() const
{
  TiDocumentNode* node;

  for (  node = NextSibling();
  node;
  node = node->NextSibling() )
  {
    if ( node->ToElement() )
      return node->ToElement();
  }
  return 0;
}

TiXmlElement* TiDocumentNode::NextSiblingElement( const char * value ) const
{
  TiDocumentNode* node;

  for (  node = NextSibling( value );
  node;
  node = node->NextSibling( value ) )
  {
    if ( node->ToElement() )
      return node->ToElement();
  }
  return 0;
}



TiDocument* TiDocumentNode::GetDocument() const
{
  const TiDocumentNode* node;

  for( node = this; node; node = node->parent )
  {
    if ( node->ToDocument() )
      return node->ToDocument();
  }
  return 0;
}


TiXmlElement::TiXmlElement ()
{
  value = 0;
  type = ELEMENT;
}

TiXmlElement::~TiXmlElement()
{
}


void TiXmlElement::SetValue (const char * name)
{
  if (name == 0)
  {
    value = 0;
  }
  else
  {
    TiDocument* document = GetDocument ();
    csStringID name_id = document->strings.Request (name);
    const char* reg_name = document->strings.Request (name_id);
    value = reg_name;
  }
}

const char * TiXmlElement::Attribute( const char * name ) const
{
  size_t nodeidx = attributeSet.Find (name);

  if (nodeidx != (size_t)-1)
    return attributeSet.set[nodeidx].Value ();

  return 0;
}


const char * TiXmlElement::Attribute( const char * name, int* i ) const
{
  const char * s = Attribute( name );
  if ( i )
  {
    if ( s )
      *i = atoi( s );
    else
      *i = 0;
  }
  return s;
}


void TiXmlElement::SetAttribute( TiDocument* document,
  const char * name, int val )
{  
  csString buf;
  buf.Format ("%d", val);
  SetAttribute( document, name, buf );
}


TiDocumentAttribute& TiXmlElement::GetAttributeRegistered (
  const char * reg_name)
{
  size_t nodeidx = attributeSet.FindExact (reg_name);
  if (nodeidx != (size_t)-1)
  {
    return attributeSet.set[nodeidx];
  }

  TiDocumentAttribute at;
  size_t idx = attributeSet.set.Push (at);
  attributeSet.set[idx].SetName (reg_name);
  return attributeSet.set[idx];
}

void TiXmlElement::SetAttribute (TiDocument* document,
  const char * name, const char * value)
{
  csStringID name_id = document->strings.Request (name);
  const char* reg_name = document->strings.Request (name_id);
  GetAttributeRegistered (reg_name).SetValue (value);
}

static void StrPrintf (iString* file, const char* msg, ...)
{
  csString str;
  str.SetGrowsBy (0);
  va_list args;
  va_start (args, msg);
  str.FormatV (msg, args);
  va_end (args);
  file->Append (str);
}

static void StrPuts (const char* msg, iString* file)
{
  file->Append (msg);
}

void TiXmlElement::Print( iString* cfile, int depth ) const
{
  int d;
  for ( d=0; d<depth; d++ )
  {
    StrPrintf ( cfile, "    " );
  }

  StrPrintf ( cfile, "<%s", value );

  for (size_t i = 0 ; i < attributeSet.set.GetSize () ; i++)
  {
    const TiDocumentAttribute& attrib = attributeSet.set[i];
    StrPrintf ( cfile, " " );
    attrib.Print( cfile, depth );
  }

  // There are 3 different formatting approaches:
  // 1) An element without children is printed as a <foo /> node
  // 2) An element with only a text child is printed as <foo> text </foo>
  // 3) An element with children is printed on multiple lines.
  TiDocumentNode* node;
  if ( !firstChild )
  {
    StrPrintf ( cfile, " />" );
  }
  else if ( (firstChild->next == 0) && firstChild->ToText() )
  {
    StrPrintf ( cfile, ">" );
    firstChild->Print( cfile, depth + 1 );
    StrPrintf ( cfile, "</%s>", value );
  }
  else
  {
    StrPrintf ( cfile, ">" );

    for ( node = firstChild; node; node=node->NextSibling() )
    {
      if ( !node->ToText() )
      {
        StrPrintf ( cfile, "\n" );
      }
      node->Print( cfile, depth+1 );
    }
    StrPrintf ( cfile, "\n" );
    for( d=0; d<depth; ++d )
    StrPrintf ( cfile, "    " );
    StrPrintf ( cfile, "</%s>", value );
  }
}

csPtr<TiDocumentNode> TiXmlElement::Clone(TiDocument* document) const
{
  csRef<TiXmlElement> clone;
  void* p = document->blk_element.Alloc (sizeof (TiXmlElement));
  clone.AttachNew (new (p) TiXmlElement ());
  if ( !clone )
    return 0;

  clone->SetValueRegistered (Value ());
  CopyToClone( clone );

  // Clone the attributes, then clone the children.
  size_t i;
  for (i = 0 ; i < attributeSet.set.GetSize () ; i++)
  {
    const TiDocumentAttribute& attrib = attributeSet.set[i];
    clone->GetAttributeRegistered (attrib.Name ()).
      SetValue (attrib.Value ());
  }

  TiDocumentNode* lastNode = 0;
  for (TiDocumentNode*  node = firstChild; node; node = node->NextSibling() )
  {
    csRef<TiDocumentNode> newNode = node->Clone(document);
    clone->InsertAfterChild (lastNode, newNode);
    lastNode = newNode;
  }
  return csPtr<TiDocumentNode> (clone);
}


TiDocument::TiDocument() :
  deleteNest (0),
  strings (3541),
  blk_element (1000, DocHeapAlloc (&docHeap)),
  blk_text (1000, DocHeapAlloc (&docHeap))
{
  errorId = TIXML_NO_ERROR;
  //  ignoreWhiteSpace = true;
  type = DOCUMENT;
  parse.document = this;
}

TiDocument::TiDocument( const char * documentName ) :
  deleteNest (0),
  strings (3541),
  blk_element (1000, DocHeapAlloc (&docHeap)),
  blk_text (1000, DocHeapAlloc (&docHeap))
{
  //  ignoreWhiteSpace = true;
  value = documentName;
  errorId = TIXML_NO_ERROR;
  type = DOCUMENT;
  parse.document = this;
}

TiDocument::~TiDocument ()
{
  // Call explicit clear so that all children are destroyed
  // before 'blk_element' and 'blk_text' are destroyed.
  Clear ();
  // The Clear() call may have enqueue a number of nodes to delete,
  // and it needs to be emptied *before* we're destructed.
  EmptyDestroyQueue ();
}

csPtr<TiDocumentNode> TiDocument::Clone(TiDocument* document) const
{
  csRef<TiDocument> clone;
  clone.AttachNew (new TiDocument());
  if ( !clone )
    return 0;

  CopyToClone( clone );
  clone->errorId = errorId;
  clone->errorDesc = errorDesc.c_str ();

  TiDocumentNode* lastNode = 0;
  for (TiDocumentNode*  node = firstChild; node; node = node->NextSibling() )
  {
    csRef<TiDocumentNode> newNode = node->Clone(document);
    clone->InsertAfterChild (lastNode, newNode);
    lastNode = newNode;
  }
  return csPtr<TiDocumentNode> (clone);
}


void TiDocument::Print( iString* cfile, int depth ) const
{
  TiDocumentNode* node;
  for ( node=FirstChild(); node; node=node->NextSibling() )
  {
    node->Print( cfile, depth );
    StrPrintf ( cfile, "\n" );
  }
}

void TiDocumentAttribute::Print( iString* cfile, int /*depth*/ ) const
{
  TiXmlString n, v;

  TiXmlBase::PutString( Name(), &n );
  TiXmlBase::PutString( Value(), &v );

  char* idx = strchr (value, '\"');
  if (idx == 0)
    StrPrintf  (cfile, "%s=\"%s\"", n.c_str(), v.c_str() );
  else
    StrPrintf  (cfile, "%s='%s'", n.c_str(), v.c_str() );
}


void TiDocumentAttribute::SetIntValue( int value )
{
  csString buf;
  buf.Format ("%d", value);
  SetValue (buf);
}

void TiDocumentAttribute::SetDoubleValue( double value )
{
  csString buf;
  buf.Format ("%f", value);
  SetValue (buf);
}

int TiDocumentAttribute::IntValue() const
{
  return atoi (value);
}

double  TiDocumentAttribute::DoubleValue() const
{
  return atof (value);
}

void TiXmlComment::Print( iString* cfile, int depth ) const
{
  for ( int i=0; i<depth; i++ )
  {
    StrPuts ( "    ", cfile );
  }
  StrPrintf ( cfile, "<!--%s-->", value );
}

csPtr<TiDocumentNode> TiXmlComment::Clone(TiDocument* document) const
{
  csRef<TiXmlComment> clone;
  void* ptr = document->docHeap.Alloc (sizeof (TiXmlComment));
  clone.AttachNew (new (ptr) TiXmlComment());

  if ( !clone )
    return 0;

  CopyToClone( clone );
  return csPtr<TiDocumentNode> (clone);
}


void TiXmlText::SetValue (const char * name)
{
  if (name == 0)
  {
    value = 0;
  }
  else
  {
    TiDocument* document = GetDocument ();
    csStringID name_id = document->strings.Request (name);
    const char* reg_name = document->strings.Request (name_id);
    value = reg_name;
  }
}

void TiXmlText::Print( iString* cfile, int /*depth*/ ) const
{
  bool printCData = 
    (strchr (value, '\r') != 0) || (strchr (value, '\n') != 0);

  if (printCData)
  {
    StrPrintf ( cfile, "<![CDATA[%s]]>", value );
  }
  else
  {
    TiXmlString buffer;
    PutString( value, &buffer );
    StrPrintf ( cfile, "%s", buffer.c_str() );
  }
}


csPtr<TiDocumentNode> TiXmlText::Clone(TiDocument* document) const
{  
  csRef<TiXmlText> clone;
  void* p = document->blk_text.Alloc (sizeof (TiXmlText));
  clone.AttachNew (new (p) TiXmlText ());

  if ( !clone )
    return 0;

  CopyToClone( clone );
  return csPtr<TiDocumentNode> (clone);
}


TiXmlDeclaration::TiXmlDeclaration( const char * _version,
  const char * _encoding,
  const char * _standalone )
{
  version = _version;
  encoding = _encoding;
  standalone = _standalone;
  type = DECLARATION;
}


void TiXmlDeclaration::Print( iString* cfile, int /*depth*/ ) const
{
  StrPrintf  (cfile, "<?xml ");

  if ( !version.empty() )
    StrPrintf  (cfile, "version=\"%s\" ", version.c_str ());
  if ( !encoding.empty() )
    StrPrintf  (cfile, "encoding=\"%s\" ", encoding.c_str ());
  if ( !standalone.empty() )
    StrPrintf  (cfile, "standalone=\"%s\" ", standalone.c_str ());
  StrPrintf  (cfile, "?>");
}

csPtr<TiDocumentNode> TiXmlDeclaration::Clone(TiDocument* document) const
{  
  csRef<TiXmlDeclaration> clone;
  void* ptr = document->docHeap.Alloc (sizeof (TiXmlDeclaration));
  clone.AttachNew (new (ptr) TiXmlDeclaration());

  if ( !clone )
    return 0;

  CopyToClone( clone );
  clone->version = version;
  clone->encoding = encoding;
  clone->standalone = standalone;
  return csPtr<TiDocumentNode> (clone);
}


void TiXmlUnknown::Print( iString* cfile, int depth ) const
{
  for ( int i=0; i<depth; i++ )
    StrPrintf ( cfile, "    " );
  StrPrintf ( cfile, "<%s>", value.c_str() );
}

csPtr<TiDocumentNode> TiXmlUnknown::Clone(TiDocument* document) const
{
  csRef<TiXmlUnknown> clone;
  void* ptr = document->docHeap.Alloc (sizeof (TiXmlUnknown));
  clone.AttachNew (new (ptr) TiXmlUnknown());

  if ( !clone )
    return 0;

  CopyToClone( clone );
  return csPtr<TiDocumentNode> (clone);
}


size_t TiDocumentAttributeSet::Find (const char * name) const
{
  size_t i;
  for (i = 0 ; i < set.GetSize () ; i++)
  {
    if (strcmp (set[i].name, name) == 0) return i;
  }
  return (size_t)-1;
}

size_t TiDocumentAttributeSet::FindExact (const char * reg_name) const
{
  size_t i;
  for (i = 0 ; i < set.GetSize () ; i++)
  {
    if (set[i].name == reg_name) return i;
  }
  return (size_t)-1;
}

}
}
} // namespace CS
