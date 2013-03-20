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
#ifndef __GRAPHNODEFACTORY_H__
#define __GRAPHNODEFACTORY_H__

#include <stdarg.h>

#include "csutil/csstring.h"
//#include "iutil/pluginconfig.h"
#include "pluginconfig.h"

class csOptionDescription;

class GraphNodeFactory
{
 public:
  GraphNodeFactory ();
  GraphNodeFactory (const char* name);

  void SetName (const char* name);

  const char* GetName () const; 

  void SetId (int id) {this->id = id;}
  int GetId () {return id;}
       
  void AddParameter (csOptionDescription& description);

  size_t GetParameterCount () const;

  csOptionDescription const*  GetParameterDescription (size_t index) const;
        
 private:         
  csString name;
  int id;
  csArray<csOptionDescription> optionDescriptions;     
};
#endif 
