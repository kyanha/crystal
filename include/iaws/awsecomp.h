 #ifndef __AWS_EMBEDDED_COMPONENT_H__
 #define __AWS_EMBEDDED_COMPONENT_H__
/**************************************************************************
    Copyright (C) 2001 by Christopher Nelson

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
*****************************************************************************/
#include "iaws/aws.h"
#include "iaws/awsparm.h"
#include "iutil/event.h"

/**
 * Class used to create new AWS components.<p>
 * To create component from scratch, you need to subclass from awsEmbeddedComponent
 * After instantiating from that object, you need to call
 * Initialize() with a component gotten from
 * iAWS::CreateEmbeddableComponent().  After writing the code for that
 * component, you need to subclass from awsEmbeddedComponentFactory.  Do
 * your constant registrations, etc.  At runtime you simply need to
 * instantiate the derived factory, which handles registration and creation
 * for you. The instantiation of the component needs to happen
 * in the Factory in a function called Create().  
 */
class awsEmbeddedComponent : public iAwsComponent
{
  iAwsComponent *comp;

public:
  awsEmbeddedComponent() :comp(NULL) {}
  virtual ~awsEmbeddedComponent()
  { if (comp) comp->DecRef(); }

 public:

    /// Gets the component owner for this (sources are embedded)
    virtual iAwsComponent *GetComponent()
    {return this;}

    /// Registers a slot for a signal
    virtual bool RegisterSlot(iAwsSlot *slot, unsigned long signal)
    { return comp->RegisterSlot(slot, signal); }

    /// Unregisters a slot for a signal.
    virtual bool UnregisterSlot(iAwsSlot *slot, unsigned long signal)
    { return comp->UnregisterSlot(slot, signal); }

    /// Broadcasts a signal to all slots that are interested.
    virtual void Broadcast(unsigned long signal)
    { comp->Broadcast(signal); }

public:
    /// Sets the embedded component.  MUST BE CALLED BEFORE ANY OTHER FUNCTION!
    virtual void Initialize(iAwsComponent *component)
    { 
      comp=component; 
      if (comp) comp->IncRef ();
    }

    /// Sets up component
    virtual bool Setup(iAws *wmgr, awsComponentNode *settings)
    { return comp->Setup(wmgr, settings); }

    /// Event dispatcher, demultiplexes events and sends them off to the proper event handler
    virtual bool HandleEvent(iEvent& Event)
     {
       switch (Event.Type)
       {
       case csevMouseMove:
         return OnMouseMove (Event.Mouse.Button, Event.Mouse.x, Event.Mouse.y);
       case csevMouseUp:
         return OnMouseUp (Event.Mouse.Button, Event.Mouse.x, Event.Mouse.y);
       case csevMouseDown:
         return OnMouseDown (Event.Mouse.Button, Event.Mouse.x, Event.Mouse.y);
       case csevMouseClick:
         return OnMouseClick (Event.Mouse.Button, Event.Mouse.x, Event.Mouse.y);
       case csevMouseEnter:
         return OnMouseEnter ();
       case csevMouseExit:
         return OnMouseExit ();
       case csevKeyDown:
         return OnKeypress (Event.Key.Char, Event.Key.Modifiers);
       case csevGainFocus:
         return OnGainFocus ();
       case csevLostFocus:
         return OnLostFocus ();
       case csevFrameStart:
         return OnFrame ();
       }
       return false;
     }
 
     /// Gets the property specified, setting the parameter to a COPY of the property's value. Returns false if there's no such property.
    virtual bool GetProperty(char *name, void **parm)
    { return comp->GetProperty(name, parm); }

    /// Sets the property specified, setting the proprty to whatever is in parm. Returns false if there's no such property.
    virtual bool SetProperty(char *name, void *parm)
    { return comp->SetProperty(name, parm); }

    /// Executes scriptable actions for this window
    virtual bool Execute(char *action, iAwsParmList &parmlist)
    { return comp->Execute(action, parmlist); }

    /// Sets the flag (can handle multiple simultaneous sets)
    virtual void SetFlag(unsigned int flag)
    { comp->SetFlag(flag); }

    /// Clears the flag (can handle multiple simultaneous clears)
    virtual void ClearFlag(unsigned int flag)
    { comp->ClearFlag(flag); }

    /// Returns the current state of the flags
    virtual unsigned int Flags()
    { return comp->Flags(); }

    /// Invalidation routine: allow the component to be redrawn when you call this
    virtual void Invalidate()
    { comp->Invalidate(); }

    /// Invalidation routine: allow component to be redrawn, but only part of it
    virtual void Invalidate(csRect area)
    { comp->Invalidate(area); }

    /// Get this component's frame
    virtual csRect& Frame()
    { return comp->Frame(); }

    /// Returns the named TYPE of the component, like "Radio Button", etc. This should always be overridden.
    virtual char *Type()
    { return comp->Type(); }

    /// Returns true if this window overlaps the given rect.
    virtual bool Overlaps(csRect &r)
    { return comp->Overlaps(r); }

    /// Returns the state of the hidden flag
    virtual bool isHidden()
    { return comp->isHidden(); }

    /// Hides a component
    virtual void Hide()
    { comp->Hide(); }

    /// Shows a component
    virtual void Show()
    { comp->Show(); }

    /// Get's the unique id of this component.
    virtual unsigned long GetID()
    { return comp->GetID(); }

