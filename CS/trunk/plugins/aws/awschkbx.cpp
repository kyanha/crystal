#include "cssysdef.h"
#include "awschkbx.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/fontserv.h"
#include "csutil/scfstr.h"
#include "csutil/csevent.h"
#include "iutil/evdefs.h"

#include <stdio.h>

const int awsCheckBox:: alignLeft = 0x0;
const int awsCheckBox:: alignRight = 0x1;
const int awsCheckBox:: alignCenter = 0x2;

const int awsCheckBox:: signalClicked = 0x1;

awsCheckBox::awsCheckBox () :
  is_down(false),
  mouse_is_over(false),
  is_on(false),
  frame_style(0),
  alpha_level(96),
  alignment(0),
  caption(NULL)
{
  tex[0] = tex[1] = tex[2] = tex[3] = NULL;
  SetFlag (AWSF_CMP_ALWAYSERASE);
}

awsCheckBox::~awsCheckBox ()
{
}

char *awsCheckBox::Type ()
{
  return "Check Box";
}

bool awsCheckBox::Setup (iAws *_wmgr, awsComponentNode *settings)
{
  if (!awsComponent::Setup (_wmgr, settings)) return false;

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  pm->GetInt (settings, "Alpha", alpha_level);
  pm->GetInt (settings, "Align", alignment);
  pm->GetString (settings, "Caption", caption);

  tex[0] = pm->GetTexture ("CheckBoxUp");
  tex[1] = pm->GetTexture ("CheckBoxDn");
  tex[2] = pm->GetTexture ("CheckBoxOn");
  tex[3] = pm->GetTexture ("CheckBoxOff");

  return true;
}

bool awsCheckBox::GetProperty (char *name, void **parm)
{
  if (awsComponent::GetProperty (name, parm)) return true;

  if (strcmp ("Caption", name) == 0)
  {
    char *st = NULL;

    if (caption) st = caption->GetData ();

    iString *s = new scfString (st);
    *parm = (void *)s;
    return true;
  }
  else if (strcmp ("State", name) == 0)
  {
    // in this case, the parm should point to a bool.
    bool **pb = (bool **)parm;

    *pb = &is_on;
    return true;
  }

  return false;
}

bool awsCheckBox::SetProperty (char *name, void *parm)
{
  if (awsComponent::SetProperty (name, parm)) return true;

  if (strcmp ("Caption", name) == 0)
  {
    iString *s = (iString *) (parm);

    if (s && s->Length ())
    {
      if (caption) caption->DecRef ();
      caption = s;
      caption->IncRef ();
      Invalidate ();
    }
    else
    {
      if (caption) caption->DecRef ();
      caption = NULL;
    }

    return true;
  }
  else if (strcmp ("State", name) == 0)
  {
   
    is_on = *(bool *)parm;
    return true;
  }

  return false;
}

void awsCheckBox::OnDraw (csRect /*clip*/)
{
  iGraphics2D *g2d = WindowManager ()->G2D ();
  iGraphics3D *g3d = WindowManager ()->G3D ();

  int txw = 0, txh = 0;
  int txy = 0, txx = 0;

  /// Get the size of our textures
  if (tex[0]) tex[0]->GetOriginalDimensions (txw, txh);

  txy = (Frame ().Height () >> 1) - (txh >> 1);

  switch (alignment)
  {
    case alignLeft:   txx = 0; break;
    case alignRight:  txx = Frame ().Width () - txw; break;
  }

  if (!is_down && tex[0])
  {
    g3d->DrawPixmap (
        tex[0],
        Frame ().xmin + txx + is_down,
        Frame ().ymin + txy + is_down,
        txw,
        txh,
        0,
        0,
        txw,
        txh,
        alpha_level);
  }
  else if (is_down && tex[1])
  {
    g3d->DrawPixmap (
        tex[1],
        Frame ().xmin + txx + is_down,
        Frame ().ymin + txy + is_down,
        txw,
        txh,
        0,
        0,
        txw,
        txh,
        alpha_level);
  }

  if (is_on && tex[2])
    g3d->DrawPixmap (
        tex[2],
        Frame ().xmin + txx + is_down,
        Frame ().ymin + txy + is_down,
        txw,
        txh,
        0,
        0,
        txw,
        txh,
        0);
  else if (!is_on && tex[3])
    g3d->DrawPixmap (
        tex[3],
        Frame ().xmin + txx + is_down,
        Frame ().ymin + txy + is_down,
        txw,
        txh,
        0,
        0,
        txw,
        txh,
        0);

  // Draw the caption, if there is one and the style permits it.
  if (caption)
  {
    int tw, th, tx, ty, mcc;

    mcc = WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetLength (
        caption->GetData (),
        Frame ().Width () - txw - 2);

    scfString tmp (caption->GetData ());
    tmp.Truncate (mcc);

    // Get the size of the text
    WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetDimensions (
        tmp.GetData (),
        tw,
        th);

    // Calculate the center
    ty = (Frame ().Height () >> 1) - (th >> 1);

    switch (alignment)
    {
      case alignRight:
        tx = Frame ().Width () - txw - tw - 2;
        break;
      case alignLeft:
      default:
        tx = txw + 2;
        break;
    }

    // Draw the text
    g2d->Write (
        WindowManager ()->GetPrefMgr ()->GetDefaultFont (),
        Frame ().xmin + tx + is_down,
        Frame ().ymin + ty + is_down,
        WindowManager ()->GetPrefMgr ()->GetColor (AC_TEXTFORE),
        -1,
        tmp.GetData ());
  }
}

bool awsCheckBox::OnMouseDown (int, int, int)
{
  is_down = true;
  Invalidate ();
  return true;
}

bool awsCheckBox::OnMouseUp (int, int, int)
{
  if (is_down) Broadcast (signalClicked);

  if (!is_on)
    is_on = true;
  else
    is_on = false;

  is_down = false;

  Invalidate ();
  return true;
}

bool awsCheckBox::OnMouseMove (int, int, int)
{
  return false;
}

bool awsCheckBox::OnMouseClick (int, int, int)
{
  return false;
}

bool awsCheckBox::OnMouseDoubleClick (int, int, int)
{
  return false;
}

bool awsCheckBox::OnMouseExit ()
{
  mouse_is_over = false;
  Invalidate ();

  if (is_down) is_down = false;

  return true;
}

bool awsCheckBox::OnMouseEnter ()
{
  mouse_is_over = true;
  Invalidate ();
  return true;
}

bool awsCheckBox::OnKeypress (int, int)
{
  return false;
}

bool awsCheckBox::OnLostFocus ()
{
  return false;
}

bool awsCheckBox::OnGainFocus ()
{
  return false;
}

/************************************* Command Button Factory ****************/
awsCheckBoxFactory::awsCheckBoxFactory (iAws *wmgr) :
  awsComponentFactory(wmgr)
{
  Register ("Check Box");

  RegisterConstant ("cbAlignLeft", awsCheckBox::alignLeft);
  RegisterConstant ("cbAlignRight", awsCheckBox::alignRight);
  RegisterConstant ("signalCheckBoxClicked", awsCheckBox::signalClicked);
}

awsCheckBoxFactory::~awsCheckBoxFactory ()
{
  // empty
}

iAwsComponent *awsCheckBoxFactory::Create ()
{
  return new awsCheckBox;
}
