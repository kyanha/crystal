#include "cssysdef.h"
#include "awsscrbr.h"
#include "aws3dfrm.h"
#include "awskcfct.h"
#include "awsslot.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/fontserv.h"
#include "csutil/scfstr.h"
#include "csutil/csevent.h"
#include "iutil/evdefs.h"

#include <stdio.h>

const int awsScrollBar:: signalChanged = 0x1;
const int awsScrollBar:: sboVertical = 0x0;
const int awsScrollBar:: sboHorizontal = 0x1;

awsScrollBar::awsScrollBar () :
  is_down(false),
  mouse_is_over(false),
  was_down(false),
  orientation(0),
  decVal(NULL),
  incVal(NULL),
  knob(NULL),
  timer(NULL),
  sink(NULL),
  dec_slot(NULL),
  inc_slot(NULL),
  knob_slot(NULL),
  tick_slot(NULL),
  value(0),
  max(1),
  min(0),
  amntvis(0),
  value_delta(0.1),
  value_page_delta(0.25)
{
  //SetFlag (AWSF_CMP_ALWAYSERASE);
  captured = false;
}

awsScrollBar::~awsScrollBar ()
{
  if (dec_slot)
    dec_slot->Disconnect (
        decVal,
        awsCmdButton::signalClicked,
        sink,
        sink->GetTriggerID ("DecValue"));
  if (inc_slot)
    inc_slot->Disconnect (
        incVal,
        awsCmdButton::signalClicked,
        sink,
        sink->GetTriggerID ("IncValue"));
  if (knob_slot)
    knob_slot->Disconnect (
        knob,
        awsCmdButton::signalClicked,
        sink,
        sink->GetTriggerID ("KnobTick"));
  if (tick_slot)
    tick_slot->Disconnect (
        timer,
        awsTimer::signalTick,
        sink,
        sink->GetTriggerID ("TickTock"));

  SCF_DEC_REF (incVal);
  SCF_DEC_REF (decVal);
  SCF_DEC_REF (knob);
  SCF_DEC_REF (sink);
  SCF_DEC_REF (inc_slot);
  SCF_DEC_REF (dec_slot);
  SCF_DEC_REF (knob_slot);
  SCF_DEC_REF (tick_slot);
  SCF_DEC_REF (timer);

  if (captured) WindowManager ()->ReleaseMouse ();
}

const char *awsScrollBar::Type ()
{
  return "Scroll Bar";
}

