#include "cssysdef.h"

#include <ctype.h>

#include "awstimer.h"
#include "awstxtbx.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/fontserv.h"
#include "csutil/scfstr.h"
#include "iutil/evdefs.h"

#include <stdio.h>

const int awsTextBox::fsNormal = 0x0;
const int awsTextBox::fsBitmap = 0x1;

const int awsTextBox::signalChanged = 0x1;
const int awsTextBox::signalLostFocus = 0x2;
const int awsTextBox::signalEnterPressed = 0x3;
const int awsTextBox::signalTabPressed = 0x4;
const int awsTextBox::signalFocused = 0x5;

static iAwsSink *textbox_sink = 0;

CS_IMPLEMENT_STATIC_VAR (GetTextBoxBlinkingCursorSlot, awsSlot,())

static awsSlot *textbox_slot = NULL;

static void BlinkCursor (void *, iAwsSource *source)
{
  iAwsComponent *comp = source->GetComponent ();

  //  Setting blink actually forces an inversion of blink's property, and if the textbox is the

  // focus then it will be Invalidated as well.
  comp->SetProperty ("Blink", 0);
}

awsTextBox::awsTextBox () :
  mouse_is_over(false),
  has_focus(false),
  should_mask(0),
  bkg(NULL),
  frame_style(0),
  alpha_level(92),
  text(NULL),
  disallow(NULL),
  maskchar(NULL),
  start(0),
  cursor(0),
  blink_timer(NULL),
  blink(true)
{
  textbox_slot = GetTextBoxBlinkingCursorSlot ();
}

awsTextBox::~awsTextBox ()
{
  if (blink_timer != 0)
  {
    textbox_slot->Disconnect (
        blink_timer,
        awsTimer::signalTick,
        textbox_sink,
        textbox_sink->GetTriggerID ("Blink"));
    delete blink_timer;
  }
}

const char *awsTextBox::Type ()
{
  return "Text Box";
}

bool awsTextBox::Setup (iAws *_wmgr, iAwsComponentNode *settings)
{
  if (!awsComponent::Setup (_wmgr, settings)) return false;

  // Setup blink event handling
  if (textbox_sink == 0)
  {
    textbox_sink = WindowManager ()->GetSinkMgr ()->CreateSink (NULL);
    textbox_sink->RegisterTrigger ("Blink", &BlinkCursor);
  }

  blink_timer = new awsTimer (WindowManager ()->GetObjectRegistry (), this);
  blink_timer->SetTimer (350);
  blink_timer->Start ();

  textbox_slot->Connect (
      blink_timer,
      awsTimer::signalTick,
      textbox_sink,
      textbox_sink->GetTriggerID ("Blink"));

  ////////////////
  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  pm->LookupIntKey ("ButtonTextureAlpha", alpha_level); // global get
  pm->GetInt (settings, "Style", frame_style);
  pm->GetInt (settings, "Alpha", alpha_level);          // local overrides, if present.
  pm->GetInt (settings, "Masked", should_mask);
  pm->GetString (settings, "Text", text);
  pm->GetString (settings, "Disallow", disallow);
  pm->GetString (settings, "MaskChar", maskchar);

  if (text) {
      cursor = text->Length ();
      text = new scfString(*text);
  } else
    text = new scfString ();

  switch (frame_style)
  {
    case fsNormal:
      bkg = pm->GetTexture ("Texture");
      break;

    case fsBitmap:
      {
        iString *tn1 = NULL;

        pm->GetString (settings, "Bitmap", tn1);

        if (tn1) bkg = pm->GetTexture (tn1->GetData (), tn1->GetData ());
      }
      break;
  }

  return true;
}

bool awsTextBox::GetProperty (const char *name, void **parm)
{
  if (awsComponent::GetProperty (name, parm)) return true;

  if (strcmp ("Text", name) == 0)
  {
    char *st = NULL;

    if (text) st = text->GetData ();

    iString *s = new scfString (st);
    *parm = (void *)s;
    return true;
  }
  else if (strcmp ("Disallow", name) == 0)
  {
    char *st = NULL;

    if (disallow) st = disallow->GetData ();

    iString *s = new scfString (st);
    *parm = (void *)s;
    return true;
  }

  return false;
}

bool awsTextBox::SetProperty (const char *name, void *parm)
{
  if (awsComponent::SetProperty (name, parm)) return true;

  if (strcmp ("Blink", name) == 0)
  {
    blink = !blink;
    if (has_focus) Invalidate ();
    return true;
  }
  else if (strcmp ("Text", name) == 0)
  {
    iString *s = (iString *) (parm);

    if (s)
    {
      if (text) text->DecRef ();
      text = s;
      text->IncRef ();
      Invalidate ();
      start = 0;
      cursor = 0;
    }

    return true;
  }
  else if (strcmp ("Disallow", name) == 0)
  {
    iString *s = (iString *) (parm);

    if (s)
    {
      if (disallow) disallow->DecRef ();
      disallow = s;
      disallow->IncRef ();
    }

    return true;
  }

  return false;
}

