/*
    Copyright (C) 1998 by Jorrit Tyberghein and K. Robert Bate.
  
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

/*----------------------------------------------------------------
	Written by K. Robert Bate 1998.
----------------------------------------------------------------*/

#include <Devices.h>
#include <Dialogs.h>
#include <DiskInit.h>
#include <Events.h>
#include <Fonts.h>
#include <LowMem.h>
#include <Menus.h>
#include <Quickdraw.h>
#include <TextUtils.h>
#include <ToolUtils.h>
#include <Windows.h>
#include <DrawSprocket.h>
#include "sysdef.h"
#include "types.h"
#include "csutil/inifile.h"
#include "csgeom/math3d.h"
#include "cssys/system.h"
#include "cssys/sysdriv.h"
#include "csutil/inifile.h"
#include "IMacGraphics.h"
#include "igraph3d.h"
#include "igraph2d.h"

#define kAppleMenuID			128
#define kFileMenuID				129
#define kEditMenuID				130

#define kMenuBarID				128

#define kAlertOrWarningDialog	1024
#define kCommandLineDialog		1025
#define kDepthFailureDialog		1026
#define kChangeDepthDialog		1027
#define kAboutDialog			1028

#define kCommandLineString		1024

static OSErr GetPath( FSSpec theFSSpec, char *theString );
static OSErr AppleEventHandler( AppleEvent *event, AppleEvent *reply, long refCon );
static AEEventHandlerUPP AppleEventHandlerUPP = NULL;
static SysSystemDriver * gSysSystemDriver = NULL;


IMPLEMENT_IBASE(SysSystemDriver)
    IMPLEMENTS_INTERFACE(iSystem)
IMPLEMENT_IBASE_END

SysSystemDriver::SysSystemDriver()
				 : csSystemDriver ()
{
	unsigned int		i;
	ProcessSerialNumber	theCurrentProcess;
	ProcessInfoRec		theInfo;

	CONSTRUCT_IBASE(NULL);

	gSysSystemDriver = this;

 	/*
 	 *	Initialize all the needed managers.
 	 */
	::InitGraf(&qd.thePort);
	::InitFonts();
	::InitWindows();
	::InitMenus();
	::TEInit();
	::InitDialogs(nil);
	::InitCursor();
	::FlushEvents ( everyEvent, 0 );

	::GetDateTime((unsigned long*) &qd.randSeed);

	/*
	 *	Initialise keyboard state.
	 */
	for ( i = 0; i < 4; ++i )
		mKeyboardState[i] = 0L;

	/*
	 *	Get the application name and location on disk.
	 */
	GetCurrentProcess( &theCurrentProcess );
	theInfo.processInfoLength = sizeof( ProcessInfoRec );
	theInfo.processName = mAppName;
	theInfo.processAppSpec = &mAppLocation;
	GetProcessInformation( &theCurrentProcess, &theInfo );
	mAppName[ mAppName[0] + 1 ] = '\0';

	/*
	 *	Setup the handler for the apple events.
	 */
	AppleEventHandlerUPP = NewAEEventHandlerProc( AppleEventHandler );
	if ( AppleEventHandlerUPP )
		AEInstallEventHandler( typeWildCard, typeWildCard, 
							  (AEEventHandlerUPP)AppleEventHandlerUPP, 0, FALSE );

	/*
	 *	Intialize the command line.
	 */
	CommandLine[0] = '\0';
}


SysSystemDriver::~SysSystemDriver()
{
	/*
	 *	If the handler for the apple events were setup,
	 *	remove them.
	 */
	if ( AppleEventHandlerUPP ) {
		AERemoveEventHandler( typeWildCard, typeWildCard, 
							  (AEEventHandlerUPP)AppleEventHandlerUPP, FALSE );
		DisposeRoutineDescriptor( AppleEventHandlerUPP );
		AppleEventHandlerUPP = NULL;
	}
}


void SysSystemDriver::Close(void)
{
}


static void NLtoCR( UInt8 *theString )
{
	int i = *theString++;

	while ( i-- ) {
		if ( '\n' == *theString )
			*theString = '\r';
		++theString;
	}
}


void SysSystemDriver::Alert(const char* s)
{
	Str255	theMessage;

	strcpy( (char *)&theMessage[1], s );
	theMessage[0] = strlen( s );
	NLtoCR( theMessage );
	ParamText( "\pFatal Error", theMessage, "\p", "\p" );
	StopAlert( kAlertOrWarningDialog, NULL );
}