bool awsScrollBar::Setup (iAws *_wmgr, awsComponentNode *settings)
{
  if (!awsPanel::Setup (_wmgr, settings)) return false;

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  pm->GetInt (settings, "Orientation", orientation);
  int h = (int)min;
  h = (int)min;
  pm->GetInt (settings, "Min", h);
  min = (float)h;
  h = (int)value;
  pm->GetInt (settings, "Value", h);
  value = (float)h;
  h = 1;
  pm->GetInt (settings, "Max", h);
  max = (float)h;
  h = (int)amntvis;
  pm->GetInt (settings, "PageSize", h);
  amntvis = (float)h;

  // Setup embedded buttons
  incVal = new awsSliderButton;
  decVal = new awsSliderButton;
  knob = new awsSliderButton;
  timer = new awsTimer (WindowManager ()->GetObjectRegistry (), this);

  awsKeyFactory incinfo, decinfo, knobinfo;

  decinfo.Initialize ("decVal", "Slider Button");
  incinfo.Initialize ("incVal", "Slider Button");
  knobinfo.Initialize ("knob", "Slider Button");

  decinfo.AddIntKey ("Style", awsCmdButton::fsNormal);
  incinfo.AddIntKey ("Style", awsCmdButton::fsNormal);
  knobinfo.AddIntKey ("Style", awsCmdButton::fsNormal);

  switch (orientation)
  {
    case sboVertical:
      {
        incimg = pm->GetTexture ("ScrollBarDn");
        decimg = pm->GetTexture ("ScrollBarUp");

        // Abort if the images are not found
        if (!incimg || !decimg) return false;

        int img_w, img_h;

        incimg->GetOriginalDimensions (img_w, img_h);

        decinfo.AddRectKey ("Frame", csRect (0, 0, Frame ().Width (), img_h));

        incinfo.AddRectKey ("Frame", csRect (
              0,
              Frame ().Height () - img_h,
              Frame ().Width (),
              Frame ().Height ()));

        knobinfo.AddRectKey ("Frame", csRect (0, img_h + 1, Frame ().Width (), 2 * img_h + 1));
      }
      break;

    default:
      {
        incimg = pm->GetTexture ("ScrollBarRt");
        decimg = pm->GetTexture ("ScrollBarLt");

        // Abort if the images are not found
        if (!incimg || !decimg) return false;

        int img_w, img_h;

        incimg->GetOriginalDimensions (img_w, img_h);

        decinfo.AddRectKey ("Frame",
            csRect (
              Frame ().xmin,
              Frame ().ymin,
              Frame ().xmin + img_w + 5,
              Frame ().ymax));

        incinfo.AddRectKey ("Frame",
            csRect (
              Frame ().xmax - img_w - 5,
              Frame ().ymin,
              Frame ().xmax,
              Frame ().ymax));

        knobinfo.AddRectKey ("Frame",
            csRect (
              Frame ().xmin + img_w + 6,
              Frame ().ymin,
              Frame ().xmin + 2 * img_w + 6,
              Frame ().ymax));
      }
      break;
  } // end switch framestyle

  //  DEBUG_BREAK;

  decVal->SetParent (this);
  incVal->SetParent (this);
  knob->SetParent (this);

  decVal->Setup (_wmgr, decinfo.GetThisNode ());
  incVal->Setup (_wmgr, incinfo.GetThisNode ());
  knob->Setup (_wmgr, knobinfo.GetThisNode ());

  decVal->SetProperty ("Image", decimg);
  incVal->SetProperty ("Image", incimg);


  csTicks t = (csTicks) 10;
  incVal->SetProperty ("TicksPerSecond", (void *) &t);
  decVal->SetProperty ("TicksPerSecond", (void *) &t);
  knob->SetProperty ("TicksPerSecond", (void *) &t);

  sink = new awsSink (this);

  sink->RegisterTrigger ("DecValue", &DecClicked);
  sink->RegisterTrigger ("IncValue", &IncClicked);
  sink->RegisterTrigger ("TickTock", &TickTock);
  sink->RegisterTrigger ("KnobTick", &KnobTick);

  dec_slot = new awsSlot ();
  inc_slot = new awsSlot ();
  tick_slot = new awsSlot ();
  knob_slot = new awsSlot ();

  dec_slot->Connect (
      decVal,
      awsCmdButton::signalClicked,
      sink,
      sink->GetTriggerID ("DecValue"));
  inc_slot->Connect (
      incVal,
      awsCmdButton::signalClicked,
      sink,
      sink->GetTriggerID ("IncValue"));
  knob_slot->Connect (
      knob,
      awsCmdButton::signalClicked,
      sink,
      sink->GetTriggerID ("KnobTick"));

  tick_slot->Connect (
      timer,
      awsTimer::signalTick,
      sink,
      sink->GetTriggerID ("TickTock"));

  return true;
}

bool awsScrollBar::GetProperty (const char *name, void **parm)
{
  if (awsComponent::GetProperty (name, parm)) return true;

  if (strcmp ("Value", name) == 0)
  {
    *parm = (void *) &value;
    return true;
  }

  return false;
}

bool awsScrollBar::SetProperty (const char *name, void *parm)
{
  if (awsComponent::SetProperty (name, parm)) return true;

  if (strcmp ("Change", name) == 0)
  {
    value_delta = *(int *)parm;
    return true;
  }
  else if (strcmp ("BigChange", name) == 0)
  {
    value_page_delta = *(int *)parm;
    return true;
  }
  else if (strcmp ("Min", name) == 0)
  {
    min = *(int *)parm;

    Invalidate ();
    return true;
  }
  else if (strcmp ("Max", name) == 0)
  {
    max = *(int *)parm;

    // Fix the page size
    if (amntvis > max) amntvis = max;
    int maxval = (int)(max-amntvis);

    // Fix value in case it's out of range
    value = (value < 0 ? 0 : (value > maxval ? maxval : value));

    Invalidate ();
    return true;
  }
  else if (strcmp ("PageSize", name) == 0)
  {
    amntvis = *(int *)parm;

    // Fix the page size
    if (amntvis > max) amntvis = max;
    int maxval = (int)(max-amntvis);

    // Fix value in case it's out of range
    value = (value < 0 ? 0 : (value > maxval ? maxval : value));

    Invalidate ();
    return true;
  }

  return false;
}

