/*
    Copyright (C) 1998 by Jorrit Tyberghein

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

#include <gl/gl.h>

#include <Windows.h>
#include <Dialogs.h>
#include <TextUtils.h>

#include "sysdef.h"
#include "csutil/scf.h"
#include "cs2d/openglmac/oglg2d.h"
#include "cssys/mac/MacRSRCS.h"
#include "isystem.h"

/////The 2D Graphics Driver//////////////

IMPLEMENT_FACTORY (csGraphics2DOpenGL)

EXPORT_CLASS_TABLE (Driver2DGL)
  EXPORT_CLASS (csGraphics2DOpenGL, OPENGL_2D_DRIVER,
    "Crystal Space OpenGL 2D driver for Macintosh")
EXPORT_CLASS_TABLE_END

IMPLEMENT_IBASE (csGraphics2DOpenGL)
  IMPLEMENTS_INTERFACE (iPlugIn)
  IMPLEMENTS_INTERFACE (iGraphics2D)
  IMPLEMENTS_INTERFACE (iMacGraphics)
IMPLEMENT_IBASE_END

csGraphics2DOpenGL::csGraphics2DOpenGL(iBase* iParent) : 
                   csGraphics2DGLCommon (iParent)
{
	CONSTRUCT_IBASE(iParent);

	mMainWindow = NULL;
	mColorTable = NULL;
	mGLContext = NULL;
	mMainPalette = NULL;
	mPaletteChanged = false;
	mDoubleBuffering = true;
	mOldDepth = 0;
	mSavedPort = NULL;
	mSavedGDHandle = NULL;
	mActivePage = 0;
}

csGraphics2DOpenGL::~csGraphics2DOpenGL(void)
{
	if ( mOldDepth ) {
		GDHandle	theMainDevice;
		theMainDevice = GetMainDevice();
		SetDepth( theMainDevice, mOldDepth, (**theMainDevice).gdFlags, 1 );
		mOldDepth = 0;
	}

	if ( mColorTable ) {
		::DisposeHandle( (Handle)mColorTable );
		mColorTable = NULL;
	}
}

bool csGraphics2DOpenGL::Initialize (iSystem *pSystem)
{
	long					pixel_format;
	OSErr					err;
	Boolean					showDialogFlag;

	if (!csGraphics2DGLCommon::Initialize (pSystem))
		return false;

	/*
	 *	Get the depth of the main gdevice.
	 *	FIXMEkrb: Should ask user which gdevice to use if more then one.
	 */
	mMainGDevice = GetMainDevice();
	pixel_format = GETPIXMAPPIXELFORMAT( *((**mMainGDevice).gdPMap) );

	/*
	 *	If the programs needs a certain pixel depth and the gdevice
	 *	is not at this depth, ask the user if we can set the gdevice
	 *	to the required depth.
	 */
	if (( Depth ) && ( Depth != pixel_format )) {
		/*
		 *	Check to see if the main screen can handle the requested depth.
		 */
		if ( ! HasDepth( mMainGDevice, Depth, (**mMainGDevice).gdFlags, 1 )) {
			DisplayErrorDialog( kBadDepthString );
		}
		/*
		 *	Ask the user if we can change the depth, if not then quit.
		 */
		if ( Depth == 32 )
			ParamText( "\pmillions of", "\p", "\p", "\p" );
		else if ( Depth == 16 )
			ParamText( "\pthousands of", "\p", "\p", "\p" );
		else
			ParamText( "\p256", "\p", "\p", "\p" );
		if ( StopAlert( kAskForDepthChangeDialog, NULL ) != 1 ) {
			::ExitToShell();
		}
	}

	/*
	 *	If the program does not need a certain pixel depth, check to make
	 *	sure the pixel depth is on of the ones we can deal with.  If not,
	 *	ask the user if we can set the gdevice to the required depth.
	 */
	if (( ! Depth ) && (( pixel_format != 8 ) && ( pixel_format != 16 ) && ( pixel_format != 32 ))) {
		/*
		 *	Check to see if the main screen can handle the default depth.
		 */
		if ( ! HasDepth( mMainGDevice, 8, (**mMainGDevice).gdFlags, 1 )) {
			/*
			 *	No, see if it can deal with the other depths.
			 */
			if ( ! HasDepth( mMainGDevice, 16, (**mMainGDevice).gdFlags, 1 )) {
				if ( ! HasDepth( mMainGDevice, 32, (**mMainGDevice).gdFlags, 1 )) {
					DisplayErrorDialog( kBadDepthString );
				} else {
					Depth = 32;
				}
			} else {
				Depth = 16;
			}
		} else {
			Depth = 8;
		}
		/*
		 *	Ask the user if we can change the depth, if not then quit.
		 */
		if ( Depth == 32 )
			ParamText( "\pmillions of", "\p", "\p", "\p" );
		else if ( Depth == 16 )
			ParamText( "\pthousands of", "\p", "\p", "\p" );
		else
			ParamText( "\p256", "\p", "\p", "\p" );
		if ( StopAlert( kAskForDepthChangeDialog, NULL ) != 1 ) {
			::ExitToShell();
		}
	}

	/*
	 *	If the programs needs a certain pixel depth and the main device
	 *	is not at this depth, change the main device to the required depth.
	 */
	if (( Depth ) && ( Depth != pixel_format )) {
		SetDepth( mMainGDevice, Depth, (**mMainGDevice).gdFlags, 1 );
		mOldDepth = pixel_format;
	} else {
		Depth = pixel_format;
	}

	/*
	 *	Setup the pixel format, color table and drawing
	 *	routines to the correct pixel depth.
	 */
	if ( Depth == 32 ) {
		mColorTable = NULL;			// No color table needed
		pfmt.PalEntries = 0;
		pfmt.PixelBytes = 4;
  		pfmt.RedMask = 0xFF << 16;
  		pfmt.GreenMask = 0xFF << 8;
  		pfmt.BlueMask = 0xFF;
  		complete_pixel_format();
	} else if ( Depth == 16 ) {
		mColorTable = NULL;			// No color table needed
		pfmt.PalEntries = 0;
		pfmt.PixelBytes = 2;
  		pfmt.RedMask = 0x1F << 10;
  		pfmt.GreenMask = 0x1F << 5;
  		pfmt.BlueMask = 0x1F;
  		complete_pixel_format();
	} else {
		/*
		 *	The 8 bit pixel data was filled in by csGraphics2D
		 *	so all we need to do is make an empty color table.
		 */
		mColorTable = (CTabHandle)::NewHandleClear( sizeof(ColorSpec) * 256 + 8 );
		(*mColorTable)->ctSeed = ::GetCTSeed();
		(*mColorTable)->ctFlags = 0;
		(*mColorTable)->ctSize = 255;

		pfmt.PalEntries = 256;
		pfmt.PixelBytes = 1;
		pfmt.RedMask = 0;
		pfmt.GreenMask = 0;
		pfmt.BlueMask = 0;

  		complete_pixel_format();
	}
  
	CsPrintf (MSG_INITIALIZATION, "Using %d bits per pixel (%d color mode).\n", Depth, 1 << Depth);

	return true;
}

