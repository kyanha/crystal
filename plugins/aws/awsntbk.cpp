#include "cssysdef.h"
#include "awsntbk.h"
#include "aws3dfrm.h"
#include "awskcfct.h"
#include "awsslot.h"
#include "awsscrbr.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "ivideo/fontserv.h"
#include "csutil/scfstr.h"
#include "csutil/csevent.h"
#include "iutil/evdefs.h"

#include <stdio.h>

const int awsNotebook::nbTop = 1;
const int awsNotebook::nbBottom = 2;

const int awsNotebook::nbBreak = 1;
const int awsNotebook::nbSlide = 2;

const int awsNotebook::fsBump = 0;
const int awsNotebook::fsSimple = 1;
const int awsNotebook::fsRaised = 2;
const int awsNotebook::fsSunken = 3;
const int awsNotebook::fsFlat = 4;
const int awsNotebook::fsNone = 5;

awsNotebook::awsNotebook () :
  bb_location(nbTop),
  bb_style(nbSlide),
  maxheight (0),
  sink(this)
{
  SetFlag (AWSF_CMP_ALWAYSERASE);
}

awsNotebook::~awsNotebook ()
{}

const char *awsNotebook::Type ()
{
  return "Notebook";
}

bool awsNotebook::Setup (iAws *_wmgr, iAwsComponentNode *settings)
{
  if (!awsPanel::Setup (_wmgr, settings)) return false;

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  pm->GetInt (settings, "Location", bb_location);
  pm->GetInt (settings, "Mode", bb_style);

  sink.RegisterTrigger("ActivateTab", &OnActivateTab);
  sink.RegisterTrigger("DeactivateTab", &OnDeactivateTab);

  awsKeyFactory info;

  info.Initialize ("ButtonBar", "Notebook ButtonBar");

  csRect r(0, 0, Frame().Width (), 20);

  tab_ctrl.SetParent (this);
  tab_ctrl.Setup (_wmgr, info.GetThisNode ());
  tab_ctrl.ResizeTo(r);
  tab_ctrl.SetFlag(AWSF_CMP_NON_CLIENT);
  awsComponent::AddChild (&tab_ctrl);
  tab_ctrl.SetTopBottom (bb_location == awsNotebook::nbTop);
  return true;
}

bool awsNotebook::GetProperty (const char *name, void **parm)
{
  if (awsPanel::GetProperty (name, parm)) return true;

  if (strcmp ("Location", name) == 0)
    *parm = (void *) &bb_location;
  else
  if (strcmp ("Mode", name) == 0)
    *parm = (void *) &bb_style;
  else
    return false;

  return true;
}

bool awsNotebook::SetProperty (const char *name, void *parm)
{
  if (awsPanel::SetProperty (name, parm)) return true;

  if (strcmp ("Location", name) == 0)
  {
    int h = *(int *)parm;
    if (h == awsNotebook::nbTop || h == awsNotebook::nbBottom)
    {
      if (bb_location != h)
      {
        bb_location = h;
        tab_ctrl.SetTopBottom (h == awsNotebook::nbTop);
        Invalidate ();
      }
      return true;
    }
    return false;
  }
  else if (strcmp ("Mode", name) == 0)
  {
    int h = *(int *)parm;
    if (h == awsNotebook::nbBreak || h == awsNotebook::nbSlide)
    {
      if ( h != bb_style)
      {
        bb_style = h;
        Invalidate ();
      }
      return true;
    }
    return false;
  }

  return false;
}

void awsNotebook::OnDraw (csRect )
{

  csRect f (Frame ());
  if (bb_location == nbTop)
    f.ymin += tab_ctrl.Frame ().Height ();
  else
    f.ymax -= tab_ctrl.Frame ().Height ();
  
  frame_drawer.Draw (
      f,
      style,
      Window()->Frame());
}

