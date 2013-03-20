/*
    Copyright (C) 2012 by Christian Van Brussel

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

#ifndef __IEDITOR_COMPONENTS_H__
#define __IEDITOR_COMPONENTS_H__

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

class wxTextCtrl;
class wxWindow;

namespace CS {
namespace EditorApp {
namespace Component {

/**
 * 
 */
struct iLogger : public virtual iBase
{
  SCF_INTERFACE (iLogger, 1, 0, 0);

  virtual void SetMaximumReportCount (size_t count) = 0;
  virtual size_t GetMaximumReportCount () const = 0;

  virtual wxTextCtrl* CreateTextCtrl (wxWindow* parent) = 0;
};

} // namespace Component
} // namespace EditorApp
} // namespace CS

#endif // __IEDITOR_COMPONENTS_H__