void SysSystemDriver::Warn(const char* s)
{
	Str255	theMessage;

	strcpy( (char *)&theMessage[1], s );
	theMessage[0] = strlen( s );
	NLtoCR( theMessage );
	ParamText( "\pWarning", theMessage, "\p", "\p" );
	CautionAlert( kAlertOrWarningDialog, NULL );
}


bool SysSystemDriver::Initialize (int argc, char *argv[], const char *iConfigName )
{
	Handle			theMenuBar;
	MenuHandle		theMenu;
	Str255			theText;

	argc = GetCommandLine( &argv );
	if ( ! csSystemDriver::Initialize ( argc, argv, iConfigName ))
		return false;

	/*
	 *	Get the menu bar for this application
	 */

	theMenuBar = GetNewMBar( kMenuBarID );
	if ( theMenuBar ) {
		SetMenuBar( theMenuBar );

		/*
		 *	Add the items in the apple menu
		 */
		theMenu = GetMenuHandle( kAppleMenuID );
		if ( theMenu ) {
			AppendResMenu( theMenu, 'DRVR' );
		}
		strcpy( (char *)&theText[1], "About " );
		strcat(  (char *)&theText[1], (char *)&mAppName[1] );
		strcat(  (char *)&theText[1], "�" );
		theText[0] = strlen( (char *)&theText[1] );
		SetMenuItemText( theMenu, 1, theText );

		/*
		 *	Disable the edit menu
		 */
		theMenu = GetMenuHandle( kEditMenuID );
		if ( theMenu ) {
			DisableItem( theMenu, 0 );
		}

		/*
		 *	Draw the menu bar onto the screen
		 */
		DrawMenuBar();
	} else {
		return false;
	}

	return true;
}


void SysSystemDriver::Loop(void)
{
    long prev_time;
    time_t current_time;
	EventRecord anEvent;
	iMacGraphics* piG2D = NULL;
	bool	driverNeedsEvent = false;
	bool	eventHandled;

	piG2D = QUERY_INTERFACE(G2D, iMacGraphics);

	if (piG2D) {
		driverNeedsEvent = piG2D->DoesDriverNeedEvent();
      	piG2D->SetColorPalette();
	}

    prev_time = current_time = Time();
	while (( ! Shutdown ) && ( ! ExitLoop )) {
    	NextFrame( current_time - prev_time, current_time );
    	prev_time = current_time;

#if SCAN_KEYBOARD
		ScanKeyboard( current_time );
#else
		Keyboard->Reset();
#endif

		if ( WaitNextEvent( everyEvent, &anEvent, 1, NULL ) ) {
			eventHandled = false;
			if (( driverNeedsEvent ) && ( piG2D )) {
				if ( piG2D->HandleEvent( &anEvent ))
					eventHandled = true;
			}
			if ( ! eventHandled ) {
				DispatchEvent( current_time, &anEvent, piG2D );
			}
		} else {
			Point	theMouse;
			bool	isIn = false;

			theMouse = anEvent.where;
			if (( piG2D ) && ( piG2D->PointInWindow( &theMouse ) )) {
				Mouse->do_mousemotion( current_time, theMouse.h, theMouse.v );
			}
		}
		current_time = Time();
	}

	if ( piG2D ) {
		piG2D->DecRef();
		piG2D = NULL;
	}
}


void SysSystemDriver::DispatchEvent( time_t current_time, EventRecord *theEvent, iMacGraphics* piG2D )
{
	// dispatch the event according to its type and location
	
	switch ( theEvent->what )
	{
		case mouseDown:
		case mouseUp:
			HandleMouseEvent( current_time, theEvent, piG2D );
			break;

		case activateEvt:
			{
				if ( piG2D )
					piG2D->ActivateWindow( (WindowPtr) theEvent->message, theEvent->modifiers & activeFlag );
			}
			break;

		case keyDown:
		case autoKey:
			{
				char theKey = theEvent->message & charCodeMask;
				char theCode = theEvent->message & keyCodeMask;
				HandleKey( current_time, theKey, theCode, theEvent->modifiers, true );
			}
			break;

		case keyUp:
			{
				char theKey = theEvent->message & charCodeMask;
				char theCode = theEvent->message & keyCodeMask;
				HandleKey( current_time, theKey, theCode, theEvent->modifiers, false );
			}
			break;

		case updateEvt:
			{
				bool	updateDone = false;
				if ( piG2D )
					updateDone = piG2D->UpdateWindow( (WindowPtr) theEvent->message );
			}
			break;

		case diskEvt:
				Point dPt = {100,100};					
				if( HiWord( theEvent->message ) != 0)
					DIBadMount( dPt, theEvent->message );
				break;

		case kHighLevelEvent:
			HandleHLEvent( current_time, theEvent );
			break;

		case osEvt:
			HandleOSEvent( current_time, theEvent, piG2D );
			break;

		default:
			break;
	}
}