    /// Set's the unique id of this component. Note: only to be used by window manager.
    virtual void SetID(unsigned long _id)
    { comp->SetID(_id); }

    /// Recursively moves children (and all nested children) by relative amount given.
    virtual void MoveChildren(int delta_x, int delta_y)
    { comp->MoveChildren(delta_x, delta_y); }

public:
    /// Adds a child
    virtual void AddChild(iAwsComponent* child, bool owner=true)
    { comp->AddChild(child, owner); }

    /// Removes a child
    virtual void RemoveChild(iAwsComponent *child)
    { comp->RemoveChild(child); }

    /// Get's the number of children
    virtual int GetChildCount()
    { return comp->GetChildCount(); }

    /// Get's a specific child
    virtual iAwsComponent *GetChildAt(int i)
    { return comp->GetChildAt(i); }

    /// Returns true if this component has children
    virtual bool HasChildren()
    { return comp->HasChildren(); }

    /** Get's this components idea of the window manager.
      * Should be used internally by the component ONLY,
      * or by embedding classes. */
    iAws *WindowManager()
    { return comp->Window()->WindowManager(); }

    /// Get's the window that this component resides in.
    iAwsWindow *Window()
    { return comp->Window(); }

    /// Get's the parent component of this component;
    iAwsComponent *Parent()
    { return comp->Parent(); }

    /// Sets the window that this component resides in.
    virtual void SetWindow(iAwsWindow *win)
    { comp->SetWindow(win); }

    /// Sets the parent component of this component;
    virtual void SetParent(iAwsComponent *parent)
    { comp->SetParent(parent); }

public:
    /// Triggered when the component needs to draw
    virtual void OnDraw(csRect clip)
    { comp->OnDraw (clip); }

    /// Triggered when the user presses a mouse button down
    virtual bool OnMouseDown(int button, int x, int y)
    { return comp->OnMouseDown (button, x, y); }

    /// Triggered when the user unpresses a mouse button
    virtual bool OnMouseUp(int button, int x, int y)
    { return comp->OnMouseUp (button, x, y); }

    /// Triggered when the user moves the mouse
    virtual bool OnMouseMove(int button, int x, int y)
    { return comp->OnMouseMove (button, x, y); }

    /// Triggered when the user clicks the mouse
    virtual bool OnMouseClick(int button, int x, int y)
    { return comp->OnMouseClick (button, x, y); }

    /// Triggered when the user double clicks the mouse
    virtual bool OnMouseDoubleClick(int button, int x, int y)
    { return comp->OnMouseDoubleClick (button, x, y); }

    /// Triggered when this component loses mouse focus
    virtual bool OnMouseExit()
    { return comp->OnMouseExit (); }

    /// Triggered when this component gains mouse focus
    virtual bool OnMouseEnter()
    { return comp->OnMouseEnter (); }

    /// Triggered when the user presses a key
    virtual bool OnKeypress(int key, int modifiers)
    { return comp->OnKeypress (key, modifiers); }

    /// Triggered when the keyboard focus is lost
    virtual bool OnLostFocus()
    { return comp->OnLostFocus (); }

    /// Triggered when the keyboard focus is gained
    virtual bool OnGainFocus()
    { return comp->OnGainFocus (); }

    /// Gets the layout manager for this component.
    virtual awsLayoutManager *Layout()
    { return comp->Layout ();}

    /// Set the layout manager
    virtual void SetLayout(awsLayoutManager *layoutMgr)
    { comp->SetLayout(layoutMgr); }

    /// get the components preferred size, used by layout manager
    virtual csRect getPreferredSize()
    { return comp->getPreferredSize (); }

    /// get the components minimal size, used by layout manager
    virtual csRect getMinimumSize()
    { return comp->getMinimumSize (); }

    /// get the components insets, used by layout manager
    virtual csRect getInsets()
    { return comp->getInsets (); }

    /// does the component listen to events ?
    virtual bool isDeaf()
    { return comp->isDeaf (); }

    /// let the component listen to events or not
    virtual void SetDeaf (bool isDeaf)
    { comp->SetDeaf (isDeaf); }

    /// Triggered at the start of each frame
    virtual bool OnFrame()
    { return comp->OnFrame ();}

    /// Triggered when a child is added to the parent (triggered on the child)
    virtual void OnAdded()
    { comp->OnAdded ();}

    /// Triggered when a component is resized by the layout manager.
    virtual void OnResized()
    { comp->OnResized ();}

};

/**
 * Factory for custom AWS component. See also awsEmbeddedComponent.
 */
class awsEmbeddedComponentFactory : public iAwsComponentFactory
{
 protected:
    iAws *wmgr;

 public:
    /// Calls register to register the component that it builds with the window manager
    awsEmbeddedComponentFactory(iAws *_wmgr)
    {
      wmgr=_wmgr;
      if (wmgr) wmgr->IncRef();
    }

    /// Does nothing
    virtual ~awsEmbeddedComponentFactory()
    {
      if (wmgr) wmgr->DecRef();
    }

    /// Returns the current window manager
    iAws *WindowManager() { return wmgr; }

    /// Registers this factory with the window manager
    virtual void Register(char *type)
    {
      wmgr->RegisterComponentFactory(this, type);
    }

    /// Registers constants for the parser so that we can construct right.
    virtual void RegisterConstant(char *name, int value)
    {
      wmgr->GetPrefMgr()->RegisterConstant(name, value);
    }
};

#endif
