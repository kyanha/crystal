/*
    Crystal Space Windowing System: radio button class
    Copyright (C) 1998,1999 by Andrew Zabolotny <bit@eltech.ru>

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
#include "sysdef.h"
#include "csws/csradbut.h"
#include "csws/csapp.h"
#include "csws/cswsutil.h"

#define RADIOBUT_TEXTURE_NAME	"csws::RadioButton"

csRadioButton::csRadioButton (csComponent *iParent, int iButtonID, int iButtonStyle)
  : csButton (iParent, cscmdNothing, iButtonStyle, csbfsNone)
{
  id = iButtonID;
  SetRadioButtonState (false);
  SetSuggestedSize (0, 0);
}

void csRadioButton::Press ()
{
  csButton::Press ();
  if (!RadioButtonState)
    SetRadioButtonState (true);
}

void csRadioButton::SetButtBitmap (char *id_n, char *id_p)
{
  int tx,ty,tw,th;
  FindCFGBitmap (app->System, *(app->dialogdefs), id_n, &tx, &ty, &tw, &th);
  CHK (csPixmap *bmpn = new csPixmap (app->GetTexture (
    RADIOBUT_TEXTURE_NAME), tx, ty, tw, th));
  FindCFGBitmap (app->System, *(app->dialogdefs), id_p, &tx, &ty, &tw, &th);
  CHK (csPixmap *bmpp = new csPixmap (app->GetTexture (
    RADIOBUT_TEXTURE_NAME), tx, ty, tw, th));
  SetBitmap (bmpn, bmpp);
}

void csRadioButton::SetRadioButtonState (bool iNewState)
{
  switch (iNewState)
  {
    case false:
      SetButtBitmap ("RADOFFN", "RADOFFP");
      RadioButtonState = iNewState;
      break;
    case true:
      SetButtBitmap ("RADONN", "RADONP");
      RadioButtonState = iNewState;
      DeselectNeighbours ();
      if (parent)
        parent->SendCommand (cscmdRadioButtonSelected, (void *)this);
      break;
  } /* endswitch */
}

bool csRadioButton::HandleEvent (csEvent &Event)
{
  switch (Event.Type)
  {
    case csevCommand:
      switch (Event.Command.Code)
      {
        case cscmdAreYouDefault:
          return true;
        case cscmdButtonDeselect:
          if (RadioButtonState)
            SetRadioButtonState (false);
          return true;
        case cscmdRadioButtonSet:
          SetRadioButtonState ((bool)Event.Command.Info);
          return true;
        case cscmdRadioButtonQuery:
          Event.Command.Info = (void *)RadioButtonState;
          return true;
      } /* endswitch */
      break;
  } /* endswitch */
  return csButton::HandleEvent (Event);
}
