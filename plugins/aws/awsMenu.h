// awsMenu.h: interface for the awsMenu class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __AWSMENU_H__
#define __AWSMENU_H__

#include "aws/awsControlBar.h"
#include "aws/awstimer.h"
#include "csutil/scfstr.h"
#include "aws/awscmdbt.h"

/** You should be able to write your own class for menu entries as well if you
 *  don't like these simple ones. A menu entry should support the following:
 *  Properties:
 *     Selected -- Determines if the menu entry is the selected one. 
 *     PopupMenu -- Optional, but if present the popup it refers to will be shown
 *                  when the menu entry is selected
 *     Caption  -- The text you would generally show
 *     UserParam -- Just a a holder for any void* piece of data the user wants to
 *                  keep with this menu entry
 *     CloseSignal -- a constant that should be the value of the close signal
 *     SelectedSignal -- a constant that should be the value of the selected signal
 *  Signals:
 *     close    -- this should fire when te user has made some sort of choice from the
 *                  menu and the menu should now close.
 *     selected -- this should fire when the user selects/deselects a given entry
 *                 usually by putting the mouse over it. Which will be determined
 *                 by querying the selected property.
 */
class awsPopupMenu;

class awsMenuEntry : public awsPanel
{
private:
	iString* caption;
	awsPopupMenu* popup;
	bool selected;
  bool mouse_down;
  bool mouse_over;
	void* user_param;

  /// an image which will displayed to the left of the caption
  iTextureHandle* image;

  /// the max image width we will display,
  /// if the image is larger it will be cropped, if smaller empty space will be left
  int image_width;
  
  /// the max image height we will display,
  /// if the image is larger it will be cropped, if smaller empty space will be left
  int image_height;

  /// the image drawn to the right of the caption when there is a sub-menu
  iTextureHandle* sub_menu_image;

  /// the max sub_menu image width we will display
  int sub_menu_image_width;

  /// the max sub_menu image height we will display
  int sub_menu_image_height;

protected:
  void SizeToFit();

public:
	awsMenuEntry();
	~awsMenuEntry();

  virtual bool Setup(iAws* wmgr, awsComponentNode* settings);
	char* Type();
	
	virtual bool GetProperty(char* name, void **parm);
	virtual bool SetProperty(char* name, void *parm);

	virtual void OnDraw(csRect clip);

  virtual bool OnMouseEnter();
  virtual bool OnMouseExit(); 
  virtual bool OnMouseDown(int button, int x, int y);
  virtual bool OnMouseUp(int button, int x, int y);

	static const int signalClicked;
  static const int signalSelected;
};

class awsMenuEntryFactory : public awsComponentFactory
{
public:
  awsMenuEntryFactory(iAws* wmgr);
  ~awsMenuEntryFactory();

  iAwsComponent* Create();
};

class awsMenuBarEntry : public awsCmdButton
{
private:
  awsPopupMenu* popup;

public:
  awsMenuBarEntry();
  ~awsMenuBarEntry();

  virtual bool Setup(iAws* wmgr, awsComponentNode* settings);
  const char* Type();
	
  virtual bool GetProperty(const char* name, void **parm);
  virtual bool SetProperty(const char* name, void *parm);

  virtual bool OnMouseEnter();

};

class awsMenuBarEntryFactory : public awsComponentFactory
{
public:
  awsMenuBarEntryFactory(iAws* wmgr);
  ~awsMenuBarEntryFactory();

  iAwsComponent* Create();
};

class awsMenu : public awsControlBar  
{
protected:
  /// the currently selected component
  iAwsComponent* select;

  /// the component currently showing it's child 
  iAwsComponent* popup_showing;

  /// the 'child' menu
  awsMenu* child_menu;

  /// the 'parent' menu
  awsMenu* parent_menu;

  /// The current position of the mouse
  csPoint mouse_pos;

  /// a slots and sink for receiving events
  awsSlot slot_select, slot_close;
  awsSink sink;

  /// True if we currently have captured the mouse
  bool mouse_captured;

  /// True if we should not capture the mouse the next time it exits
  bool let_mouse_exit;

