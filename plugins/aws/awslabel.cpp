#include "cssysdef.h"
#include "awslabel.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/fontserv.h"
#include "iutil/evdefs.h"
#include "csutil/scfstr.h"

#include <stdio.h>

const int awsLabel:: signalClicked = 0x1;
const int awsLabel:: signalFocused = 0x2;
const int awsLabel:: alignLeft = 0x0;
const int awsLabel:: alignRight = 0x1;
const int awsLabel:: alignCenter = 0x2;

awsLabel::awsLabel () :
  is_down(false),
  mouse_is_over(false),
  alignment(0),
  caption(0)
{
  SetFlag (AWSF_CMP_ALWAYSERASE);
}

awsLabel::~awsLabel ()
{
}

const char *awsLabel::Type ()
{
  return "Label";
}

bool awsLabel::Setup (iAws *_wmgr, iAwsComponentNode *settings)
{
  if (!awsComponent::Setup (_wmgr, settings)) return false;

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  pm->GetString (settings, "Caption", caption);
  pm->GetInt (settings, "Align", alignment);

  int _focusable = 0;
  pm->GetInt (settings, "Focusable", _focusable);
	focusable = _focusable;

  return true;
}

bool awsLabel::GetProperty (const char *name, void **parm)
{
  if (awsComponent::GetProperty (name, parm)) return true;

  if (strcmp ("Caption", name) == 0)
  {
    char *st = 0;

    if (caption) st = caption->GetData ();

    iString *s = new scfString (st);
    *parm = (void *)s;
    return true;
  }

  return false;
}

bool awsLabel::SetProperty (const char *name, void *parm)
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
      caption = 0;
    }

    return true;
  }

  return false;
}

csRect awsLabel::getMinimumSize ()
{
  int tw = 0, th = 0;

  if (caption)
  {
    // Get the size of the text
    WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetDimensions (
        caption->GetData (),
        tw,
        th);
  }

  return csRect (0, 0, tw + 6 + (tw >> 2), th + 6 + (th >> 1));
}

void awsLabel::OnDraw (csRect /*clip*/)
{
  iGraphics2D *g2d = WindowManager ()->G2D ();

  // Draw the caption, if there is one
  if (caption)
  {
    int tw, th, tx, ty, mcc;

    mcc = WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetLength (
        caption->GetData (),
        Frame ().Width ());

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
      case alignRight:  tx = Frame ().Width () - tw; break;

      case alignCenter: tx = (Frame ().Width () >> 1) - (tw >> 1); break;

      default:          tx = 0; break;
    }

    // Draw the text
    g2d->Write (
        WindowManager ()->GetPrefMgr ()->GetDefaultFont (),
        //Frame ().xmin + tx + is_down,
        //Frame ().ymin + ty + is_down,
        Frame ().xmin + tx,
        Frame ().ymin + ty,
        WindowManager ()->GetPrefMgr ()->GetColor (AC_TEXTFORE),
        -1,
        tmp.GetData ());
  }
}

bool awsLabel::OnMouseDown (int, int, int)
{
  is_down = true;

  //Invalidate();
  return false;
}

bool awsLabel::OnMouseUp (int, int, int)
{
  if (is_down) Broadcast (signalClicked);

  is_down = false;

  //Invalidate();
  return false;
}

bool awsLabel::OnMouseExit ()
{
  mouse_is_over = false;

  //Invalidate();
  if (is_down) is_down = false;

  return true;
}

bool awsLabel::OnMouseEnter ()
{
  mouse_is_over = true;

  //Invalidate();
  return true;
}

bool awsLabel::OnKeyboard (const csKeyEventData& eventData)
{
  switch(eventData.codeCooked)
  {
    case CSKEY_ENTER:
		  Broadcast (signalClicked);
    return true;//'true' means event was eaten.
  }

	Invalidate ();

  //Only the CSKEY_ENTER is eaten here, so for any other keys pressed
  //we must return false. Luca (groton@gmx.net)
  return false;
}

void awsLabel::OnSetFocus()
{
	Broadcast (signalFocused);
}

/************************************* Command Button Factory ****************/

awsLabelFactory::awsLabelFactory (iAws *wmgr) :
  awsComponentFactory(wmgr)
{
  Register ("Label");
  RegisterConstant ("signalLabelClicked", awsLabel::signalClicked);
  RegisterConstant ("signalLabelFocused", awsLabel::signalFocused);

  RegisterConstant ("lblAlignLeft", awsLabel::alignLeft);
  RegisterConstant ("lblAlignRight", awsLabel::alignRight);
  RegisterConstant ("lblAlignCenter", awsLabel::alignCenter);
}

awsLabelFactory::~awsLabelFactory ()
{
  // empty
}

iAwsComponent *awsLabelFactory::Create ()
{
  return new awsLabel;
}
