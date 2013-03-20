/*
    Copyright (C) 2012 by Andrei Bârsan

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
#include "translation.h"
#include "iutil/vfs.h"
#include "cstool/initapp.h"
#include "ieditor/context.h"
#include "imap/reader.h"
#include "iutil/document.h"
#include "iutil/stringarray.h"

CS_PLUGIN_NAMESPACE_BEGIN (CSEditor)
{
  SCF_IMPLEMENT_FACTORY (EditorTranslation)

  using namespace std;

  const char* EditorTranslation::languageDir = "/data/editor/lang/";
  const char* EditorTranslation::language     = "de_DE";

  EditorTranslation::EditorTranslation (iBase* parent)
    : scfImplementationType (this, parent)
  {
  }

  EditorTranslation::~EditorTranslation () 
  {
  }

  bool EditorTranslation::Initialize(iEditor* editor)
  {
    this->editor = editor;
    object_reg = editor->GetContext ()->GetObjectRegistry ();

    // Prepare to load stuff
    csRef<iPluginManager> pluginManager = csQueryRegistry<iPluginManager> (object_reg);

    // Load translator and document system plugins
    translator = csLoadPlugin<iTranslator>(pluginManager, "crystalspace.translator.standard");
    if (!translator)
    {
      csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.editor.component.translation",
        "Could not find the translator plugin");
      return false;
    }

    csRef<iDocumentSystem> documentSystem =
      csLoadPlugin<iDocumentSystem>(pluginManager, "crystalspace.documentsystem.xmlread");
    if (!documentSystem)
    {
      csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.editor.component.translation",
        "Could not find the translator XML plugin");
      return false;
    }

    csRef<iVFS> vfs = csQueryRegistry<iVFS>(object_reg);

    csString langPath (languageDir);
    csString langFile; langFile.Format("%s.xml", language);

    csRef<iDataBuffer> path(vfs->GetRealPath(langPath.GetData()));
    if (!path.IsValid())
    {
      csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.editor.component.translation",
	      "Translation file path %s is not active",
	      CS::Quote::Double(langPath.GetData()));
      return false;
    }

#ifdef CS_DEBUG
    csRef<iStringArray> ls(vfs->FindFiles(langPath.GetData()));
    printf ("Available language files in /lang/:\n");
    for(size_t i = 0; i < ls->GetSize(); i++)
      printf ("%s\n", ls->Get(i));
#endif

    csString fullPath(langPath);
    fullPath += langFile;

    csRef<iFile> file(vfs->Open(fullPath.GetData() , VFS_FILE_READ));
    if(file.IsValid())
    {
      csRef<iDataBuffer> data(file->GetAllData());
      csRef<iDocument> document(documentSystem->CreateDocument());

      document->Parse(data->GetData());
      csRef<iDocumentNode> root = document->GetRoot();

      csRef<iLoaderPlugin> loader = csLoadPlugin<iLoaderPlugin> (object_reg, "crystalspace.translator.loader.xml");
      csRef<iBase> result = loader->Parse(root, 0, 0, translator);
      csRef<iTranslator> tl = csQueryRegistry<iTranslator>(object_reg);
    } else {
      csReport(object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.editor.component.translation",
        "Could not open file: %s",
        CS::Quote::Double(fullPath.GetData()));
      return false;
    }

    return true;
  }

  void EditorTranslation::Update ()
  {
  }

  void EditorTranslation::Save (iDocumentNode* node) const
  {
  }

  bool EditorTranslation::Load (iDocumentNode* node)
  {
    return false;
  }

}
CS_PLUGIN_NAMESPACE_END(CSEditor)
