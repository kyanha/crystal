#ifndef __CS_AWS_COMPONENT_H__
#define __CS_AWS_COMPONENT_H__

/**************************************************************************
    Copyright (C) 2000-2001 by Christopher Nelson

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
# include "iaws/aws.h"
# include "iaws/awsdefs.h"
# include "iutil/eventh.h"
# include "iutil/comp.h"
# include "csgeom/csrect.h"
# include "awsprefs.h"
# include "awsslot.h"

class awsCanvas;

/**************************************************************************************************************************
*   The general idea for a component's initialization stage is like this:                                                 *
*       1. construction - any internal structures should be created and intialized.                                       *
*       2. setup - the window manager calls Setup() on the component being created when it is added into the window       *
*       manager's hierarchy.  During setup the component should get a handle to the preference manager (via GetPrefMgr()) *
*       and ask the preference manager about things like it's default texture, colors, etc; using the Lookup* functions.  *
*       The component can then get texture handles from the preference manager using the values of the keys that it has   *
*       looked up.
*           e.g.
*
*            iAwsPrefManager *pm = wmgr->GetPrefMgr();
*
*            pm->LookupStringKey("Texture", texturename);
*
*            SetTexture(pm->GetPixmapFor(texturename));
*
*
*
*       
*                                                                                                                         *
**************************************************************************************************************************/
class awsComponent : public iAwsComponent
{
protected:
  /// The stored handle to the window manager, in case a component needs it.
  iAws *wmgr;

  /// The stored handle to the parent
  iAwsComponent *parent;

  /// The top child component
  iAwsComponent* top_child;

  /// The sibling below this one
  iAwsComponent* below;

  /// The sibling above this one
  iAwsComponent* above;

  /// The layout manager for this component (if one exists)
  csRef<iAwsLayoutManager> layout;

  /// The rectangle marking the frame of this component
  csRect frame;

  /// The stored un-zoomed frame size when the component is maximized
  csRect unzoomed_frame;

  /// True if the component is maximized
  bool is_zoomed;

  /// Every component will have a name, which is translated to an id
  unsigned long id;

  /// 32 bits worth of flags, like hidden, transparent, etc.
  unsigned int flags;

  /// Embedded awsSource
  awsSource signalsrc;


  /** Contains the redraw tag.  This tag changes everytime we redraw the window system, but only once per frame.
      we use it to keep track of which windows have been redrawn and which haven't.
     */
  unsigned int redraw_tag;

public:
  SCF_DECLARE_IBASE;

  awsComponent ();
  virtual ~awsComponent ();

public:
  /// Registers a slot for a signal
  virtual bool RegisterSlot (iAwsSlot *slot, unsigned long signal);

  /// Unregisters a slot for a signal.
  virtual bool UnregisterSlot (iAwsSlot *slot, unsigned long signal);

  /// Broadcasts a signal to all slots that are interested.
  virtual void Broadcast (unsigned long signal);

  /// Returns component that this belongs to.
  virtual iAwsComponent *GetComponent ();

  /** This function takes care of the creation tasks required to prepare this
    * component for use. If you create a component via the window manager's creation functions
    * then you should not call this, the window manager has done it for you. If you create
    * components programatically then you are encouraged to call this func to make setup
    * easier. For component developers, you should not need to override Create but 
    * rather do your setup work in Setup. 
    *
    * If it returns false then the component was not able to initialize properly and
    * shouldn't be used.
    **/
  virtual bool Create(iAws* mgr, iAwsComponent* parent, iAwsComponentNode* settings);

  /**
     *  This is the function that components use to set themselves up.  All components MUST implement this function.
     *  You should also call awsComponent::Setup() so that it can perform some default initialization work.
     */
  virtual bool Setup (iAws *wmgr, iAwsComponentNode *settings);

  /// Event dispatcher, demultiplexes events and sends them off to the proper event handler
  virtual bool HandleEvent (iEvent &Event);

  /// Gets the property specified, setting the parameter to a COPY of the property's value. Returns false if there's no such property.
  virtual bool GetProperty (const char* name, void **parm);

  /// Sets the property specified, setting the proprty to whatever is in parm. Returns false if there's no such property.
  virtual bool SetProperty (const char* name, void *parm);

