/*
    Copyright (C) 2012 by Jorrit Tyberghein

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

#ifndef __CS_CSTOOL_OBJECTCOMMENT_H__
#define __CS_CSTOOL_OBJECTCOMMENT_H__

/**\file
 * Saving multiple files.
 */

#include "csutil/scf.h"
#include "csutil/scfstr.h"
#include "csutil/csobject.h"
#include "imap/objectcomment.h"

/**
 * Default implementation of iObjectComment.
 */
class csObjectComment : public scfImplementationExt1<csObjectComment,csObject,iObjectComment>
{
private:
  csRef<scfString> comment;

public:
  csObjectComment () : scfImplementationType (this)
  {
    comment.AttachNew (new scfString ());
  }
  virtual ~csObjectComment () { }

  virtual iString* GetComment () { return comment; }
  virtual iObject* QueryObject () { return (csObject*)this; }
}; 

#endif // __CS_CSTOOL_OBJECTCOMMENT_H__