void awsNotebook::AddChild (iAwsComponent *child)
{
  child->ResizeTo (csRect(0, tab_ctrl.Frame ().Height ()+2, 
	                      Frame ().Width (), Frame ().Height ()));
  child->Hide();
  awsComponent::AddChild (child);

  iString *str = NULL;
  child->GetProperty ("Caption", (csSome*)&str);
  iAwsSource* src = tab_ctrl.AddTab (str, (void*)child);

  // hook up the source so we receive the signals
  slot.Connect(src, awsTab::signalActivateTab, &sink, sink.GetTriggerID("ActivateTab"));
  slot.Connect(src, awsTab::signalDeactivateTab, &sink, sink.GetTriggerID("DeactivateTab"));
}

void awsNotebook::OnActivateTab(void* param, iAwsSource* src)
{
  iAwsComponent* child;
  src->GetComponent()->GetProperty("User Param", (void**)&child);
  child->Show();
  ((awsNotebook*)param)->Invalidate();
}

void awsNotebook::OnDeactivateTab(void* param, iAwsSource* src)
{
  iAwsComponent* child;
  src->GetComponent()->GetProperty("User Param", (void**)&child);
  child->Hide();
  ((awsNotebook*)param)->Invalidate();
}

awsNotebookFactory::awsNotebookFactory (
  iAws *wmgr) :
    awsComponentFactory(wmgr)
{

  Register ("Notebook");
  RegisterConstant ("nbTop", awsNotebook::nbTop);
  RegisterConstant ("nbBottom", awsNotebook::nbBottom);
  RegisterConstant ("nbBreak", awsNotebook::nbBreak);
  RegisterConstant ("nbSlide", awsNotebook::nbSlide);

  RegisterConstant ("nbfsBump", awsNotebook::fsBump);
  RegisterConstant ("nbfsSimple", awsNotebook::fsSimple);
  RegisterConstant ("nbfsRaised", awsNotebook::fsRaised);
  RegisterConstant ("nbfsSunken", awsNotebook::fsSunken);
  RegisterConstant ("nbfsFlat", awsNotebook::fsFlat);
  RegisterConstant ("nbfsNone", awsNotebook::fsNone);

}

iAwsComponent *awsNotebookFactory::Create ()
{
  return new awsNotebook;
}


/********************************** Notebook Page ********************************/

awsNotebookPage::awsNotebookPage ():
  tex(NULL),
  caption (NULL),
  icon (NULL),
  iconalign(0)
{
  SetFlag (AWSF_CMP_ALWAYSERASE);
}

awsNotebookPage::~awsNotebookPage ()
{
  SCF_DEC_REF (caption);
}

bool awsNotebookPage::Setup (iAws *_wmgr, iAwsComponentNode *settings)
{
  if (!awsComponent::Setup (_wmgr, settings)) return false;

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  tex = pm->GetTexture ("Texture");
  pm->GetString (settings, "Caption", caption);
  pm->GetString (settings, "Icon", icon);
  pm->GetInt (settings, "IconAlign", iconalign);

  return true;
}

bool awsNotebookPage::GetProperty (const char *name, void **parm)
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
  else
  if (strcmp ("Icon", name) == 0)
  {
    char *st = NULL;

    if (icon) st = icon->GetData ();

    iString *s = new scfString (st);
    *parm = (void *)s;
    return true;
  }
  else
  if (strcmp ("IconAlign", name) == 0)
  {
    int **t = (int **)parm;
    *t = &iconalign;
    return true;
  }

  return false;
}

bool awsNotebookPage::SetProperty (const char *name, void *parm)
{
  if (awsComponent::SetProperty (name, parm)) return true;
  
  if (strcmp ("Caption", name) == 0)
  {
    iString *s = (iString *) (parm);

    SCF_DEC_REF (caption);

    if (s && s->Length ())
      (caption = s)->IncRef ();
    else
      caption = NULL;
    Invalidate ();

    return true;
  }
  else
  if (strcmp ("Icon", name) == 0)
  {
    iString *s = (iString *) (parm);

    SCF_DEC_REF (icon);

    if (s && s->Length ())
      (icon = s)->IncRef ();
    else
      icon = NULL;

    return true;
  }
  else
  if (strcmp ("IconAlign", name) == 0)
  {
    iconalign =*(int *)parm;;
    return true;
  }
  return false;
}

