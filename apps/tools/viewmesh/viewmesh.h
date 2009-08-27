/*
    Copyright (C) 2001 by Jorrit Tyberghein

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

#ifndef __VIEWMESH_H__
#define __VIEWMESH_H__

#include "cssysdef.h"
#include "csgfx/shadervarcontext.h"
#include "cstool/csapplicationframework.h"
#include "cstool/csview.h"
#include "cstool/meshobjtmpl.h"
#include "csutil/cmdhelp.h"
#include "csutil/cmdline.h"
#include "csutil/csbaseeventh.h"
#include "csutil/common_handlers.h"
#include "csutil/evoutlet.h"
#include "csutil/plugmgr.h"
#include "csutil/virtclk.h"
#include "csutil/xmltiny.h"
#include "iengine/camera.h"
#include "iengine/engine.h"
#include "iengine/light.h"
#include "iengine/material.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/sector.h"
#include "imap/loader.h"
#include "imap/writer.h"
#include "imesh/sprite3d.h"
#include "imesh/spritecal3d.h"
#include "ivaria/icegui.h"

struct iAnimatedMesh;
struct iAnimatedMeshFactory;
struct iSkeletonAnimNode2;
struct iSkeletonAnimNodeFactory2;

struct vmAnimCallback;

class ViewMesh : public csApplicationFramework, public csBaseEventHandler
{
 private:

  csRef<iCollection> collection;

  csVector3 camTarget;
  float     camDist;
  float     camYaw;
  float     camPitch;

  bool      camModePan;
  bool      camModeRotate;
  bool      camModeZoom;

  csString reloadFilename;

  int       lastMouseX, lastMouseY;

  csRef<iEngine> engine;
  csRef<iLoader> loader;
  csRef<iSaver> saver;
  csRef<iGraphics3D> g3d;
  csRef<iKeyboardDriver> kbd;
  csRef<iVirtualClock> vc;
  csRef<iVFS> vfs;
  csRef<iView> view;
  csRef<iCEGUI> cegui;
  csString renderLoop;
  iSector* room;
  int x,y;
  csRef<FramePrinter> printer;

  CEGUI::Window* form;
  CEGUI::Window* stddlg;
  enum { load, loadlib, save, savebinary, attach } stddlgPurpose;

  enum { movenormal, moveorigin, rotateorigin } camMode;

  float rotX, rotY;
  float roomsize, scale;
  float move_sprite_speed;

  csRef<iMeshWrapper> spritewrapper;
  csRef<iSprite3DFactoryState> sprite;
  csRef<iAnimatedMeshFactory> animeshsprite;
  csRef<iSpriteCal3DFactoryState> cal3dsprite;
  csRef<iSprite3DState> state;
  csRef<iAnimatedMesh> animeshstate;
  csRef<iSpriteCal3DState> cal3dstate;
  iSpriteSocket* selectedSocket;
  iSpriteCal3DSocket* selectedCal3dSocket;
  const char* selectedAnimation;
  const char* selectedMorphTarget;
  float meshTx, meshTy, meshTz;

  vmAnimCallback* callback;

  void ResetCamera();
  void UpdateCamera();
  void FixCameraForOrigin(const csVector3 & desiredOrigin);

  bool OnKeyboard (iEvent&);

  bool OnMouseDown (iEvent&);
  bool OnMouseUp (iEvent&);
  bool OnMouseMove (iEvent&);

  void Frame ();

  static void Help ();
  void HandleCommandLine();

  bool CreateRoom ();
  bool CreateGui ();
  void LoadLibrary(const char* file);
  void LoadTexture(const char* file, const char* name);
  void LoadSprite (const char* file);
  void SaveSprite (const char* file, bool binary);
  void AttachMesh (const char* file);
  void SelectSocket (const char* newsocket);
  void ScaleSprite (float newScale);
  void MoveLights (const csVector3 &a, const csVector3 &b, const csVector3 &c);
  void UpdateSocketList ();
  void UpdateMorphList ();
  void UpdateAnimationList ();
  void WalkSkel2Nodes (CEGUI::Listbox* list, iSkeletonAnimNodeFactory2* node);
  void UpdateSocket ();

  //SETTING
  bool CameraModeRotate (const CEGUI::EventArgs& e);
  bool CameraModeMoveOrigin (const CEGUI::EventArgs& e);
  bool CameraModeMoveNormal (const CEGUI::EventArgs& e);
  bool LightThreePoint (const CEGUI::EventArgs& e);
  bool LightFrontBackTop (const CEGUI::EventArgs& e);
  bool LightUnlit (const CEGUI::EventArgs& e);
  bool LoadButton (const CEGUI::EventArgs& e);
  bool LoadLibButton (const CEGUI::EventArgs& e);
  bool SaveButton (const CEGUI::EventArgs& e);
  bool SaveBinaryButton (const CEGUI::EventArgs& e);
  bool SetScaleSprite (const CEGUI::EventArgs& e);
  //ANIMATION
  bool ReversAnimation (const CEGUI::EventArgs& e);
  bool StopAnimation (const CEGUI::EventArgs& e);
  bool SlowerAnimation (const CEGUI::EventArgs& e);
  bool AddAnimation (const CEGUI::EventArgs& e);
  bool FasterAnimation (const CEGUI::EventArgs& e);
  bool SetAnimation (const CEGUI::EventArgs& e);
  bool RemoveAnimation (const CEGUI::EventArgs& e);
  bool ClearAnimation (const CEGUI::EventArgs& e);
  bool SelAnimation (const CEGUI::EventArgs& e);
  bool HandleSkel2Node (const char* animName, iSkeletonAnimNode2* node);
  //SOCKET
  bool SetMesh (const CEGUI::EventArgs& e);
  bool SetSubMesh (const CEGUI::EventArgs& e);
  bool SetTriangle (const CEGUI::EventArgs& e);
  bool SetRotX (const CEGUI::EventArgs& e);
  bool SetRotY (const CEGUI::EventArgs& e);
  bool SetRotZ (const CEGUI::EventArgs& e);
  bool AttachButton (const CEGUI::EventArgs& e);
  bool DetachButton (const CEGUI::EventArgs& e);
  bool AddSocket (const CEGUI::EventArgs& e);
  bool DelSocket (const CEGUI::EventArgs& e);
  bool SelSocket (const CEGUI::EventArgs& e);
  bool RenameSocket (const CEGUI::EventArgs& e);
  //MORPH
  bool SelMorph (const CEGUI::EventArgs& e);
  bool BlendButton (const CEGUI::EventArgs& e);
  bool ClearButton (const CEGUI::EventArgs& e);
  bool ResetCameraButton (const CEGUI::EventArgs& e);
  bool ReloadButton (const CEGUI::EventArgs& e);

 public:

  ViewMesh ();
  ~ViewMesh ();

  void OnExit ();
  bool OnInitialize (int argc, char* argv[]);

  bool Application ();

private:

  void StdDlgUpdateLists(const char* filename);

  bool StdDlgOkButton (const CEGUI::EventArgs& e);
  bool StdDlgCancleButton (const CEGUI::EventArgs& e);
  bool StdDlgFileSelect (const CEGUI::EventArgs& e);
  bool StdDlgDirSelect (const CEGUI::EventArgs& e);
  bool StdDlgDirChange (const CEGUI::EventArgs& e);

  CS_EVENTHANDLER_NAMES ("crystalspace.viewmesh")
  
  virtual const csHandlerID * GenericPrec (csRef<iEventHandlerRegistry> &r1, 
    csRef<iEventNameRegistry> &r2, csEventID event) const 
  {
    static csHandlerID precConstraint[2];
    
    precConstraint[0] = r1->GetGenericID("crystalspace.cegui");
    precConstraint[1] = CS_HANDLERLIST_END;
    return precConstraint;
  }

  CS_EVENTHANDLER_DEFAULT_INSTANCE_CONSTRAINTS
};

#endif // __VIEWMESH_H__