  virtual void SetMenuParent(awsMenu* parent_menu);
  virtual bool IsOverChildMenu(int x, int y);
  virtual bool IsOverParentMenu(int x, int y);
  virtual void SwitchPopups();
  virtual bool ShouldTrackMouse();
  virtual void StartPopupChange()=0;
  virtual void PositionPopupMenu(iAwsComponent* showing_entry, awsMenu* popup)=0;
  virtual iAwsComponent* GetNewDefaultEntry()=0;

public:
  awsMenu();
  virtual ~awsMenu();

  virtual bool Setup(iAws *wmgr, awsComponentNode *settings);
  virtual bool GetProperty(const char* name, void** parm);

  /// These can be used to add/remove any type of menu entry to the menu
  virtual void AddChild(iAwsComponent* comp);

  /// Adds a menu entry of the default type to the menu
  virtual iAwsSource* AddChild(const char* caption, iTextureHandle* image, awsPopupMenu* popup = NULL); 
  
  /// Removes a menu entry from the component. If you created the entry by name and
  /// have not yet IncRef'ed the item it will be destroyed automatically
  virtual void RemoveChild(iAwsComponent* comp);

  /// Removes a menu entry from the component by searching for the first which has this
  /// caption. If you created the entry by name and have not yet IncRef'ed the item
  /// it will be destroyed automatically
  virtual void RemoveChild(const char* caption);

  /// Removes a menu entry from the component associated with this source
  /// If you created the entry by name and have not yet IncRef'ed the item
  /// it will be destroyed automatically
  virtual void RemoveChild(iAwsSource* cmp);

  /// Puts an item in the selected state. This usually mimics what happens when
  /// a user places the mouse over a menu item.
  virtual void Select(iAwsComponent* child);

  /// A callback
  //static void OnTimer(void* param, iAwsSource* src);

  /// Hides the component
  virtual void Hide();

  // Keeps track of the mouse position
  virtual bool HandleEvent(iEvent &Event);

  virtual bool OnMouseExit();
  virtual bool OnMouseMove(int button, int x, int y);
  virtual bool OnMouseDown(int button, int x, int y);
  static void OnSelect(void* p, iAwsSource* src);
  static void OnClose(void* p, iAwsSource* src);

  
  /// Closes all the popup windows
  /// Called automatically when any menu item signals that the user has made a choice
  virtual void HideAllPopups()=0;

  // call this to when you show a popup so that the popup can properly react to
  // the mouse
  virtual void TrackMouse();
};
	
class awsMenuBar : public awsMenu 
{
public:
  awsMenuBar();
  virtual ~awsMenuBar();

  virtual const char* Type();
  virtual bool Setup(iAws *wmgr, awsComponentNode *settings);
  virtual void StartPopupChange();
  virtual void PositionPopupMenu(iAwsComponent* showing_entry, awsMenu* popup);
  virtual void HideAllPopups();
  virtual bool ShouldTrackMouse();
  virtual bool Create(iAws* manager, iAwsComponent* parent, awsComponentNode* settings);
  virtual iAwsComponent* GetNewDefaultEntry();

};

class awsMenuBarFactory : public awsComponentFactory
{
public:
  awsMenuBarFactory(iAws* wmgr);
  ~awsMenuBarFactory();

  iAwsComponent* Create();
};


class awsPopupMenu : public awsMenu 
{
private:
  /// a timer for delaying the changing of popups
  awsTimer* timer;

  /// a slot for the timer
  awsSlot slot_timer;

public:
  awsPopupMenu();
  virtual ~awsPopupMenu();

  virtual bool Create(iAws *wmgr, iAwsComponent* parent, awsComponentNode* settings);

  virtual bool Setup(iAws *wmgr, awsComponentNode *settings);
  virtual const char* Type();

  static void OnTimer(void* param, iAwsSource* src);
  virtual void StartPopupChange();
  virtual void PositionPopupMenu(iAwsComponent* showing_entry, awsMenu* popup);
  virtual void HideAllPopups();

  virtual void SwitchPopups();
  virtual bool OnMouseExit();
  virtual iAwsComponent* GetNewDefaultEntry();
};

class awsPopupMenuFactory : public awsComponentFactory
{
public:
  awsPopupMenuFactory(iAws* wmgr);
  ~awsPopupMenuFactory();

  iAwsComponent* Create();
};

#endif 