const char *awsNotebookPage::Type ()
{
  return "Notebook Page";
}

awsNotebookPageFactory::awsNotebookPageFactory (
  iAws *wmgr) :
    awsComponentFactory(wmgr)
{
  Register ("Notebook Page");

  RegisterConstant ("nbiaLeft", awsCmdButton::iconLeft);
  RegisterConstant ("nbiaRight", awsCmdButton::iconRight);
  RegisterConstant ("nbiaTop", awsCmdButton::iconTop);
  RegisterConstant ("nbiaBottom", awsCmdButton::iconBottom);
}

iAwsComponent *awsNotebookPageFactory::Create ()
{
  return new awsNotebookPage;
}

/*********************** awsNotebookButton ***********************/

const int awsNotebookButton::signalActivateTab = 1;
const int awsNotebookButton::iconLeft = 0;
const int awsNotebookButton::iconRight = 1;
const int awsNotebookButton::iconTop = 2;
const int awsNotebookButton::iconBottom = 3;

awsNotebookButton::awsNotebookButton ():
  is_active(false),
  is_first(false),
  is_top(true),
  caption (NULL),
  captured(false),
  icon_align(0),
  alpha_level(92)
{
  tex[0]=tex[1]=tex[2]=NULL;
}

awsNotebookButton::~awsNotebookButton ()
{
}

bool awsNotebookButton::Setup (iAws *_wmgr, iAwsComponentNode *settings)
{
  if (!awsComponent::Setup (_wmgr, settings)) return false;

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  pm->LookupIntKey ("OverlayTextureAlpha", alpha_level);  // global get
  pm->GetInt (settings, "Alpha", alpha_level);            // local overrides, if present.
  pm->GetInt (settings, "IconAlign", icon_align);
  pm->GetString (settings, "Caption", caption);

  iString *tn = NULL;

  tex[0] = pm->GetTexture ("Texture");
  pm->GetString (settings, "Image", tn);

  if (tn) tex[1] = pm->GetTexture (tn->GetData (), tn->GetData ());

  iString *in = NULL;
  pm->GetString (settings, "Icon", in);
  if (in) tex[2] = pm->GetTexture (in->GetData (), in->GetData ());

  return true;
}

void awsNotebookButton::GetClientRect (csRect &pf)
{
  awsComponent *t = (awsComponent *)Parent ();
  iAwsClientRect *cr = SCF_QUERY_INTERFACE (t, iAwsClientRect);
  if (cr)
  {
    pf = cr->GetClientRect ();
    cr->DecRef ();
  }
  else
    pf = Parent ()->Frame ();
}

