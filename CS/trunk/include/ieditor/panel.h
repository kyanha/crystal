/*
    Copyright (C) 2011 by Jelle Hellemans

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

#ifndef __IEDITOR_PANEL_H__
#define __IEDITOR_PANEL_H__

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

namespace CS {
namespace EditorApp {

struct iContext;
struct iLayout;
  
/**
 * A panel is a window in the editor.
 * 
 * This panel interface allows plugins to implement custom panels.
 */
struct iPanel : public virtual iBase
{
  SCF_INTERFACE (iPanel, 0, 0, 1);
  
  /**
   * Check whether this panel can be drawn.
   */
  virtual bool Poll (iContext* context) = 0;

  /**
   * This will be called when the UI needs to be redrawn or the context
   * changed.
   */
  virtual void Draw (iContext* context, iLayout* layout) = 0;

/*
  virtual void Save (iDocumentNode* node) const = 0;
  virtual bool Load (iDocumentNode* node) = 0;
*/
};

} // namespace EditorApp
} // namespace CS

#endif