void SysSystemDriver::HandleMouseEvent( time_t current_time, EventRecord *theEvent, iMacGraphics* piG2D )
{
	WindowPtr	targetWindow;
	short		partCode;
	long		menucode;
	Point		theMouse;
	
	// find out what part of the screen the mouse was clicked on
	
	partCode = FindWindow( theEvent->where, &targetWindow );
	
	switch (partCode)
	{	
		case inDesk:
			break;
		
		case inMenuBar:
			// the user clicked the menubar so
			// update the menus
			HandleMenuUpdate();

			// pull down the menu and track it
			menucode = MenuSelect( theEvent->where );
			
			// handle the user's choice	
			if ( HiWord( menucode ))
				HandleMenuSelection( HiWord(menucode), LoWord(menucode) );
			HiliteMenu(0);

			break;
		
		case inSysWindow:
			SystemClick( theEvent, targetWindow );	// system will handle it (DA window)
			break;
	
		case inContent:
			if ( targetWindow )						// the user clicked in a window
			{
				if ( targetWindow == (WindowPtr)FrontWindow() ) {
					SetPort( targetWindow );
					theMouse = theEvent->where;
					::GlobalToLocal( &theMouse );
					if ( theEvent->what == mouseDown )
    					Mouse->do_buttonpress( current_time, 1, theMouse.h, theMouse.v,
    							theEvent->modifiers & shiftKey, theEvent->modifiers & optionKey,
    							theEvent->modifiers & controlKey );
    				else
    					Mouse->do_buttonrelease( current_time, 1, theMouse.h, theMouse.v );
				} else {
					// If the window wasn't active, make it active.
					
					// (If your app supports modal dialogs, then you'll want to check
					//	to make sure the front window isn't one of these.)
					
					SelectWindow( targetWindow );
				}
			}
			break;

		case inDrag:
			// Drag the window.
			// Again, disallow this if the front window is modal.
			bool	dblBfrState;
			Rect r = (*GetGrayRgn())->rgnBBox;
			InsetRect( &r, 4, 4 );
			DragWindow( targetWindow, theEvent->where, &r );
			{
				if ( piG2D ) {
					piG2D->WindowChanged();
				}
			}
			break;
		
		case inGrow:
		{
			// the user wants to resize the window
			// first, get min and max bounds
			Rect r;	// minimum and maximum window size
			SetRect( &r, 100, 50, 640, 800 );	// arbitrary bounds for windows!
			long newSize = GrowWindow( targetWindow, theEvent->where, &r );
			if (newSize) {
				short newHeight = HiWord(newSize);
				short newWidth = LoWord(newSize);
				SizeWindow( targetWindow, newWidth, newHeight, true );
				Rect invalr = {0,0,newHeight,newWidth};
				InvalRect( &invalr );
			}
			break;
		}

		case inGoAway:
			if( TrackGoAway( targetWindow, theEvent->where ))
			{
				// if the user clicks in the go-away box, the application will be shutdown.
				
				Shutdown = true;
			}
			break;
		
		case inZoomIn:
		case inZoomOut:
			if( TrackBox( targetWindow, theEvent->where, partCode ))
			{
				// if the user clicked in the zoom box, the window will be zoomed				
			}
			break;
	}

}


void SysSystemDriver::HandleMenuUpdate( void )
{
}


void SysSystemDriver::DoAboutDialog( void )
{
	ParamText( mAppName, "\p", "\p", "\p" );
	::Alert( kAboutDialog, NULL );
}