void awsNotebookButton::OnDraw (csRect)
{
  int tw=0, th=0, tx, ty, itx=0, ity=0;
  csRect oldClip;

  iGraphics2D *g2d = WindowManager ()->G2D ();
  iGraphics3D *g3d = WindowManager ()->G3D ();
  
  csRect pf;
  GetClientRect (pf);

  g2d->GetClipRect(oldClip.xmin, oldClip.ymin, oldClip.xmax, oldClip.ymax);
  pf.Intersect(oldClip);
  g2d->SetClipRect (pf.xmin, pf.ymin, pf.xmax, pf.ymax);

  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();
  int hi = pm->GetColor (AC_HIGHLIGHT);
  int hi2 = pm->GetColor (AC_HIGHLIGHT2);
  int lo = pm->GetColor (AC_SHADOW);
  int lo2 = pm->GetColor (AC_SHADOW2);
  int fill = pm->GetColor (AC_FILL);
  int dfill = pm->GetColor (AC_DARKFILL);

  const csRect &r = Frame ();

  if (is_active)
  {
    g2d->DrawLine (r.xmin + 1, is_top ? r.ymin : r.ymax, r.xmax-1, is_top ? r.ymin : r.ymax, hi);
    g2d->DrawLine (r.xmin, r.ymin + 1, r.xmin, r.ymax, hi);
    g2d->DrawLine (r.xmax-1, r.ymin + 1, r.xmax-1, r.ymax, lo);
    g2d->DrawLine (r.xmax, r.ymin + 1, r.xmax, r.ymax, lo2);
  }
  else
  {
    g2d->DrawLine (r.xmin, r.ymin + 1, r.xmin, r.ymax, is_first ? hi2 : lo);
    g2d->DrawLine (r.xmin + 1, is_top ? r.ymin : r.ymax, r.xmax, is_top ? r.ymin : r.ymax, hi2);
    g2d->DrawLine (r.xmax, r.ymin + 1, r.xmax, r.ymax, lo);
  }

  g2d->DrawBox (r.xmin + 1, r.ymin + 1, r.Width () - 1, r.Height () - 1, is_active ? fill : dfill);

  if (tex[0])
    g3d->DrawPixmap (tex[0], r.xmin+1, r.ymin+1, r.Width ()-1, r.Height ()-1,
                     r.xmin+1, r.ymin+1, r.Width ()-1, r.Height ()-1, alpha_level);

  if (tex[1])
  {
    int img_w, img_h;

    tex[1]->GetOriginalDimensions (img_w, img_h);

    g3d->DrawPixmap (tex[1], r.xmin+1, r.ymin+1, r.Width ()-1, r.Height ()-1,
                     0, 0, img_w, img_h, 0);
  }

  tx = r.Width () >> 1;
  ty = r.Height () >> 1;

  if (caption)
    pm->GetDefaultFont ()->GetDimensions (caption->GetData (), tw, th);

  if (tex[2])
  {
    int img_w, img_h;
    itx = tx, ity = ty;

    tex[2]->GetOriginalDimensions (img_w, img_h);

    itx -= (img_w>>1);
    ity -= (img_h>>1);

    switch (icon_align)
    {
    case iconLeft:
      itx = tx - ((tw+img_w)>>1) - 1;
      ity = ty - (img_h>>1);
      tx = itx + img_w + 2;
      ty = ty - (th>>1);
      break;
    case iconRight:
      itx = tx + ((tw-img_w)>>1) + 1;
      ity = ty - (img_h>>1);
      tx = tx - ((tw+img_w)>>1) - 1;
      ty = ty - (th>>1);
      break;
    case iconTop:
      itx = tx - (img_w>>1);
      ity = ty - ((th+img_h)>>1) - 1;
      tx = tx - (tw>>1);
      ty = ity + img_h + 2;
      break;
    case iconBottom:
      itx = tx - (img_w>>1);
      ity = ty + ((th-img_h)>>1) + 1;
      tx = tx - (tw>>1);
      ty = ty - ((th+img_h)>>1) - 1;
      break;
    }
        
    g3d->DrawPixmap (tex[2], r.xmin + itx, r.ymin + ity, img_w, img_h,
                     0, 0, img_w, img_h, 0);
  }
  else
  {
    tx -= (tw>>1);
    ty -= (th>>1);
  }

  // Draw the caption, if there is one and the style permits it.
  if (caption)
  {

    // Draw the text
    g2d->Write (pm->GetDefaultFont (), r.xmin + tx, r.ymin + ty,
                pm->GetColor (AC_TEXTFORE), -1, caption->GetData ());

  }

}

bool awsNotebookButton::GetProperty (const char *name, void **parm)
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
  return false;
}

bool awsNotebookButton::SetProperty (const char *name, void *parm)
{
  if (awsComponent::SetProperty (name, parm)) return true;
  
  if (strcmp ("Caption", name) == 0)
  {
    iString *s = (iString *) (parm);

    SCF_DEC_REF (caption);

    if (s && s->Length ())
      (caption = s)->IncRef ();
    else
      caption = NULL;
    Invalidate ();

    return true;
  }
  return false;
}

bool awsNotebookButton::OnMouseDown (int, int, int)
{
  if (!is_active)
  {
    captured = true;
    WindowManager ()->CaptureMouse (this);
    return true;
  }
  return false;
}

