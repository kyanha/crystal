/*
    Copyright (C) 2003 by Odes B. Boatwright.

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


#include <cssysdef.h>
#include <iutil/event.h>
#include <iutil/eventq.h>
#include <csappframe/csapplicationframework.h>

// Static 
iObjectRegistry*  csApplicationFramework::mp_object_reg            = 0;
csApplicationFramework* csApplicationFramework::m_Ptr              = 0;
char*             csApplicationFramework::m_ApplicationStringName  = "user app";
const char*       csApplicationFramework::m_FoundationStringName   = "crystalspace.libcsappframe";



csApplicationFramework::csApplicationFramework ()
{
  // It is a fatal error to have more than one csApplicationFramework
  // derived class in an application.
  CS_ASSERT ( 0 == m_Ptr );
  m_Ptr = this;
}


csApplicationFramework::~csApplicationFramework ()
{
  m_Ptr = 0;
}


bool csApplicationFramework::Start ()
{
  CS_ASSERT (0 != m_Ptr);
  return m_Ptr->Application ();
}

void csApplicationFramework::End ()
{
  CS_ASSERT (0 != m_Ptr);
  DestroyApplication (mp_object_reg);
  mp_object_reg = 0;
	m_Ptr->OnExit ();
}

void csApplicationFramework::OnExit ()
{
}

bool csApplicationFramework::Initialize (int argc, char *argv[])
{
  mp_object_reg = CreateEnvironment (argc, argv);

  if ( ! mp_object_reg )
  {
    return false;
  }

  CS_ASSERT (0 != m_Ptr);
  return m_Ptr->OnInitialize (argc, argv);
}

void csApplicationFramework::Quit (void)
{
  csRef<iEventQueue> q (CS_QUERY_REGISTRY (GetObjectRegistry(), iEventQueue));
  if (q)
  {
    q->GetEventOutlet()->Broadcast (cscmdQuit);
  }
  else
  {
    exit (2);
  }

}