  /// Executes a scriptable action
  virtual bool Execute (const char* action, iAwsParmList* parmlist);

  /// Invalidation routine: allow the component to be redrawn when you call this
  virtual void Invalidate ();

  /// Invalidation routine: allow component to be redrawn, but only part of it
  /// IMPORTANT: a csRect does not include the lines y = ymax and x = xmax
  /// so adjust your rect accordingly
  virtual void Invalidate (csRect area);

  /// Get this component's frame
  /// IMPORTANT: same as above, no drawing on the lines y = ymax and x = xmax
  virtual csRect Frame ();

  /// Get this component's client area
  virtual csRect ClientFrame();

  /// Returns the named TYPE of the component, like "Radio Button", etc.
  virtual const char* Type ();

  /// Sets the flag (can handle multiple simultaneous sets)
  virtual void SetFlag (unsigned int flag);

  /// Clears the flag (can handle multiple simultaneous clears)
  virtual void ClearFlag (unsigned int flag);

  /// Returns the current state of the flags
  virtual unsigned int Flags ();

  /// Returns true if this window overlaps the given rect.
  virtual bool Overlaps (csRect &r);

  /// Returns the state of the hidden flag
  virtual bool isHidden ();

  /// Returns true if this component is maximized
  virtual bool IsMaximized();

  /// Hides a component
  virtual void Hide ();

  /// Shows a component
  virtual void Show ();

  /// Returns the state of the DEAF flag
  virtual bool isDeaf ();

  /// set component deaf or "undeaf"
  virtual void SetDeaf (bool bDeaf);

  /// Get's the unique id of this component.
  virtual unsigned long GetID ();

  /// Set's the unique id of this component. Note: only to be used by window manager.
  virtual void SetID (unsigned long _id);

  /// Gets a child awsComponent by name, returns NULL on failure.
  virtual iAwsComponent *FindChild(const char* name);

  /// Returns the highet child (if any) whose frame contains (x,y).
  virtual iAwsComponent *ChildAt(int x, int y);

  /// Moves this component and all its children
  virtual void Move(int delta_x, int delta_y);

  /// Moves this component and all its children to the absolute coordinates
  virtual void MoveTo(int x, int y);

  /// Resizes the component and any nested children
  virtual void Resize(int width, int height);

  /// Resizes the component and any nested children
  virtual void ResizeTo(csRect newFrame);

  /// Maximize the component
  virtual void Maximize();

  /// Return the component to its un-maximized size
  virtual void UnMaximize();

  /// Moves this component above a sibling
  virtual void SetAbove(iAwsComponent* comp);

  /// Moves this component below a sibling
  virtual void SetBelow(iAwsComponent* comp);

  /// Moves this component above all its siblings
  virtual void Raise();

  /// Moves this component below all its siblings
  virtual void Lower();

  /** 
   *  Uses the current layout to update the location/size of all children
   */
  virtual void LayoutChildren ();

  /** 
   *  Adds a component to the layout, mostly used for custom components or
   * programmatic component creation.
   */
  virtual void AddToLayout(iAwsComponent* cmp, iAwsComponentNode* settings);

  /// Returns the redraw tag
  unsigned int RedrawTag();

  /// Sets the redraw tag
  void SetRedrawTag(unsigned int tag);

public:
  /** Adds a child into this component.  It's frame should be respective this component, not absolute.
     * This component can grab a reference to the child and dispose of it when it destructs, unless you
     * call RemoveChild() beforehand.  
     */
  virtual void AddChild (iAwsComponent *child);

  /** Removes a child from this component.  Important!! The child will be destroyed automatically if owner
     *  was true when you called AddChild().
     */
  virtual void RemoveChild (iAwsComponent *child);

  /// Get's the number of children
  virtual int GetChildCount ();

  /// Get's a specific child
  virtual iAwsComponent *GetTopChild ();

  /// Returns the sibling above this one
  virtual iAwsComponent *ComponentAbove ();

  /// Returns the sibling below this one
  virtual iAwsComponent *ComponentBelow ();

  /// Sets the sibling above this one
  virtual void SetComponentAbove (iAwsComponent* above);

  /// Sets the sibling below this one
  virtual void SetComponentBelow (iAwsComponent* below);

  /// Returns true if this component has children
  virtual bool HasChildren ();