bool awsNotebookButton::OnMouseUp (int, int x, int y)
{
  return HandleClick (x, y);
}

bool awsNotebookButton::OnMouseClick (int, int x, int y)
{
  return HandleClick (x, y);
}

bool awsNotebookButton::OnMouseDoubleClick (int, int x, int y)
{
  return HandleClick (x, y);
}

bool awsNotebookButton::HandleClick (int x, int y)
{
  if (captured)
  {
    WindowManager ()->ReleaseMouse ();
    captured = false;
    if (!is_active && Frame().Contains (x,y))
    {
      Broadcast (signalActivateTab);
      is_active = true;
    }
    Invalidate ();
    return true;
  }
  return false;
}

csRect awsNotebookButton::getPreferredSize ()
{
  return getMinimumSize ();
}

csRect awsNotebookButton::getMinimumSize ()
{
  int tw = 0, th = 0;

  if (caption)
  {
    // Get the size of the text
    WindowManager ()->GetPrefMgr ()->GetDefaultFont ()->GetDimensions (
        caption->GetData (), tw, th);
  }

  if (tex[2])
  {
    int img_w = 0, img_h = 0;
    tex[2]->GetOriginalDimensions (img_w, img_h);
    
    if (icon_align == iconLeft || icon_align == iconRight)
    {
      tw += img_w + 2;
      th = MAX(th, img_h);
    }
    else
    {
      th += img_h + 2;
      tw = MAX(tw, img_w);
    }
  }

  return csRect (0, 0, tw + 4, th + 4);
}

awsNotebookButtonFactory::awsNotebookButtonFactory (iAws *wmgr) :
  awsComponentFactory(wmgr)
{
  Register ("Notebook Button");

  RegisterConstant ("signalActivateTab", awsNotebookButton::signalActivateTab);
}

iAwsComponent *awsNotebookButtonFactory::Create ()
{
  return new awsNotebookButton;
}

/************************ awsNotebookButtonBar ***************************/

SCF_IMPLEMENT_IBASE_EXT(awsNotebookButtonBar)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iAwsClientRect)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE(awsNotebookButtonBar::eiAwsClientRect)
  SCF_IMPLEMENTS_INTERFACE (iAwsClientRect)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

const int awsNotebookButtonBar::HandleSize = 12;

awsNotebookButtonBar::awsNotebookButtonBar ():
  next(NULL),
  prev(NULL),
  next_slot(NULL),
  prev_slot(NULL),
  nextimg(NULL),
  previmg(NULL),
  first(-1),
  active(-1),
  is_top(true),
  sink(NULL)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiAwsClientRect);
}

awsNotebookButtonBar::~awsNotebookButtonBar ()
{
  if (prev_slot)
    prev_slot->Disconnect (prev, awsCmdButton::signalClicked,
                           sink, sink->GetTriggerID ("Prev"));
  if (next_slot)
    next_slot->Disconnect (next, awsCmdButton::signalClicked,
                           sink, sink->GetTriggerID ("Next"));

  SCF_DEC_REF (sink);
  SCF_DEC_REF (prev_slot);
  SCF_DEC_REF (next_slot);
}