bool csGraphics2DOpenGL::Open(const char *Title)
{
	Str255			theTitle;
	Rect			theBounds;
	Rect			displayRect;
	int				i;
	int				theOffset;
	OSErr			theError;
	int				displayWidth;
	int				displayHeight;
	GLint			attrib[5];
	AGLPixelFormat	fmt;

	/*
	 *	Create the main window with the given title
	 *	centered on the main gdevice.
	 */
	displayRect = (**mMainGDevice).gdRect;
	displayWidth = displayRect.right - displayRect.left;
	displayHeight = displayRect.bottom - displayRect.top;
	
	theBounds.left = displayRect.left + ((displayWidth - Width) / 2);
	theBounds.top = displayRect.top + ((displayHeight - Height) / 2);
	
	theBounds.right = theBounds.left + Width;
	theBounds.bottom = theBounds.top + Height;

	strcpy( (char *)&theTitle[1], Title );
	theTitle[0] = strlen( Title );
	mMainWindow = (CWindowPtr)::NewCWindow( nil, &theBounds, theTitle, true, noGrowDocProc, 
											(WindowPtr) -1, false, 0 );

	// set the color table into the video card
	::SetGWorld( (CGrafPtr)mMainWindow, NULL );
	if ( Depth == 8 ) {
		mMainPalette = ::NewPalette( 256, mColorTable, /* pmExplicit + */ pmTolerant, 0x2000 );
		::SetPalette( (WindowPtr)mMainWindow, mMainPalette, true );
		mPaletteChanged = true;
	} else {
		mPaletteChanged = false;
	}
	::ShowWindow( (WindowPtr)mMainWindow );
	::SelectWindow( (WindowPtr)mMainWindow );

	mDoubleBuffering = true;

	/*
	 *	Choose the pixel format.
	 */
	 i = 0;
	if ( Depth != 8 )
		attrib[i++] = AGL_RGBA;
	attrib[i++] = AGL_DOUBLEBUFFER;
	attrib[i++] = AGL_DEPTH_SIZE;
	attrib[i++] = Depth;
	attrib[i++] = AGL_NONE;

	fmt = aglChoosePixelFormat( NULL, 0, attrib );

	mGLContext = aglCreateContext( fmt, NULL );

	aglDestroyPixelFormat( fmt );

	aglSetDrawable( mGLContext, mMainWindow );
	aglSetCurrentContext( mGLContext );

	csGraphics2DGLCommon::Open( Title );

	aglSwapBuffers( mGLContext );
  
	return true;
}