  /** Get's this components idea of the window manager.
      * Should be used internally by the component ONLY,
      * or by embedding classes. */
  virtual iAws *WindowManager ();

  /// Get's the parent component of this component;
  virtual iAwsComponent *Parent ();

  /// Returns the lowest-level ancestor (starting with this component itself)
  /// which either has the window flag set or is top level
  virtual iAwsComponent *Window();

  /// Sets the layout manager for this component.
  virtual iAwsLayoutManager *Layout ();

  /// Sets the parent component of this component;
  virtual void SetParent (iAwsComponent *parent);

  /// Sets the layout manager for this component.
  virtual void SetLayout (iAwsLayoutManager *layout);

  /// Gets the preferred size of the component
  virtual csRect getPreferredSize ();

  /// Gets the minimum size that the component can be
  virtual csRect getMinimumSize ();

  /// Gets the inset amounts that are need to fit components properly.
  virtual csRect getInsets ();

  /// Triggered when a component is resized
  virtual void OnResized ();

public:
  /// Triggered when the component needs to draw
  virtual void OnDraw (csRect clip);

  /// Triggered when the user presses a mouse button down
  virtual bool OnMouseDown (int button, int x, int y);

  /// Triggered when the user unpresses a mouse button
  virtual bool OnMouseUp (int button, int x, int y);

  /// Triggered when the user moves the mouse
  virtual bool OnMouseMove (int button, int x, int y);

  /// Triggered when the user clicks the mouse
  virtual bool OnMouseClick (int button, int x, int y);

  /// Triggered when the user double clicks the mouse
  virtual bool OnMouseDoubleClick (int button, int x, int y);

  /// Triggered when this component loses mouse focus
  virtual bool OnMouseExit ();

  /// Triggered when this component gains mouse focus
  virtual bool OnMouseEnter ();

  /// Triggered when the user presses a key
  virtual bool OnKeypress (int key, int cha, int modifiers);

  /// Triggered when the keyboard focus is lost
  virtual bool OnLostFocus ();

  /// Triggered when the keyboard focus is gained
  virtual bool OnGainFocus ();

  /// Triggered at the beginning of each frame
  virtual bool OnFrame ();

  /// Triggered when a child is added to the parent (triggered on the child)
  virtual void OnAdded ();

  /// Triggered when a child moves or resizes
  virtual void OnChildMoved ();

  /// Triggered when the Raise function is called
  virtual void OnRaise();

  /// Triggered when the Lower function is called
  virtual void OnLower();

  /// Triggered when a child is shown
  virtual void OnChildShow();

  /// Triggered when a child is hidden
  virtual void OnChildHide();


protected:

  /// Recursively moves children (and all nested children) by relative amount given.
  virtual void MoveChildren (int delta_x, int delta_y);

  /// Unlinks this window from the window hierarchy.
  void Unlink ();

  /// Links this window in above the passed in component.  
  /// This component must be unlinked!
  void LinkAbove (iAwsComponent *comp);

  /// Links this component in below the passed in component.
  /// This component must be unlinked!
  void LinkBelow (iAwsComponent *comp);

  /// Sets the top child
  virtual void SetTopChild(iAwsComponent* comp);

  /// Performs the work of actually finding a child
  iAwsComponent *DoFindChild(unsigned id);

  /// Performs a consistency check on the linked list for debugging
  bool LinkedListCheck();

};

class awsComponentFactory : public iAwsComponentFactory
{
  iAws *wmgr;
public:
  SCF_DECLARE_IBASE;

  /// Calls register to register the component that it builds with the window manager
  awsComponentFactory (iAws *_wmgr);

  /// Does nothing
  virtual ~awsComponentFactory ();

  /// Returns the current window manager
  iAws *WindowManager ()  { return wmgr; }

  /// Returns a newly created component of the type this factory handles.
  virtual iAwsComponent *Create ();

  /// Registers this factory with the window manager
  void Register (const char* type);

  /// Registers constants for the parser so that we can construct right.
  void RegisterConstant (const char* name, int value);
};

class awsComponentVector : public csVector
{
public:
  iAwsComponent *Get(int idx) const; 
  int Push (iAwsComponent* comp); 
};

#endif // __CS_AWS_COMPONENT_H__