void SysSystemDriver::HandleMenuSelection( const short menuNum, const short itemNum )
{
	// User has selected a menu command.
	// Here, we'll handle only a few default cases.
	if (menuNum == kAppleMenuID) {
		if (itemNum == 1) {
			// show the about box
			DoAboutDialog();
		} else {
			// launch Apple Menu item
			Str255 appleItemName;
			GetMenuItemText( GetMenuHandle(menuNum), itemNum, appleItemName );
			GrafPtr savePort;
			GetPort( &savePort );
			OpenDeskAcc( appleItemName );
			SetPort( savePort );
		}
		return;
	}
	
	if (menuNum == kFileMenuID) {
		// for the file menu, we only handle "quit" here.
		// We'll assume that's the last item in the File menu.
		if (itemNum == CountMenuItems( GetMenuHandle(menuNum) )) {
			// QUIT!
			ExitLoop = true;
		}
	}
}


#if 0
static int KeyToChar[128] = {
	'a',		/* 0 */
	's',
	'd',
	'f',
	'h',
	'g',
	'z',
	'x',
	'c',
	'v',
	'\0',		/* 10 */
	'b',
	'q',
	'w',
	'e',
	'r',
	'y',
	't',
	'1',
	'2',
	'3',		/* 20 */
	'4',
	'6',
	'5',
	'=',
	'9',
	'7',
	'-',
	'8',
	'0',
	']',		/* 30 */
	'o',
	'u',
	'[',
	'i',
	'p',
	CSKEY_ENTER,
	'l',
	'j',
	'\'',
	'k',		/* 40 */
	';',
	'\\',
	'.',
	'/',
	'n',
	'm',
	'.',
	CSKEY_TAB,
	' ',
	'`',		/* 50 */
	CSKEY_BACKSPACE,
	'\0',
	CSKEY_ESC,
	'\0',
	'\0',
	CSKEY_SHIFT,
	CSKEY_SHIFT,
	CSKEY_ALT,
	CSKEY_CTRL,
	'\0',		/* 60 */
	'\0',
	'\0',
	'\0',
	'\0',
	'.',
	'\0',
	'*',
	'\0',
	'+',
	'\0',		/* 70 */
	'\0',
	'\0',
	'\0',
	'\0',
	'/',
	CSKEY_ENTER,
	'\0',
	'-',
	'\0',
	'\0',		/* 80 */
	'=',
	'0',
	'1',
	'2',
	'3',
	'4',
	CSKEY_CENTER,
	'6',
	'7',
	'\0',		/* 90 */
	'8',
	'9',
	'\0',
	'\0',
	'\0',
	CSKEY_F5,
	CSKEY_F6,
	CSKEY_F7,
	CSKEY_F3,
	CSKEY_F8,	/* 100 */
	CSKEY_F9,
	'\0',
	CSKEY_F11,
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	CSKEY_F10,
	'\0',		/* 110 */
	CSKEY_F12,
	'\0',
	'\0',
	CSKEY_INS,
	CSKEY_HOME,
	CSKEY_PGUP,
	CSKEY_DEL,
	CSKEY_F4,
	CSKEY_END,
	CSKEY_F2,	/* 120 */
	CSKEY_PGDN,
	CSKEY_F1,
	CSKEY_LEFT,
	CSKEY_RIGHT,
	CSKEY_UP,
	CSKEY_DOWN,
	'\0' };
#else
static int KeyToChar[128] = {
	'=',		/* 0 */
	'9',
	'7',
	'-',
	'8',
	'0',
	']',
	'o',
	'y',
	't',
	'1',		/* 10 */
	'2',
	'3',
	'4',
	'6',
	'5',
	'c',
	'v',
	'\0',
	'b',
	'q',		/* 20 */
	'w',
	'e',
	'r',
	'a',
	's',
	'd',
	'f',
	'h',
	'g',
	'z',		/* 30 */
	'x',
	CSKEY_SHIFT,
	'\0',
	CSKEY_ALT,
	CSKEY_CTRL,
	'\0',
	'\0',
	'\0',
	'\0',
	CSKEY_TAB,	/* 40 */
	' ',
	CSKEY_DEL,
	'`',
	'\0',
	CSKEY_ESC,
	'\0',
	'\0',		/* Command Key (Not Filled in on purpose) */
	'k',
	';',
	'\\',		/* 50 */
	',',
	'/',
	'n',
	'm',
	'.',
	'u',
	'[',
	'i',
	'p',
	CSKEY_ENTER,/* 60 */
	'l',
	'j',
	'\'',
	CSKEY_RIGHT,
	CSKEY_HOME,
	'\0',
	CSKEY_UP,
	CSKEY_PGUP,
	'\0',
	'\0',		/* 70 */
	'\0',
	'\0',
	'=',
	CSKEY_INS,
	CSKEY_END,
	CSKEY_DOWN,
	CSKEY_PGDN,
	CSKEY_LEFT,
	CSKEY_CENTER,
	'\0',		/* 80 */
	'\0',
	'\0',
	'/',
	CSKEY_ENTER,
	'\0',
	'-',
	'\0',
	'\0',
	CSKEY_DEL,
	'\0',		/* 90 */
	'*',
	'\0',
	'+',
	'\0',
	'\0',
	CSKEY_F2,
	CSKEY_PGDN,
	CSKEY_F1,
	CSKEY_LEFT,
	CSKEY_RIGHT, /* 100 */
	CSKEY_DOWN,
	CSKEY_UP,
	'\0',
	'\0',
	'\0',
	CSKEY_INS,
	CSKEY_HOME,
	CSKEY_PGUP,
	CSKEY_DEL,
	CSKEY_F4,	/* 110 */
	CSKEY_END,
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	CSKEY_F10,
	'\0',
	CSKEY_F12,
	CSKEY_F5,		/* 120 */
	CSKEY_F6,
	CSKEY_F7,
	CSKEY_F3,
	CSKEY_F8,
	CSKEY_F9,
	'\0',
	CSKEY_F11 };
