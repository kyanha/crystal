/*
    Copyright (C) 1998,1999 by Jorrit Tyberghein
    Written by Xavier Planet.
    Overhauled and re-engineered by Eric Sunshine <sunshine@sunshineco.com>
  
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "sysdef.h"
#include "isystem.h"
#include "cs2d/be/belibg2d.h"
#include "cs2d/be/CrystWindow.h"
#include "cssys/be/beitf.h"

CrystView::CrystView(BRect frame, IBeLibSystemDriver* isys) :
	BView(frame, "", B_FOLLOW_ALL, B_WILL_DRAW), be_system(isys)
{
	be_system->AddRef();
}

CrystView::~CrystView()
{
	be_system->Release();
}

void CrystView::ProcessUserEvent() const
{
	be_system->ProcessUserEvent(Looper()->CurrentMessage());
}

void CrystView::KeyDown(const char *bytes, int32 numBytes)
{
	ProcessUserEvent();
}

void CrystView::KeyUp(const char *bytes, int32 numBytes)
{
	ProcessUserEvent();
}

void CrystView::MouseMoved(BPoint point, uint32 transit, BMessage const* m)
{
	ProcessUserEvent();
}

void CrystView::MouseDown(BPoint point)
{
	ProcessUserEvent();
	if (!IsFocus())
		MakeFocus();
}

void CrystView::MouseUp(BPoint point)
{
	ProcessUserEvent();
}

CrystWindow::CrystWindow(BRect frame, char const* name, CrystView* v,
	csGraphics2DBeLib* piBeG2D, ISystem* isys, IBeLibSystemDriver* bsys) :
	BDirectWindow(frame,name, B_TITLED_WINDOW, B_NOT_RESIZABLE|B_NOT_ZOOMABLE),
	view(v), cs_system(isys), be_system(bsys)
{
	cs_system->AddRef();
	be_system->AddRef();
	
	// Initialise local flags
#if 0
	pi_BeG2D = piBeG2D;
	pi_BeG2D->fConnected = false;
	pi_BeG2D->fConnectionDisabled = false;
	pi_BeG2D->locker = new BLocker();
#endif
	
	// FIXME: Is this level of abstraction needed?  Why not just use the
	// incoming piBeG2D?
	HRESULT hRes = piBeG2D->QueryInterface ((REFIID)IID_IBeLibGraphicsInfo, (void**)&piG2D);
	if (SUCCEEDED(hRes)) {
//		printf("piBeG2D->QueryInterface succeeded \n");
	}

	view->SetViewColor(0, 0, 0);// remove for direct framebuffer access
//	view->SetViewColor(B_TRANSPARENT_32_BIT);// uncomment for direct framebuffer access
	AddChild(view);

	// Add a shortcut to switch in and out of fullscreen mode.
//	AddShortcut('f', B_COMMAND_KEY, new BMessage('full'));
	
	// As we said before, the window shouldn't get wider than 2048 in any
	// direction, so those limits will do.
	SetSizeLimits(40.0, 2000.0, 40.0, 2000.0); // This isn't needed in BDirectWindow
}

CrystWindow::~CrystWindow()
{
//	pi_BeG2D->fConnectionDisabled = true;
	Hide();
	Flush();
//	delete pi_BeG2D->locker;
//	pi_BeG2D->locker = 0;
	piG2D->Release();
	be_system->Release();
	cs_system->Release();
}

bool CrystWindow::QuitRequested()
{
	cs_system->Shutdown();
	// FIXME: Don't destroy window before "LoopThread" has finished.
	return true;
}

void CrystWindow::MessageReceived(BMessage* m)
{
	switch(m->what) {
#if 0
		case 'full' :
			SetFullScreen(!IsFullScreen());
			break;
#endif
		default :
			BWindow::MessageReceived(m);
			break;
	}
}

void CrystWindow::DirectConnected(direct_buffer_info *info)
{
//	printf("Entered CrystWindow::DirectConnected \n");
//	HRESULT hRes = piG2D->DirectConnect(info); //dh:uncomment this to get direct framebuffer access

//	This bit just keeps conventional window behaviour until DH has sorted out DirectConnected
	BDirectWindow::DirectConnected(info);
}
