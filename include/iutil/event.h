/*
    Event system related interfaces
    Written by Andrew Zabolotny <bit@eltech.ru>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __CS_IUTIL_EVENT_H__
#define __CS_IUTIL_EVENT_H__

#include "iutil/evdefs.h"
#include "csutil/scf.h"

/**\file
 * Event system related interfaces
 */
/**
 * \addtogroup event_handling
 * @{ */

struct iEventHandler;

struct iNetworkSocket2;
struct iNetworkPacket;

SCF_VERSION (iEvent, 0, 0, 2);

/**
 * This interface describes any system event.<p>
 * Events can be generated by hardware (keyboard, mouse)
 * as well as by software (commands and broadcasts). Not all
 * events neccessarily pass through the system event queue;
 * you may organize point-to-point event flows between some
 * plugins and so on.
 *<p>
 * The events can be generated by the event outlet (see the CreateEvent
 * method in iEventOutlet) if you don't want to create your own
 * implementations of this interface. On the other hand, if you want to
 * provide extra functionality you may subclass iEvent interface and
 * add another interface (say iExtEvent) then you may query that interface
 * using normal SCF QueryInterface method.
 */
struct iEvent : public iBase
{
  /// Event type (one of #csevKeyDown etc.)
  uint8 Type;			
  /// Event cathegory (unused by CSWS)
  uint8 Category;		
  /// Even finer granularity
  uint8 SubCategory;		
  /// Miscelaneous event flags
  uint8 Flags;			
  /// Time when the event occured
  csTicks Time;			
  union
  {
    struct
    {
      /// Key code
      int Code;			
      /// Character code
      int Char;			
      /// Control key state
      int Modifiers;		
    } Key;
    struct
    {
      /// Mouse coords
      int x,y;			
      /**
       * Button number: 1-left, 2-right, 3-middle 
       * (higher indices may occur)
       */
      int Button;		
      /// Control key state
      int Modifiers;		
    } Mouse;
    struct
    {
      /// Joystick number (1, 2, ...)	
      int number;		
      /// Joystick x
      int x;
      /// Joystick y
      int y;			
      /// Joystick button number
      int Button;		
      /// Control key state
      int Modifiers;		
    } Joystick;
    struct
    {
      /// Command code	
      uint Code;		
      /// Command info
      void *Info;		
    } Command;
    struct
    {
      /// Socket data recieved on	
      iNetworkSocket2 *From;	
      /// Packet of data recieved
      iNetworkPacket *Data;	
    } Network;
  };
};

/** \page EventFlow Overall structure of the basic event flow in Crystal Space
 * <pre>
 *   ......................
 *   .User application    .
 *   . +----------------+ .
 * +-->+ Event consumer | .
 * | . +----------------+ .
 * | ......................
 * |
 * | .....................................
 * | .System driver plugin               .
 * | .                          +------+ .   +-----+
 * | .                       +<-|event +-<<--+event|
 * | .   +---------------+   |  |outlet| .   |plug |
 * +-----+  event queue  +<--+  +------+ .   +-----+
 * | .   +---------------+   |  +------+ .   +-----+
 * | .                       +<-|event +-<<--+event|
 * | .   +---------------+   |  |outlet| .   |plug |
 * +-----+ event cord    +<--|  +------+ .   +-----+
 *   .   +---------------+   |  +------+ .   +-----+
 *   .                       +<-|event +-<<--+event|
 *   .                          |outlet| .   |plug |
 *   .                          +------+ .   +-----+
 *   .                            ....   .     ...
 *   .....................................
 * </pre>
 * The events are generated by 'event plugs' which are plugged into
 * 'event outlets'. The event outlets are reponsible for filtering
 * the possibly duplicate messages that are coming from different event
 * plugs (for example two different plugs may both intercept the keyboard
 * and generate duplicate keydown events).<p>
 * Events are put into the event queue, from where they are sent to
 * applications and plugins.
 * Event cords bypass the queue for specific command event categories
 * and deliver events immediately in a prioritizied chain to specific plugins
 * which request the categories.
 */

