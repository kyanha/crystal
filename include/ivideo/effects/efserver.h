/*
    Copyright (C) 2002 by Anders Stenberg
    Written by Anders Stenberg

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

#ifndef __IEFFECTSERVER_H__
#define __IEFFECTSERVER_H__

#include "csutil/scf.h"
#include "csutil/strset.h"
#include "cstypes.h"

struct iEffectDefinition;
struct iEffectTechnique;

SCF_VERSION (iEffectServer, 0, 0, 1);

/**
 * Effect server
 */
struct iEffectServer : public iBase
{
public:
  virtual iEffectDefinition* CreateEffect() = 0;

  virtual bool Validate( iEffectDefinition* effect ) = 0;

  virtual iEffectTechnique* SelectAppropriateTechnique( iEffectDefinition* effect ) = 0;

  virtual csStringID RequestString( const char *s ) = 0;
  virtual const char* RequestString( csStringID id ) = 0;
};

#endif // __IEFFECTSERVER_H__
