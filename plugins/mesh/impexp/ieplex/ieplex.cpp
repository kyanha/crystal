/*
    Copyright (C) 2001 by Martin Geisse <mgeisse@gmx.net>

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

#include "cssysdef.h"
#include "iutil/databuff.h"
#include "iutil/objreg.h"
#include "iutil/strvec.h"
#include "csutil/csstring.h"
#include "csutil/csvector.h"
#include "csutil/parray.h"
#include "csutil/refarr.h"
#include "iutil/plugin.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "imesh/mdlconv.h"
#include "imesh/mdldata.h"

#define MY_CLASSNAME	"crystalspace.modelconverter.multiplexer"

typedef csPArray<const csModelConverterFormat> csModelConverterFormatVector;

class csModelConverterMultiplexer : iModelConverter
{
private:
  csRef<iStrVector> classlist;
  csRefArray<iModelConverter> Converters;
  csRef<iPluginManager> plugin_mgr;

  bool LoadNextPlugin ();
public:
  SCF_DECLARE_IBASE;
  csModelConverterFormatVector Formats;

  /// constructor
  csModelConverterMultiplexer (iBase *p);

  /// destructor
  virtual ~csModelConverterMultiplexer ();

  bool Initialize (iObjectRegistry *object_reg);
  virtual int GetFormatCount ();
  virtual const csModelConverterFormat *GetFormat (int idx);
  virtual csPtr<iModelData> Load (uint8* Buffer, uint32 Size);
  virtual csPtr<iDataBuffer> Save (iModelData*, const char *Format);

  struct Component : public iComponent {
    SCF_DECLARE_EMBEDDED_IBASE (csModelConverterMultiplexer);
    virtual bool Initialize (iObjectRegistry *object_reg)
    { return scfParent->Initialize (object_reg); }
  } scfiComponent;
};

SCF_IMPLEMENT_IBASE (csModelConverterMultiplexer)
  SCF_IMPLEMENTS_INTERFACE (iModelConverter)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csModelConverterMultiplexer::Component)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_FACTORY (csModelConverterMultiplexer);

SCF_EXPORT_CLASS_TABLE (ieplex)
  SCF_EXPORT_CLASS (csModelConverterMultiplexer,
    MY_CLASSNAME,
    "Multiplexer for Model Converters")
SCF_EXPORT_CLASS_TABLE_END

CS_IMPLEMENT_PLUGIN

csModelConverterMultiplexer::csModelConverterMultiplexer (iBase *p)
{
  SCF_CONSTRUCT_IBASE (p);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);
}

csModelConverterMultiplexer::~csModelConverterMultiplexer ()
{
  if (classlist) classlist->DeleteAll ();
  // don't delete the elements of the 'formats' vector. We don't own them!
  classlist = NULL;
  plugin_mgr = NULL;
}

bool csModelConverterMultiplexer::Initialize (iObjectRegistry *object_reg)
{
  plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);

  // collect converter plugins
  classlist = csPtr<iStrVector> (
      iSCF::SCF->QueryClassList ("crystalspace.modelconverter."));

  return true;
}

bool csModelConverterMultiplexer::LoadNextPlugin ()
{
  if (!classlist) return false;
  
  csRef<iModelConverter> plugin;
  if (classlist && !plugin)
  {
    char const* classname = NULL;
    do
    {
      if (classname) classlist->Delete (0);
      if (classlist->Length() == 0)
      {
	classlist = NULL;
	plugin_mgr = NULL;
	return false;
      }
      classname = classlist->Get(0);
    } while (!strcasecmp (classname, MY_CLASSNAME));
    
    plugin = CS_LOAD_PLUGIN (plugin_mgr, classname, iModelConverter);
    if (plugin)
    {
      // remember the plugin
      Converters.Push (plugin);
      // and load its description, since we gonna return it on request
      int i;
      for (i=0; i<plugin->GetFormatCount (); i++)
	Formats.Push (plugin->GetFormat (i));
    }
    classlist->Delete (0);
  }
  return true;
}

int csModelConverterMultiplexer::GetFormatCount ()
{
  while (LoadNextPlugin ());
  return Formats.Length ();
}

const csModelConverterFormat *csModelConverterMultiplexer::GetFormat (int idx)
{
  while (LoadNextPlugin ());
  return Formats.Get (idx);
}

csPtr<iModelData> csModelConverterMultiplexer::Load (uint8* Buffer, uint32 Size)
{
  bool consecutive = false; // set to true if we searched the list completely.
  do
  {
    int i;
    for (i=Converters.Length(); (i--)>0; ) 
      // i is decremented after comparison but before we use it below;
      //  hence it goes from Converters.Length()-1 to 0
    {
      csRef<iModelConverter> conv = Converters.Get(i);
      csRef<iModelData> mdl (conv->Load (Buffer, Size));
      if (mdl)
      {
	/*
	  move used plugin to the bottom of the list.
	  the idea is that some formats are used more
	  commonly than other formats and that those
	  plugins are asked first. 
	 */
	if ((Converters.Length()-i) > 4)
	  // keep a 'top 4'; no need to shuffle the list
	  // when a plugin is already one of the first asked
	{
	  Converters.Push (conv);
	  Converters.Delete (i);
	}
	return csPtr<iModelData> (mdl);
      }
      // if we just loaded a plugin only check that.
      if (consecutive) break;
    }
    consecutive = true;
  } while (LoadNextPlugin());
  return NULL;
}

csPtr<iDataBuffer> csModelConverterMultiplexer::Save (iModelData *mdl,
	const char *Format)
{
  bool consecutive = false; // set to true if we searched the list completely.
  do
  {
    int i;
    for (i=Converters.Length(); (i--)>0; ) 
    {
      csRef<iModelConverter> conv = Converters.Get(i);
      csRef<iDataBuffer> dbuf (conv->Save (mdl, Format));
      if (dbuf)
      {
	if ((Converters.Length()-i) > 4)
	{
	  Converters.Push (conv);
	  Converters.Delete (i);
	}
	return csPtr<iDataBuffer> (dbuf);
      }
      // if we just loaded a plugin only check that.
      if (consecutive) break;
    }
    consecutive = true;
  } while (LoadNextPlugin());
  return NULL;
}