SCF_VERSION (iEventPlug, 0, 0, 1);

/**
 * Event plug interface, also referred as "event source".<p>
 * This interface should be implemented by any plugin that wants to be able
 * to generate events and to put them into system event queue. The plugin
 * registers itself with an event queue as an event source, and gets a
 * pointer to a new iEventOutlet object which manages event the event flow
 * from this particular event source.
 */
struct iEventPlug : public iBase
{
  /**
   * Get the mask of events that can be generated by this source
   * and are generated directly from user actions (e.g. key presses,
   * mouse clicks and so on). This is used to locate potentialy conflicting
   * combinations of event source plugins (for example two event sources
   * may generate a csevKeyDown event each from every key press).<p>
   * The mask is a combination of CSEVTYPE_XXX values ORed together.
   */
  virtual unsigned GetPotentiallyConflictingEvents () = 0;

  /**
   * Query how strong the plug's wish to generate certain class of events is.
   * The plug with the strongest wish wins. The argument is one of CSEVTYPE_XXX
   * values (but never a combination of several OR'ed together).<p>
   * The typical value is somewhere around 100; the event plugs which are
   * sometimes implemented inside the system drivers (such as for Windows
   * and DJGPP) usually have the priority 100.
   */
  virtual unsigned QueryEventPriority (unsigned iType) = 0;

  /**
   * Enable or disable certain event class(es).<p>
   * This is not a mandatory function; in fact most event plugs may safely
   * ignore it. The mean of this function is purely advisory; for example
   * if both keyup and keydown events are disabled the plug may want to
   * release the keyboard and so on.
   */
  virtual void EnableEvents (unsigned /*iType*/, bool /*iEnable*/) {}
};

SCF_VERSION (iEventOutlet, 0, 1, 0);

/**
 * The iEventOutlet is the interface to an object that is provided by
 * an event queue to every event plug when it registers itself. Any event
 * plug will interact with event outlet to put events into system queue
 * and so on.<p>
 * The event queue is responsible for detecting potentially conflicting
 * situations when several event plugs may generate a event from same
 * original event (e.g. a key press will cause several keydown events
 * coming from several event source plugins). In this case the event
 * sources are queried for how strong their "wish" to generate certain
 * types of events is, and the one with the strongest "wish" wins. Then
 * the respective outlet is set up such that any unwanted events coming
 * from 'disqualified' event plugs are discarded.
 */
struct iEventOutlet : public iBase
{
  /**
   * Create a event object on behalf of the event queue.<p>
   * A general function for generating virtually any type of event.  Since all
   * events for a particular event queue should be created from the same heap,
   * you should generate first a event object (through CreateEvent method) then
   * you fill it whatever you like and finally you insert it into the event
   * queue with the Post() method.
   */
  virtual iEvent* CreateEvent () = 0;

  /**
   * Put a previously created event into system event queue.<p>
   * <b>NOTE:</b> YOU SHOULD PASS HERE ONLY OBJECTS THAT WERE CREATED
   * VIA CreateEvent() FUNCTION! IF YOU PASS ARBITRARY EVENTS CREATED
   * BY YOUR PROGRAM/PLUGIN IN SOME ENVIRONMENTS IT WILL CRASH!
   */
  virtual void Post (iEvent*) = 0;

  /**
   * Put a keyboard event into event queue.<p>
   * Note that iKey is the key code, either the alphanumeric symbol
   * that is emmited by the given key when no shift keys/modes are
   * active (e.g. 'a', 'b', '.', '/' and so on) or one of CSKEY_XXX
   * values (with value above 255) and the iChar parameter is the
   * translated key, after applying all modeshift keys. Never assume
   * that any of these two codes is always less 127, not being 255
   * or 224 -- these are common mistakes for English-speaking programmers.
   *<p>
   * if you pass -1 as character code, the iChar argument is computed
   * using an simple internal translation table that takes care of
   * Control/Shift/Alt for English characters. But in general it is
   * hardly advised your driver to make the conversion using OS-specific
   * National Language Support subsystem so that national characters
   * are properly supported.
   */
  virtual void Key (int iKey, int iChar, bool iDown) = 0;

