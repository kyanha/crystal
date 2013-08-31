/*
    Copyright (C) 2013 by Frank Richter

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

#include "csplugincommon/rendermanager/cameracache.h"

#include "iengine/engine.h"

namespace CS
{
  namespace RenderManager
  {
    void CameraCache::Initialize (iEngine* engine)
    {
      this->engine = engine;
    }

    iCamera* CameraCache::GetClone (iCamera* originalCam, uint syncFlags)
    {
      if (!engine) return nullptr;

      CameraClone* prevClone (clones.GetElementPointer (originalCam));
      if (prevClone)
      {
        iCamera* cam (prevClone->cam->GetCamera());
        if ((prevClone->origCamNum != originalCam->GetCameraNumber())
          || (prevClone->cloneCamNum != cam->GetCameraNumber()))
        {
          prevClone->origCamNum = originalCam->GetCameraNumber();
          if ((syncFlags & syncTransform) != 0)
            cam->SetTransform (originalCam->GetTransform());
          if ((syncFlags & syncSector) != 0)
            cam->SetSector (originalCam->GetSector());
          if ((syncFlags & syncMirrored) != 0)
            cam->SetMirrored (originalCam->IsMirrored());
          if ((syncFlags & syncFarPlane) != 0)
            cam->SetFarPlane (originalCam->GetFarPlane());
          if ((syncFlags & syncOnlyPortals) != 0)
            cam->OnlyPortals (originalCam->GetOnlyPortals());
          if ((syncFlags & syncProjection) != 0)
            prevClone->cam->SetProjectionMatrix (originalCam->GetProjectionMatrix());
          prevClone->cloneCamNum = cam->GetCameraNumber();
        }
        return cam;
      }

      csRef<iCustomMatrixCamera> newcam (engine->CreateCustomMatrixCamera (originalCam));
      CameraClone newClone (originalCam, newcam);
      clones.PutUnique (originalCam, newClone);
      return newcam->GetCamera();
    }

    void CameraCache::Purge ()
    {
      clones.Purge();
    }
  } // namespace RenderManager
} // namespace CS