void awsTextBox::OnDraw (csRect /*clip*/)
{
  iGraphics2D *g2d = WindowManager ()->G2D ();
  iGraphics3D *g3d = WindowManager ()->G3D ();

  int hi = WindowManager ()->GetPrefMgr ()->GetColor (AC_HIGHLIGHT);
  int hi2 = WindowManager ()->GetPrefMgr ()->GetColor (AC_HIGHLIGHT2);
  int lo = WindowManager ()->GetPrefMgr ()->GetColor (AC_SHADOW);
  int lo2 = WindowManager ()->GetPrefMgr ()->GetColor (AC_SHADOW2);
  int dfill = WindowManager ()->GetPrefMgr ()->GetColor (AC_DARKFILL);
  int black = WindowManager ()->GetPrefMgr ()->GetColor (AC_BLACK);

  switch (frame_style)
  {
    case fsNormal:
      g2d->DrawLine (
          Frame ().xmin + 0,
          Frame ().ymin + 0,
          Frame ().xmax - 1,
          Frame ().ymin + 0,
          lo2);
      g2d->DrawLine (
          Frame ().xmin + 0,
          Frame ().ymin + 0,
          Frame ().xmin + 0,
          Frame ().ymax - 1,
          lo2);
      g2d->DrawLine (
          Frame ().xmin + 1,
          Frame ().ymin + 1,
          Frame ().xmax - 0,
          Frame ().ymin + 1,
          lo);
      g2d->DrawLine (
          Frame ().xmin + 1,
          Frame ().ymin + 1,
          Frame ().xmin + 1,
          Frame ().ymax - 0,
          lo);
      g2d->DrawLine (
          Frame ().xmin + 1,
          Frame ().ymax - 0,
          Frame ().xmax - 0,
          Frame ().ymax - 0,
          hi);
      g2d->DrawLine (
          Frame ().xmax - 0,
          Frame ().ymin + 1,
          Frame ().xmax - 0,
          Frame ().ymax - 0,
          hi);

      g2d->DrawLine (
          Frame ().xmin + 2,
          Frame ().ymin + 2,
          Frame ().xmax - 1,
          Frame ().ymin + 2,
          black);
      g2d->DrawLine (
          Frame ().xmin + 2,
          Frame ().ymin + 2,
          Frame ().xmin + 2,
          Frame ().ymax - 1,
          black);
      g2d->DrawLine (
          Frame ().xmin + 2,
          Frame ().ymax - 1,
          Frame ().xmax - 1,
          Frame ().ymax - 1,
          hi2);
      g2d->DrawLine (
          Frame ().xmax - 1,
          Frame ().ymin + 2,
          Frame ().xmax - 1,
          Frame ().ymax - 1,
          hi2);

      g2d->DrawBox (
          Frame ().xmin + 3,
          Frame ().ymin + 3,
          Frame ().Width () - 3,
          Frame ().Height () - 3,
          dfill);

      if (bkg)
      {
        g3d->DrawPixmap (
            bkg,
            Frame ().xmin,
            Frame ().ymin,
            Frame ().Width () + 1,
            Frame ().Height () + 1,
            0,
            0,
            Frame ().Width () + 1,
            Frame ().Height () + 1,
            alpha_level);
      }
      break;

    case fsBitmap:
      if (bkg)
      {
        g3d->DrawPixmap (
            bkg,
            Frame ().xmin,
            Frame ().ymin,
            Frame ().Width (),
            Frame ().Height (),
            0,
            0,
            Frame ().Width (),
            Frame ().Height (),
            alpha_level);
      }
      break;
  }

  // Draw the caption, if there is one and the style permits it.
  if (text && text->Length ())
  {
    int tw, th, tx, ty, mcc;
    iString *saved = NULL;

    if (should_mask && maskchar)
    {
      saved = text->Clone ();

      unsigned int i;
      for (i = 0; i < text->Length (); ++i)
        text->SetAt (i, maskchar->GetAt (0));
    }

    // Get the maximum number of characters we can use
    mcc = WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetLength (
        text->GetData () + start,
        Frame ().Width () - 10);

    if (mcc)
    {

      // Check to see if we're getting weird.
      // this was changed to avoid 
      // jittering in the start value
      if ((int)cursor > start + mcc) start = cursor - mcc;
      if (start < 0) start = 0;

      // Make the text the right length
      scfString tmp (text->GetData () + start);
      tmp.Truncate (mcc);

      // Get the size of the text
      WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->
          GetDimensions (tmp.GetData (), tw, th);

      // Calculate the center
      tx = 4;
      ty = (Frame ().Height () >> 1) - (th >> 1);

      // Draw the text
      g2d->Write (
      WindowManager ()->GetPrefMgr ()->GetDefaultFont (),
      Frame ().xmin + tx,
      Frame ().ymin + ty,
      WindowManager ()->GetPrefMgr ()->GetColor (AC_TEXTFORE),
      -1,
      tmp.GetData ());

      if (should_mask && maskchar && saved)
      {
    text->Clear ();
    text->Append (saved);
    saved->Clear ();
    saved->DecRef ();
      }

    if (has_focus && blink)
    {
      int co, cx, tty;
      co = cursor - start;
      tmp.Truncate (co);
      // figure out where to put the cursor
      WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->
         GetDimensions (tmp, cx, tty);
      g2d->DrawLine (
          Frame ().xmin + tx + cx + 1,
          Frame ().ymin + ty,
          Frame ().xmin + tx + cx + 1,
          Frame ().ymin + ty + th,
          WindowManager ()->GetPrefMgr ()->GetColor (AC_TEXTFORE));
    }
    }
  }
  else if (has_focus && blink)
  {
    g2d->DrawLine (
        Frame ().xmin + 5,
        Frame ().ymin + 5,
        Frame ().xmin + 5,
        Frame ().ymax - 5,
        WindowManager ()->GetPrefMgr ()->GetColor (AC_TEXTFORE));
  }
}