#endif

/*----------------------------------------------------------------
	Check to see if any keys have been pressed or released.
----------------------------------------------------------------*/

void SysSystemDriver::ScanKeyboard( time_t current_time )
{
	unsigned long	km[4];
	unsigned long	keys;
	unsigned long	oldkeys;
	unsigned int	i;
	unsigned int	j;
	int				theChar;

	::GetKeys( km );

	/* If the command key is down abort key processing */

	if ( km[1] & 0x00008000L )
		return;

	/* Check to see if a key has changed state */

	for ( i = 0; i < 4; ++i ) {
		if ( km[i] != mKeyboardState[i] ) {
			keys = (unsigned long)km[i];
			oldkeys = (unsigned long)mKeyboardState[i];
			for ( j = 0; j < 32; ++j, keys >>= 1, oldkeys >>= 1 ) {
				if (( oldkeys & 1 ) != ( keys & 1 )) {
					theChar = KeyToChar[ ( i * 32 ) + j ];
					if ( theChar != '\0' ) {
						if ( keys & 1 )
        					Keyboard->do_keypress( current_time, theChar );
        				else
        					Keyboard->do_keyrelease( current_time, theChar );
        			}
				}
			}
		}
	}

	for ( i = 0; i < 4; ++i )
		mKeyboardState[i] = km[i];
}

void SysSystemDriver::HandleKey( time_t current_time, const char key, const char /* keycode */, const short modifiers, bool /* down */ )
{
	// A key has been pressed -- handle typical cases.
	if (modifiers & cmdKey) {
		// command key was pressed -- check for menu shortcut.
		long menucode = MenuKey( key );		
		// handle the user's choice	
		if ( HiWord( menucode )) {
			HandleMenuSelection( HiWord(menucode), LoWord(menucode) );
			HiliteMenu(0);
			return;
		}
	}

#if ! SCAN_KEYBOARD
	Keyboard->Key.shift = ( modifiers & shiftKey );
	Keyboard->Key.ctrl = ( modifiers & controlKey );
	Keyboard->Key.alt = ( modifiers & optionKey );

	switch ( key ) {
		case kHomeCharCode:
        	Keyboard->do_keypress( current_time, CSKEY_HOME );
			break;

		case kEndCharCode:
        	Keyboard->do_keypress( current_time, CSKEY_END );
			break;

		case kEscapeCharCode:
			Keyboard->do_keypress( current_time, CSKEY_ESC );
			break;

		case kReturnCharCode: 
			Keyboard->do_keypress( current_time, CSKEY_ENTER );
			break;

		case kUpArrowCharCode:
			Keyboard->do_keypress( current_time, CSKEY_UP );
			break;

		case kDownArrowCharCode:
			Keyboard->do_keypress( current_time, CSKEY_DOWN );
			break;

		case kLeftArrowCharCode:
			Keyboard->do_keypress( current_time, CSKEY_LEFT );
			break;

		case kRightArrowCharCode:
			Keyboard->do_keypress( current_time, CSKEY_RIGHT );
			break;

		case kPageUpCharCode:
			Keyboard->do_keypress( current_time, CSKEY_PGUP );
			break;

		case kPageDownCharCode:
			Keyboard->do_keypress( current_time, CSKEY_PGDN );
			break;

		default:
			Keyboard->do_keypress( current_time, key );
			Keyboard->do_keyrelease( current_time, key );
			break;
	}
#endif
}


