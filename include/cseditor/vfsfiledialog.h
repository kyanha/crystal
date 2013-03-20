/*
    Copyright (C) 2003 by Philipp Aumayr

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
#ifndef __VFSFILEDIALOG_H__
#define __VFSFILEDIALOG_H__

struct iVFS;
class wxTreeCtrl;
class wxTextCtrl;
class wxComboBox;
class wxTreeItemId;
class wxTreeEvent;
class wxListCtrl;
class wxListEvent;
class wxTreeItemData;
class wxImageList;
class wxBitmap;
class wxButton;
class wxBitmapButton;
class wxStaticText;
class wxChoice;

#include "csutil/csstring.h"
#include "csutil/scf.h"

#include <wx/treectrl.h>
#include <wx/dialog.h>

namespace CS {
namespace EditorApp {

/**
 * Type of the operation to be made by a given VFSFileDialog.
 */
enum FileDialogType
{
  VFS_OPEN,    /*!< 'Open' operation. */
  VFS_SAVE     /*!< 'Save' operation. */
};

/**
 * Dialog for selecting a iVFS path.
 *
 * The following code snippet shows how to create and use the VFS dialog:
 * \code
 *  // Create the VFS file dialog.
 *  wxWindow* parent = ...
 *  VFSFileDialog dialog (parent, wxID_ANY, _("Select the file to open"),
 *                        wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE,
 *			  vfs, "/data/castle/world", VFS_OPEN);
 *
 *  // Display it and wait for the user leaving the dialog.
 *  if (!dialog.ShowModal ()) return false;
 *
 *  // Do something with the selected path.
 *  csString path = dialog.GetPath ();
 *  csString file = dialog.GetFilename ();
 *  // ...
 * \endcode
 */
class /*CS_CRYSTALSPACE_EXPORT*/ VFSFileDialog : public wxDialog
{
public:
  /**
   * Constructor.
   * \param parent Parent window of the dialog.
   * \param id ID of this dialog (can be wxID_ANY).
   * \param title Title of the dialog (typically displayed at the top of the window).
   * \param pos Position in screen space.
   * \param size Size in screen space.
   * \param style Style of the dialog.
   * \param vfs Optional reference to the iVFS plugin. If not provided, then it will be taken
   * from the iObjectRegistry.
   * \param startpath Starting path of the dialog.
   * \param dialogType Type of the operation to be made by this dialog.
   */
  VFSFileDialog
    ( wxWindow *parent,
      wxWindowID id,
      const wxString &title,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_DIALOG_STYLE,
      iVFS* vfs = nullptr,
      const csString& startpath = "/",
      FileDialogType dialogType = VFS_OPEN
      );
  virtual ~VFSFileDialog();

  /// Get the path selected by the user
  virtual const char* GetPath() { return curdvpath.GetData(); }
  /// Get the file selected by the user
  virtual const char* GetFilename() { return filename.GetData(); }

protected:
  //EVENT Methods
  void OnDirTreeExpandBranch( wxTreeEvent& event );
  void OnDirTreeCollapseBranch( wxTreeEvent& event );
  void OnDirTreeSelChange( wxTreeEvent& event );
  void OnRightClickItem( wxTreeEvent& event);

  void OnDirViewSelChange( wxListEvent& event );
  void OnDirViewActivated( wxListEvent & event );

  void OnPathTextEnter(wxCommandEvent& event);

  virtual void OnOk( wxCommandEvent& event );
  virtual void OnCancel( wxCommandEvent& event );

  void OnViewModeSelect( wxCommandEvent& event );

  void OnOpenParent( wxCommandEvent& event );

private:
  //Enumerators
  enum ctrllist
  {
    DIRECTORY_TREE = wxID_HIGHEST + 1,
    DIRECTORY_LIST,
		
    TEXT_PATH,

    CHOICE_FILTER,

    BUTTON_OPEN,
    BUTTON_CANCEL,
    BUTTON_NEW_FOLDER,
    BUTTON_PARENT,

    CHOICE_VIEW_MODE
  };

  enum imageId
  {
    IMAGE_FOLDER_OPEN,
    IMAGE_FOLDER_CLOSED,
    IMAGE_FILE,
    IMAGE_NEW_DIR,
    IMAGE_TO_PARENT,
    IMAGE_COUNT		//Should always be at the bottom.
  };

  enum viewTypes
  {
    LIST,
    REPORT,
    ICON,
    DETAIL
  };

  enum fileTypes
  {
    FOLDER,
    FILE
  };

  //Operations
  wxImageList* CreateImageList();
  csString GetPathByTreeItem(wxTreeItemId id);
  bool HasChildDir( wxTreeItemId parent );
  void InitFilterList(wxString choices []);
  void InitViewTypeList();
  bool IsDir( const char *path );
  bool IsFile( const char *path );
  bool LoadVFSDirView( const char *path);
  bool LoadVFSTreeBranch(wxTreeItemId parent, const char *path);

  void OpenTreeItemChild( wxString childName );

  //Images
  wxImageList* imageList;
  int imageIdList[IMAGE_COUNT];

  //Frame Elements
  wxTreeCtrl *dirtree;
  wxTextCtrl *thepath;
  wxChoice *filter;
  wxButton *okbutton;
  wxButton *cancelbut;
  wxStaticText *pathtext;
  wxListCtrl *fileView;
  wxChoice *viewTypes;
  wxBitmapButton *newFolderButton;
  wxBitmapButton *parentFolderButton;

  csString curdvpath;
  csString filename;
  FileDialogType dialogType;

  csRef<iVFS> vfs;

  class FileDlgTreeItemData : public wxTreeItemData
  {
  public:
    explicit FileDlgTreeItemData(const wxString& newData ) : data(newData) {}

    const wxChar *GetData() const { return data.c_str(); }
    void SetData(wxString newData) { data = newData; }

  private:
    wxString data;
  };

  DECLARE_EVENT_TABLE();
};

} // namespace EditorApp
} //namespace CS

#endif