bool awsNotebookButtonBar::Setup (iAws *_wmgr, iAwsComponentNode *settings)
{
  if (!awsComponent::Setup (_wmgr, settings)) return false;

  // set up the next/prev buttons and hide them initially
  next = new awsSliderButton;
  prev = new awsSliderButton;

  awsKeyFactory previnfo, nextinfo;

  previnfo.Initialize ("prev", "Slider Button");
  nextinfo.Initialize ("next", "Slider Button");

  previnfo.AddIntKey ("Style", awsCmdButton::fsToolbar);
  nextinfo.AddIntKey ("Style", awsCmdButton::fsToolbar);

  nextimg = WindowManager ()->GetPrefMgr ()->GetTexture ("ScrollBarRt");
  previmg = WindowManager ()->GetPrefMgr ()->GetTexture ("ScrollBarLt");

  if (!previmg || !nextimg) return false;

  csRect r(0, 0, HandleSize, HandleSize);

  r.Move (Frame ().Width () - 2*HandleSize-1, Frame ().Height ()-HandleSize);
  previnfo.AddRectKey ("Frame", r);

  r.Move (HandleSize+1, 0);
  nextinfo.AddRectKey ("Frame", r);

  prev->SetParent (this);
  next->SetParent (this);

  prev->Setup (_wmgr, previnfo.GetThisNode ());
  next->Setup (_wmgr, nextinfo.GetThisNode ());

  prev->SetProperty ("Image", previmg);
  next->SetProperty ("Image", nextimg);

  sink = new awsSink (this);

  sink->RegisterTrigger ("Prev", &PrevClicked);
  sink->RegisterTrigger ("Next", &NextClicked);

  prev_slot = new awsSlot ();
  next_slot = new awsSlot ();

  prev_slot->Connect (prev, awsCmdButton::signalClicked, sink, 
                      sink->GetTriggerID ("Prev"));
  next_slot->Connect (next, awsCmdButton::signalClicked, sink,
                      sink->GetTriggerID ("Next"));

  prev->Hide ();
  next->Hide ();

  AddChild (prev);
  AddChild (next);

  sink->RegisterTrigger ("ActivateTab", &ActivateTab);

  return true;
}

void awsNotebookButtonBar::SetTopBottom (bool to_top)
{
  if (is_top != to_top)
  {
    is_top = to_top;
    DoLayout ();
  }
}

void awsNotebookButtonBar::DoLayout ()
{
  int i, x=0;
  csRect r = Frame ();
  csRect cr = Parent ()->Frame ();

  r.xmin = cr.xmin;
  r.xmax = cr.xmax;

  if (is_top)
  {
    r.ymax = cr.ymin + r.Height ();
    r.ymin = cr.ymin;
    cr.ymin = r.ymax+1;
  }
  else
  {
    r.ymin = cr.ymax - r.Height ();
    r.ymax = cr.ymax;
	// should this be -= or just = ???
	// Noah
    cr.ymax -= (r.ymin+1);
  }

  ResizeTo(r);

  for (i=first-1; i >=0 ; i--)
  {
    awsNotebookButton *btn = vTabs.Get (i)->button;
    csRect br =  btn->Frame ();
    btn->Hide ();
    r.xmax = r.xmin - 1;
    r.xmin = r.xmax - br.Width ();
    btn->ResizeTo(r);
    btn->SetTop (is_top);
    //csRect o = vTabs.Get (i)->comp->Frame ();
    vTabs.Get (i)->comp->ResizeTo(cr);
    //vTabs.Get (i)->comp->MoveChildren (cr.xmin - o.xmin, cr.ymin - o.ymin);
  }

  r = Frame ();

  for (i=MAX(first,0); i < vTabs.Length (); i++)
  {
    awsNotebookButton *btn = vTabs.Get (i)->button;
    csRect br =  btn->Frame ();
    r.xmax = r.xmin + br.Width ();
    btn->ResizeTo(r);
	br = r;
    r.xmin = r.xmax+1;
    x += br.Width ();
    btn->SetTop (is_top);
    //csRect o = vTabs.Get (i)->comp->Frame ();
	vTabs.Get(i)->comp->ResizeTo(cr);
    //vTabs.Get (i)->comp->Frame () = cr;
    //vTabs.Get (i)->comp->MoveChildren (cr.xmin - o.xmin, cr.ymin - o.ymin);
  }

  if (x > Frame ().Width ())
  {
    csRect r = Frame ();
    r.xmin = r.xmax - 2*HandleSize-1;
    r.ymin = r.ymax - HandleSize;
    r.xmax = r.xmin + HandleSize;
    prev->ResizeTo(r);
    prev->Show ();
    r.Move (HandleSize+1,0);
    next->ResizeTo(r);
    next->Show ();
  }
  else
  {
    next->Hide();
    prev->Hide();
  }
}