void SysSystemDriver::HandleOSEvent( time_t current_time, EventRecord *theEvent, iMacGraphics* piG2D )
{
	unsigned char	osEvtFlag;

	osEvtFlag = (unsigned char) (((unsigned long) theEvent->message) >> 24);
	if (osEvtFlag == mouseMovedMessage) {
		Point	theMouse;

		theMouse = theEvent->where;
		if (( piG2D ) && ( piG2D->PointInWindow( &theMouse ) )) {
			Mouse->do_mousemotion( current_time, theMouse.h, theMouse.v );
		}
	} else if (osEvtFlag == suspendResumeMessage) {
		if (theEvent->message & resumeFlag) {
		} else {
			::HiliteMenu(0);				// Unhighlight menu titles
		}
	}
}


void SysSystemDriver::HandleHLEvent( time_t current_time, EventRecord *theEvent )
{
#pragma unused( current_time )

	OSErr	err = ::AEProcessAppleEvent( theEvent );
}


/************************************************************************************************
 *	AppleEventHandler
 *
 *		Callback for handling Apple Events
 */
static OSErr AppleEventHandler( AppleEvent *event, AppleEvent *reply, long refCon )
{
#pragma unused( reply, refCon )

	return gSysSystemDriver->HandleAppleEvent( event );
}


static OSErr GetPath( FSSpec theFSSpec, char *theString )
{
	CInfoPBRec	thePB;
	Str255		dirName;
	char		thePath[256];
	OSErr		theError;

	strncpy( thePath, (char *)&theFSSpec.name[1], theFSSpec.name[0] );
	thePath[ thePath[0] + 1 ] = '\0';

	thePB.dirInfo.ioNamePtr = dirName;
	thePB.dirInfo.ioVRefNum = theFSSpec.vRefNum;
	thePB.dirInfo.ioDrParID = theFSSpec.parID;
	thePB.dirInfo.ioFDirIndex = -1;
	do {
		thePB.dirInfo.ioDrDirID = thePB.dirInfo.ioDrParID;
		theError = PBGetCatInfoSync( &thePB );
		if ( theError == noErr ) {
			dirName[ dirName[0] + 1 ] = '\0';
			strcat( (char *)&dirName[1], ":" );
			strcat( (char *)&dirName[1], thePath );
			strcpy( thePath, (char *)&dirName[1] );
		}
	} while ( thePB.dirInfo.ioDrDirID != fsRtDirID );

	strcpy( theString, thePath );

	return theError;
}


/************************************************************************************************
 *	HandleAppleEvent
 *
 */
OSErr SysSystemDriver::HandleAppleEvent( AppleEvent *theEvent )
	{
	DescType	eventClass;
	DescType	eventID;
	DescType	actualType;
	long		actualSize;
	AEDesc		fileListDesc;
	OSErr		err;
	long		numFiles;
	int			i;
	AEKeyword	actualKeyword;
	FSSpec		filespec;
	FInfo		theFileInfo;
	short		refNum;
	long		count;
	
	// Get the event class		                      
	err = AEGetAttributePtr( theEvent, keyEventClassAttr,
				typeType, &actualType, (Ptr)&eventClass, 
				sizeof(eventClass), &actualSize );
	if ( err != noErr )
		return err;

	// Get the event ID
	err = AEGetAttributePtr( theEvent, keyEventIDAttr,
				typeType, &actualType, (Ptr)&eventID, 
				sizeof(eventID), &actualSize );
	if ( err != noErr )
		return err;
		                      
	if ( eventClass == kCoreEventClass ) {	
		switch (eventID) {
			case kAEOpenApplication:
				break;

			case kAEOpenDocuments:
			case kAEPrintDocuments:
				err = AEGetKeyDesc( theEvent, keyDirectObject, typeAEList, &fileListDesc );
				if (err == noErr) {					
					err = AECountItems( &fileListDesc, &numFiles );
					if (err == noErr) {
						for (i = 1; i <= numFiles; i++) {
							err = AEGetNthPtr( &fileListDesc, i, typeFSS, &actualKeyword,
												&actualType, (Ptr)&filespec, sizeof(filespec), &actualSize );
							if ( err == noErr ) {
								FSpGetFInfo( &filespec, &theFileInfo );
								if ( theFileInfo.fdType == 'TEXT' ) {
									err = FSpOpenDF( &filespec, fsRdPerm, &refNum );
									if ( err == noErr ) {
										count = 255;
										err = FSRead( refNum, &count, CommandLine );
										if ( err == noErr )
											CommandLine[ count ] = '\0';
										FSClose( refNum );
									}
								} else {
									err = GetPath( filespec, CommandLine );
								}
							}
							AEDisposeDesc( &fileListDesc );
						}
					}
				}
				break;

			case kAEQuitApplication:
				ExitLoop = true;
				break;

			default:
				err = errAEEventNotHandled;		// We got an event we don't understand
				break;
		}
	} else {
		err = errAEEventNotHandled;		// We got an event we don't understand
	}
	
	return err;
}