void awsScrollBar::KnobTick (void *sk, iAwsSource *)
{
  // adjust position of knob and scrollbar value
  awsScrollBar *sb = (awsScrollBar *)sk;
  int maxval = (int)(sb->max - sb->amntvis);

  if (sb->orientation == sboVertical)
  {
    int height = 10;
    csRect f (sb->Frame ());

    // Get the bar height
    f.ymin += sb->decVal->Frame ().Height () + 1;
    f.ymax -= sb->incVal->Frame ().Height () + 1;

    // Get the knob height
    if (sb->amntvis == 0)
      sb->WindowManager ()->GetPrefMgr ()->LookupIntKey (
          "ScrollBarHeight",
          height);
    else
      height = (int)((sb->amntvis * f.Height ()) / sb->max);

    // Get the actual height that we can traverse with the knob
    int bh = f.Height () - height;
    
    if (maxval == 0)
      sb->value = 0;
    else
      sb->value = (sb->knob->last_y - sb->decVal->Frame ().ymax) * maxval / bh;
  }
  else if (sb->orientation == sboHorizontal)
  {
    int width = 10;
    csRect f (sb->Frame ());

    f.xmin += sb->decVal->Frame ().Width () + 1;
    f.xmax -= sb->incVal->Frame ().Width () + 1;

    if (sb->amntvis == 0)
      sb->WindowManager ()->GetPrefMgr ()->LookupIntKey (
          "ScrollBarWidth",
          width);
    else
      width = (int)((sb->amntvis * f.Width ()) / sb->max);

    // Get the actual height that we can traverse with the knob
    int bw = f.Width () - width;

    if (maxval == 0)
      sb->value = 0;
    else
      sb->value = (sb->knob->last_x - sb->decVal->Frame ().xmax) * maxval / bw;
  }
  else
    return ;

  sb->value =
    (
      sb->value < sb->min ? 0 :
        (sb->value > maxval ? maxval : sb->value)
    );
  sb->Broadcast (signalChanged);
  sb->Invalidate ();
}

void awsScrollBar::TickTock (void *sk, iAwsSource *)
{
  awsScrollBar *sb = (awsScrollBar *)sk;

  if (sb->orientation == sboVertical)
  {
    if (sb->last_y < sb->knob->Frame ().ymin)
      sb->value -= sb->amntvis;
    else if (sb->last_y > sb->knob->Frame ().ymax)
      sb->value += sb->amntvis;
    else
      return ;
  }
  else
  {
    if (sb->last_x < sb->knob->Frame ().xmin)
      sb->value -= sb->amntvis;
    else if (sb->last_x > sb->knob->Frame ().xmax)
      sb->value += sb->amntvis;
    else
      return ;
  }

 int maxval = (int)(sb->max - sb->amntvis);
  sb->value =
    (
      sb->value < sb->min ? 0 :
        (sb->value > maxval ? maxval : sb->value)
    );

  sb->Broadcast (signalChanged);
  sb->Invalidate ();
}

void awsScrollBar::IncClicked (void *sk, iAwsSource *)
{
  awsScrollBar *sb = (awsScrollBar *)sk;

  sb->value += sb->value_delta;

  /// Check floor and ceiling
 int maxval = (int)(sb->max - sb->amntvis);
  sb->value =
    (
     sb->value < sb->min ? 0 :
     (sb->value > maxval ? maxval : sb->value)
     );

  sb->Broadcast (signalChanged);
  sb->Invalidate ();
}

void awsScrollBar::DecClicked (void *sk, iAwsSource *)
{
  awsScrollBar *sb = (awsScrollBar *)sk;

  sb->value -= sb->value_delta;

  /// Check floor and ceiling
 int maxval = (int)(sb->max - sb->amntvis);
  sb->value =
    (
      sb->value < sb->min ? 0 :
        (sb->value > maxval ? maxval : sb->value)
    );

  sb->Broadcast (signalChanged);
  sb->Invalidate ();
}

