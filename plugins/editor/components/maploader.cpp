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

#include "cssysdef.h"
#include "csutil/scf.h"
#include "ieditor/context.h"
#include "iengine/engine.h"
#include "imap/loader.h"
#include "imap/saver.h"
#include "iutil/cmdline.h"
#include "iutil/plugin.h"
#include "iutil/vfs.h"
#include "ivaria/pmeter.h"

#include "maploader.h"
#include "cseditor/vfsfiledialog.h"

#include <wx/frame.h>
#include <wx/menuitem.h>

CS_PLUGIN_NAMESPACE_BEGIN(CSEditor)
{

SCF_IMPLEMENT_FACTORY (MapLoader)

MapLoader::MapLoader (iBase* parent)
  : scfImplementationType (this, parent), loadingResource (nullptr)
{
}

MapLoader::~MapLoader()
{
  delete loadingResource;
}

bool MapLoader::Initialize (iEditor* editor)
{
  this->editor = editor;
  object_reg = editor->GetContext ()->GetObjectRegistry ();

  // Request the main loader plugins of Crystal Space
  csRef<iThreadedLoader> threadedLoader =
    csQueryRegistryOrLoad<iThreadedLoader> (object_reg, "crystalspace.level.threadedloader");
  if (!threadedLoader) return ReportError ("Failed to initialize the iThreadedloader plugin!");
  csRef<iLoader> loader = csQueryRegistryOrLoad<iLoader> (object_reg, "crystalspace.level.loader");
  if (!loader) return ReportError ("Failed to initialize the iLoader plugin!");

  // Setup the saveable flag of the engine
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  if (!engine) return ReportError ("Failed to locate 3D engine!");
  engine->SetSaveableFlag (true);

  // Load the iSaver plugin
  csRef<iSaver> saver = csQueryRegistryOrLoad<iSaver> (object_reg, "crystalspace.level.saver");
  if (!saver) return ReportError ("Failed to initialize the iSaver plugin!");

  vfs = csQueryRegistry<iVFS> (object_reg);
  if (!vfs) return ReportError ("Failed to locate Virtual File System!");

  // Setup the loading menus
  fileMenu = editor->GetMenuManager ()->GetSubMenu ("&File");
  openItem = fileMenu->AppendItem ("&Open file\tCTRL-O", "file.open");
  reloadItem = fileMenu->AppendItem ("&Reload file\tCTRL-R", "file.reload");
  saveItem = fileMenu->AppendItem ("&Save file\tCTRL-S", "file.save");

  saveItem->GetwxMenuItem ()->Enable (false);
  reloadItem->GetwxMenuItem ()->Enable (false);

  csEventID events[] = { openItem->GetEventID (),
			 reloadItem->GetEventID (),
			 saveItem->GetEventID (),
			 CS_EVENTLIST_END };
  RegisterQueue (editor->GetContext ()->GetEventQueue (), events);

  // Initialize the files to be loaded
  lastResource.path = "/lev/";
  lastResource.file = "";

  //lastResource.path = "/lev/castle/"; lastResource.file = "world";
  //lastResource.path = "/data/scaletest/"; lastResource.file = "world";
  //lastResource.path = "/data/frankie/"; lastResource.file = "frankie.xml";
  //lastResource.path = "/data/particles/"; lastResource.file = "fire1.cslib";
  //lastResource.path = "/data/partsys/"; lastResource.file = "world";
  //resourceData.Push (lastResource);

  // Analyze the command line arguments for a file to be loaded
  csRef<iCommandLineParser> cmdline =
    csQueryRegistry<iCommandLineParser> (object_reg);

  // Check for a new path to be mounted
  const char* realPath = cmdline->GetOption ("R");
  if (realPath)
  {
    // For some reason vfs Mount likes to have $/ instead of / for paths. 
    csString tempPath = realPath;
    tempPath.ReplaceAll ("/", "$/");

    if (!vfs->Mount ("/tmp/cseditor", tempPath))
      ReportError ("Cannot mount real path %s\n", CS::Quote::Single (realPath));

    //vfs->ChDir ("/tmp/cseditor");
    else realPath = "/tmp/cseditor";
  }

  // Check for a path and filename provided by the user
  csString filename = cmdline->GetName (0);
  csString vfsDir = cmdline->GetOption ("C");
  if (vfsDir.IsEmpty ())
    vfsDir = realPath;

  if (!filename && !vfsDir.IsEmpty ()) filename = "world";

  if (vfsDir.IsEmpty () && filename)
  {
    size_t index = filename.FindLast ('/');
    if (index != (size_t) -1)
    {
      vfsDir = filename.Slice (0, index);
      filename = filename.Slice (index + 1);
    }
  }

  // If there is a valid file provided then start loading it
  if (filename)
  {
    if (!vfsDir.IsEmpty () && vfsDir.GetAt (vfsDir.Length () - 1) != '/')
      vfsDir += '/';

    lastResource.path = vfsDir;
    lastResource.file = filename;
    resourceData.Push (lastResource);
  }

  return true;
}

void MapLoader::Update ()
{
  // Check the status of any resource currently loaded
  if (loadingResource && loadingReturn->IsFinished ())
  {
    reloadItem->GetwxMenuItem ()->Enable (true);

    if (loadingReturn->WasSuccessful ())
    {
      csString text;
      text.Format ("File %s%s was loaded successfully",
		   CS::Quote::SingleLeft (loadingResource->path.GetData ()),
		   CS::Quote::SingleRight (loadingResource->file.GetData ()));
      editor->ReportStatus (text.GetData ());

      csRef<iContextFileLoader> fileLoaderContext =
	scfQueryInterface<iContextFileLoader> (editor->GetContext ());
      fileLoaderContext->SetPath (loadingResource->path);
      fileLoaderContext->SetFilename (loadingResource->file);
      fileLoaderContext->SetCollection (collection);
    }

    else
    {
      csString text;
      text.Format ("Failed to load file %s%s",
		   CS::Quote::SingleLeft (loadingResource->path.GetData ()),
		   CS::Quote::SingleRight (loadingResource->file.GetData ()));
      editor->ReportStatus (text.GetData ());
    }

    //loadingReturn->IncRef ();

    delete loadingResource;
    loadingResource = nullptr;
    loadingReturn = nullptr;
    progressMeter = nullptr;
  }

  // If there are no loading active then push any pending one
  if (!loadingResource && resourceData.GetSize ())
  {
    loadingResource = new ResourceData ();
    *loadingResource = resourceData[0];
    resourceData.DeleteIndex (0);

    // Set the status text
    csString text;
    text.Format ("Loading file %s%s",
		 CS::Quote::SingleLeft (loadingResource->path.GetData ()),
		 CS::Quote::SingleRight (loadingResource->file.GetData ()));
    editor->ReportStatus (text.GetData ());

    // Create a progress meter
    progressMeter = editor->CreateProgressMeter ();
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    engine->Prepare (progressMeter);

    // Delete the previous collection from the engine and create a new one
    if (collection)
    {
      csRef<iContextFileLoader> fileLoaderContext =
	scfQueryInterface<iContextFileLoader> (editor->GetContext ());
      fileLoaderContext->SetCollection (nullptr);
      collection->ReleaseAllObjects ();
      engine->RemoveCollection (collection);
    }
    collection = engine->CreateCollection ("loader");

    // Start the file loading
    csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
    loadingReturn = loader->LoadFile (loadingResource->path, loadingResource->file, collection);
  }
}

void MapLoader::Save (iDocumentNode* node) const
{

}

bool MapLoader::Load (iDocumentNode* node)
{
  return false;
}

bool MapLoader::HandleEvent (iEvent &event)
{
  if (event.GetName () == openItem->GetEventID ())
  {
    // Ask the user to specify a file from the file dialog
    VFSFileDialog dialog ((wxWindow*) editor->GetFrame (0)->GetwxFrame (), wxID_ANY,
			  wxT("Select the file to open"),
			  wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE,
			  vfs, lastResource.path + lastResource.file, VFS_OPEN);
    dialog.CentreOnParent ();
    if (!dialog.ShowModal ()) return false;

    // Save the last file selected & push that file on the loading list
    lastResource.path = dialog.GetPath ();
    lastResource.file = dialog.GetFilename ();
    resourceData.Push (lastResource);

    return true;
  }

  else if (event.GetName () == reloadItem->GetEventID ())
  {
    if (!lastResource.file.IsEmpty ())
      resourceData.Push (lastResource);

    return true;
  }

  else if (event.GetName () == saveItem->GetEventID ())
  {
    printf ("TODO: Save!\n");
    return true;
  }

  return false;
}

}
CS_PLUGIN_NAMESPACE_END(CSEditor)