/*
 *	Command line input.
 */

int SysSystemDriver::ParseCommandLine(char *s)
{
	int  n = 1, Quote = 0;
	char *p = s, *p1, c;
	
	p1 = (char *) argStr;
	while ((c = *p++) != 0) {
		if (c==' ') continue;
		argv[n++] = p1;
		if (n > MAX_ARGS)				/* mm 970404 */
			return (n-1);				/* mm 970404 */
		do {
			if (c=='\\' && *p++) 
				c = *p++;
			else
				if ((c=='"') || (c == '\'')) {
					if (!Quote) {
						Quote = c;
						continue;
					}
					if (c == Quote) {
						Quote = 0;
						continue;
					}
				}
			*p1++ = c;
		} while (*p && ((c = *p++) != ' ' || Quote));
		*p1++ = 0;
		
	}
	return n;
	
}


int SysSystemDriver::GetCommandLine(char ***arg)
{
	short		theItem;
	Str255		theString;
	DialogPtr	theDialog;
	short		itemType;
	Handle		itemHandle;
	Rect		itemRect;
	EventRecord	theEvent;
	enum { kOK = 1, kQUIT, kARGUMENTS };

	/*
	 *	Setup the first argument as the application name.
	 */
	argv[0] = (char *) &mAppName[1];

	/*
	 *	Allow an apple event to come in just in case it is an open file request.
	 */
	if ( ::WaitNextEvent( kHighLevelEvent, &theEvent, 1, NULL )) {
		HandleHLEvent( 0, &theEvent );
	}

	/*
	 *	If there is no command line yet, see if there is one in a string resource.
	 */
	if ( strlen( CommandLine ) == 0 ) {
		::GetIndString( theString, kCommandLineString, 1 );
		if ( theString[0] != '\0' ) {
			theString[ theString[0] + 1 ] = '\0';
			strcpy( CommandLine, (char *)&theString[1] );
		}
	}

	/*
	 *	If the user does not have the option key down then skip the command line dialog.
	 */
	if ( ! ( theEvent.modifiers & optionKey )) {
		*arg = argv;
		return ParseCommandLine( CommandLine );
	}

	/*
	 *	Put up our minimal command line dialog.
	 */
	theDialog = ::GetNewDialog( kCommandLineDialog, NULL, (WindowPtr) -1 );

	::SetDialogDefaultItem( theDialog, kOK );
	::SetDialogCancelItem( theDialog, kQUIT );
	::SetDialogTracksCursor( theDialog, true );

	::GetDialogItem( theDialog, kARGUMENTS, &itemType, &itemHandle, &itemRect );
	strcpy( (char *)&theString[1], CommandLine );
	theString[ 0 ] = strlen( CommandLine );
	::SetDialogItemText( itemHandle, theString );		// Show the current command line

	::SetWTitle( theDialog, mAppName );

	::SetCursor( &qd.arrow );
	::ShowWindow( theDialog );

	while (1) {
		::ModalDialog( NULL, &theItem );
		if ( kOK == theItem ) {
			::GetDialogItemText( itemHandle, theString );
			/*convert argument to C string*/
			theString[ theString[0] + 1 ] = '\0';
			strcpy( CommandLine, (char *)&theString[1] );
			::DisposeDialog( theDialog );
			break; 
		} else if ( kQUIT == theItem ) {
			::DisposeDialog( theDialog );
			fatal_exit( 0, false );
		}
	}

	*arg = argv;
	return ParseCommandLine( CommandLine );
}
