/*
    Copyright (C) 2005 Dan Hardfeldt and Seth Yastrov

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

#ifndef _CS_CEGUI_RENDERER_H
#define _CS_CEGUI_RENDERER_H

/**\file 
*/
/**
* \addtogroup CEGUI
* @{ */

// hack: work around problems caused by #defining 'new'
#if defined(CS_EXTENSIVE_MEMDEBUG) || defined(CS_MEMORY_TRACKER)
# undef new
#endif
#include <new>
#include <CEGUI.h>
#if defined(CS_EXTENSIVE_MEMDEBUG) || defined(CS_MEMORY_TRACKER)
# define new CS_EXTENSIVE_MEMDEBUG_NEW
#endif

#include "csgeom/vector2.h"
#include "csgeom/vector3.h"
#include "csgeom/vector4.h"
#include "csutil/dirtyaccessarray.h"
#include "csutil/parray.h"
#include "csutil/ref.h"
#include "csutil/scf.h"
#include "iutil/comp.h"
#include "ivaria/icegui.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"

#include "ceguievthandler.h"
#include "ceguiresourceprovider.h"
#include "ceguiscriptmodule.h"

struct iObjectRegistry;

class csCEGUITexture;

/**
* The actual implementation of the CEGUI wrapper for CS.
*/
class csCEGUIRenderer : public CEGUI::Renderer, 
  public scfImplementation2<csCEGUIRenderer, 
			    iCEGUI,
			    iComponent>
{
public:
  /// Constructor.
  csCEGUIRenderer (iBase *parent);

  /// Destructor.
  virtual ~csCEGUIRenderer ();

  /**
   * Initialize the plugin.
   * \param script iScript plugin to use as a scripting module.
   */
  virtual bool Initialize (iScript* script=0);

  virtual bool IsInitialized () { return initialized; }

  /// Initialize with an iObjectRegistry pointer (called by plugin loader).
  virtual bool Initialize (iObjectRegistry *reg) 
  {
    obj_reg = reg; 
    return true;
  }

  /// Render the GUI.
  virtual void Render () const
  {
    CEGUI::System::getSingletonPtr()->renderGUI();
  }

  /// Get a pointer to the CEGUI system.
  virtual CEGUI::System* GetSystemPtr () const
  {return CEGUI::System::getSingletonPtr();}

  /// Get a pointer to the CEGUI font manager.
  virtual CEGUI::FontManager* GetFontManagerPtr () const
  {return CEGUI::FontManager::getSingletonPtr();}

  /// Get a pointer to the CEGUI global event set.
  virtual CEGUI::GlobalEventSet* GetGlobalEventSetPtr () const
  {return CEGUI::GlobalEventSet::getSingletonPtr();}

  /// Get a pointer to the CEGUI imageset manager.
  virtual CEGUI::ImagesetManager* GetImagesetManagerPtr () const
  {return CEGUI::ImagesetManager::getSingletonPtr();}

  /// Get a pointer to the CEGUI logger.
  virtual CEGUI::Logger* GetLoggerPtr () const
  {return CEGUI::Logger::getSingletonPtr();}

  /// Get a pointer to the CEGUI mouse cursor.
  virtual CEGUI::MouseCursor* GetMouseCursorPtr () const
  {return CEGUI::MouseCursor::getSingletonPtr();}

  /// Get a pointer to the CEGUI scheme manager.
  virtual CEGUI::SchemeManager* GetSchemeManagerPtr () const
  {return CEGUI::SchemeManager::getSingletonPtr();}

  /// Get a pointer to the CEGUI window factory manager.
  virtual CEGUI::WindowFactoryManager* GetWindowFactoryManagerPtr () const
  {return CEGUI::WindowFactoryManager::getSingletonPtr();}

  /// Get a pointer to the CEGUI window manager.
  virtual CEGUI::WindowManager* GetWindowManagerPtr () const
  {return CEGUI::WindowManager::getSingletonPtr();}

  /// Allow CEGUI to capture mouse events.
  void EnableMouseCapture ();

  /// Keep CEGUI from capturing mouse events.
  void DisableMouseCapture ();

  /// Allow CEGUI to capture keyboard events.
  void EnableKeyboardCapture ();

  /// Keep CEGUI from capturing keyboard events.
  void DisableKeyboardCapture ();

  /// Set the display size of the CEGUI render area.
  void setDisplaySize (const CEGUI::Size& sz);

  /// Destroy all textures that are still active.
  virtual void destroyAllTextures ();

private:
  bool initialized;
  iObjectRegistry* obj_reg;
  csCEGUIEventHandler* events;
  csCEGUIScriptModule* scriptModule;

  csRef<iGraphics3D> g3d;
  csRef<iGraphics2D> g2d;

  /// Add a quad to the renderer queue.
  virtual void addQuad (const CEGUI::Rect& dest_rect, float z, 
    const CEGUI::Texture* tex, const CEGUI::Rect& texture_rect, 
    const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quad_split_mode);

  /// Render the GUI.
  virtual void doRender ();

  /// Remove all meshes in the render list.
  virtual void clearRenderList ();

  /**
   * Set if queueing for quads should be enabled.
   * If enabled, queueing gives a large speed boost when quads are rendered
   * several times, although it is better to disable this
   * for the mouse cursor and similar things. This method is
   * called from CEGUI.
   */
  virtual void setQueueingEnabled (bool setting)
  {
    queueing = setting;
  }

  /// Create an empty texture.
  virtual CEGUI::Texture* createTexture ();

  /**
   * Create a texture based on a filename, belonging to a special resource
   * group.
   */
  virtual CEGUI::Texture* createTexture (const CEGUI::String& filename, 
    const CEGUI::String& resourceGroup);

  /// Create a texture from a CS texturehandle.
  virtual CEGUI::Texture* CreateTexture (iTextureHandle* htxt);

  /// Create an empty texture, but specify its size (square, and power of 2).
  virtual CEGUI::Texture* createTexture (float size);

  /// Destroy a texture, given the pointer to it.
  virtual void destroyTexture (CEGUI::Texture* texture);

  /// Check if queuing is enabled or not.
  virtual bool isQueueingEnabled () const
  {
    return queueing;
  }

  /// Get the display area width.
  virtual float getWidth () const
  {
    return m_displayArea.getWidth();
  }

  /// Get the display area height.
  virtual float getHeight () const
  {
    return m_displayArea.getHeight();
  }

  /// Get the display area size.
  virtual CEGUI::Size getSize () const
  {
    return m_displayArea.getSize();
  }

  /// Get the display area.
  virtual CEGUI::Rect getRect () const
  {
    return m_displayArea;
  }

  /// Get the maximum possible texture size.
  virtual uint getMaxTextureSize () const
  {
    return m_maxTextureSize;
  }

  /// Get the horizontal dots per inch.
  virtual uint getHorzScreenDPI () const
  {
    return 96;
  }

  /// Get the vertical dots per inch.
  virtual uint getVertScreenDPI () const
  {
    return 96;
  }

  /// Create a resource provider, which enables the use of VFS.
  virtual CEGUI::ResourceProvider* createResourceProvider ();

  /**
   * Adds all current vertices in buffer to a mesh, 
   * to enable rendering.
   */
  void UpdateMeshList();

  /// Render a quad directly instead of queueing it.
  void RenderQuadDirect (const CEGUI::Rect& dest_rect, float z, 
    const CEGUI::Texture* tex, const CEGUI::Rect& texture_rect, 
    const CEGUI::ColourRect& colours, CEGUI::QuadSplitMode quad_split_mode);

  /// Convert a CEGUI::colour to a CS csVector4 color
  csVector4 ColorToCS (const CEGUI::colour &color) const;

  CEGUI::Rect m_displayArea;

#define BUFFER_ARRAY(T, ElementStep)		\
  csDirtyAccessArray<T,				\
    csArrayElementHandler<T>,			\
    CS::Container::ArrayAllocDefault,		\
    csArrayCapacityFixedGrow<ElementStep*2048> >
  BUFFER_ARRAY(csVector2, 4) tcBuf;
  BUFFER_ARRAY(csVector4, 4) colBuf;
  BUFFER_ARRAY(csVector3, 4) vertBuf;
  BUFFER_ARRAY(uint, 6) indexBuf;
#undef BUFFER_ARRAY
  csRef<csRenderBufferHolder> bufHolder;
  bool buffersDirty;

  bool queueing;
  size_t queueStart;
  const csCEGUITexture* texture;

  //!< List used to track textures.
  csPDelArray<csCEGUITexture> textureList;

  csDirtyAccessArray<csSimpleRenderMesh> meshList;

  /// Maximum supported texture size (in pixels).
  uint m_maxTextureSize;
};

#endif