csRect awsNotebookButtonBar::GetClientRect ()
{
  csRect r(Frame ());

  if (!next->isHidden ())
    r.xmax -= (2*HandleSize+2);

  return r;
}

bool awsNotebookButtonBar::Add (iAwsComponent *comp)
{
  // determine caption of tab
  iString *str = NULL;

  comp->GetProperty ("Caption", (csSome*)&str);
  if (!str || !str->GetData ())
  {
    SCF_DEC_REF(str);
    csString theCap ("Tab ");
    theCap += vTabs.Length ()+1;
    str = new scfString ((const char*)theCap);
  }

  // create a button
  awsNotebookButton *btn = new awsNotebookButton;

  // initialize and setup the button
  awsKeyFactory btninfo;

  btninfo.Initialize (str->GetData(), "Notebook Button");
  btninfo.AddRectKey ("Frame", csRect (0, 0, Frame ().Width (), Frame ().Height ()));

  iString *icon = NULL;
  if (comp->GetProperty ("Icon", (csSome*)&icon) && icon && icon->Length ())
  {
    btninfo.AddStringKey ("Icon", icon ? icon->GetData() : "");
    int *iconalign;
    if (comp->GetProperty ("IconAlign", (csSome*)&iconalign))
      btninfo.AddIntKey ("IconAlign", *iconalign);
  }

  btn->SetParent (this);
  btn->Setup (WindowManager (), btninfo.GetThisNode ());
  btn->SetProperty ("Caption", str);

  // resize button
  csRect r(btn->getPreferredSize ());
  int last = vTabs.Length ();
  if (r.Height () > Frame ().Height())
  {
    int delta = r.Height () - Frame ().Height ();
    Resize(Frame().Width(), Frame().Height() + delta);
  }
  //btn->Frame () = r;
  btn->ResizeTo(r);

  if (last>0)
  {
    btn->SetActive (false);
    btn->SetFirst (false);
    comp->Hide ();
    comp->SetDeaf (true);
  }
  else
  {
    first = 0;
    active = 0;
    btn->SetActive (true);
    btn->SetFirst (true);
    comp->Show ();
    comp->SetDeaf (false);
  }

  AddChild (btn);

  // connect myself with button to keep informed about state changes
  awsSlot *slot = new awsSlot;
  slot->Connect (btn, awsNotebookButton::signalActivateTab, sink, sink->GetTriggerID ("ActivateTab"));
  vTabs.Push (btn, slot, comp, sink);
  //csRect newFrame(Frame());
  //newFrame.ymin = newFrame.ymax+1;
  //ResizeTo(newFrame);
  DoLayout ();

  btn->Invalidate ();
  str->DecRef ();
  return true;
}

bool awsNotebookButtonBar::Remove (iAwsComponent *comp)
{
  int idx = vTabs.Find ((csSome)comp);

  if (idx!=-1)
  {
    if (idx == active)
      if (vTabs.Length () -1 == active)
        Activate (active-1);
      else
        Activate (active+1);

    vTabs.Get (first)->button->SetFirst (false);
    if ((idx < first) || (idx == first && (first > 0 || vTabs.Length () < 2)))
      first--;

    if (first > -1)
      vTabs.Get (first)->button->SetFirst (true);

    if (idx < active)
      active--;

    vTabs.Delete (idx);
    return true;
  }
  return false;
}

void awsNotebookButtonBar::OnDraw (csRect)
{
  iGraphics2D *g2d = WindowManager ()->G2D ();
  iAwsPrefManager *pm = WindowManager ()->GetPrefMgr ();

  int dark = pm->GetColor (AC_SHADOW);
  
  csRect r = Frame();

  int y = (is_top ? r.ymax : r.ymin);

  if (active != -1)
  {
    const csRect &b = vTabs.Get (active)->button->Frame();
    if (b.xmin >= r.xmax || b.xmax <= r.xmin)
      g2d->DrawLine (r.xmin, y, r.xmax, y, dark);
    else
    {
      if (b.xmax < r.xmax && b.xmin > r.xmin)
      {
        g2d->DrawLine (r.xmin, y, b.xmin-1, y, dark);
        g2d->DrawLine (b.xmax+1, y, r.xmax, y, dark);
      }
      else
        if (b.xmax > r.xmin && b.xmax < r.xmax)
          g2d->DrawLine (b.xmax+1, y, r.xmax, y, dark);
        else if (b.xmin > r.xmin && b.xmin < r.xmax)
          g2d->DrawLine (r.xmin, y, b.xmin-1, y, dark);
    }    
  }
  else
    g2d->DrawLine (r.xmin, y, r.xmax, y, dark);

}