void awsScrollBar::OnDraw (csRect clip)
{


  int height = 10, width = 10;

  csRect f (Frame ());

  if (orientation == sboVertical)
  {
    // Get the bar height
    f.ymin += decVal->Frame ().Height () + 1;
    f.ymax -= incVal->Frame ().Height () + 1;

    // Get the knob height
    if (amntvis == 0)
      WindowManager ()->GetPrefMgr ()->LookupIntKey (
          "ScrollBarHeight",
          height);
    else
      height = (int)((amntvis * f.Height ()) / max);

    // Get the actual height that we can traverse with the knob
    int bh = f.Height () - height;

    // Get the knob's position
    int ky;
    if ((max-amntvis) == 0)
      ky = 0;
    else
      ky = (int)((value * bh) / (max-amntvis));

    f.ymin += ky;
    f.ymax = f.ymin + height;

    if (f.ymax > incVal->Frame ().ymin - 1)
      f.ymax = incVal->Frame ().ymin - 1;
  }
  else
  {
    f.xmin += decVal->Frame ().Width () + 1;
    f.xmax -= incVal->Frame ().Width () + 1;

    if (amntvis == 0)
      WindowManager ()->GetPrefMgr ()->LookupIntKey ("ScrollBarWidth", width);
    else
      width = (int)((amntvis * f.Width ()) / max);

    // Get the actual height that we can traverse with the knob
    int bw = f.Width () - width;

    // Get the knob's position
    int kx;
    if ((max-amntvis)==0)
      kx = 0;
    else
      kx = (int)((value * bw) / max);

    f.xmin += kx;
    f.xmax = f.xmin + width;

    if (f.xmax > incVal->Frame ().xmin - 1)
      f.xmax = incVal->Frame ().xmin - 1;
  }

  knob->ResizeTo(f);

  awsPanel::OnDraw(clip);
}

bool awsScrollBar::OnMouseDown (int btn, int x, int y)
{
  if (btn == 1 && !captured)
  {
    WindowManager ()->CaptureMouse (this);
    captured = true;
    timer->SetTimer (100);
    timer->Start ();
    last_x = x;
    last_y = y;
    return true;
  }

  return false;
}

bool awsScrollBar::OnMouseUp (int btn, int, int)
{
  if (captured && btn == 1)
  {
    WindowManager ()->ReleaseMouse ();
    captured = false;
    timer->Stop ();
  }

  return true;
}

bool awsScrollBar::OnMouseMove (int, int x, int y)
{
  if (captured)
  {
    last_x = x;
    last_y = y;
    return true;
  }

  return false;
}

bool awsScrollBar::OnMouseClick (int btn, int x, int y)
{
  return HandleClicking (btn, x, y);
}

bool awsScrollBar::OnMouseDoubleClick (int btn, int x, int y)
{
  return HandleClicking (btn, x, y);
}

bool awsScrollBar::HandleClicking (int btn, int x, int y)
{
  if (btn == 1)
  {
    if (captured) WindowManager ()->ReleaseMouse ();
    if (orientation == sboVertical)
    {
      if (y < knob->Frame ().ymin && y > decVal->Frame ().ymax)
        value -= amntvis;
      else if (y > knob->Frame ().ymax && y < incVal->Frame ().ymin)
        value += amntvis;
    }
    else
    {
      if (x < knob->Frame ().xmin && x > decVal->Frame ().xmax)
        value -= amntvis;
      else if (x > knob->Frame ().xmax && x < incVal->Frame ().xmin)
        value += amntvis;
    }

    // Check floor and ceiling
    value = (value < min ? min : (value > max ? max : value));

    Broadcast (signalChanged);
    Invalidate ();
    return true;
  }

  return false;
}

bool awsScrollBar::OnMouseExit ()
{
  mouse_is_over = false;
  Invalidate ();

  if (is_down) is_down = false;

  return true;
}

bool awsScrollBar::OnMouseEnter ()
{
  mouse_is_over = true;
  Invalidate ();
  return true;
}

bool awsScrollBar::OnKeypress (int, int)
{
  return false;
}

bool awsScrollBar::OnLostFocus ()
{
  return false;
}

bool awsScrollBar::OnGainFocus ()
{
  return false;
}

void awsScrollBar::OnAdded ()
{
  AddChild (incVal);
  AddChild (decVal);
  AddChild (knob);
}

void awsScrollBar::OnResized ()
{
  int h = incVal->Frame ().Height ();
  int w = incVal->Frame ().Width ();

  decVal->MoveTo(Frame ().xmax - w, Frame ().ymin);
  incVal->MoveTo(Frame ().xmax - w, Frame ().ymax - h);

}

/************************************* Command Button Factory ****************/

