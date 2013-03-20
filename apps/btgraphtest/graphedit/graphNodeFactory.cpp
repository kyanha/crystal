/*
  Copyright (C) 2011 Christian Van Brussel, Eutyche Mukuama, Dodzi de Souza
      Institute of Information
      and Communication Technologies, Electronics and Applied Mathematics
      at Universite catholique de Louvain, Belgium
      http://www.uclouvain.be/en-icteam.html

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "cssysdef.h"

#include "graphNodeFactory.h"

GraphNodeFactory::GraphNodeFactory ()
{
}

GraphNodeFactory::GraphNodeFactory (const char* _name): name (_name)
{
}

void GraphNodeFactory::SetName (const char* name)
{
  this->name = name;
}

const char* GraphNodeFactory::GetName () const
{
  return name;
}

size_t GraphNodeFactory::GetParameterCount () const
{
  return optionDescriptions.GetSize ();
}

csOptionDescription const* GraphNodeFactory::GetParameterDescription (size_t index) const
{
  return &optionDescriptions.Get (index);
}

void GraphNodeFactory::AddParameter (csOptionDescription& description)
{
 optionDescriptions.Push (description);
}
