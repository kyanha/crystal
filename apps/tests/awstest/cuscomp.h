/*
    Copyright (C) 2001

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

#ifndef _AWSCPMP_H_
#define _AWSCOMP_H_

#include "iaws/awsecomp.h"

class CustomComponent : public awsEmbeddedComponent  
{
public:
  CustomComponent();
  ~CustomComponent();

  SCF_DECLARE_IBASE;

  void OnDraw(csRect clip);
  const char* Type();
  virtual bool Setup(iAws* manager, iAwsComponentNode* settings);
};

class CustomComponentFactory : public awsEmbeddedComponentFactory
{
public:
  CustomComponentFactory(iAws *manager);

  SCF_DECLARE_IBASE;

  iAwsComponent* Create();
};

#endif