void awsNotebookButtonBar::ScrollLeft ()
{
  if (vTabs.Length () && first != vTabs.Length ()-1)
  {
    int xdelta = vTabs.Get (first)->button->Frame ().Width ()+1;
    vTabs.Get (first)->button->SetFirst (false);
    for (int i=0; i < vTabs.Length (); i++)
    {
      awsNotebookButton *btn = vTabs.Get (i)->button;
      csRect f = btn->Frame ();
      btn->Move (-xdelta,0);
      //if (f.xmin >= Frame ().xmax || f.xmax <= Frame ().xmin)
      //  btn->Hide ();
      //else
      //  btn->Show ();
    }
    first++;
    vTabs.Get (first)->button->SetFirst (true);
  }
}

void awsNotebookButtonBar::ScrollRight ()
{
  if (vTabs.Length () && first != 0)
  {
    int xdelta = vTabs.Get (first-1)->button->Frame ().Width ()+1;
    vTabs.Get (first)->button->SetFirst (false);
    for (int i=0; i < vTabs.Length (); i++)
    {
      awsNotebookButton *btn = vTabs.Get (i)->button;
      csRect f = btn->Frame ();
      btn->Move (xdelta,0);
      //if (f.xmin >= Frame ().xmax || f.xmax <= Frame ().xmin)
      //  btn->Hide ();
      //else
      //  btn->Show ();
    }
    first--;
    vTabs.Get (first)->button->SetFirst (true);
  }
}

void awsNotebookButtonBar::MakeVisible (int idx)
{
  // make sure the <idx>-th button is visible
  const csRect &r = vTabs.Get (idx)->button->Frame ();
  csRect cr = GetClientRect ();
  if (first != idx && r.xmax > cr.xmax)
  {
    // scroll buttons left until the <idx>-th button becomes visible
    while (first != idx && r.xmax > cr.xmax)
      ScrollLeft ();

    Invalidate ();
  }
  else
  if (first != idx && r.xmin < cr.xmin)
  {
    // scroll buttons right until the <idx>-th button becomes visible
    while (first != idx && r.xmin < cr.xmin)
      ScrollRight ();
    
    Invalidate ();
  }

}

void awsNotebookButtonBar::Activate (int idx)
{
  vTabs.Get (active)->comp->Hide ();
  vTabs.Get (active)->comp->SetDeaf (true);
  vTabs.Get (active)->button->SetActive (false);
  vTabs.Get (active)->button->Invalidate ();
  vTabs.Get (idx)->comp->Show ();
  vTabs.Get (idx)->comp->SetDeaf (false);
  active = idx;
}

void awsNotebookButtonBar::ActivateTab (void *sk, iAwsSource *source)
{
  awsNotebookButtonBar *bb = (awsNotebookButtonBar *)sk;
  int idx = bb->vTabs.FindKey ((csConstSome)source->GetComponent (), 1);
  if (idx != -1 && bb->active != idx)
  {
    // hide the active and make the new one active
    bb->Activate (idx);
    bb->MakeVisible (idx);
  }
}

void awsNotebookButtonBar::PrevClicked (void *sk, iAwsSource *)
{
  awsNotebookButtonBar *bb = (awsNotebookButtonBar *)sk;
  bb->ScrollRight ();
}

void awsNotebookButtonBar::NextClicked (void *sk, iAwsSource *)
{
  awsNotebookButtonBar *bb = (awsNotebookButtonBar *)sk;
  bb->ScrollLeft ();
}
