/*
  Copyright (C) 2010-11 Christian Van Brussel, Institute of Information
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
#ifndef __CS_IVARIA_CAMERAMANAGER_H__
#define __CS_IVARIA_CAMERAMANAGER_H__

/**\file 
 * Helper class to control the motion of the camera from keyboard and mouse events.
 */

/**
 * \addtogroup appframe
 * @{ */

#include "cssysdef.h"
#include "csutil/scf.h"

struct iCamera;
struct iMovable;
class csVector3;

namespace CS {
namespace Utility {

/**
 * Various camera modes which can be used with CS::Utility::CameraManager.
 */
enum CameraMode
{
  CAMERA_NO_MOVE = 0,     /*!< The application will manage the camera by itself */
  CAMERA_MOVE_FREE,       /*!< The camera is free to move */
  CAMERA_MOVE_LOOKAT,     /*!< The camera is free to move but keeps looking at the target */
  CAMERA_ROTATE           /*!< The camera rotates around the target */
};

/**
 * A generic tool to control the motion of the camera through the keyboard
 * and/or the mouse.
 *
 * To use it, you need to load a CS::Utility::iCameraManager plugin, specify the camera to be
 * controlled with SetCamera(), and configure the behavior of the manager eg by defining
 * the camera mode through SetCameraMode(). After that, the camera manager will remain active
 * until it is destroyed.
 */
struct iCameraManager : public virtual iBase
{
  SCF_INTERFACE (CS::Utility::iCameraManager, 2, 0, 0);

  /// Set the camera to be controlled by this manager. This can be nullptr.
  virtual void SetCamera (iCamera* camera) = 0;
  /// Get the camera controlled by this manager, or nullptr if there are none.
  virtual iCamera* GetCamera () = 0;

  /// Set the camera mode to be used. The default value is CS::Utility::CAMERA_MOVE_NORMAL.
  virtual void SetCameraMode (CameraMode cameraMode) = 0;
  /// Return the current camera mode.
  virtual CameraMode GetCameraMode () const = 0;

  /**
   * Set the starting position of the camera. This position is used when ResetCamera() is called.
   * \sa ClearStartPosition()
   */
  virtual void SetStartPosition (csVector3 position) = 0;

  /**
   * Get the starting position of the camera. This position is used when ResetCamera() is called.
   */
  virtual csVector3 GetStartPosition () const = 0;

  /**
   * Clear the starting position of the camera. The next calls to ResetCamera() will now use the
   * current camera position set by SwitchCameraPosition().
   */
  virtual void ClearStartPosition () = 0;

  /**
   * Return whether or not a starting position has been defined by SetStartPosition().
   */
  virtual bool HasStartPosition () const = 0;

  /**
   * Switch to the next engine camera position (see iEngine::GetCameraPositions()). This position
   * will be used for each subsequent call to ResetCamera() (unless another position has been
   * set by SetStartPosition()).
   */
  virtual void SwitchCameraPosition () = 0;

  /**
   * Set the target of the camera, ie what it is looking at. This is relevant
   * only for the CS::Utility::CAMERA_MOVE_LOOKAT and CS::Utility::CAMERA_ROTATE camera
   * modes. The default value is 'csVector3 (0.0f, 0.0f, 0.0f)'.
   */
  virtual void SetCameraTarget (csVector3 position) = 0;

  /**
   * Get the target of the camera, ie what it is looking at. This is relevant
   * only for the CS::Utility::CAMERA_MOVE_LOOKAT and CS::Utility::CAMERA_ROTATE camera
   * modes. If the target of the camera is a iMovable, then this method will return the
   * current position of the iMovable.
   */
  virtual csVector3 GetCameraTarget () const = 0;

  /**
   * Set the closest distance there can be between the camera and its
   * target. This is relevant only for the CS::Utility::CAMERA_MOVE_LOOKAT and
   * CS::Utility::CAMERA_ROTATE camera modes. The default value is \a 0.1f.
   */
  virtual void SetCameraMinimumDistance (float distance) = 0;

  /**
   * Get the closest distance there can be between the camera and its
   * target. This is relevant only for the CS::Utility::CAMERA_MOVE_LOOKAT and
   * CS::Utility::CAMERA_ROTATE camera modes.
   */
  virtual float GetCameraMinimumDistance () const = 0;

  /**
   * Set whether the camera can be moved or not through the mouse.
   * The default value is true.
   * If enabled, then the camera will be moved when the user drags
   * the mouse while holding one of the following button:
   * - left button: the camera is moved sideways
   * - right button: the camera is rotated around the target returned
   * by CameraManager::GetCameraTarget().
   * - middle button: the camera is moved forward and backward. The camera
   * cannot get closer than CameraManager::GetCameraMinimumDistance().
   */
  virtual void SetMouseMoveEnabled (bool enabled) = 0;

  /**
   * Return whether the camera can be moved or not through the mouse.
   */
  virtual bool GetMouseMoveEnabled () const = 0;

  /**
   * Reset the camera position to its initial position. This position is either the one defined by
   * SetStartPosition() or the current engine camera position defined by SwitchCameraPosition().
   * If no starting position has been defined at all, then this method will simply don't do anything.
   */
  virtual void ResetCamera () = 0;

  /**
   * Set the speed of the camera's motion, in unit per second. The default value is 5.
   * Note that the camera moves ten times faster when the CTRL key is pressed. 
   */
  virtual void SetMotionSpeed (float speed) = 0;

  /**
   * Get the speed of the camera's motion, in unit per second.
   */
  virtual float GetMotionSpeed () const = 0;

  /**
   * Set the rotation speed of the camera, in radian per second. The default value is 2.
   * Note that the camera rotates five times faster when the CTRL key is pressed. 
   */
  virtual void SetRotationSpeed (float speed) = 0;

  /**
   * Get the speed of the camera's motion, in radian per second.
   */
  virtual float GetRotationSpeed () const = 0;

  /**
   * Set the target of the camera, ie what it is looking at. This is relevant
   * only for the CS::Utility::CAMERA_MOVE_LOOKAT and CS::Utility::CAMERA_ROTATE camera
   * modes.
   */
  virtual void SetCameraTarget (iMovable* target) = 0;
};

} //namespace Utility
} //namespace CS

/** @} */

#endif // __CS_IVARIA_CAMERAMANAGER_H__
