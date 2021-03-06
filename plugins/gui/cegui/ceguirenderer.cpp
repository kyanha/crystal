/*
    Copyright (C) 2005 Dan Hardfeldt, Seth Yastrov and Jelle Hellemans

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

#include "ceguiimports.h"
#include "ceguirenderer.h"
#include "ceguigeometrybuffer.h"
#include "ceguitexturetarget.h"
#include "ceguitexture.h"
#include "ceguiwindowtarget.h"
#include "ceguicsimagecodec.h"
#include "ceguiresourceprovider.h"

#define DEFAULT_TEXTURE_SIZE 128

CS_PLUGIN_NAMESPACE_BEGIN(cegui)
{
  SCF_IMPLEMENT_FACTORY (Renderer)

  //----------------------------------------------------------------------------//
  CEGUI::String Renderer::d_rendererID(
    "CS::Renderer - Crystal Space renderer module.");

  //----------------------------------------------------------------------------//
  CEGUI::RenderingRoot& Renderer::getDefaultRenderingRoot()
  {
    return *d_defaultRoot;
  }

  //----------------------------------------------------------------------------//
  CEGUI::GeometryBuffer& Renderer::createGeometryBuffer()
  {
    GeometryBuffer* gb = new GeometryBuffer(obj_reg);
    d_geometryBuffers.push_back(gb);
    return *gb;
  }

  //----------------------------------------------------------------------------//
  void Renderer::destroyGeometryBuffer(const CEGUI::GeometryBuffer& buffer)
  {
    GeometryBufferList::iterator i = std::find(d_geometryBuffers.begin(),
      d_geometryBuffers.end(),
      &buffer);

    if (d_geometryBuffers.end() != i)
    {
      d_geometryBuffers.erase(i);
      delete &buffer;
    }
  }

  //----------------------------------------------------------------------------//
  void Renderer::destroyAllGeometryBuffers()
  {
    while (!d_geometryBuffers.empty())
      destroyGeometryBuffer(**d_geometryBuffers.begin());
  }

  //----------------------------------------------------------------------------//
  CEGUI::TextureTarget* Renderer::createTextureTarget()
  {
    Texture* texture = static_cast<Texture*> (&createTexture());
    TextureTarget* tt = new TextureTarget(*this, obj_reg, texture);
    d_textureTargets.push_back(tt);
    return tt;
  }

  //----------------------------------------------------------------------------//
  void Renderer::destroyTextureTarget(CEGUI::TextureTarget* target)
  {
    TextureTargetList::iterator i = std::find(d_textureTargets.begin(),
      d_textureTargets.end(),
      target);

    if (d_textureTargets.end() != i)
    {
      d_textureTargets.erase(i);
      delete target;
    }
  }

  //----------------------------------------------------------------------------//
  void Renderer::destroyAllTextureTargets()
  {
    while (!d_textureTargets.empty())
      destroyTextureTarget(*d_textureTargets.begin());
  }

  //----------------------------------------------------------------------------//
  CEGUI::Texture& Renderer::createTexture()
  {
    Texture* t = new Texture(this, obj_reg);
    csRef<iTextureHandle> texHandle = g3d->GetTextureManager()->CreateTexture
      (DEFAULT_TEXTURE_SIZE, DEFAULT_TEXTURE_SIZE, csimg2D,
       "rgb8", CS_TEXTURE_2D | CS_TEXTURE_NPOTS);
    t->SetTexHandle(texHandle);
    d_textures.push_back(t);
    return *t;
  }

  //----------------------------------------------------------------------------//
  CEGUI::Texture& Renderer::createTexture(const CEGUI::String& filename,
    const CEGUI::String& resourceGroup)
  {
    Texture* t = new Texture(this, obj_reg);
    t->loadFromFile(filename, resourceGroup);
    d_textures.push_back(t);
    return *t;
  }

  //----------------------------------------------------------------------------//
  CEGUI::Texture& Renderer::createTexture(const CEGUI::Size& size)
  {
    Texture* t = new Texture(this, obj_reg);
    csRef<iTextureHandle> texHandle = g3d->GetTextureManager()->CreateTexture
      (size.d_width, size.d_height, csimg2D, "rgb8", CS_TEXTURE_2D | CS_TEXTURE_NPOTS);
    t->SetTexHandle(texHandle);
    d_textures.push_back(t);
    return *t;
  }

  //----------------------------------------------------------------------------//
  CEGUI::Texture& Renderer::CreateTexture(iTextureHandle* htxt)
  {
    Texture* t = new Texture(this, obj_reg);
    t->SetTexHandle(htxt);
    d_textures.push_back(t);
    return *t;
  }

  //----------------------------------------------------------------------------//
  void Renderer::destroyTexture(CEGUI::Texture& texture)
  {
    TextureList::iterator i = std::find(d_textures.begin(),
      d_textures.end(),
      &texture);

    if (d_textures.end() != i)
    {
      d_textures.erase(i);
      delete &static_cast<Texture&>(texture);
    }
  }

  //----------------------------------------------------------------------------//
  void Renderer::destroyAllTextures()
  {
    while (!d_textures.empty())
      destroyTexture(**d_textures.begin());
  }

  //----------------------------------------------------------------------------//
  void Renderer::beginRendering()
  {
    // Nothing to be done here
  }

  //----------------------------------------------------------------------------//
  void Renderer::endRendering()
  {
    // Nothing to be done here
  }

  //----------------------------------------------------------------------------//
  const CEGUI::Size& Renderer::getDisplaySize() const
  {
    return d_displaySize;
  }

  //----------------------------------------------------------------------------//
  const CEGUI::Vector2& Renderer::getDisplayDPI() const
  {
    return d_displayDPI;
  }

  //----------------------------------------------------------------------------//
  uint Renderer::getMaxTextureSize() const
  {
    return d_maxTextureSize;
  }

  //----------------------------------------------------------------------------//
  const CEGUI::String& Renderer::getIdentifierString() const
  {
    return d_rendererID;
  }

  //----------------------------------------------------------------------------//
  Renderer::Renderer(iBase *parent) :
    scfImplementationType (this, parent),
    d_displayDPI(96, 96),
    initialized(false),
    obj_reg(0),
    events(0),
    scriptModule(0)
  {}

  //----------------------------------------------------------------------------//
  bool Renderer::Initialize (iObjectRegistry *reg) 
  {
    obj_reg = reg; 
    d_defaultTarget = new WindowTarget(*this, obj_reg);
    d_defaultRoot = new CEGUI::RenderingRoot(*d_defaultTarget);
    return true;
  }

  //----------------------------------------------------------------------------//
  bool Renderer::Initialize (iScript* script)
  {
    // Load the configuration file and the path for the log file
    csString logFile = "CEGUI.log";
    csRef<iVFS> vfs = csQueryRegistry<iVFS> (obj_reg);
    if (vfs)
    {
      // Read the config file
      csRef<iConfigManager> cfg = csQueryRegistry<iConfigManager> (obj_reg);
      cfg->AddDomain ("/config/cegui.cfg", vfs, iConfigManager::ConfigPriorityPlugin);
      logFile = cfg->GetStr ("CEGUI.LogFile", logFile);

      // TODO: Make sure that the filename provided is valid (e.g. all
      // characters are allowed)

      // Convert the virtual path of the log file in a physical path
      if (logFile.StartsWith ("VFS:"))
      {
	logFile.DeleteAt (0, 4);
	csRef<iDataBuffer> buffer (vfs->GetRealPath (logFile));
	if (buffer) logFile = (char *) buffer->GetData ();
      }
    }

    // Find the 3D renderer
    g3d = csQueryRegistry<iGraphics3D> (obj_reg);

    if (!g3d) {
      return false;
    }

    int w, h, a;

    // Initialize maximum texture size, CEGUI wants squares
    g3d->GetTextureManager ()->GetMaxTextureSize (w, h, a);

    if (w < h)
      d_maxTextureSize = w;
    else
      d_maxTextureSize = h;

    d_displaySize.d_width = g3d->GetWidth ();
    d_displaySize.d_height = g3d->GetHeight ();

    g2d = g3d->GetDriver2D ();

    if (!g2d)
      return false;

    ResourceProvider* rp = new ResourceProvider(obj_reg);
    ImageCodec* ic = new ImageCodec(obj_reg);

    if (script)
    {
      scriptModule = new CEGUIScriptModule (script, obj_reg);
      CEGUI::System::create (*this, rp, 0, ic, scriptModule, "", logFile.GetData ());
    }
    else
    {
      CEGUI::System::create (*this, rp, 0, ic, nullptr, "", logFile.GetData ());
    }

    settingsSliderFact.obj_reg = obj_reg;
    CEGUI::WindowFactoryManager::getSingletonPtr()->addFactory(&settingsSliderFact);

    settingComboBoxFact.obj_reg = obj_reg;
    CEGUI::WindowFactoryManager::getSingletonPtr()->addFactory(&settingComboBoxFact);

    g2d->SetMouseCursor (csmcNone);
    events = new CEGUIEventHandler (obj_reg, this);
    events->Initialize ();

    initialized = true;

    return true;
  }

  //----------------------------------------------------------------------------//
  Renderer::~Renderer()
  {
    destroyAllGeometryBuffers();
    destroyAllTextureTargets();
    destroyAllTextures(); 

    CEGUI::System* sys = CEGUI::System::getSingletonPtr();
    if (sys)
    {
      ResourceProvider* rp = static_cast<ResourceProvider*>(sys->getResourceProvider());
      ImageCodec* ic = &static_cast<ImageCodec&>(sys->getImageCodec());
      CEGUI::System::destroy();
      delete rp;
      delete ic;
    }
    
    delete d_defaultRoot;
    delete d_defaultTarget;
    
    delete scriptModule;
    delete events;
    
    RemoveAutoEventHandler ();
  }

  //----------------------------------------------------------------------------//

  /// Allow CEGUI to capture mouse events.
  void Renderer::EnableMouseCapture ()
  {
    events->EnableMouseCapture ();
  }

  /// Keep CEGUI from capturing mouse events.
  void Renderer::DisableMouseCapture ()
  {
    events->DisableMouseCapture ();
  }

  /// Allow CEGUI to capture keyboard events.
  void Renderer::EnableKeyboardCapture ()
  {
    events->EnableKeyboardCapture ();
  }

  /// Keep CEGUI from capturing keyboard events.
  void Renderer::DisableKeyboardCapture ()
  {
    events->DisableKeyboardCapture ();
  }

  //----------------------------------------------------------------------------//

  void Renderer::setDisplaySize (const CEGUI::Size& sz)
  {
    // Check if the size has changed
    if (fabs (sz.d_width - d_displaySize.d_width) > EPSILON
      || fabs (sz.d_height - d_displaySize.d_height) > EPSILON)
    {
      d_displaySize = sz;

      // Update the drawing area
      CEGUI::Rect area(d_defaultTarget->getArea());
      area.setSize(sz);
      d_defaultTarget->setArea(area);

      // Notify the CEGUI system that the display has changed
      CEGUI::System::getSingletonPtr()->notifyDisplaySizeChanged(sz);
    }
  }

  //----------------------------------------------------------------------------//
  
  void Renderer::SetAutoRender (bool autoRender)
  {
    if (autoRender)
      InstallAutoEventHandler ();
    else
      RemoveAutoEventHandler ();
  }
  
  bool Renderer::GetAutoRender ()
  {
    return autoRenderHandler.IsValid ();
  }
  
  void Renderer::InstallAutoEventHandler ()
  {
    if (!autoRenderHandler.IsValid ())
    {
      csRef<iEventQueue> eventQueue = csQueryRegistry<iEventQueue> (obj_reg);
      if (eventQueue)
      {
	autoRenderHandler.AttachNew (new AutoRenderEventHandler (this));
	eventQueue->RegisterListener (autoRenderHandler, csevFrame (obj_reg));
      }
    }
  }
  
  void Renderer::RemoveAutoEventHandler ()
  {
    if (autoRenderHandler.IsValid ())
    {
      csRef<iEventQueue> eventQueue = csQueryRegistry<iEventQueue> (obj_reg);
      if (eventQueue)
	eventQueue->RemoveListener (autoRenderHandler);
    }
    autoRenderHandler.Invalidate ();
  }

} CS_PLUGIN_NAMESPACE_END(cegui)