bool awsTextBox::OnMouseDown (int, int x, int y)
{
  // make sure text is valid
  if (text && text->Length() > 0)
  {
    // determine how many chars in the mouse was clicked
    int tp,cp;
    scfString tmp(text->GetData () + start);
    tp = x - 4 - Frame ().xmin;
    cp = WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->
      GetLength(tmp,tp);
    cursor = cp + start;
  }
  // This is needed to get keyboard focus for the mouse!
  return true;
}

bool awsTextBox::OnMouseExit ()
{
  mouse_is_over = false;
  Invalidate ();
  return true;
}

bool awsTextBox::OnMouseEnter ()
{
  mouse_is_over = true;
  Invalidate ();
  return true;
}

bool awsTextBox::OnKeypress (int key, int Char, int)
{
  switch (key)
  {
    case CSKEY_ENTER:
      Broadcast (signalEnterPressed);
      break;

    case CSKEY_TAB:
      Broadcast (signalTabPressed);
      break;

    case CSKEY_BACKSPACE:
      if (cursor > 0) 
      {
        cursor--;
        if (cursor - start < 5) start = cursor - 5;
        if (start < 0) start = 0;
        if (text && (text->Length () > 1))
          if (cursor == text->Length ())
            text->Truncate (text->Length () - 1);
          else
          {
            scfString tmp(text->GetData());
            tmp.Truncate (cursor);
            tmp.Append (text->GetData()+cursor+1);
            text->Replace(&tmp);
          }
        else
          text->Clear ();
      }

      break;

    case CSKEY_DEL:
      if (cursor > 0 && cursor == text->Length ()) cursor--;
      if (cursor - start < 5) start = cursor - 5;
      if (start < 0) start = 0;
      if (text && (text->Length () > 1))
        if (cursor == text->Length ())
          text->Truncate (text->Length () - 1);
        else
        {
          scfString tmp(text->GetData());
          tmp.Truncate (cursor);
          tmp.Append (text->GetData()+cursor+1);
          text->Replace(&tmp);
        }
      else
        text->Clear ();

      break;

    case CSKEY_LEFT:
      if (cursor > 0) cursor--;
      if (cursor - start < 5) start = cursor - 5;
      if (start < 0) start = 0;

      break;

    case CSKEY_RIGHT:
      if (cursor < text->Length ()) {
        cursor++;
      }

      break;

    case CSKEY_HOME:
      cursor=0;
      start=0;

      break;

    case CSKEY_END:
      cursor=text->Length();

      break;

    default:
      {
	if (!isprint (Char))
	  break;
	
	if (disallow && (strchr (disallow->GetData (), Char) != 0))
	  break;
	
	char str[2];
	str[0] = (char)Char;
	str[1] = 0;
        scfString tmp(str);

        if (cursor == text->Length ()) 
        {
	  text->Append (str);
        }
        else
        {
          text->Insert (cursor, &tmp);
        }
	cursor++;
	Broadcast (signalChanged);
      }
  }         // end switch
  Invalidate ();
  return true;
}

bool awsTextBox::OnLostFocus ()
{
  has_focus = false;
  Broadcast (signalLostFocus);
  Invalidate ();
  return true;
}

bool awsTextBox::OnGainFocus ()
{
  has_focus = true;
  Invalidate ();
  return true;
}

void awsTextBox::OnSetFocus ()
{
	Broadcast (signalFocused);
}

/************************************* Command Button Factory ****************/
awsTextBoxFactory::awsTextBoxFactory (iAws *wmgr) :
  awsComponentFactory(wmgr)
{
  Register ("Text Box");
  RegisterConstant ("tbfsNormal", awsTextBox::fsNormal);
  RegisterConstant ("tbfsBitmap", awsTextBox::fsBitmap);

  RegisterConstant ("signalTextBoxChanged", awsTextBox::signalChanged);
  RegisterConstant ("signalTextBoxLostFocus", awsTextBox::signalLostFocus);
  RegisterConstant ("signalEnterKeyPressed", awsTextBox::signalEnterPressed);
  RegisterConstant ("signalTabKeyPressed", awsTextBox::signalTabPressed);
  RegisterConstant ("signalTextBoxFocused", awsTextBox::signalFocused);
}

awsTextBoxFactory::~awsTextBoxFactory ()
{
  // empty
}

iAwsComponent *awsTextBoxFactory::Create ()
{
  return new awsTextBox;
}
