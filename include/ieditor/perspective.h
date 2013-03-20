/*
    Copyright (C) 2012 by Christian Van Brussel, Jelle Hellemans

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

#ifndef __IEDITOR_PERSPECTIVE_H__
#define __IEDITOR_PERSPECTIVE_H__

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

namespace CS {
namespace EditorApp {

enum SplitMode
{
  SPLIT_NONE = 0,
  SPLIT_HORIZONTAL,
  SPLIT_VERTICAL
};

/**
 *
 */
struct iPerspectiveWindow : public virtual iBase
{
  SCF_INTERFACE (iPerspectiveWindow, 1, 0, 0);

  virtual void SetSpace (const char* pluginName) = 0;
  virtual void SetSplitMode (SplitMode mode) = 0;
  virtual void SetSplitPosition (int position) = 0;
  virtual iPerspectiveWindow* GetChild1 () const = 0;
  virtual iPerspectiveWindow* GetChild2 () const = 0;

/*
  virtual void Save (iDocumentNode* node) const = 0;
  virtual bool Load (iDocumentNode* node) = 0;
*/
};

/**
 *
 */
struct iPerspective : public virtual iBase
{
  SCF_INTERFACE (iPerspective, 1, 0, 0);

  virtual void SetName (const char* name) = 0;
  virtual const char* GetName () const = 0;

  virtual iPerspectiveWindow* GetRootWindow () const = 0;
/*
  virtual void Save (iDocumentNode* node) const = 0;
  virtual bool Load (iDocumentNode* node) = 0;
*/
};

/**
 * 
 */
struct iPerspectiveManager : public virtual iBase
{
  SCF_INTERFACE (iPerspectiveManager, 1, 0, 0);

  virtual iPerspective* CreatePerspective
    (const char* name, iPerspective* other = nullptr) = 0;
  virtual void DeletePerspective (size_t index) = 0;
  virtual size_t GetPerspectiveCount () const = 0;
  virtual iPerspective* GetPerspective (size_t index) const = 0;

/*
  virtual void Save (iDocumentNode* node) const = 0;
  virtual bool Load (iDocumentNode* node) = 0;
*/
};

} // namespace EditorApp
} // namespace CS

#endif // __IEDITOR_PERSPECTIVE_H__
