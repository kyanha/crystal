/*
    Crystal Space Windowing System: Base skin support
    Copyright (C) 2000 by Andrew Zabolotny <bit@eltech.ru>

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

#include <string.h>

#include "cssysdef.h"
#include "csws/cscomp.h"
#include "csws/csapp.h"
#include "csws/csskin.h"
#include "csws/csbackgr.h"
#include "csutil/scanstr.h"
#include "icfgfile.h"

//--//--//--//--//--//--//--//--//--//--//--// The skin repository class -//--//

csSkin::~csSkin ()
{
  DeleteAll ();
}

bool csSkin::FreeItem (csSome Item)
{
  delete (csSkinSlice *)Item;
  return true;
}

int csSkin::CompareKey (csSome Item, csConstSome Key, int) const
{
  return strcmp (((csSkinSlice *)Item)->GetName (), (char *)Key);
}

int csSkin::Compare (csSome Item1, csSome Item2, int) const
{
  return strcmp (((csSkinSlice *)Item1)->GetName (), ((csSkinSlice *)Item2)->GetName ());
}

void csSkin::Apply (csComponent *iComp)
{
  iComp->SendBroadcast (cscmdSkinChanged, this);
}

void csSkin::Initialize (csApp *iApp)
{
  app = iApp;
  for (int i = 0; i < count; i++)
    Get (i)->Initialize (iApp, this);
}

void csSkin::Deinitialize ()
{
  for (int i = 0; i < count; i++)
    Get (i)->Deinitialize ();
  app = NULL;
}

const char *csSkin::GetConfigStr (const char *iSection, const char *iKey,
  const char *iDefault)
{
  if (Prefix)
  {
    char temp [100];
    strcat (strcat (strcpy (temp, Prefix), "::"), iSection);
    if (app->Config->KeyExists (temp, iKey))
      return app->Config->GetStr (temp, iKey, iDefault);
  }
  return app->Config->GetStr (iSection, iKey, iDefault);
}

bool csSkin::GetConfigYesNo (const char *iSection, const char *iKey,
  bool iDefault)
{
  if (Prefix)
  {
    char temp [100];
    strcat (strcat (strcpy (temp, Prefix), "::"), iSection);
    if (app->Config->KeyExists (temp, iKey))
      return app->Config->GetYesNo (temp, iKey, iDefault);
  }
  return app->Config->GetYesNo (iSection, iKey, iDefault);
}

bool csSkin::ReadGradient (const char *iText, csRGBcolor *color, int iNum)
{
  if (!iText)
    return false;

  int idx = 0;
  while (idx < iNum)
  {
    char temp [100];
    const char *end = strchr (iText, ':');
    if (!end)
      end = strchr (iText, 0);
    memcpy (temp, iText, end - iText);
    temp [end - iText] = 0;
    iText = end + 1;
    int r, g, b;
    if (ScanStr (temp, "%d,%d,%d", &r, &g, &b) != 3)
      return false;
    color [idx++].Set (r, g, b);
  }
  return true;
}

void csSkin::Load (csBackground &oBack, const char *iSection, const char *iPrefix)
{
  oBack.Free ();

  csRGBcolor colors [4];
  char temp [100];
  size_t sl = strlen (iPrefix);
  memcpy (temp, iPrefix, sl);
  strcpy (temp + sl, ".Texture");
  const char *name = GetConfigStr (iSection, temp, NULL);
  if (name)
    oBack.SetTexture (app->GetTexture (name));

  strcpy (temp + sl, ".Color");
  if (ReadGradient (GetConfigStr (iSection, temp, NULL), colors, 1))
  {
    oBack.SetColor (0, colors [0]);
    oBack.SetColor (1, colors [0]);
    oBack.SetColor (2, colors [0]);
    oBack.SetColor (3, colors [0]);
    oBack.SetColor (app->FindColor (colors [0].red, colors [0].green, colors [0].blue));
  }
  strcpy (temp + sl, ".HGradient");
  if (ReadGradient (GetConfigStr (iSection, temp, NULL), colors, 2))
  {
    oBack.SetColor (0, colors [0]);
    oBack.SetColor (1, colors [1]);
    oBack.SetColor (2, colors [1]);
    oBack.SetColor (3, colors [0]);
  }
  strcpy (temp + sl, ".VGradient");
  if (ReadGradient (GetConfigStr (iSection, temp, NULL), colors, 2))
  {
    oBack.SetColor (0, colors [0]);
    oBack.SetColor (1, colors [0]);
    oBack.SetColor (2, colors [1]);
    oBack.SetColor (3, colors [1]);
  }
  strcpy (temp + sl, ".Gradient");
  if (ReadGradient (GetConfigStr (iSection, temp, NULL), colors, 4))
  {
    oBack.SetColor (0, colors [0]);
    oBack.SetColor (1, colors [1]);
    oBack.SetColor (2, colors [2]);
    oBack.SetColor (3, colors [3]);
  }
}

//--//--//--//--//--//--//--//--//- Basic functionality for skin slices --//--//

void csSkinSlice::Apply (csComponent &This)
{
  if (This.skinslice)
    This.skinslice->Reset (This);
  This.skinslice = this;
  if (This.GetState (CSS_VISIBLE))
    This.SetRect (This.bound);
}

void csSkinSlice::Reset (csComponent &This)
{
  This.ResetPalette ();
  if (This.GetState (CSS_VISIBLE))
    This.SetRect (This.bound);
}