  /**
   * Put a mouse event into event queue.<p>
   * If iButton == 0, this is a mouse motion event, and iDown argument
   * is ignored. Otherwise an mousedown or mouseup event is generated at
   * respective location. iButton can be in the range from 1 to
   * CS_MAX_MOUSE_BUTTONS (or 0 for mouse move event).
   */
  virtual void Mouse (int iButton, bool iDown, int x, int y) = 0;

  /**
   * Put a joystick event into event queue.<p>
   * iNumber is joystick number (from 0 to CS_MAX_JOYSTICK_COUNT-1).<p>
   * If iButton == 0, this is a joystick move event and iDown is ignored.
   * Otherwise an joystick up/down event is generated. iButton can be from
   * 1 to CS_MAX_JOYSTICK_BUTTONS (or 0 for a joystick move event).
   */
  virtual void Joystick(int iNumber, int iButton, bool iDown, int x,int y) = 0;

  /**
   * Put a broadcast event into event queue.<p>
   * This is a generalized way to put any broadcast event into the system
   * event queue. Command code may be used to tell user application that
   * application's focused state has changed (cscmdFocusChanged), that
   * a graphics context has been resized (cscmdContextResize), that it
   * has been closed (cscmdContextClose), to finish the application
   * immediately (cscmdQuit) and so on.
   */
  virtual void Broadcast (int iCode, void *iInfo = NULL) = 0;

  /**
   * This is a special routine which is called for example when the
   * application is going to be suspended (suspended means "frozen",
   * that is, application is forced to not run for some time). This happens
   * for example when user switches away from a full-screen application on
   * any OS with MGL canvas driver, or when it presses <Pause> with the OS/2
   * DIVE driver, or in any other drivers that supports forced pausing of
   * applications.<p>
   * This generates a `normal' broadcast event with given command code;
   * the crucial difference is that the event is being delivered to all
   * clients *immediately*. The reason is that the application is frozen
   * right after returning from this routine thus it will get the next
   * chance to process any events only after it will be resumed (which
   * is kind of too late to process this kind of events).
   */
  virtual void ImmediateBroadcast (int iCode, void *iInfo) = 0;
};

SCF_VERSION (iEventCord, 0, 0, 3);

/**
 * The iEventCord is an interface provided by an event queue to
 * any event handlers wanting to receive some subclasses of events ASAP
 * in a specified priority, bypassing the queue itself.
 * Events may also optionally be sent to the normal event queue itself
 * if none of the event handlers in the cord handle the event.
 */
struct iEventCord : public iBase
{
  /**
   * Insert an event handler into the cord.  The priority defines when it
   * will receive the event with respect to other registered event handlers.
   * Event handlers with the same priority are handled in a first-come
   * first-served fashion.  This is significant since returning true from
   * HandleEvent() will stop further event processing.
   */
  virtual int Insert (iEventHandler*, int priority) = 0;

  /**
   * Remove an event handler from the cord.
   */
  virtual void Remove (iEventHandler*) = 0;

  /**
   * Returns true if events are passed on to the owning event queue if all
   * plugins in the cord return false from HandleEvent().
   */
  virtual bool GetPass () const = 0;

  /**
   * Sets whether events are passed along to the owning event queue if all
   * plugins in the cord return false from HandleEvent().
   */
  virtual void SetPass (bool) = 0;

  /// Get the category of this cord.
  virtual int GetCategory() const = 0;
  // Get the subcategory of this cord.
  virtual int GetSubcategory() const = 0;
};

/** @} */


#endif // __CS_IUTIL_EVENT_H__