awsScrollBarFactory::awsScrollBarFactory (
  iAws *wmgr) :
    awsComponentFactory(wmgr)
{
  Register ("Scroll Bar");
  RegisterConstant ("sboVertical", awsScrollBar::sboVertical);
  RegisterConstant ("sboHorizontal", awsScrollBar::sboHorizontal);

  RegisterConstant ("signalScrollBarChanged", awsScrollBar::signalChanged);
}

awsScrollBarFactory::~awsScrollBarFactory ()
{
  // empty
}

iAwsComponent *awsScrollBarFactory::Create ()
{
  return new awsScrollBar;
}

/************************************* Slider Button ****************/

awsSliderButton::awsSliderButton () :
  timer(NULL),
  captured(false),
  nTicks((csTicks) 0),
  sink(NULL),
  tick_slot(NULL)
{
}

awsSliderButton::~awsSliderButton ()
{
  if (tick_slot)
    tick_slot->Disconnect (
        timer,
        awsTimer::signalTick,
        sink,
        sink->GetTriggerID ("TickTock"));

  if (captured) WindowManager ()->ReleaseMouse ();

  SCF_DEC_REF (tick_slot);
  SCF_DEC_REF (timer);
}

bool awsSliderButton::Setup (iAws *wmgr, awsComponentNode *settings)
{
  if (!awsCmdButton::Setup (wmgr, settings)) return false;

  timer = new awsTimer (WindowManager ()->GetObjectRegistry (), this);
  sink = new awsSink (this);

  sink->RegisterTrigger ("TickTock", &TickTock);

  tick_slot = new awsSlot ();

  tick_slot->Connect (
      timer,
      awsTimer::signalTick,
      sink,
      sink->GetTriggerID ("TickTock"));
  return true;
}

bool awsSliderButton::GetProperty (const char *name, void **parm)
{
  if (awsCmdButton::GetProperty (name, parm)) return true;

  if (strcmp ("TicksPerSecond", name) == 0)
  {
    *parm = (void *) &nTicks;
    return true;
  }

  return false;
}

bool awsSliderButton::SetProperty (const char *name, void *parm)
{
  if (awsCmdButton::SetProperty (name, parm)) return true;

  if (strcmp ("TicksPerSecond", name) == 0)
  {
    csTicks n = *(csTicks *)parm;
    if (n <= 0)
      nTicks = (csTicks) 0;
    else
      nTicks = (csTicks) (1000 / n);
    timer->SetTimer (nTicks);

    return true;
  }

  return false;
}

const char *awsSliderButton::Type ()
{
  return "Slider Button";
}

void awsSliderButton::TickTock (void *sk, iAwsSource *)
{
  awsSliderButton *sb = (awsSliderButton *)sk;
  sb->Broadcast (signalClicked);
}

bool awsSliderButton::OnMouseDown (int btn, int x, int y)
{
  bool succ = awsCmdButton::OnMouseDown (btn, x, y);

  if (!is_switch && btn == 1 && nTicks != 0 && !captured)
  {
    timer->Start ();
    WindowManager ()->CaptureMouse (this);
    last_x = x;
    last_y = y;
    captured = true;
  }

  return succ;
}

bool awsSliderButton::OnMouseUp (int btn, int x, int y)
{
  bool succ = awsCmdButton::OnMouseUp (btn, x, y);
  if (!is_switch && captured)
  {
    timer->Stop ();
    WindowManager ()->ReleaseMouse ();
    captured = false;
  }

  return succ;
}

bool awsSliderButton::OnMouseMove (int, int x, int y)
{
  if (captured) last_x = x, last_y = y;

  return false;
}

bool awsSliderButton::OnMouseClick (int, int, int)
{
  if (captured)
  {
    timer->Stop ();
    WindowManager ()->ReleaseMouse ();
    captured = false;
  }

  return false;
}

bool awsSliderButton::OnMouseDoubleClick (int, int, int)
{
  if (captured)
  {
    timer->Stop ();
    WindowManager ()->ReleaseMouse ();
    captured = false;
  }

  return false;
}

/************************************* Slider Button Factory ****************/

awsSliderButtonFactory::awsSliderButtonFactory (
  iAws *wmgr) :
    awsCmdButtonFactory(wmgr)
{
  Register ("Slider Button");
}

awsSliderButtonFactory::~awsSliderButtonFactory ()
{
  // empty
}

iAwsComponent *awsSliderButtonFactory::Create ()
{
  return new awsSliderButton;
}