void csGraphics2DOpenGL::Close(void)
{
	csGraphics2DGLCommon::Close();

	if ( mGLContext ) {
		aglSetCurrentContext(NULL);
		aglSetDrawable(mGLContext, NULL);
		aglDestroyContext( mGLContext );
		mGLContext = NULL;
	}

	if ( mMainWindow ) {
		::DisposeWindow( (WindowPtr)mMainWindow );
		mMainWindow = NULL;
	}

	if ( mMainPalette )
		::RestoreDeviceClut( NULL );
}

int csGraphics2DOpenGL::GetPage ()
{
	return mActivePage;
}

bool csGraphics2DOpenGL::DoubleBuffer( bool Enable )
{
	if ( Enable )
		mDoubleBuffering = false;
	else
		mDoubleBuffering = true;

	return true;
}

bool csGraphics2DOpenGL::DoubleBuffer()
{
	return mDoubleBuffering;
}

void csGraphics2DOpenGL::Print( csRect *area )
{
	aglSwapBuffers( mGLContext );
	glFlush ();
}

bool csGraphics2DOpenGL::BeginDraw()
{
  csGraphics2D::BeginDraw ();
  if (FrameBufferLocked != 1)
    return true;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.,(GLdouble)Width,0.,(GLdouble)Height,-1.0,100.0);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  return true;
}

void csGraphics2DOpenGL::FinishDraw()
{
  csGraphics2D::FinishDraw ();
  if (FrameBufferLocked)
    return;

  if (mActivePage == 0)
    mActivePage = 1;
  else
    mActivePage = 0;
}

void csGraphics2DOpenGL::SetColorPalette()
{  
	if (( Depth == 8 ) && ( mPaletteChanged )) {
		mPaletteChanged = false;

		aglUpdateContext( mGLContext );
	}
}

void csGraphics2DOpenGL::SetRGB(int i, int r, int g, int b)
{
	CTabHandle	theCTable;
	ColorSpec	theColor;

	if (( i < 0 ) || ( i >= pfmt.PalEntries ))
		return;

	theColor.rgb.red =  r | (r << 8);
	theColor.rgb.green = g | (g << 8);
	theColor.rgb.blue = b | (b << 8);

	theCTable = (**(mMainWindow->portPixMap)).pmTable;
	(*theCTable)->ctTable[i].value = i;
	(*theCTable)->ctTable[i].rgb = theColor.rgb;
	CTabChanged( theCTable );

	if ( mMainPalette ) {
		SetEntryColor( mMainPalette, i, &theColor.rgb );
	}

	mPaletteChanged = true;
}

bool csGraphics2DOpenGL::SetMouseCursor( csMouseCursorID iShape)
{
	bool		cursorSet = true;
	CursHandle	theCursor;

	if ( iShape == csmcNone )
		HideCursor();
	else if ( iShape == csmcArrow )
		InitCursor();
	else {
		if ( iShape == csmcWait )
			theCursor = GetCursor( watchCursor );
		else if ( iShape == csmcCross )
			theCursor = GetCursor( crossCursor );
		else
			theCursor = GetCursor( iShape + kArrowCursor );

		if ( theCursor )
			SetCursor( *theCursor );
		else {
			HideCursor();
			cursorSet = false;
		}
	}

  return cursorSet;
}


/*----------------------------------------------------------------
	Activate the window.
----------------------------------------------------------------*/
void csGraphics2DOpenGL::ActivateWindow( WindowPtr inWindow, bool active )
{
	CGrafPtr	thePort;
	GDHandle	theGDHandle;

	if ( inWindow != (WindowPtr)mMainWindow )
		return;

	if ( active ) {
		::SelectWindow( (WindowPtr)mMainWindow );
		::ActivatePalette( (WindowPtr)mMainWindow );
	}

	return;
}


/*----------------------------------------------------------------
	Update the window.
----------------------------------------------------------------*/
bool csGraphics2DOpenGL::UpdateWindow( WindowPtr inWindow )
{
	return false;
}

bool csGraphics2DOpenGL::PointInWindow( Point *thePoint )
{
	GrafPtr	thePort;
	bool	inWindow = false;

	if ( PtInRgn( *thePoint, ((WindowPeek)mMainWindow)->strucRgn )) {
		::GetPort( &thePort );
		::SetPort( (WindowPtr)mMainWindow );
		::GlobalToLocal( thePoint );
		::SetPort( thePort );

		inWindow = true;
	}

	return inWindow;
}


bool csGraphics2DOpenGL::DoesDriverNeedEvent( void )
{
	return false;
}


void csGraphics2DOpenGL::WindowChanged( void )
{
	aglUpdateContext( mGLContext );
	return;
}


bool csGraphics2DOpenGL::HandleEvent( EventRecord *inEvent )
{
	return false;
}


void csGraphics2DOpenGL::DisplayErrorDialog( short errorIndex )
{
	Str255	theString;
	Str255	theString2;

	GetIndString( theString, kErrorStrings, kFatalErrorInOpenGL2D );
	GetIndString( theString2, kErrorStrings, errorIndex );
	ParamText( theString, theString2,  "\p", "\p" );
	StopAlert( kGeneralErrorDialog, NULL );

	ExitToShell();
}
